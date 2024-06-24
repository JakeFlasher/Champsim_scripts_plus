import sys
import csv
import lzma
import struct

def parse_array_dest(arr_string):
    """Parse the space-separated index:value string into a list."""
    arr = [0] * 2  # Assuming the arrays have a fixed length of 2
    if arr_string:
        for item in arr_string.split():
            index, value = item.split(':')
            arr[int(index)] = int(value)
    return arr

def parse_array(arr_string):
    """Parse the space-separated index:value string into a list."""
    arr = [0] * 4  # Assuming the arrays have a fixed length of 4
    if arr_string:
        for item in arr_string.split():
            index, value = item.split(':')
            arr[int(index)] = int(value)
    return arr

def write_trace_file(input_filename, output_filename):
    try:
        with open(input_filename, mode='r') as csv_file, lzma.open(output_filename, 'wb') as xz_file:
            reader = csv.DictReader(csv_file)

            for row in reader:
                ip = int(row['ip'])
                is_branch = int(row['is_branch'])
                branch_taken = int(row['branch_taken'])
                destination_registers = parse_array_dest(row['destination_registers'])
                source_registers = parse_array(row['source_registers'])
                destination_memory = parse_array_dest(row['destination_memory'])
                source_memory = parse_array(row['source_memory'])

                # Create the binary structure to be written to the trace file
                packed_data = (
                    struct.pack('Q', ip) +  # ip (8 bytes, unsigned long long)
                    struct.pack('B', is_branch) +  # is_branch (1 byte, unsigned char)
                    struct.pack('B', branch_taken) +  # branch_taken (1 byte, unsigned char)
                    struct.pack('2B', *destination_registers) +  # destination_registers (4 * 4 bytes, unsigned int)
                    struct.pack('4B', *source_registers) +  # source_registers (4 * 4 bytes, unsigned int)
                    struct.pack('2Q', *destination_memory) +  # destination_memory (2 * 8 bytes, unsigned long long)
                    struct.pack('4Q', *source_memory)  # source_memory (4 * 8 bytes, unsigned long long)
                )

                xz_file.write(packed_data)

        print(f"CSV data successfully written to {output_filename}")

    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python encoder_script.py <input_csvfile> <output_tracefile>")
        sys.exit(1)

    input_filename = sys.argv[1]
    output_filename = sys.argv[2]

    write_trace_file(input_filename, output_filename)
