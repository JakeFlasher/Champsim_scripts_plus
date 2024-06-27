#!/bin/bash

# Path to the simpoint binary
SIMPOINT_BIN="/root/developing/qemu/qemu_plugins_champsim_scripts/Simpoint.3.2_fix/bin/simpoint"
# Maximum number of clusters
MAX_K=5

# Find all .bbv files and loop over them
for bbv_file in $(ls benchspec/CPU/*/run/*/result/bbv); do
    # Extract the directory and base name of the bbv file
    output_dir=$(dirname "$bbv_file")
    base_name=$(basename "$bbv_file" .bbv)

    # Define the output file paths
    simpoints_file="${output_dir}/${base_name}.simpoints"
    weights_file="${output_dir}/${base_name}.weights"

    # Run the simpoint command
    "$SIMPOINT_BIN" -maxK "$MAX_K" -loadFVFile "$bbv_file" -saveSimpoints "$simpoints_file" -saveSimpointWeights "$weights_file" >/dev/null

    echo "Processed $bbv_file, saved simpoints to $simpoints_file and weights to $weights_file"
done
