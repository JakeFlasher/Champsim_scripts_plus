// load_trace_resultfile.cc

#include "champsim-trace-decoder.h"

#include <regex>
#include <sstream>
#include "tracereader.h"
#include "trace_encoder.h"
#include <fstream>
#include <iostream>
#include <limits>

using namespace clueless;

// Function Prototypes
void process_trace_file(const std::string &trace_file_path, const std::string &output_trace_file,
                        size_t nwarmup, size_t nsimulate, size_t heartbeat, const std::string &imputed_data_file,
                        const std::string &ipc_change_method, double ipc_change_threshold);

int main(int argc, char *argv[]) {
    if (argc != 9) {
        std::cerr << "Usage: " << argv[0] << " <trace_file> <nwarmup> <nsimulate> <heartbeat> <imputed_data_file> <output_trace_file> <ipc_change_method> <ipc_change_threshold>\n";
        return EXIT_FAILURE;
    }

    std::string trace_file_path = argv[1];
    size_t nwarmup = std::stoull(argv[2]);
    size_t nsimulate = std::stoull(argv[3]);
    size_t heartbeat = std::stoull(argv[4]);
    std::string imputed_data_file = argv[5];
    std::string output_trace_file = argv[6];
    std::string ipc_change_method = argv[7];
    double ipc_change_threshold = std::stod(argv[8]);

    process_trace_file(trace_file_path, output_trace_file, nwarmup, nsimulate, heartbeat, imputed_data_file, ipc_change_method, ipc_change_threshold);

    return EXIT_SUCCESS;
}

void process_trace_file(const std::string &trace_file_path, const std::string &output_trace_file,
                        size_t nwarmup, size_t nsimulate, size_t heartbeat, const std::string &imputed_data_file,
                        const std::string &ipc_change_method, double ipc_change_threshold) {
    tracereader reader(trace_file_path.c_str());
    champsim_trace_decoder decoder;
    trace_encoder encoder(output_trace_file.c_str());

    size_t skipped_instr = 0;
    // Open the imputed data CSV file
    std::ifstream imputed_file(imputed_data_file);
    if (!imputed_file.is_open()) {
        std::cerr << "Failed to open imputed data file: " << imputed_data_file << std::endl;
        return;
    }

    // Skip the header line in the CSV file
    std::string imputed_line;
    if (!std::getline(imputed_file, imputed_line)) {
        std::cerr << "Imputed data file is empty or missing header." << std::endl;
        return;
    }

    // Skip warmup instructions and corresponding imputed data lines
    for (size_t i = 0; i < nwarmup; ++i) {
        auto warmup_instr = reader.read_single_instr();
        encoder.write_single_instr(warmup_instr);

        // Skip corresponding line in imputed data file
        if (!std::getline(imputed_file, imputed_line)) {
            std::cerr << "Reached end of imputed data file before end of warmup." << std::endl;
            return;
        }
    }

    double prev_ipc_value = 0.0;
    bool first_line = true;

    for (size_t i = 0; i < nsimulate; ++i) {
        if (!(i % heartbeat)) {
            printf("Processed %zu instructions\n", i);
            fflush(stdout);
        }

        auto input_ins = reader.read_single_instr();

        // Read corresponding line from imputed_data.csv
        if (!std::getline(imputed_file, imputed_line)) {
            std::cerr << "Reached end of imputed data file during simulation instructions." << std::endl;
            return;
        }

        // Parse CSV line to get instruction_index and ipc_value
        std::istringstream iss(imputed_line);
        std::string token;
        size_t instruction_idx;
        double ipc_value;

        // Parse instruction_index
        std::getline(iss, token, ','); // instruction_index
        instruction_idx = std::stoull(token);

        // Skip other fields (ip, address, opcode, reuse_distance)
        for (int j = 0; j < 4; ++j) {
            std::getline(iss, token, ',');
        }

        // Parse ipc value
        std::getline(iss, token); // ipc
        if (token == "NaN" || token.empty()) {
            ipc_value = 0.0; // Or handle as per requirements
        } else {
            ipc_value = std::stod(token);
        }

        // Check instruction index alignment
        size_t expected_idx = i + nwarmup;
        if (instruction_idx != expected_idx) {
            std::cerr << "Instruction index mismatch at instruction " << i << ": expected " << expected_idx << ", got " << instruction_idx << std::endl;
            return;
        }

        // Calculate IPC change
        double ipc_change = 0.0;
        if (first_line) {
            // For the first instruction, we cannot compute IPC change
            first_line = false;
            encoder.write_single_instr(input_ins);
            prev_ipc_value = ipc_value;
            continue;
        } else {
            if (ipc_change_method == "difference") {
                ipc_change = std::abs(ipc_value - prev_ipc_value);
            } else if (ipc_change_method == "percentage") {
                if (prev_ipc_value != 0.0) {
                    ipc_change = std::abs((ipc_value - prev_ipc_value) / prev_ipc_value);
                } else {
                    ipc_change = std::numeric_limits<double>::infinity();
                }
            } else {
                std::cerr << "Invalid ipc_change_method: " << ipc_change_method << std::endl;
                return;
            }
        }

        // Decide whether to skip the instruction
        if (ipc_change < ipc_change_threshold) {
            // Skip this instruction
            // For debug purposes, you might print skipped instructions
            // printf("Skipping instruction %zu with ipc_change %.6f\n", instruction_idx, ipc_change);
            skipped_instr++;
            continue;
        } else {
            // Keep this instruction
            encoder.write_single_instr(input_ins);
        }

        // Update previous IPC value
        prev_ipc_value = ipc_value;
    }
    
    printf("Skipping %ld instructions. Trace file encoded to %s\n", skipped_instr, output_trace_file.c_str());
}
