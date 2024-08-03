/*
 * clueless --- Characterises vaLUEs Leaking as addrESSes
 * Copyright (C) 2023  Xiaoyue Chen
 *
 * This file is part of clueless.
 *
 * clueless is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * clueless is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with clueless.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "tracereader.h"
#include "propagator.h"
#include "champsim-trace-decoder.h"
#include "trace-instruction.h"

#include <iostream>

namespace clueless
{

class reuse_distance
{
public:
  void process_trace_file(const std::string &trace_file_path)
  {
    tracereader reader(trace_file_path.c_str());
    champsim_trace_decoder decoder;
    propagator prop;

    while (true)
    {
      input_instr input = reader.read_single_instr();
      const auto &decoded_instr = decoder.decode(input);
      // prop.propagate(decoded_instr);
      
      // Log decoded and propagated instruction
      log_instruction(decoded_instr);
    }
  }

private:
  void log_instruction(const propagator::instr &ins)
  {
    std::cout << "Instruction IP: " << ins.ip << ", Opcode: " << static_cast<int>(ins.op) << "\n";
    std::cout << "Source Registers: ";
    for (const auto &reg : ins.src_reg)
    {
      std::cout << static_cast<int>(reg) << " ";
    }
    std::cout << "\nDestination Registers: ";
    for (const auto &reg : ins.dst_reg)
    {
      std::cout << static_cast<int>(reg) << " ";
    }
    std::cout << "\nMemory Registers: ";
    for (const auto &reg : ins.mem_reg)
    {
      std::cout << static_cast<int>(reg) << " ";
    }
    std::cout << "\nAddress: " << ins.address << "\n\n";
  }
};

}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " <trace_file_path>\n";
    return 1;
  }

  clueless::reuse_distance rd;
  rd.process_trace_file(argv[1]);

  return 0;
}
