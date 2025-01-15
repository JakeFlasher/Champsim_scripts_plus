#!/bin/bash
# /load_trace <trace_file> <nwarmup> <nsimulate> <heartbeat> <stable_load_file> <output_trace_file>

# Base directory where the trace files are stored
TRACE_DIR="/root/champsim_traces/spec17"
# Simulator command and parameters
SIMULATOR="/root/champsim_traces/dift-addr_btaken/torchdata_last_value_ipc"
WARMUP_INSTRUCTIONS=0
SIMULATION_INSTRUCTIONS_LIST=$1  # Add more if needed
TARGET_FOLDER=$2
MAX_JOBS=30  # Maximum number of parallel jobs

# Function to run the simulator
run_simulator() {
  local BENCHMARK_DIR=$1
  local TRACE_FILE=$2
  local SIMULATION_INSTRUCTIONS=$3
  local TRACE_FILENAME=$(basename "${TRACE_FILE}")

  # Format the simulation instruction count for folder naming (e.g., 100000000 -> 100M)

  APP_NAME="${TRACE_FILENAME}"
  # Create results directory if it doesn't exist
  local DAT_DIR="${BENCHMARK_DIR}/${DAT_FOLDER}"
  echo ${DAT_DIR}
  local RESULTS_DIR="${BENCHMARK_DIR}/${SIMULATION_INSTRUCTIONS_FOLDER}"
  mkdir -p "${RESULTS_DIR}"

 ${SIMULATOR} "${TRACE_FILE}" ${SIMULATION_INSTRUCTIONS}  "${TARGET_FOLDER}/${APP_NAME}.csv" "./lvp_ipc_values/${APP_NAME}.csv"  
  # echo "${SIMULATOR} "${TRACE_FILE}" ${WARMUP_INSTRUCTIONS}  ${SIMULATION_INSTRUCTIONS} 100000000 "${DAT_DIR}/${TRACE_FILENAME}.syn.xz" "/root/imputation/pypots/gap/${APP_NAME}/change_points.log" 0.25" 
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

