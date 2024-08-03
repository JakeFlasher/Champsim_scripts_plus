#include "champsim-trace-decoder.h"
#include "propagator.h"
#include "tracereader.h"
#include "trace_encoder.h"
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
#include <lzma.h>
#include <sstream>
using namespace clueless;

void process_trace_file(const std::string &trace_file_path, const std::string &output_trace_file, size_t nwarmup, size_t nsimulate, size_t heartbeat);

int main(int argc, char *argv[]) {
  if (argc != 6) {
    std::cerr << "Usage: " << argv[0] << " <trace_file> <nwarmup> <nsimulate> <heartbeat> <output_trace_file>\n";
    std::cerr << "  trace_file       : Path to the trace file\n";
    std::cerr << "  nwarmup          : Number of instructions to warm up\n";
    std::cerr << "  nsimulate        : Number of instructions to simulate\n";
    std::cerr << "  heartbeat        : Frequency of progress updates\n";
    std::cerr << "  output_trace_file: Output trace filename\n";
    return EXIT_FAILURE;
  }

  std::string trace_file_path = argv[1];
  size_t nwarmup = std::stoull(argv[2]);
  size_t nsimulate = std::stoull(argv[3]);
  size_t heartbeat = std::stoull(argv[4]);
  std::string output_trace_file = argv[5];

  process_trace_file(trace_file_path, output_trace_file, nwarmup, nsimulate, heartbeat);

  return EXIT_SUCCESS;
}

void process_trace_file(const std::string &trace_file_path, const std::string &output_trace_file, size_t nwarmup, size_t nsimulate, size_t heartbeat) {
  tracereader reader(trace_file_path.c_str());
  champsim_trace_decoder decoder;
  propagator prop;
  trace_encoder encoder(output_trace_file.c_str());
  std::cout << nwarmup << std::endl;

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
    encoder.write_single_instr(input_ins);
  }

  printf("Trace file encoded to %s\n", output_trace_file.c_str());
}
