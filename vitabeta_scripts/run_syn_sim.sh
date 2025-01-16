#!/bin/bash

# Base directory where the trace files are stored
TRACE_DIR="/root/developing/champsim_traces_dev/gap/IPC_single_instr/500M/syn_trace_reversed_resultfile"

# baseline: 479M(on ramulator version, need to redo for berti), 100M, 200M
# Simulator command and parameters
# SIMULATOR="/root/GST_champsim/ChampDRAM/bin/final_curr"
# SIMULATOR="/root/GST_champsim/champdram/bin/bigger"
SIMULATOR="/root/developing/rowhammerRam/bin/champsim_1M_interval"
WARMUP_INSTRUCTIONS=20000000
SIMULATION_INSTRUCTIONS_LIST=$1  # Add more if needed
MAX_JOBS=20  # Maximum number of parallel jobs

# Function to run the simulator
run_simulator() {
  local BENCHMARK_DIR=$1
  local TRACE_FILE=$2
  local SIMULATION_INSTRUCTIONS=$3
  local TRACE_FILENAME=$(basename "${TRACE_FILE}")

  # Format the simulation instruction count for folder naming (e.g., 100000000 -> 100M)
  local SIMULATION_INSTRUCTIONS_FOLDER=golden_ref/$((${SIMULATION_INSTRUCTIONS} / 1000000))M

  # Create results directory if it doesn't exist
  local RESULTS_DIR="${BENCHMARK_DIR}/${SIMULATION_INSTRUCTIONS_FOLDER}"
  mkdir -p "${RESULTS_DIR}"

  # Run the simulator command
  ${SIMULATOR} -w 0  "${TRACE_FILE}" > "${RESULTS_DIR}/${TRACE_FILENAME}.out"
}

# Export the function and variables for parallel execution
export -f run_simulator
export SIMULATOR WARMUP_INSTRUCTIONS SIMULATION_INSTRUCTIONS_LIST MAX_JOBS

# Function to manage the job queue
function add_job {
  # Add a job to the background and keep track of it
  "$@" &
  # Store the PID of the background job
  local job=$!
  JOBS+=($job)
  
  # If the number of jobs has reached MAX_JOBS, wait for the oldest one
  if [[ ${#JOBS[@]} -ge $MAX_JOBS ]]; then
    wait "${JOBS[0]}"
    # Remove the job from the list
    JOBS=("${JOBS[@]:1}")
  fi
}

# Initialize an empty array to keep track of jobs
JOBS=()

# Loop over each trace file
for TRACE_FILE in $(ls ${TRACE_DIR}/*.xz); do
  # Extract the benchmark name and trace filename
  BENCHMARK_DIR=$(dirname "${TRACE_FILE}")

  # Loop over each simulation instruction count and add them to the job queue
  for SIMULATION_INSTRUCTIONS in "${SIMULATION_INSTRUCTIONS_LIST[@]}"; do
    add_job run_simulator "${BENCHMARK_DIR}" "${TRACE_FILE}" "${SIMULATION_INSTRUCTIONS}"
  done
done

# Wait for all remaining jobs to complete
wait

