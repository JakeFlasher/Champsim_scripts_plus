// constable_extended.cc

#include "champsim-trace-decoder.h"
#include "propagator.h"
#include "tracereader.h"
#include <argp.h>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <limits>
#include <cmath>

using namespace clueless;

// Command-line argument parsing
const char *argp_program_version = "AdaptiveStableInstructions 1.2";
const char *argp_program_bug_address = "<your_email@example.com>";
static char doc[] = "Adaptively identify global stable instructions (loads, stores) in ChampSim traces.";
static char args_doc[] = "<trace_file> <nwarmup> <nsimulate> <output_stable_instructions>";

static struct argp_option options[] = {
    {"warmup", 'w', "N", 0, "Number of warmup instructions (default: 0)"},
    {"simulate", 's', "N", 0, "Number of instructions to simulate (default: all)"},
    {"error_rate", 'e', "E", 0, "Tolerable error rate (default: 0.01)"},
    {"adjust_interval", 'a', "N", 0, "Threshold adjustment interval (default: 10000)"},
    {0}
};

struct arguments {
    const char *trace_file;
    size_t nwarmup = 0;
    size_t nsimulate = SIZE_MAX;
    double tolerable_error_rate = 0.01; // Default tolerable error rate (1%)
    size_t adjust_interval = 10000;     // Default adjustment interval
    std::string output_stable_instructions;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *args = (struct arguments *) state->input;
    switch (key) {
        case 'w':
            args->nwarmup = std::stoull(arg);
            break;
        case 's':
            args->nsimulate = std::stoull(arg);
            break;
        case 'e':
            args->tolerable_error_rate = std::stod(arg);
            break;
        case 'a':
            args->adjust_interval = std::stoull(arg);
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num == 0)
                args->trace_file = arg;
            else if (state->arg_num == 1)
                args->nwarmup = std::stoull(arg);
            else if (state->arg_num == 2)
                args->nsimulate = std::stoull(arg);
            else if (state->arg_num == 3)
                args->output_stable_instructions = arg;
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

void process_trace_file(const std::string &trace_file_path, const std::string &output_stable_instructions,
                        size_t nwarmup, size_t nsimulate, double tolerable_error_rate, size_t adjust_interval);

int main(int argc, char *argv[]) {
    struct arguments args;
    // Define the argp structure
    static struct argp argp = { options, parse_opt, args_doc, doc };
    argp_parse(&argp, argc, argv, 0, 0, &args);

    std::string trace_file_path = args.trace_file;
    size_t nwarmup = args.nwarmup;
    size_t nsimulate = args.nsimulate;
    double E_TOLERABLE = args.tolerable_error_rate;
    size_t ADJUST_INTERVAL = args.adjust_interval;
    std::string output_stable_instructions = args.output_stable_instructions;

    process_trace_file(trace_file_path, output_stable_instructions, nwarmup, nsimulate, E_TOLERABLE, ADJUST_INTERVAL);

    return EXIT_SUCCESS;
}

void process_trace_file(const std::string &trace_file_path, const std::string &output_stable_instructions,
                        size_t nwarmup, size_t nsimulate, double E_TOLERABLE, size_t ADJUST_INTERVAL) {

    tracereader reader(trace_file_path.c_str());
    champsim_trace_decoder decoder;

    std::ofstream stable_instructions_file(output_stable_instructions);
    if (!stable_instructions_file.is_open()) {
        std::cerr << "Failed to open stable instructions file for writing." << std::endl;
        return;
    }

    // Skip warmup instructions
    for (size_t i = 0; i < nwarmup; ++i) {
        reader.read_single_instr();
    }

    // Data structures for tracking
    struct InstrInfo {
        size_t last_occurrence = 0;
        uint64_t last_address = 0;  // For memory instructions
        int confidence = 0;
        int C_pred = 0;     // Total predictions
        int C_err = 0;      // Total mispredictions
    };

    // Tracking maps for instructions
    std::unordered_map<uint64_t, InstrInfo> load_info_map;  // Key: Load instruction PC
    std::unordered_map<uint64_t, InstrInfo> store_info_map; // Key: Store instruction PC

    // State tracking maps
    std::unordered_map<unsigned char, size_t> last_write_to_reg;   // Register writes
    std::unordered_map<uint64_t, size_t> last_write_to_mem;        // Memory writes

    // Global counters
    int total_predictions = 0;
    int total_errors = 0;
    size_t last_adjustment = nwarmup;

    // Initial confidence threshold
    int CONFIDENCE_THRESHOLD = 5;

    size_t actual_simulated_instr_count = nwarmup;

    for (size_t i = nwarmup; i < nwarmup + nsimulate; ++i) {
        auto input_ins = reader.read_single_instr();
        const auto &decoded_instr = decoder.decode(input_ins);
        ++actual_simulated_instr_count;

        auto ip = decoded_instr.ip;

        // Update last writes to destination registers
        for (const auto &reg : decoded_instr.dst_reg) {
            last_write_to_reg[reg] = i;
        }

        bool predicted = false;
        bool correct_prediction = false;
        bool inputs_unchanged = true;

        // Check if source registers have been written
        for (const auto &reg : decoded_instr.src_reg) {
            if (last_write_to_reg.find(reg) != last_write_to_reg.end() &&
                last_write_to_reg[reg] > i) {
                inputs_unchanged = false;
                break;
            }
        }

        if (decoded_instr.op == propagator::instr::opcode::OP_LOAD) {
            auto &instr_info = load_info_map[ip];
            auto address = decoded_instr.address;

            // Check if source registers and address have not changed
            bool address_match = (instr_info.last_address == address);

            // No store has modified the memory location since last load
            bool memory_unchanged = true;
            auto last_store_it = last_write_to_mem.find(address);
            if (last_store_it != last_write_to_mem.end() && last_store_it->second > instr_info.last_occurrence) {
                memory_unchanged = false;
            }

            correct_prediction = inputs_unchanged && address_match && memory_unchanged;

            if (instr_info.confidence >= CONFIDENCE_THRESHOLD && inputs_unchanged) {
                predicted = true;
                total_predictions++;
                instr_info.C_pred++;
            } else {
                predicted = false;
            }

            if (predicted) {
                if (correct_prediction) {
                    instr_info.confidence = std::min(instr_info.confidence + 1, 100);
                } else {
                    instr_info.confidence = instr_info.confidence / 2; // Halve the confidence
                    instr_info.C_err++;
                    total_errors++;
                }
            } else {
                // Update confidence if not predicted
                if (correct_prediction) {
                    instr_info.confidence++;
                } else {
                    instr_info.confidence = std::max(instr_info.confidence / 2, 0);
                }
            }

            // Update last occurrence and address
            instr_info.last_occurrence = i;
            instr_info.last_address = address;

            // If we predicted and are confident, log the instruction
            if (predicted && instr_info.confidence >= CONFIDENCE_THRESHOLD) {
                stable_instructions_file << "LOAD " << i << "\n";
            }

        } else if (decoded_instr.op == propagator::instr::opcode::OP_STORE) {
            auto &instr_info = store_info_map[ip];
            auto address = decoded_instr.address;

            // Check if source registers and address have not changed
            bool address_match = (instr_info.last_address == address);

            // Memory location has not been modified since last store
            bool memory_unchanged = true;
            auto last_store_it = last_write_to_mem.find(address);
            if (last_store_it != last_write_to_mem.end() && last_store_it->second > instr_info.last_occurrence) {
                memory_unchanged = false;
            }

            correct_prediction = inputs_unchanged && address_match && memory_unchanged;

            if (instr_info.confidence >= CONFIDENCE_THRESHOLD && inputs_unchanged) {
                predicted = true;
                total_predictions++;
                instr_info.C_pred++;
            } else {
                predicted = false;
            }

            if (predicted) {
                if (correct_prediction) {
                    instr_info.confidence = std::min(instr_info.confidence + 1, 100);
                } else {
                    instr_info.confidence = instr_info.confidence / 2; // Halve the confidence
                    instr_info.C_err++;
                    total_errors++;
                }
            } else {
                // Update confidence if not predicted
                if (correct_prediction) {
                    instr_info.confidence++;
                } else {
                    instr_info.confidence = std::max(instr_info.confidence / 2, 0);
                }
            }

            // Update last occurrence and address
            instr_info.last_occurrence = i;
            instr_info.last_address = address;

            // Update last write to memory address
            last_write_to_mem[address] = i;

            // If we predicted and are confident, log the instruction
            if (predicted && instr_info.confidence >= CONFIDENCE_THRESHOLD) {
                stable_instructions_file << "STORE " << i << "\n";
            }
        }

        // Periodically adjust the confidence threshold
        if ((i - last_adjustment) >= ADJUST_INTERVAL) {
            last_adjustment = i;
            if (total_predictions > 0) {
                double observed_error_rate = static_cast<double>(total_errors) / total_predictions;
                if (observed_error_rate > E_TOLERABLE) {
                    CONFIDENCE_THRESHOLD = std::min(CONFIDENCE_THRESHOLD + 1, 100);
                } else if (observed_error_rate < E_TOLERABLE / 2) {
                    CONFIDENCE_THRESHOLD = std::max(CONFIDENCE_THRESHOLD - 1, 1);
                }
                // Reset global counters
                total_predictions = 0;
                total_errors = 0;
            }
        }
    }

    stable_instructions_file.close();

    std::string base_name = std::filesystem::path(trace_file_path).stem().string();
    std::cout << "Trace: " << base_name << "\n";
    std::cout << "Total simulated instructions: " << actual_simulated_instr_count << "\n";
    std::cout << "Stable instructions written to: " << output_stable_instructions << "\n";
}
