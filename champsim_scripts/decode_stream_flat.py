import sys
import csv
import champsim_trace_reader as ctr

def format_array(arr):
    """Format the array as a space-separated string of index:value pairs, excluding zeros."""
    return ' '.join(f"{i}:{val}" for i, val in enumerate(arr) if val != 0)

def parse_trace_file(input_filename, output_filename, max_instructions=100_000_000):
    try:
        f = ctr.TraceReader(input_filename)
        print(f"Opening trace file {input_filename}")

        with open(output_filename, mode='w', newline='') as csv_file:
            fieldnames = ['ip', 'is_branch', 'branch_taken', 'destination_registers', 'source_registers', 'destination_memory', 'source_memory']
            writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
            writer.writeheader()

            instruction_count = 0

            while instruction_count < max_instructions:
                line = f.read()
                if not line:
                    break

                writer.writerow({
                    'ip': line.ip,
                    'is_branch': line.is_branch,
                    'branch_taken': line.branch_taken,
                    'destination_registers': format_array(line.destination_registers),
                    'source_registers': format_array(line.source_registers),
                    'destination_memory': format_array(line.destination_memory),
                    'source_memory': format_array(line.source_memory)
                })

                instruction_count += 1

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
