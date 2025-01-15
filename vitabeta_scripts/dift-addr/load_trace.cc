#include "champsim-trace-decoder.h"
#include <sstream>
#include "tracereader.h"
#include "trace_encoder.h"
#include <unordered_set>
#include <fstream>
#include <iostream>

using namespace clueless;

void process_trace_file(const std::string &trace_file_path, const std::string &output_trace_file,
                        size_t nwarmup, size_t nsimulate, size_t heartbeat, const std::string &stable_load_file);

std::unordered_set<size_t> load_stable_instructions(const std::string &stable_load_file);

int main(int argc, char *argv[]) {
    if (argc != 7) {
        std::cerr << "Usage: " << argv[0] << " <trace_file> <nwarmup> <nsimulate> <heartbeat> <stable_load_file> <output_trace_file>\n";
        return EXIT_FAILURE;
    }

    std::string trace_file_path = argv[1];
    size_t nwarmup = std::stoull(argv[2]);
    size_t nsimulate = std::stoull(argv[3]);
    size_t heartbeat = std::stoull(argv[4]);
    std::string stable_load_file = argv[5];
    std::string output_trace_file = argv[6];

    process_trace_file(trace_file_path, output_trace_file, nwarmup, nsimulate, heartbeat, stable_load_file);

    return EXIT_SUCCESS;
}

void process_trace_file(const std::string &trace_file_path, const std::string &output_trace_file,
                        size_t nwarmup, size_t nsimulate, size_t heartbeat, const std::string &stable_load_file) {
    std::unordered_set<size_t> stable_instructions = load_stable_instructions(stable_load_file);
    tracereader reader(trace_file_path.c_str());
    champsim_trace_decoder decoder;
    trace_encoder encoder(output_trace_file.c_str());

    // Skip warmup instructions
    for (size_t i = 0; i < nwarmup; ++i) {
        auto warmup_instr = reader.read_single_instr();
        encoder.write_single_instr(warmup_instr);
    }

    for (size_t i = 0; i < nsimulate; ++i) {
        if (!(i % heartbeat)) {
            printf("Processed %zu instructions\n", i);
            fflush(stdout);
        }

        auto input_ins = reader.read_single_instr();

        // Skip logging if this instruction is a global stable instruction
        if (stable_instructions.find(i) == stable_instructions.end()) {
            encoder.write_single_instr(input_ins);
        }
    }

    printf("Trace file encoded to %s\n", output_trace_file.c_str());
}

std::unordered_set<size_t> load_stable_instructions(const std::string &stable_load_file) {
    std::unordered_set<size_t> stable_instructions;
    std::ifstream file(stable_load_file);

    if (!file.is_open()) {
        std::cerr << "Failed to open stable instructions file." << std::endl;
        return stable_instructions;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string instr_type;
        size_t index;
        if (iss >> instr_type >> index) {
            stable_instructions.insert(index);
        }
    }

    return stable_instructions;
}
