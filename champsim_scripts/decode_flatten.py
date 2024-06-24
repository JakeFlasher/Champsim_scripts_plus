import sys
import pandas as pd
import champsim_trace_reader as ctr

def flatten_dict(d, prefix=''):
    """Flatten the dictionary by expanding array values."""
    flat_dict = {}
    for key, value in d.items():
        if isinstance(value, list):
            for i, val in enumerate(value):
                flat_dict[f"{prefix}{key}_{i}"] = val
        else:
            flat_dict[f"{prefix}{key}"] = value
    return flat_dict

def parse_trace_file(input_filename, output_filename, max_instructions=100_000_000):
    try:
        f = ctr.TraceReader(input_filename)
        print(f"Opening trace file {input_filename}")

        data = []
        instruction_count = 0

        while instruction_count < max_instructions:
            line = f.read()
            if not line:
                break

            # Convert the line to a dictionary and flatten it
            instr_dict = {
                'ip': line.ip,
                'is_branch': line.is_branch,
                'branch_taken': line.branch_taken,
                'destination_registers': line.destination_registers,
                'source_registers': line.source_registers,
                'destination_memory': line.destination_memory,
                'source_memory': line.source_memory
            }
            flat_instr_dict = flatten_dict(instr_dict)
            data.append(flat_instr_dict)

            instruction_count += 1

        df = pd.DataFrame(data)
        df.to_csv(output_filename, index=False)
        print(f"Trace data successfully written to {output_filename}")

    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    if len(sys.argv) not in [3, 4]:
        print("Usage: python script.py <input_tracefile> <output_csvfile> [max_instructions]")
        sys.exit(1)

    input_filename = sys.argv[1]
    output_filename = sys.argv[2]
    max_instructions = int(sys.argv[3]) if len(sys.argv) == 4 else 100_000_000

    parse_trace_file(input_filename, output_filename, max_instructions)
