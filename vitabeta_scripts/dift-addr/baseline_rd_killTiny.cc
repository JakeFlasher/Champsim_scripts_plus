// reuse_distance_filter.cpp
#include <sstream>
#include "champsim-trace-decoder.h"
#include "tracereader.h"
#include <argp.h>
#include <unordered_map>
#include <list>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <iostream>
#include <random>

using namespace clueless;

// Command-line argument parsing
const char *argp_program_version = "ReuseDistanceFilter 1.1";
static char doc[] = "Filter instructions based on reuse distance";
static char args_doc[] = "<trace_file> <window_size> <reuse_distance_threshold> <global_stable_file> <output_file>";

static struct argp_option options[] = {
    { 0 }
};

struct arguments {
    const char *trace_file;
    size_t nsimulate = 0;
    size_t window_size = 1000000;
    size_t reuse_distance_threshold = 128;
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
                args->reuse_distance_threshold = std::stoull(arg);
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
            std::cerr << "Failed to open global stable instructions file." << std::endl;
            return;
        }
        std::string line;
        while (std::getline(stable_file, line)) {
            std::istringstream iss(line);
            size_t index;
	        std::string type;
            if (iss >> type >> index) {
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
    size_t reuse_distance_threshold = args.reuse_distance_threshold;

    size_t instruction_index = 0;
    size_t window_start = 0;

    std::vector<size_t> window_instructions;
    std::vector<size_t> filtered_instructions;

    // LRU Stack represented as a list and a hash map for O(1) access
    std::list<uint64_t> lru_stack; // Stores addresses
    std::unordered_map<uint64_t, std::list<uint64_t>::iterator> address_iter_map; // Maps address to its iterator in lru_stack

    for (size_t i = 0; i < args.nsimulate; ++i) {
        auto input_ins = reader.read_single_instr();
        const auto &decoded_instr = decoder.decode(input_ins);

        // Collect window instructions
        window_instructions.push_back(instruction_index);

        // Only consider load and store instructions
        if (decoded_instr.op == propagator::instr::opcode::OP_LOAD ||
            decoded_instr.op == propagator::instr::opcode::OP_STORE) {

            uint64_t address = decoded_instr.address;

            size_t reuse_distance = 0;
            if (address_iter_map.find(address) != address_iter_map.end()) {
                // Address has been accessed before
                auto it = address_iter_map[address];
                // Reuse distance is the position in the LRU stack
                reuse_distance = std::distance(lru_stack.begin(), it);

                // Move the address to the front (MRU position)
                lru_stack.erase(it);
                lru_stack.push_front(address);
                address_iter_map[address] = lru_stack.begin();
            } else {
                // First time access
                reuse_distance = std::numeric_limits<size_t>::max(); // Can be considered as 'infinite'

                // Add the address to the front of the stack
                lru_stack.push_front(address);
                address_iter_map[address] = lru_stack.begin();
            }

            // Remove entries from the LRU stack if it grows too large to save memory
            // Adjust this threshold based on available memory
            const size_t LRU_STACK_SIZE_LIMIT = 10000000;
            if (lru_stack.size() > LRU_STACK_SIZE_LIMIT) {
                // Remove the least recently used address
                uint64_t lru_address = lru_stack.back();
                lru_stack.pop_back();
                address_iter_map.erase(lru_address);
            }

            // Check if reuse distance exceeds the threshold
            if (reuse_distance < reuse_distance_threshold) {
                filtered_instructions.push_back(instruction_index);
            }
        }

        instruction_index++;

        // Check if the window is full
        if (instruction_index - window_start >= window_size) {
            // Process the current window

            // Determine the number of global stable instructions in this window
            size_t num_global_stable = 0;
            for (size_t idx = window_start; idx < instruction_index; ++idx) {
                if (global_stable_indices.find(idx) != global_stable_indices.end()) {
                    num_global_stable++;
                }
            }

            // Adjust the number of filtered instructions
            if (filtered_instructions.size() < num_global_stable) {
                // Need to add more instructions
                size_t deficit = num_global_stable - filtered_instructions.size();
                std::cout << "remaining:" << num_global_stable - filtered_instructions.size() << std::endl;
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
		std::cout << "exceeding:" << filtered_instructions.size() - num_global_stable << std::endl;
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

            // Optionally clear the LRU stack and address_iter_map (depends on whether we want to reset reuse distance calculations per window)
            // For continuous reuse distance calculation, comment out the following lines
            lru_stack.clear();
            address_iter_map.clear();
        }
    }

    output_file.close();

    std::cout << "Processing complete. Filtered instructions written to: " << args.output_file << std::endl;
}
