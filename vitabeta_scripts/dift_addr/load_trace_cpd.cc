// load_trace_cpd.cc

#include "champsim-trace-decoder.h"

#include <regex>
#include <sstream>
#include "tracereader.h"
#include "trace_encoder.h"
#include <fstream>
#include <iostream>
#include <limits>
#include <set>
#include <algorithm>
#include <vector>
#include <cmath>

using namespace clueless;

// Function Prototypes
void process_trace_file(const std::string &trace_file_path, const std::string &output_trace_file,
                        size_t nwarmup, size_t nsimulate, size_t heartbeat,
                        const std::string &change_point_file, double reduction_rate, double tolerance);

struct InstructionRange {
    size_t start;
    size_t end;
};

// Function to read change points from the log file
std::vector<size_t> load_change_points(const std::string &change_point_file);

void calculate_adaptive_windows(const std::vector<size_t> &change_points, size_t total_instructions,
                                double reduction_rate, double tolerance,
                                std::vector<InstructionRange> &preserved_ranges);

int main(int argc, char *argv[]) {
    if (argc != 8) {
        std::cerr << "Usage: " << argv[0] << " <trace_file> <nwarmup> <nsimulate> <heartbeat> <output_trace_file> <change_point_file> <reduction_rate>\n";
        return EXIT_FAILURE;
    }

    std::string trace_file_path = argv[1];
    size_t nwarmup = std::stoull(argv[2]);
    size_t nsimulate = std::stoull(argv[3]);
    size_t heartbeat = std::stoull(argv[4]);
    std::string output_trace_file = argv[5];
    std::string change_point_file = argv[6];
    double reduction_rate = std::stod(argv[7]);  // e.g., 0.1 for 10% reduction
    double tolerance = 0.05; // Acceptable tolerance (e.g., ±5%)

    process_trace_file(trace_file_path, output_trace_file, nwarmup, nsimulate, heartbeat,
                       change_point_file, reduction_rate, tolerance);

    return EXIT_SUCCESS;
}

void process_trace_file(const std::string &trace_file_path, const std::string &output_trace_file,
                        size_t nwarmup, size_t nsimulate, size_t heartbeat,
                        const std::string &change_point_file, double reduction_rate, double tolerance) {
    tracereader reader(trace_file_path.c_str());
    champsim_trace_decoder decoder;
    trace_encoder encoder(output_trace_file.c_str());

    size_t total_instructions = nwarmup + nsimulate;
    size_t instructions_reduced = 0;

    // Load change points
    std::vector<size_t> change_points = load_change_points(change_point_file);

    // Calculate adaptive windows
    std::vector<InstructionRange> preserved_ranges;
    calculate_adaptive_windows(change_points, total_instructions, reduction_rate, tolerance, preserved_ranges);

    // Create a set of preserved instruction indices for fast lookup
    std::set<size_t> preserved_indices;
    for (const auto &range : preserved_ranges) {
        for (size_t i = range.start; i <= range.end; ++i) {
            preserved_indices.insert(i);
        }
    }

    // Process warmup instructions
    for (size_t i = 0; i < nwarmup; ++i) {
        auto warmup_instr = reader.read_single_instr();
        encoder.write_single_instr(warmup_instr);
    }

    // Process simulation instructions
    for (size_t i = 0; i < nsimulate; ++i) {
        if (!(i % heartbeat)) {
            printf("Processed %zu instructions\n", i);
            fflush(stdout);
        }

        auto input_ins = reader.read_single_instr();

        // Determine global instruction index
        size_t instruction_idx = i + nwarmup;

        if (preserved_indices.find(instruction_idx) != preserved_indices.end()) {
            // Keep this instruction
            encoder.write_single_instr(input_ins);
        } else {
            // Skip this instruction
            instructions_reduced++;
        }
    }

    double actual_reduction_rate = static_cast<double>(instructions_reduced) / nsimulate;

    printf("Total instructions reduced: %zu (%.2f%% reduction).\n", instructions_reduced, actual_reduction_rate * 100);
    printf("Trace file encoded to %s\n", output_trace_file.c_str());
}

std::vector<size_t> load_change_points(const std::string &change_point_file) {
    std::vector<size_t> change_points;
    std::ifstream infile(change_point_file);
    size_t cp;
    while (infile >> cp) {
        change_points.push_back(cp);
    }
    infile.close();
    return change_points;
}

void calculate_adaptive_windows(const std::vector<size_t> &change_points, size_t total_instructions,
                                double reduction_rate, double tolerance,
                                std::vector<InstructionRange> &preserved_ranges) {
    size_t desired_preservation = static_cast<size_t>(total_instructions * (1 - reduction_rate));
    size_t min_window_size = 0;
    size_t max_window_size = total_instructions;

    size_t iteration = 0;
    size_t max_iterations = 1000;

    while (min_window_size <= max_window_size && iteration < max_iterations) {
        size_t window_size = (min_window_size + max_window_size) / 2;

        // Create ranges
        std::vector<InstructionRange> ranges;
        for (size_t cp : change_points) {
            size_t half_window = window_size / 2;
            size_t start = (cp >= half_window) ? cp - half_window : 0;
            size_t end = std::min(cp + half_window - 1, total_instructions - 1);
            ranges.push_back({start, end});
        }

        // Merge overlapping ranges
        std::sort(ranges.begin(), ranges.end(),
                  [](const InstructionRange &a, const InstructionRange &b) {
                      return a.start < b.start;
                  });

        std::vector<InstructionRange> merged_ranges;
        if (!ranges.empty()) {
            merged_ranges.push_back(ranges[0]);
            for (size_t i = 1; i < ranges.size(); ++i) {
                InstructionRange &last = merged_ranges.back();
                if (ranges[i].start <= last.end + 1) {
                    last.end = std::max(last.end, ranges[i].end);
                } else {
                    merged_ranges.push_back(ranges[i]);
                }
            }
        }

        // Calculate total preserved instructions
        size_t total_preserved = 0;
        for (const auto &range : merged_ranges) {
            total_preserved += (range.end - range.start + 1);
        }

        double lower_bound = desired_preservation * (1 - tolerance);
        double upper_bound = desired_preservation * (1 + tolerance);

        if (total_preserved >= lower_bound && total_preserved <= upper_bound) {
            // Within tolerance
            preserved_ranges = merged_ranges;
            return;
        } else if (total_preserved < lower_bound) {
            // Preserved too few instructions, need to increase window size
            min_window_size = window_size + 1;
        } else {
            // Preserved too many instructions, need to decrease window size
            if (window_size == 0) {
                // Cannot decrease window size further
                break;
            }
            max_window_size = window_size - 1;
        }
        iteration++;
    }

    // If we couldn't find a suitable window size within the maximum iterations,
    // select the window size that gives the closest total preserved instructions.
    // This can be either min_window_size or max_window_size.
    size_t final_window_size = (min_window_size + max_window_size) / 2;

    // Recreate the ranges with the final window size
    std::vector<InstructionRange> ranges;
    for (size_t cp : change_points) {
        size_t half_window = final_window_size / 2;
        size_t start = (cp >= half_window) ? cp - half_window : 0;
        size_t end = std::min(cp + half_window - 1, total_instructions - 1);
        ranges.push_back({start, end});
    }

    // Merge overlapping ranges
    std::sort(ranges.begin(), ranges.end(),
              [](const InstructionRange &a, const InstructionRange &b) {
                  return a.start < b.start;
              });

    preserved_ranges.clear();
    if (!ranges.empty()) {
        preserved_ranges.push_back(ranges[0]);
        for (size_t i = 1; i < ranges.size(); ++i) {
            InstructionRange &last = preserved_ranges.back();
            if (ranges[i].start <= last.end + 1) {
                last.end = std::max(last.end, ranges[i].end);
            } else {
                preserved_ranges.push_back(ranges[i]);
            }
        }
    }
}
