// reuse_distance_filter.cpp

#include <sstream>
#include "champsim-trace-decoder.h"
#include "tracereader.h"
#include "olken.h"  // Include the Olken splay tree header
#include <argp.h>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <iostream>
#include <random>
#include <limits>

using namespace clueless;
using namespace reuse_distance;  // Namespace for the Olken splay tree

// Command-line argument parsing
const char *argp_program_version = "ReuseDistanceFilter 2.0";
static char doc[] = "Filter instructions based on reuse distance using Olken splay tree";
static char args_doc[] = "<trace_file> <nsimulate> <window_size> <reuse_distance_threshold> <global_stable_file> <output_file>";

static struct argp_option options[] = {
    { 0 }
};

struct arguments {
    const char *trace_file;
    size_t nsimulate = 0;
    size_t window_size = 1000000;
    size_t reuse_distance_threshold = 64;
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
    static struct argp argp = {options, parse_opt, args_doc, doc };
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

    // Initialize the Olken splay tree for reuse distance calculation
    olken_tree tree;

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

            // Compute reuse distance using Olken splay tree
            double distance = compute_distance(tree, address);
            if (distance == std::numeric_limits<double>::infinity()) {
                // First time access
                reuse_distance = std::numeric_limits<size_t>::max();
            } else {
                reuse_distance = static_cast<size_t>(distance);
            }

            // Update the tree with the current access
            update(tree, address, instruction_index);

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

            // Write filtered instructions to the output file
            for (size_t idx : filtered_instructions) {
                output_file << idx << "\n";
            }

            // Reset for the next window
            window_start = instruction_index;
            window_instructions.clear();
            filtered_instructions.clear();

            // Optionally clear the tree (depends on whether we want to reset reuse distance calculations per window)
            // For continuous reuse distance calculation, comment out the following lines
            // tree = olken_tree();
        }
    }

    output_file.close();

    std::cout << "Processing complete. Filtered instructions written to: " << args.output_file << std::endl;
}
