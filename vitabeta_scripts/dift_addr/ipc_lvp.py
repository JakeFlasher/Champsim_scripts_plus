#!/usr/bin/env python3

import os
import sys
import argparse
import pandas as pd
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

def parse_arguments():
    """
    Parse command-line arguments.
    
    Returns:
        argparse.Namespace: Parsed arguments with root_dir and output_dir.
    """
    parser = argparse.ArgumentParser(description='Fill NaN values in "ipc" column with last known value using multi-threading.')
    parser.add_argument('root_dir', type=str, help='Root directory containing CSV files to process.')
    parser.add_argument('output_dir', type=str, help='Output directory to save processed CSV files.')
    parser.add_argument('--threads', type=int, default=4, help='Number of threads to use (default: 4).')
    return parser.parse_args()

def process_file(file_path, output_dir):
    """
    Process a single CSV file: extract 'ipc' column, fill NaNs, and save to output directory.
    
    Args:
        file_path (Path): Path object of the input CSV file.
        output_dir (Path): Path object of the output directory.
    """
    try:
        # Read CSV
        df = pd.read_csv(file_path, engine="pyarrow")
        
        # Check if 'ipc' column exists
        if 'ipc' not in df.columns:
            print(f"Warning: 'ipc' column not found in {file_path.name}. Skipping this file.")
            return
        
        # Extract 'ipc' column
        ipc_series = df['ipc'].copy()
        
        # Convert to float (if not already)
        ipc_series = pd.to_numeric(ipc_series, errors='coerce')
        
        # Fill NaNs with the last non-NaN value (forward-fill)
        ipc_filled = ipc_series.fillna(method='ffill')
        
        # Handle leading NaNs by leaving them as NaN or filling with a default value (optional)
        # Here, we leave them as NaN
        
        # Optional: Convert 'ipc' back to string if needed
        # ipc_filled = ipc_filled.astype(str)
        
        # Create a new DataFrame with 'ipc' column
        ipc_df = pd.DataFrame({'ipc': ipc_filled})
        
        # Define output file path
        output_file = output_dir / file_path.name
        
        # Save to CSV
        ipc_df.to_csv(output_file, index=False)
        
        print(f"Processed and saved: {output_file.name}")
        
    except Exception as e:
        print(f"Error processing file {file_path.name}: {e}")

def main():
    args = parse_arguments()
    
    root_dir = Path(args.root_dir)
    output_dir = Path(args.output_dir)
    num_threads = args.threads
    
    # Validate root directory
    if not root_dir.exists() or not root_dir.is_dir():
        print(f"Error: Root directory '{root_dir}' does not exist or is not a directory.")
        sys.exit(1)
    
    # Create output directory if it doesn't exist
    output_dir.mkdir(parents=True, exist_ok=True)
    
    # List all CSV files in the root directory
    csv_files = list(root_dir.glob('*.csv'))
    
    if not csv_files:
        print(f"No CSV files found in the directory '{root_dir}'.")
        sys.exit(0)
    
    print(f"Found {len(csv_files)} CSV files to process.")
    
    # Use ThreadPoolExecutor for multi-threading
    with ThreadPoolExecutor(max_workers=num_threads) as executor:
        # Submit all file processing tasks
        futures = {executor.submit(process_file, file, output_dir): file for file in csv_files}
        
        # Iterate over completed tasks
        for future in as_completed(futures):
            file = futures[future]
            try:
                future.result()
            except Exception as e:
                print(f"Exception occurred while processing {file.name}: {e}")
    
    print("All files have been processed.")

if __name__ == '__main__':
    main()
