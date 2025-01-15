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

using namespace clueless;
using namespace reuse_distance;  // Namespace for the Olken splay tree

// Command-line argument parsing
const char *argp_program_version = "ReuseDistanceFilter 2.0";
static char doc[] = "Filter instructions based on reuse distance using Olken splay tree";
// Update args_doc
static char args_doc[] = "<trace_file> <nsimulate> <simulation_result_file> <output_file>";

static struct argp_option options[] = {
    { 0 }
};

// Update arguments struct
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

// Change std::unordered_map to std::map to use lower_bound
std::map<size_t, double> parse_simulation_result(const std::string& filename) {
    std::map<size_t, double> ipc_map;
    std::ifstream infile(filename);
    std::string line;
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

    // Parse IPC values
    auto ipc_values = parse_simulation_result(args.simulation_result_file);

    // Open output file (CSV format)
    std::ofstream output_file(args.output_file);
    if (!output_file.is_open()) {
        std::cerr << "Failed to open output file for writing." << std::endl;
        return;
    }

    // Write CSV header
    //output_file << "instruction_index,ip,address,opcode,reuse_distance,ipc" << std::endl;
    output_file << "ipc" << std::endl;

    size_t instruction_index = 0;

    // Initialize Olken splay tree for RD calculation
    olken_tree tree;

    size_t missing_ipc_count = 0;

    double last_ipc_value = 0.0;

    for (size_t i = 0; i < args.nsimulate; ++i) {
        // Extract IPC value
        double ipc_value = 0.0;
        auto ipc_itr = ipc_values.lower_bound(instruction_index);
        if (ipc_itr != ipc_values.end()) {
            ipc_value = ipc_itr->second;
	    std::cout << "i'm here" << std::endl;
            last_ipc_value = ipc_value;
        } else {
            missing_ipc_count++;
            // Handle missing IPC values (use previous IPC value)
            ipc_value = last_ipc_value;
	    // ipc_value = -1;
        }

        // Output data to CSV
        // Note: Adjust the output format based on your needs

        // Combine opcode and branch_taken into a single value

        //output_file << instruction_index << ",";
        //output_file << decoded_instr.ip << ",";
        //output_file << address << ",";
        //output_file << opcode_value << ",";
        //output_file << reuse_distance << ",";
        output_file << ipc_value << std::endl;

        instruction_index++;
    }

    output_file.close();

    std::cout << "Processing complete. Missing IPC values: " << missing_ipc_count << std::endl;
    std::cout << "Data written to: " << args.output_file << std::endl;
}
