#!/bin/bash

# Usage: ./run_inference.sh <source_trace_path> <model_name> <model_path> <trained_trace_name> [max_jobs] [device_list]

# Input arguments
SOURCE_TRACE_PATH=$1        # Path where all candidate traces reside (e.g., ../spec17/)
MODEL_NAME=$2               # Name of the model being used for inference (e.g., ImputeFormer)
MODEL_PATH=$3               # Path to the trained model (e.g., spec17/mcf_665B/ImputeFormer_model.pypots)
TRAINED_TRACE=$4            # Name of the trace used for training (without extension), e.g., 605.mcf_s-665B

# Optional arguments
MAX_JOBS=${5:-4}            # Max number of parallel jobs (default: 4)

# Function to add jobs and limit parallel execution
function add_job {
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

# Loop over each trace file in the source trace path
for TRACE_FILE in "$SOURCE_TRACE_PATH"/*.xz; do
    # Get the base filename
    TRACE_BASENAME=$(basename "$TRACE_FILE")

    # Determine APP_NAME by removing known extensions
    if [[ "$TRACE_BASENAME" == *.champsimtrace.xz ]]; then
        APP_NAME="${TRACE_BASENAME%.champsimtrace.xz}"
    elif [[ "$TRACE_BASENAME" == *.trace.gz ]]; then
        APP_NAME="${TRACE_BASENAME%.trace.gz}"
    elif [[ "$TRACE_BASENAME" == *.xz ]]; then
        APP_NAME="${TRACE_BASENAME%.xz}"
    else
        APP_NAME="${TRACE_BASENAME}"
    fi

    # Exclude the trained trace
    if [ "$APP_NAME" == "$TRAINED_TRACE" ]; then
        echo "Skipping trained trace $APP_NAME"
        continue
    fi

    # Construct data_path (path to the CSV file corresponding to this trace)
    # Adjust the path to where your CSV files are stored
    DATA_PATH="/path/to/CSV/files/${APP_NAME}.csv"  # Replace with actual path

    # Check if DATA_PATH exists
    if [ ! -f "$DATA_PATH" ]; then
        echo "Data file $DATA_PATH does not exist. Skipping $APP_NAME."
        continue
    fi

    # Construct save_path (output directory for this application)
    SAVE_PATH="spec17/${APP_NAME}"

    # Create the save_path directory if it doesn't exist
    mkdir -p "$SAVE_PATH"

    # Construct log file path
    LOG_FILE="log_${APP_NAME}_${MODEL_NAME}.txt"
    # Construct the command
    COMMAND="./load_trace_imputed $TRACE_PATH 0 500000000 100000000 $DATA_PATH $SAVE_PATH percentage 0.2"
# Usage: ./load_trace_imputed <trace_file> <nwarmup> <nsimulate> <heartbeat> <imputed_data_file> <output_trace_file> <ipc_change_method> <ipc_change_threshold>
    echo "Processing $APP_NAME on device $DEVICE"

    # Add job to run the command
    add_job eval $COMMAND
done

# Wait for all remaining jobs to complete
wait
