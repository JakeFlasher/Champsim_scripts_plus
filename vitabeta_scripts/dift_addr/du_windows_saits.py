#!/usr/bin/env python3

import dupin as du
import os
import sys
import argparse
import pandas as pd
import numpy as np
import ruptures as rpt
import matplotlib.pyplot as plt
import multiprocessing

def parse_arguments():
    """Parse command-line arguments."""
    parser = argparse.ArgumentParser(description='Change Point Detection on Multiple CSV Files')
    parser.add_argument('--data_dir', type=str, required=True, help='Path to the directory containing CSV files')
    parser.add_argument('--output_dir', type=str, required=True, help='Directory to save outputs')
    parser.add_argument('--plot', action='store_true', help='Flag to enable plotting')
    args = parser.parse_args()
    return args

def main():
    args = parse_arguments()
    data_dir = args.data_dir
    output_dir = args.output_dir
    plot_flag = args.plot

    # Get list of CSV files and their relative paths
    all_files = []
    for root, dirs, files in os.walk(data_dir):
        for file in files:
            if file.endswith('imputed_data_ImputeFormer.csv'):
                file_path = os.path.join(root, file)
                # Relative path from data_dir
                rel_path = os.path.relpath(file_path, data_dir)
                all_files.append((file_path, data_dir, output_dir, rel_path, plot_flag))

    # Use multiprocessing to process files in parallel
    max_processes = 5 
    pool = multiprocessing.Pool(processes=max_processes)

    # Run the processing function in parallel
    pool.starmap(process_file, all_files)

    pool.close()
    pool.join()

def process_file(file_path, data_dir, output_dir, rel_path, plot_flag):
    """Process a single CSV file to detect change points."""
    import pandas as pd
    import ruptures as rpt
    import matplotlib.pyplot as plt
    import numpy as np
    import os

    # Prepare output directory
    output_subdir = os.path.join(output_dir, os.path.dirname(rel_path))
    os.makedirs(output_subdir, exist_ok=True)

    # Save change points to file in the output subdirectory
    change_point_file = os.path.join(output_subdir, 'change_points.log')
    if os.path.exists(change_point_file) and os.path.getsize(change_point_file) > 0:
        print(f"skipping file {file_path}. Change points for file exists.")
        return
    print(f"Processing {file_path}")

    print(f"Processing {file_path}")
    # Read CSV file
    try:
        df = pd.read_csv(file_path, engine="pyarrow", usecols=['ipc'])
    except Exception as e:
        print(f"Error reading file {file_path}: {e}")
        return

    if 'ipc' not in df.columns:
        print(f"'ipc' column not found in {file_path}")
        return

    # Preprocess IPC values
    ipc_values = df['ipc'].fillna(method='ffill').fillna(method='bfill').to_numpy()

    # Change Point Detection using PELT algorithm
    try:
        model = 'l2'  # Using 'l2' cost function for mean shifts
        min_size = 1000  # Adjust based on expected regime lengths
        jump = 50000  # Balance between speed and detection sensitivity

        # algo = rpt.Pelt(model=model, min_size=min_size, jump=jump).fit(ipc_values)

        dynp = rpt.Dynp(min_size=min_size, jump=jump, custom_cost=du.detect.CostLinearFit(metric='l2'))
#        algo = rpt.KernelCPD(custom_cost=du.detect.CostLinearFit(metric='l2'), min_size=min_size, jump=jump)
        detector = du.detect.SweepDetector(dynp, 1000)

        # Detect change points
        change_points = detector.fit(ipc_values) 
    except Exception as e:
        print(f"Error processing file {file_path}: {e}")
        return

    try:
        with open(change_point_file, 'w') as f:
            for cp in change_points[:-1]:  # Exclude the last point
                f.write(f"{cp}\n")
    except Exception as e:
        print(f"Error writing change points to {change_point_file}: {e}")
        return

    # Plotting (if enabled)
    if plot_flag:
        try:
            plt.figure(figsize=(10, 6))
            plt.plot(ipc_values, label='IPC', color='blue')
            for cp in change_points[:-1]:
                plt.axvline(x=cp, color='red', linestyle='--')
            plt.title(f'IPC Values with Detected Change Points\n({os.path.basename(file_path)})')
            plt.xlabel('Instruction Index')
            plt.ylabel('IPC')
            plt.legend()
            plot_file = os.path.join(output_subdir, 'ipc_change_points.png')
            plt.savefig(plot_file)
            plt.close()
        except Exception as e:
            print(f"Error plotting IPC for {file_path}: {e}")

    print(f"Processed {file_path}, change points saved to {change_point_file}")

if __name__ == "__main__":
    main()
