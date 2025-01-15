// torchdata_prep.cc

#include <sstream>
#include "champsim-trace-decoder.h"
#include "tracereader.h"
#include "olken.h"
#include <argp.h>
#include <set>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <random>
#include <limits>
#include <regex>
#include <algorithm>
#include <unordered_map>
#include <cmath> // For std::isnan, std::isinf
#include <iomanip> // For setting precision in output

using namespace clueless;
using namespace reuse_distance;  // Namespace for the Olken splay tree

// Command-line argument parsing
const char *argp_program_version = "ReuseDistanceFilter 2.0";
static char doc[] = "Filter instructions based on reuse distance using Olken splay tree";
static char args_doc[] = "<trace_file> <nsimulate> <simulation_result_file> <output_file>";

static struct argp_option options[] = {
    { 0 }
};

struct arguments {
    const char* trace_file;
    size_t nsimulate = 0;
    std::string simulation_result_file;
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
                args->simulation_result_file = arg;
            else if (state->arg_num == 3)
                args->output_file = arg;
            else
                argp_usage(state);
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 4)
                argp_usage(state);
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

// Function to parse the simulation result and return a map of instruction index to IPC
std::map<size_t, double> parse_simulation_result(const std::string& filename) {
    std::map<size_t, double> ipc_map;
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Failed to open simulation result file: " << filename << std::endl;
        return ipc_map;
    }

    std::string line;
    // Regex to match heartbeat lines
    std::regex heartbeat_regex(R"(Heartbeat CPU \d+ instructions: (\d+) cycles: \d+ heartbeat IPC: ([\d\.]+))");
    std::smatch match;

    while (std::getline(infile, line)) {
        if (std::regex_search(line, match, heartbeat_regex)) {
            size_t instruction_idx = std::stoull(match[1]);
            double ipc_value = std::stod(match[2]);
            ipc_map[instruction_idx] = ipc_value;
        }
    }
    return ipc_map;
}

void process_trace_file(const arguments &args);

int main(int argc, char *argv[]) {
    arguments args;
    static struct argp argp = {options, parse_opt, args_doc, doc };
    argp_parse(&argp, argc, argv, 0, 0, &args);

    process_trace_file(args);

    return 0;
}

void process_trace_file(const arguments& args) {
    tracereader reader(args.trace_file);
    champsim_trace_decoder decoder;

    // Parse IPC values
    auto ipc_values = parse_simulation_result(args.simulation_result_file);

    // Open output file (CSV format)
    std::ofstream output_file(args.output_file);
    if (!output_file.is_open()) {
        std::cerr << "Failed to open output file for writing." << std::endl;
        return;
    }

    // Write CSV header
    output_file << "instruction_index,ip,address,opcode,reuse_distance,ipc" << std::endl;

    size_t instruction_index = 0;

    // Initialize Olken splay tree for RD calculation
    olken_tree tree;

    size_t missing_ipc_count = 0;

    // Set formatting for output (optional)
    output_file << std::fixed << std::setprecision(6);

    for (size_t i = 0; i < args.nsimulate; ++i) {
        auto input_ins = reader.read_single_instr();
        // if (input_ins.eof)
        //    break; // End of file reached

        const auto& decoded_instr = decoder.decode(input_ins);

        uint64_t address = decoded_instr.address;
        size_t reuse_distance = 0;

        // Compute reuse distance using Olken splay tree
        double distance = compute_distance(tree, address);
        if (std::isinf(distance)) {
            // First-time access
            reuse_distance = std::numeric_limits<size_t>::max();
        } else {
            reuse_distance = static_cast<size_t>(distance);
        }

        // Update the tree with the current access
        update(tree, address, instruction_index);

        // Extract IPC value
        double ipc_value = std::numeric_limits<double>::quiet_NaN(); // Set to NaN by default
        auto ipc_itr = ipc_values.find(instruction_index);
        if (ipc_itr != ipc_values.end()) {
            ipc_value = ipc_itr->second;
        } else {
            missing_ipc_count++;
            // IPC value remains NaN
        }

        // Combine opcode and branch_taken into a single value
        int opcode_value = static_cast<int>(decoded_instr.opcode_with_branch_info());

        // Output data to CSV
        output_file << instruction_index << ",";
        output_file << decoded_instr.ip << ",";
        output_file << address << ",";
        output_file << opcode_value << ",";
        output_file << reuse_distance << ",";

        if (std::isnan(ipc_value)) {
            output_file << "NaN" << std::endl;
        } else {
            output_file << ipc_value << std::endl;
        }

        instruction_index++;
    }

    output_file.close();

    std::cout << "Processing complete. Missing IPC values: " << missing_ipc_count << std::endl;
    std::cout << "Data written to: " << args.output_file << std::endl;
}
