#include "trace_encoder.h"
#include <cassert>
#include <iostream>
namespace clueless
{
trace_encoder::trace_encoder (const char *_ts) : trace_string (_ts)
{
  cmd_fmtstr = "%1$s -T0 -c > %2$s";
  comp_program = "xz";
  open (trace_string);
}
trace_encoder::~trace_encoder ()
{
  close ();
}
void
trace_encoder::write_single_instr (const input_instr &instr)
{
  fwrite (&instr, sizeof (instr), 1, trace_file);
}
void
trace_encoder::open (std::string trace_string)
{
  char compress_command[4096];
  sprintf (compress_command, cmd_fmtstr.c_str (), comp_program.c_str (),
           trace_string.c_str ());
  trace_file = popen (compress_command, "w");
  if (trace_file == NULL)
    {
      std::cerr << std::endl
                << "*** CANNOT OPEN TRACE FILE FOR WRITING: " << trace_string
                << " ***" << std::endl;
      assert (0);
    }
}
void
trace_encoder::close ()
{
  if (trace_file != NULL)
    {
      pclose (trace_file);
    }
}
}

