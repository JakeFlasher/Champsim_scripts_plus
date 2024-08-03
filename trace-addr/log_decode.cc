#include "champsim-trace-decoder.h"
#include "propagator.h"
#include "tracereader.h"
#include <argp.h>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <iostream>
#include <lzma.h>
#include <sstream>
using namespace clueless;
void process_trace_file(const std::string &trace_file_path, const std::string &output_csv, size_t nwarmup, size_t nsimulate, size_t heartbeat);

std::string format_instruction(const propagator::instr &ins) {
  std::ostringstream oss;
  oss << ins.ip << "," << static_cast<int>(ins.op) << ",";
  for (size_t j = 0; j < ins.src_reg.size(); ++j) {
    oss << static_cast<int>(ins.src_reg[j]);
    if (j < ins.src_reg.size() - 1)
      oss << "|";
  }
  oss << ",";
  for (size_t j = 0; j < ins.dst_reg.size(); ++j) {
    oss << static_cast<int>(ins.dst_reg[j]);
    if (j < ins.dst_reg.size() - 1)
      oss << "|";
  }
  oss << ",";
  for (size_t j = 0; j < ins.mem_reg.size(); ++j) {
    oss << static_cast<int>(ins.mem_reg[j]);
    if (j < ins.mem_reg.size() - 1)
      oss << "|";
  }
  oss << "," << ins.address << "\n";
  return oss.str();
}

void flush_log_buffer(std::ofstream &csv_file, std::vector<std::string> &log_buffer) {
  for (const auto &entry : log_buffer) {
    csv_file << entry;
  }
  log_buffer.clear();
}

int main(int argc, char *argv[]) {
  if (argc != 6) {
    std::cerr << "Usage: " << argv[0] << " <trace_file> <nwarmup> <nsimulate> <heartbeat> <output_csv>\n";
    std::cerr << "  trace_file  : Path to the trace file\n";
    std::cerr << "  nwarmup     : Number of instructions to warm up\n";
    std::cerr << "  nsimulate   : Number of instructions to simulate\n";
    std::cerr << "  heartbeat   : Frequency of progress updates\n";
    std::cerr << "  output_csv  : Output CSV filename\n";
    return EXIT_FAILURE;
  }

  std::string trace_file_path = argv[1];
  size_t nwarmup = std::stoull(argv[2]);
  size_t nsimulate = std::stoull(argv[3]);
  size_t heartbeat = std::stoull(argv[4]);
  std::string output_csv = argv[5];

  process_trace_file(trace_file_path, output_csv, nwarmup, nsimulate, heartbeat);

  return EXIT_SUCCESS;
}

void process_trace_file(const std::string &trace_file_path, const std::string &output_csv, size_t nwarmup, size_t nsimulate, size_t heartbeat) {
  tracereader reader(trace_file_path.c_str());
  champsim_trace_decoder decoder;
  propagator prop;

  std::ofstream csv_file(output_csv);
  if (!csv_file.is_open()) {
    std::cerr << "Failed to open CSV file for writing." << std::endl;
    return;
  }

  csv_file << "IP,Opcode,Src_Registers,Dst_Registers,Mem_Registers,Address\n";

  // Skip warmup instructions
  for (size_t i = 0; i < nwarmup; ++i) {
    reader.read_single_instr();
  }

  std::vector<std::string> log_buffer;
  log_buffer.reserve(100000000); // Adjust based on expected log frequency

  for (size_t i = 0; i < nsimulate; ++i) {
    if (!(i % heartbeat)) {
      printf("Processed %zu instructions\n", i);
      fflush(stdout);
    }

    auto input_ins = reader.read_single_instr();
    const auto &decoded_instr = decoder.decode(input_ins);
    log_buffer.push_back(format_instruction(decoded_instr));
    if (log_buffer.size() >= 100000000) {
      flush_log_buffer(csv_file, log_buffer);
    }
  }

  flush_log_buffer(csv_file, log_buffer);
  printf("Trace file encoded to encoded_trace.xz\n");
  csv_file.close();
}


