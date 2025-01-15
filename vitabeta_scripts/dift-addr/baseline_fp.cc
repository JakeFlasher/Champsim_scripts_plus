// memory_footprint_filter.cpp

#include <sstream>
#include "champsim-trace-decoder.h"
#include "tracereader.h"
#include <argp.h>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <iostream>
#include <random>

using namespace clueless;

// Command-line argument parsing
const char *argp_program_version = "MemoryFootprintFilter 1.0";
static char doc[] = "Filter instructions based on memory footprint";
static char args_doc[] = "<trace_file> <nsimulate> <window_size> <footprint_threshold> <global_stable_file> <output_file>";

static struct argp_option options[] = {
    { 0 }
};

struct arguments {
    const char *trace_file;
    size_t nsimulate = 0;
    size_t window_size = 1000000;
    size_t footprint_threshold = 900000;
    std::string global_stable_file;
    std::string output_file;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    arguments *args = (arguments *)state->input;
    switch (key) {
        case ARGP_KEY_ARG:
            if (state->arg_num == 0)
                args->trace_file = arg;
            else if (state->arg_num == 1)
                args->nsimulate = std::stoull(arg);
            else if (state->arg_num == 2)
                args->window_size = std::stoull(arg);
            else if (state->arg_num == 3)
                args->footprint_threshold = std::stoull(arg);
            else if (state->arg_num == 4)
                args->global_stable_file = arg;
            else if (state->arg_num == 5)
                args->output_file = arg;
            else
                argp_usage(state);
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 6)
                argp_usage(state);
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

void process_trace_file(const arguments &args);

int main(int argc, char *argv[]) {
    arguments args;
    static struct argp argp = {options, parse_opt, args_doc, doc};
    argp_parse(&argp, argc, argv, 0, 0, &args);

    process_trace_file(args);

    return 0;
}

void process_trace_file(const arguments &args) {
    tracereader reader(args.trace_file);
    champsim_trace_decoder decoder;

    // Load global stable instruction indices
    std::unordered_set<size_t> global_stable_indices;
    {
        std::ifstream stable_file(args.global_stable_file);
        if (!stable_file.is_open()) {
            std::cerr << "Failed to open global stable instructions file." << args.global_stable_file  << std::endl;
            return;
        }
        std::string line;
        while (std::getline(stable_file, line)) {
            std::istringstream iss(line);
	    std::string type;
            size_t index;
            if (iss  >> type >> index) {
                global_stable_indices.insert(index);
            }
        }
    }

    std::ofstream output_file(args.output_file);
    if (!output_file.is_open()) {
        std::cerr << "Failed to open output file for writing." << std::endl;
        return;
    }

    size_t window_size = args.window_size;
    size_t footprint_threshold = args.footprint_threshold;

    std::vector<size_t> window_instructions;
    std::unordered_set<uint64_t> window_addresses; // Unique addresses in the window

    size_t instruction_index = 0;
    size_t window_start = 0;

    std::vector<size_t> filtered_instructions; // Indices of instructions that meet the criteria

    for (size_t i = 0; i < args.nsimulate; ++i) {
        auto input_ins = reader.read_single_instr();
        const auto &decoded_instr = decoder.decode(input_ins);


        // Collect window instructions
        window_instructions.push_back(instruction_index);

        // Collect unique addresses
        if (decoded_instr.op == propagator::instr::opcode::OP_LOAD ||
            decoded_instr.op == propagator::instr::opcode::OP_STORE) {
            uint64_t address = decoded_instr.address;
            window_addresses.insert(address);
        }

        instruction_index++;

        // Check if the window is full
        if (instruction_index - window_start >= window_size) {
            // Process the current window

            // Calculate memory footprint
            size_t footprint = window_addresses.size();

            // Determine the number of global stable instructions in this window
            size_t num_global_stable = 0;
            for (size_t idx = window_start; idx < instruction_index; ++idx) {
                if (global_stable_indices.find(idx) != global_stable_indices.end()) {
                    num_global_stable++;
                }
            }

            // Filter instructions based on footprint threshold
            if (footprint > footprint_threshold) {
                // For illustration, let's select all load/store instructions in the window
                for (size_t idx : window_instructions) {
                    filtered_instructions.push_back(idx);
                }
            }

            // Adjust the number of filtered instructions
            if (filtered_instructions.size() < num_global_stable) {
                // Need to add more instructions
	    std::cout << "remaining: " << num_global_stable - filtered_instructions.size() << std::endl;
	size_t deficit = num_global_stable - filtered_instructions.size();

	// Collect candidate instructions (exclude already filtered ones)
	std::vector<size_t> candidate_instructions;
	for (size_t idx : window_instructions) {
	    if (std::find(filtered_instructions.begin(), filtered_instructions.end(), idx) == filtered_instructions.end()) {
		candidate_instructions.push_back(idx);
	    }
	}

	// Randomly sample additional instructions
	std::random_device rd;
	std::mt19937 gen(rd());
	std::shuffle(candidate_instructions.begin(), candidate_instructions.end(), gen);

	size_t added = 0;
	for (size_t idx : candidate_instructions) {
	    if (added >= deficit) break;
	    filtered_instructions.push_back(idx);
	    added++;
	}
    } else if (filtered_instructions.size() > num_global_stable) {
	// Need to reduce the number of instructions
	std::random_device rd;
	std::mt19937 gen(rd());
	std::shuffle(filtered_instructions.begin(), filtered_instructions.end(), gen);
	filtered_instructions.resize(num_global_stable);
    }

    // Write filtered instructions to the output file
    for (size_t idx : filtered_instructions) {
	output_file << idx << "\n";
    }

    // Reset for the next window
    window_start = instruction_index;
    window_instructions.clear();
    filtered_instructions.clear();
    window_addresses.clear();
}
}

output_file.close();

std::cout << "Processing complete. Filtered instructions written to: " << args.output_file << std::endl;
}
