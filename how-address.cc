#include "trace_encoder.h"
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
#include <lzma.h>

const char *argp_program_version = "how-address 0.1.0";
const char *argp_program_bug_address = "<xchen@vvvu.org>";

static char doc[] = "How memory addresses are made";

static char args_doc[] = "TRACE";

using namespace clueless;

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <fstream>
#include "trace_encoder.h"
#include "champsim-trace-decoder.h"
#include "propagator.h"
#include "tracereader.h"

class reuse_distance
{
public:
  void process_trace_file(const std::string &trace_file_path, size_t nwarmup, size_t nsimulate, size_t heartbeat)
  {
    tracereader reader(trace_file_path.c_str());
    champsim_trace_decoder decoder;
    propagator prop;
    trace_encoder encoder("encoded.trace.xz");

    std::ofstream output_file("encoded.trace.xz", std::ios::binary);
    if (!output_file.is_open()) {
      std::cerr << "Failed to open output file for writing." << std::endl;
      return;
    }

    // Open CSV file for logging global-stable LOADs
    std::string csv_file_name = trace_file_path + "_stable.LOADS.csv";
    std::ofstream csv_file(csv_file_name);
    if (!csv_file.is_open()) {
      std::cerr << "Failed to open CSV file for writing." << std::endl;
      return;
    }
    // Write CSV header
    csv_file << "IP,Opcode,Src_Registers,Dst_Registers,Mem_Registers,Address\n";

    // Skip warmup instructions
    for (size_t i = 0; i < nwarmup; ++i) {
      reader.read_single_instr();
    }

    // Dictionaries to track last occurrences
    std::unordered_map<uint64_t, size_t> last_occurrence;
    std::unordered_map<unsigned, size_t> last_write_to_reg;
    std::unordered_map<uint64_t, size_t> last_store_to_mem;

    // Process simulation instructions
    for (size_t i = 0; i < nsimulate; ++i) {
      if (!(i % heartbeat)) {
        printf("Processed %zu instructions\n", i);
        fflush(stdout);
      }

      auto input_ins = reader.read_single_instr();
      const auto &decoded_instr = decoder.decode(input_ins);

      // If it's a LOAD instruction
      if (decoded_instr.op == propagator::instr::opcode::OP_LOAD) {
        auto address = decoded_instr.address;

        if (last_occurrence.find(address) != last_occurrence.end()) {
          bool condition1 = true;
          bool condition2 = true;

          // Check Condition 1
          for (const auto &reg : decoded_instr.src_reg) {
            if (last_write_to_reg.find(reg) != last_write_to_reg.end() &&
                last_write_to_reg[reg] > last_occurrence[address]) {
              condition1 = false;
              break;
            }
          }

          // Check Condition 2
          if (last_store_to_mem.find(address) != last_store_to_mem.end() &&
              last_store_to_mem[address] > last_occurrence[address]) {
            condition2 = false;
          }

          // If both conditions are satisfied, log the instruction
          if (condition1 && condition2) {
            log_instruction(csv_file, decoded_instr);
          }
        }

        // Update last occurrence of the address
        last_occurrence[address] = i;
      }

      // If it's a STORE instruction, update last store to memory
      if (decoded_instr.op == propagator::instr::opcode::OP_STORE) {
        last_store_to_mem[decoded_instr.address] = i;
      }

      // Update last write to registers for all destination registers
      for (const auto &reg : decoded_instr.dst_reg) {
        last_write_to_reg[reg] = i;
      }

      // Write the instruction to the encoded file
      // encoder.write_single_instr(input_ins);
    }

    printf("Trace file encoded to encoded_trace.xz\n");
    csv_file.close();
  }

private:
  void log_instruction(std::ofstream &csv_file, const propagator::instr &ins)
  {
    csv_file << ins.ip << "," << static_cast<int>(ins.op) << ",";
    for (size_t j = 0; j < ins.src_reg.size(); ++j) {
      csv_file << static_cast<int>(ins.src_reg[j]);
      if (j < ins.src_reg.size() - 1)
        csv_file << "|";
    }
    csv_file << ",";
    for (size_t j = 0; j < ins.dst_reg.size(); ++j) {
      csv_file << static_cast<int>(ins.dst_reg[j]);
      if (j < ins.dst_reg.size() - 1)
        csv_file << "|";
    }
    csv_file << ",";
    for (size_t j = 0; j < ins.mem_reg.size(); ++j) {
      csv_file << static_cast<int>(ins.mem_reg[j]);
      if (j < ins.mem_reg.size() - 1)
        csv_file << "|";
    }
    csv_file << "," << ins.address << "\n";
  }
};

const struct argp_option option[] = {
    { "warmup", 'w', "N", 0, "Skip the first N instructions" },
    { "simulate", 's', "N", 0, "Simulate N instructions" },
    { "heartbeat", 'b', "N", 0, "Print heartbeat every N instructions" },
    { 0 }
};


struct knobs {
  size_t nwarmup = 0;
  size_t nsimulate = 10000000;
  size_t heartbeat = 100000;
  const char *trace_file = nullptr;
};

static error_t parse_opt (int key, char *arg, struct argp_state *state) {
  auto knbs = (knobs *)state->input;

  switch (key) {
    case 'w':
      knbs->nwarmup = atoll(arg);
      break;

    case 's':
      knbs->nsimulate = atoll(arg);
      break;

    case 'b':
      knbs->heartbeat = atoll(arg);
      break;

    case ARGP_KEY_ARG:
      if (state->arg_num >= 1)
        argp_usage(state);

      knbs->trace_file = arg;
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

static struct argp argp = { option, parse_opt, args_doc, doc };

// Function to handle lzma encoding
bool write_to_lzma(std::ofstream &output_file, const void *data, size_t size) {
  lzma_stream strm = LZMA_STREAM_INIT;
  lzma_ret ret = lzma_easy_encoder(&strm, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
  
  if (ret != LZMA_OK) {
    std::cerr << "Failed to initialize LZMA encoder." << std::endl;
    return false;
  }

  strm.next_in = reinterpret_cast<const uint8_t*>(data);
  strm.avail_in = size;

  uint8_t outbuf[BUFSIZ];
  strm.next_out = outbuf;
  strm.avail_out = sizeof(outbuf);

  ret = lzma_code(&strm, LZMA_FINISH);
  if (ret != LZMA_STREAM_END && ret != LZMA_OK) {
    std::cerr << "LZMA encoding failed." << std::endl;
    lzma_end(&strm);
    return false;
  }

  output_file.write(reinterpret_cast<char*>(outbuf), sizeof(outbuf) - strm.avail_out);
  lzma_end(&strm);

  return true;
}

int main (int argc, char *argv[]) {
  auto knbs = knobs{};
  argp_parse(&argp, argc, argv, 0, 0, &knbs);

  reuse_distance rd;
  rd.process_trace_file(knbs.trace_file, knbs.nwarmup, knbs.nsimulate, knbs.heartbeat);

  return 0;
}
