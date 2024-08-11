#!/bin/bash

# Loop over all CSV files in the specified directories
for csv_file in */*/*/*.csv; do
    echo "Processing $csv_file"
    python test.py "$csv_file"
done
