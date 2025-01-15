#!/bin/bash

# Input arguments
MODEL_TRACE_DIR=$1            # Directory containing model traces
REFERENCE_TRACE_DIR=$2        # Directory containing reference traces
SIMULATION_INSTRUCTIONS=$3    # Number of simulation instructions (e.g., 1000000 for 1M)
OUTPUT_DIR=$4                 # Directory to store simulation outputs
# SIMULATOR="/root/developing/rowhammerRam_dev/bin/champsim"    # Update this path to your simulator executable
SIMULATOR="/root/developing/rowhammerRam/bin/champsim_1M_interval"
# SIMULATOR="/root/developing/rowhammerRam_dev/bin/champsim_berti"
# SIMULATOR="/root/GST_champsim/ChampDRAM/bin/final_curr"
# SIMULATOR="/root/GST_champsim/rowhammer_champ/bin/ramulator_syn"
WARMUP_INSTRUCTIONS=0
MAX_JOBS=40  # Maximum number of parallel jobs

# Function to run the simulator
run_simulator() {
  local TRACE_FILE=$1
  local OUTPUT_FILE=$2
  local SIMULATION_INSTRUCTIONS=$3

  # Run the simulator command
  ${SIMULATOR} -w ${WARMUP_INSTRUCTIONS} -i ${SIMULATION_INSTRUCTIONS} "${TRACE_FILE}" > "${OUTPUT_FILE}"
}

run_simulator_limit() {
  local TRACE_FILE=$1
  local OUTPUT_FILE=$2

  # Run the simulator command
  ${SIMULATOR} -w ${WARMUP_INSTRUCTIONS}  "${TRACE_FILE}" > "${OUTPUT_FILE}"
}
# Export the function and variables for parallel execution
export -f run_simulator
export -f run_simulator_limit
export SIMULATOR WARMUP_INSTRUCTIONS MAX_JOBS

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

# Create output directories if they don't exist
mkdir -p "${OUTPUT_DIR}/models"
mkdir -p "${OUTPUT_DIR}/reference"

# Loop over each trace file in the model traces directory
for TRACE_FILE in "${MODEL_TRACE_DIR}"/*; do
  # Get the base filename
  TRACE_FILENAME=$(basename "${TRACE_FILE}")
  # Remove the extension to get the application name
  APP_NAME="${TRACE_FILENAME%.*}"
  
  # Define the output file path
  OUTPUT_FILE="${OUTPUT_DIR}/models/${APP_NAME}.out"
  
  # Add the job to run the simulator
  add_job run_simulator_limit "${TRACE_FILE}" "${OUTPUT_FILE}"
done

# Loop over each trace file in the reference traces directory
for TRACE_FILE in "${REFERENCE_TRACE_DIR}"/*; do
  # Get the base filename
  TRACE_FILENAME=$(basename "${TRACE_FILE}")
  # Remove the extension to get the application name
  APP_NAME="${TRACE_FILENAME%.*}"
  
  # Define the output file path
  OUTPUT_FILE="${OUTPUT_DIR}/reference/${APP_NAME}.out"
  
  # Add the job to run the simulator
  add_job run_simulator "${TRACE_FILE}" "${OUTPUT_FILE}" "${SIMULATION_INSTRUCTIONS}"
done

# Wait for all remaining jobs to complete
wait
