// filter_rd_olken.cpp

/**
 * Program to filter instructions based on reuse distance using Olken splay tree.
 * It first profiles the entire trace to compute the reuse distance histogram,
 * outputs the histogram into a CSV file. Then it determines the 20% cutoff
 * threshold and filters instructions whose reuse distances are in the smallest 20%.
 *
 * Usage:
 *   ./filter_rd_olken trace_file nsimulate csv_output_file output_file
 *
 * Arguments:
 *   trace_file       - Path to the ChampSim trace file.
 *   nsimulate        - Number of instructions to simulate.
 *   csv_output_file  - Path to the output CSV file for reuse distance histogram.
 *   output_file      - Path to the output file for filtered instruction indices.
 */

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
#include <map>

using namespace clueless;
using namespace reuse_distance;  // Namespace for the Olken splay tree

// Command-line argument parsing
const char *argp_program_version = "ReuseDistanceFilter 2.1";
static char doc[] = "Filter instructions based on reuse distance using Olken splay tree";
static char args_doc[] = "<trace_file> <nsimulate> <csv_output_file> <output_file>";

static struct argp_option options[] = {
    { 0 }
};

struct arguments {
    const char *trace_file;
    double fraction = 0.2; 
    size_t nsimulate = 0;
    std::string csv_output_file;
    std::string output_file;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    arguments *args = (arguments *)state->input;
    switch (key) {
        case ARGP_KEY_ARG:
            if (state->arg_num == 0)
                args->trace_file = arg;
            else if (state->arg_num == 1)
                args->fraction = std::stod(arg);
            else if (state->arg_num == 2)
                args->nsimulate = std::stoull(arg);
            else if (state->arg_num == 3)
                args->csv_output_file = arg;
            else if (state->arg_num == 4)
                args->output_file = arg;
            else
                argp_usage(state);
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 5)
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

    // File to output the reuse distance histogram
    std::ofstream csv_output(args.csv_output_file);
    if (!csv_output.is_open()) {
        std::cerr << "Failed to open CSV file for writing." << std::endl;
        return;
    }

    // File to output the filtered instruction indices
    std::ofstream output_file(args.output_file);
    if (!output_file.is_open()) {
        std::cerr << "Failed to open output file for writing." << std::endl;
        return;
    }

    size_t nsimulate = args.nsimulate;
    double fraction = args.fraction;
    // Data structures
    size_t instruction_index = 0;

    // Initialize the Olken splay tree for reuse distance calculation
    olken_tree tree;

    // Map to store reuse distance histogram: reuse_distance -> count
    std::map<size_t, size_t> reuse_distance_histogram;

    // Vector to store reuse distances for all instructions
    std::vector<std::pair<size_t, size_t>> instruction_reuse_distances; // Pair of instruction index and reuse distance

    // First pass: Compute reuse distances and build histogram
    for (size_t i = 0; i < nsimulate; ++i) {
        auto input_ins = reader.read_single_instr();
        const auto &decoded_instr = decoder.decode(input_ins);

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

            // Update the histogram
            reuse_distance_histogram[reuse_distance]++;

            // Store the instruction index and its reuse distance
            instruction_reuse_distances.emplace_back(i, reuse_distance);

            // Update the tree with the current access
            update(tree, address, instruction_index);
        }

        instruction_index++;
    }

    // Output the reuse distance histogram to CSV file
    // CSV format: ReuseDistance,Count
    csv_output << "ReuseDistance,Count\n";
    for (const auto &entry : reuse_distance_histogram) {
        csv_output << entry.first << "," << entry.second << "\n";
    }
    csv_output.close();
    std::cout << "Reuse distance histogram written to: " << args.csv_output_file << std::endl;

    // Determine the 20% cutoff threshold
    size_t total_accesses = instruction_reuse_distances.size();
    size_t cutoff_index = static_cast<size_t>(total_accesses * fraction);

    // To find the reuse distance value below which 20% of the reuse distances fall,
    // we need to sort the reuse distances and find the value at the cutoff index
    std::vector<size_t> reuse_distances; // Only reuse distances
    for (const auto &pair : instruction_reuse_distances) {
        reuse_distances.push_back(pair.second);
    }

    // Sort the reuse distances
    std::sort(reuse_distances.begin(), reuse_distances.end());

    // Find the cutoff threshold
    size_t cutoff_threshold = reuse_distances[cutoff_index];
    std::cout << "20% cutoff threshold reuse distance: " << cutoff_threshold << std::endl;

    // Second pass: Filter instructions with reuse distances <= cutoff_threshold
    for (const auto &pair : instruction_reuse_distances) {
        size_t idx = pair.first;
        size_t reuse_distance = pair.second;

        if (reuse_distance <= cutoff_threshold) {
            // Filter this instruction
            output_file << idx << "\n";
        }
    }
    output_file.close();
    std::cout << "Filtered instructions written to: " << args.output_file << std::endl;
}
