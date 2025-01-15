import argparse
import pandas as pd
import numpy as np
import ruptures as rpt
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import seaborn as sns

import scienceplots  # Assuming you're using the scienceplot style

# Use Seaborn's colorblind-friendly palette
palette = sns.color_palette("colorblind")
plt.style.use('science')

# Set Seaborn style for better aesthetics
sns.set(style="whitegrid")

def main():
    # Parse command-line arguments
    parser = argparse.ArgumentParser(description='Change Point Detection on ipc Curves')
    parser.add_argument('--original_csv', type=str, required=True, help='Path to the original ipc curve CSV file')
    parser.add_argument('--imputed_csv', type=str, required=True, help='Path to the imputed ipc curve CSV file')
    parser.add_argument('--start_row', type=int, default=0, help='Starting row (default: 0)')
    parser.add_argument('--num_rows', type=int, default=2000, help='Number of rows to read (default: 2000)')
    args = parser.parse_args()

    # Read the specified range from the original ipc curve CSV file
    original_df = pd.read_csv(args.original_csv, skiprows=range(1, args.start_row + 1), nrows=args.num_rows)
    # Assuming the ipc values are in a column named 'ipc'
    original_ipc = original_df['ipc'].fillna(method='ffill').fillna(method='bfill').to_numpy()

    # Read the specified range from the imputed ipc curve CSV file
    imputed_df = pd.read_csv(args.imputed_csv, skiprows=range(1, args.start_row + 1), nrows=args.num_rows)
    # Assuming the ipc values are in a column named 'ipc'
    imputed_ipc = imputed_df['ipc'].to_numpy()

    # Time axis
    time = np.arange(args.start_row, args.start_row + args.num_rows)

    # Change point detection on original ipc curve
    algo_orig = rpt.Pelt(model="rbf").fit(original_ipc)
    result_orig = algo_orig.predict(pen=10)

    # Change point detection on imputed ipc curve
    algo_imputed = rpt.Pelt(model="rbf").fit(imputed_ipc)
    result_imputed = algo_imputed.predict(pen=10)

    # Plotting
    fig, ax = plt.subplots(figsize=(12, 6))

    # Original ipc curve segmentation
    rpt.display(original_ipc, result_orig)
    # Imputed ipc curve segmentation
    rpt.display(imputed_ipc, result_imputed)

    # Labels and Title
    # ax.set_title('Change Point Detection on ipc Curves', fontsize=16)
    ax.set_xlabel('Instruction Index', fontsize=14)
    ax.set_ylabel('ipc', fontsize=14)

    # Legends
    handles = [
        mpatches.Patch(color='C0', label='Original ipc Segments'),
        mpatches.Patch(color='C1', label='Imputed ipc Segments')
    ]
    ax.legend(handles=handles, loc='upper right', fontsize=12)

    # Grid and Layout
    ax.grid(True, linestyle='--', alpha=0.6)
    plt.tight_layout()

    # Save the figure
    plt.savefig('ipc_change_point_detection.pdf', format='pdf', dpi=300)
    plt.show()

if __name__ == "__main__":
    main()
