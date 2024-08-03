#ifndef TRACE_ENCODER_H
#define TRACE_ENCODER_H
#include "trace-instruction.h"
#include <cstdio>
#include <string>

namespace clueless
{
class trace_encoder
{
public:
  explicit trace_encoder (const char *output_trace_string);
  ~trace_encoder ();
  void write_single_instr (const input_instr &instr);

private:
  void open (std::string trace_string);
  void close ();
  FILE *trace_file = NULL;
  std::string trace_string;
  std::string cmd_fmtstr;
  std::string comp_program;
};
}
#endif
