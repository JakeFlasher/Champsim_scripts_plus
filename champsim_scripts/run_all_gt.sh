#!/bin/bash

# Base directory where the trace files are stored
TRACE_DIR="/media/jasmine/external/champs/dpc3_traces"

# Simulator command and parameters
SIMULATOR="bin/champsim"
WARMUP_INSTRUCTIONS=200000000
SIMULATION_INSTRUCTIONS_LIST=(100000000 200000000 500000000)  # Add more if needed

# Loop over each trace file
for TRACE_FILE in $(ls ${TRACE_DIR}/*/*.xz); do
  # Extract the benchmark name and trace filename
  BENCHMARK_DIR=$(dirname "${TRACE_FILE}")
  TRACE_FILENAME=$(basename "${TRACE_FILE}")

  # Loop over each simulation instruction count
  for SIMULATION_INSTRUCTIONS in "${SIMULATION_INSTRUCTIONS_LIST[@]}"; do
    # Format the simulation instruction count for folder naming (e.g., 100000000 -> 100M)
    SIMULATION_INSTRUCTIONS_FOLDER=$((${SIMULATION_INSTRUCTIONS} / 1000000))M

    # Create results directory if it doesn't exist
    RESULTS_DIR="${BENCHMARK_DIR}/${SIMULATION_INSTRUCTIONS_FOLDER}"
    mkdir -p "${RESULTS_DIR}"

    # Run the simulator command
    ${SIMULATOR} --warmup_instructions ${WARMUP_INSTRUCTIONS} --simulation_instructions ${SIMULATION_INSTRUCTIONS} "${TRACE_FILE}" > "${RESULTS_DIR}/${TRACE_FILENAME}.out"

    # Optionally, you can add more processing or logging here
  done
done
