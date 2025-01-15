// value_leaking_baseline.cc
#include <unordered_set>
#include <random>
#include <sstream>
#include "champsim-trace-decoder.h"
#include "propagator.h"
#include "tracereader.h"
#include <argp.h>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <iostream>
#include <cassert>
#include <cstddef>
#include <cstdlib>

const char *argp_program_version = "value_leaking_baseline 1.0";
const char *argp_program_bug_address = "<your_email@example.com>";
static char doc[] = "Collect value-leaking instructions for baseline generation";
static char args_doc[] = "TRACE";
static struct argp_option options[] = {
    {"warmup", 'w', "N", 0, "Skip the first N instructions"},
    {"simulate", 's', "N", 0, "Simulate N instructions"},
    {"heartbeat", 'h', "N", 0, "Print heartbeat every N instructions"},
    {"window_size", 'z', "N", 0, "Sliding window size"},
    {"stable_instructions", 't', "FILE", 0, "File containing stable instructions"},
    {"output_baseline", 'o', "FILE", 0, "Output file for baseline instructions"},
    {0}
};

struct arguments {
    size_t nwarmup = 0;
    size_t nsimulate = 10000000;
    size_t heartbeat = 100000;
    size_t window_size = 1000000;
    char *trace_file = nullptr;
    char *stable_instructions_file = nullptr;
    char *output_baseline_file = nullptr;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    auto *args = (arguments *)state->input;
    switch (key) {
        case 'w':
            args->nwarmup = atoll(arg);
            break;
        case 's':
            args->nsimulate = atoll(arg);
            break;
        case 'h':
            args->heartbeat = atoll(arg);
            break;
        case 'z':
            args->window_size = atoll(arg);
            break;
        case 't':
            args->stable_instructions_file = arg;
            break;
        case 'o':
            args->output_baseline_file = arg;
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num >= 1)
                argp_usage(state);
            args->trace_file = arg;
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 1)
                argp_usage(state);
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char *argv[]) {
    arguments args;
    argp_parse(&argp, argc, argv, 0, 0, &args);

    if (!args.output_baseline_file) {
        std::cerr << "Error: Output baseline file not specified." << std::endl;
        return EXIT_FAILURE;
    }

    if (!args.stable_instructions_file) {
        std::cerr << "Error: Stable instructions file not specified." << std::endl;
        return EXIT_FAILURE;
    }

    using namespace clueless;
    tracereader reader(args.trace_file);
    champsim_trace_decoder decoder;
    propagator pp;

    // Load stable instructions
    std::unordered_set<size_t> stable_instructions;
    {
        std::ifstream stable_file(args.stable_instructions_file);
        if (!stable_file) {
            std::cerr << "Failed to open stable instructions file." << std::endl;
            return EXIT_FAILURE;
        }
        std::string line;
        while (std::getline(stable_file, line)) {
            std::istringstream iss(line);
            std::string instr_type;
            size_t index;
            if (iss >> instr_type >> index) {
                stable_instructions.insert(index);
            }
        }
    }

    // Output file for baseline instructions
    std::ofstream baseline_file(args.output_baseline_file);
    if (!baseline_file) {
        std::cerr << "Failed to open output baseline file." << std::endl;
        return EXIT_FAILURE;
    }

    // Skip warmup instructions
    for (size_t i = 0; i < args.nwarmup; ++i) {
        reader.read_single_instr();
    }

    // Data structures
    size_t actual_simulated_instr_count = args.nwarmup;
    size_t window_end = args.nwarmup + args.window_size;

    std::vector<size_t> value_leaking_instructions;
    std::vector<size_t> candidate_instructions;

    // Function to collect value-leaking instruction indices
    std::unordered_set<size_t> value_leaking_indices;
    pp.add_secret_exposed_hook([&](const propagator::secret_exposed_hook_param &param) {
        size_t instr_idx = actual_simulated_instr_count;
        value_leaking_indices.insert(instr_idx);
    });

    // Simulate instructions
    for (; actual_simulated_instr_count < args.nwarmup + args.nsimulate; ++actual_simulated_instr_count) {
        if (!(actual_simulated_instr_count % args.heartbeat)) {
            printf("Processed %zu instructions\n", actual_simulated_instr_count);
            fflush(stdout);
        }

        if (actual_simulated_instr_count >= window_end) {
            // Process the sliding window

            // Determine number of strict global stable instructions in this window
            size_t window_start = actual_simulated_instr_count - args.window_size;
            size_t num_stable = 0;
            for (size_t idx = window_start; idx < window_end; ++idx) {
                if (stable_instructions.find(idx) != stable_instructions.end()) {
                    num_stable++;
                }
            }

            // Collect value-leaking instructions in this window
            std::vector<size_t> window_value_leaking_instructions;
            for (size_t idx = window_start; idx < window_end; ++idx) {
                if (value_leaking_indices.find(idx) != value_leaking_indices.end()) {
                    window_value_leaking_instructions.push_back(idx);
                }
            }

            // Adjust candidate instructions
            if (window_value_leaking_instructions.size() < num_stable) {
                // Sample additional instructions from stable_instructions
                std::vector<size_t> additional_instructions;
                for (size_t idx : stable_instructions) {
                    if (idx >= window_start && idx < window_end) {
                        additional_instructions.push_back(idx);
                    }
                }
                // Randomly select instructions to fill up to num_stable
                std::shuffle(additional_instructions.begin(), additional_instructions.end(), std::mt19937{std::random_device{}()});
                size_t remaining = num_stable - window_value_leaking_instructions.size();
                for (size_t k = 0; k < remaining && k < additional_instructions.size(); ++k) {
                    window_value_leaking_instructions.push_back(additional_instructions[k]);
                }
            } else if (window_value_leaking_instructions.size() > num_stable) {
                // Reduce to num_stable
                window_value_leaking_instructions.resize(num_stable);
            }

            // Write baseline instructions to file
            for (size_t idx : window_value_leaking_instructions) {
                baseline_file << "BASELINE " << idx << "\n";
            }

            // Clear data for next window
            value_leaking_indices.clear();

            window_end += args.window_size;
        }

        auto input_ins = reader.read_single_instr();
        const auto &decoded_ins = decoder.decode(input_ins);
        pp.propagate(decoded_ins);
    }

    // Handle the last window (if any)
    if (!value_leaking_indices.empty()) {
        // Similar processing as above
        size_t window_start = actual_simulated_instr_count - args.window_size;
        size_t num_stable = 0;
        for (size_t idx = window_start; idx < actual_simulated_instr_count; ++idx) {
            if (stable_instructions.find(idx) != stable_instructions.end()) {
                num_stable++;
            }
        }

        std::vector<size_t> window_value_leaking_instructions(value_leaking_indices.begin(), value_leaking_indices.end());

        if (window_value_leaking_instructions.size() < num_stable) {
            std::vector<size_t> additional_instructions;
            for (size_t idx : stable_instructions) {
                if (idx >= window_start && idx < actual_simulated_instr_count) {
                    additional_instructions.push_back(idx);
                }
            }
            std::shuffle(additional_instructions.begin(), additional_instructions.end(), std::mt19937{std::random_device{}()});
            size_t remaining = num_stable - window_value_leaking_instructions.size();
	    std::cout << "remaining: " << remaining << std::endl;
	    fflush(stdout);
            for (size_t k = 0; k < remaining && k < additional_instructions.size(); ++k) {
                window_value_leaking_instructions.push_back(additional_instructions[k]);
            }
        } else if (window_value_leaking_instructions.size() > num_stable) {
            window_value_leaking_instructions.resize(num_stable);
        }

        // Write to baseline file
        for (size_t idx : window_value_leaking_instructions) {
            baseline_file << "BASELINE " << idx << "\n";
        }
    }

    baseline_file.close();

    std::cout << "Total instructions processed: " << actual_simulated_instr_count << "\n";
    std::cout << "Baseline instructions written to: " << args.output_baseline_file << "\n";
    fflush(stdout);

    return EXIT_SUCCESS;
}
