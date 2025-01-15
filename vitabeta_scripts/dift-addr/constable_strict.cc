// strict_example.cc

#include "champsim-trace-decoder.h"
#include "tracereader.h"
#include <argp.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>

using namespace clueless;

void process_trace_file(const std::string &trace_file_path, const std::string &output_stable_loads, size_t nwarmup, size_t nsimulate);

int main(int argc, char *argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <trace_file> <nwarmup> <nsimulate> <output_stable_loads>\n";
        return EXIT_FAILURE;
    }
    std::string trace_file_path = argv[1];
    size_t nwarmup = std::stoull(argv[2]);
    size_t nsimulate = std::stoull(argv[3]);
    std::string output_stable_loads = argv[4];
    process_trace_file(trace_file_path, output_stable_loads, nwarmup, nsimulate);
    return EXIT_SUCCESS;
}

void process_trace_file(const std::string &trace_file_path, const std::string &output_stable_loads, size_t nwarmup, size_t nsimulate) {
    tracereader reader(trace_file_path.c_str());
    champsim_trace_decoder decoder;
    std::ofstream stable_loads_file(output_stable_loads);
    if (!stable_loads_file.is_open()) {
        std::cerr << "Failed to open stable instructions file for writing." << std::endl;
        return;
    }
    // Skip warmup instructions
    for (size_t i = 0; i < nwarmup; ++i) {
        reader.read_single_instr();
    }
    std::cout << "Processing trace: " << trace_file_path << std::endl;
    std::cout << "Warmup instructions: " << nwarmup << std::endl;
    std::cout << "Simulation instructions: " << nsimulate << std::endl;
    // Data structures for tracking
    std::unordered_map<uint64_t, std::pair<size_t, uint64_t>> last_load_occurrence;     // For loads: ip -> <last occurrence index, address>
    std::unordered_map<uint64_t, std::pair<size_t, std::pair<uint64_t, uint64_t>>> last_store_occurrence; // For stores: ip -> <last occurrence index, <address, data>>
    std::unordered_map<unsigned, size_t> last_write_to_reg; // Register writes
    std::unordered_map<uint64_t, size_t> last_store_to_mem; // Memory writes

    size_t actual_simulated_instr_count = 0;

    for (size_t i = 0; i < nsimulate; ++i) {
        auto input_ins = reader.read_single_instr();
        const auto &decoded_instr = decoder.decode(input_ins);
        ++actual_simulated_instr_count;

        // Common variables
        auto ip = decoded_instr.ip;

        // Update last writes to destination registers
        for (const auto &reg : decoded_instr.dst_reg) {
            last_write_to_reg[reg] = i;
        }

        // Handle LOAD instructions
        if (decoded_instr.op == propagator::instr::opcode::OP_LOAD) {
            auto address = decoded_instr.address;
            if (last_load_occurrence.find(ip) == last_load_occurrence.end()) {
                last_load_occurrence[ip] = {i, address};
            } else {
                bool condition1 = true;
                bool condition2 = true;
                // Check if address is the same
                if (last_load_occurrence[ip].second != address) {
                    condition1 = false;
                } else {
                    // Condition 1: Source registers have not been written
                    for (const auto &reg : decoded_instr.src_reg) {
                        if (last_write_to_reg.find(reg) != last_write_to_reg.end() &&
                            last_write_to_reg[reg] > last_load_occurrence[ip].first &&
                            last_write_to_reg[reg] < i) { // Ensure the write happened after last occurrence and before current
                            condition1 = false;
                            break;
                        }
                    }
                    // Condition 2: No store to the same address since last occurrence
                    if (last_store_to_mem.find(address) != last_store_to_mem.end() &&
                        last_store_to_mem[address] > last_load_occurrence[ip].first &&
                        last_store_to_mem[address] < i) {
                        condition2 = false;
                    }
                }
                if (condition1 && condition2) {
                    stable_loads_file << "LOAD " << i << "\n";
                }
                last_load_occurrence[ip] = {i, address};
            }
        }
        // Handle STORE instructions (optional, if you want to track stores)
        else if (decoded_instr.op == propagator::instr::opcode::OP_STORE) {
            auto address = decoded_instr.address;
            // Assume we can get store data (simplified for this example)
            uint64_t store_data = 0; // Placeholder for actual store data
            if (last_store_occurrence.find(ip) == last_store_occurrence.end()) {
                last_store_occurrence[ip] = {i, {address, store_data}};
            } else {
                bool condition1 = true;
                // Check if address and data are the same
                if (last_store_occurrence[ip].second.first != address ||
                    last_store_occurrence[ip].second.second != store_data) {
                    condition1 = false;
                } else {
                    // Condition 1: Source registers have not been written
                    for (const auto &reg : decoded_instr.src_reg) {
                        if (last_write_to_reg.find(reg) != last_write_to_reg.end() &&
                            last_write_to_reg[reg] > last_store_occurrence[ip].first &&
                            last_write_to_reg[reg] < i) { // Ensure the write happened after last occurrence and before current
                            condition1 = false;
                            break;
                        }
                    }
                }
                if (condition1) {
                    stable_loads_file << "STORE " << i << "\n";
                }
                last_store_occurrence[ip] = {i, {address, store_data}};
            }
            // Update last store to memory
            last_store_to_mem[address] = i;
        }
        // Update last store to memory for LOADs (for condition 2)
        if (decoded_instr.op == propagator::instr::opcode::OP_STORE) {
            last_store_to_mem[decoded_instr.address] = i;
        }
    }
    stable_loads_file.close();
    std::string base_name = std::filesystem::path(trace_file_path).stem().string();
    std::cout << "Trace: " << base_name << "\n";
    std::cout << "Profiled Count: " << actual_simulated_instr_count << "\n";
}
