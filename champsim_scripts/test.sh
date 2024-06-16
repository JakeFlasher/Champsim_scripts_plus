#!/bin/bash

# Create a directory to store all the traces
base_dir="$PWD/dpc3_traces"
mkdir -p "$base_dir"

# URL base for downloading the traces
base_url="https://dpc3.compas.cs.stonybrook.edu/champsim-traces/speccpu"

# Read the content of index.html into an array
mapfile -t trace_files < <(curl -s https://dpc3.compas.cs.stonybrook.edu/champsim-traces/speccpu/ | grep -oP '(\d+\.\w+-\d+\w*\.champsimtrace\.xz)')

# Loop through each trace file
for trace in "${trace_files[@]}"; do
    # Extract the benchmark name (e.g., 605.mcf) from the trace file name
    bench_name=$(echo "$trace" | grep -oP '^\d+\.\w+')

    # Create a directory for the benchmark if it doesn't exist
    bench_dir="$base_dir/$bench_name"
    mkdir -p "$bench_dir"

    # Download the trace file into the corresponding benchmark directory
    wget -P "$bench_dir" -c "$base_url/$trace"
done
