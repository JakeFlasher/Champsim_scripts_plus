import os
import itertools
from collections import defaultdict
import numpy as np
import matplotlib.pyplot as plt
import logging
import re
import sys
import glob
import scienceplots
from scipy.stats import gmean

logger = logging.getLogger("plot_mrc_size")

def parse_csv_file(filepath: str) -> dict[str, list[float]]:
    """Parse a CSV file to extract miss rates for each algorithm."""
    miss_rates = defaultdict(list)
    
    with open(filepath, 'r') as f:
        for line in f:
            if line.startswith("result"):
                match = re.search(r'([\w\-.\(\):]+)\s+cache size\s+\d+.*miss ratio\s+([\d.]+)', line)
                if match:
                    algo = match.group(1)
                    miss_ratio = float(match.group(2))
                    miss_rates[algo].append(miss_ratio)
    
    return miss_rates

def calculate_geo_mean(miss_rates: dict[str, list[float]]) -> dict[str, float]:
    """Calculate geometric mean of miss rates for each algorithm after removing max and min values."""
    geo_means = {}
    for algo, rates in miss_rates.items():
        if len(rates) > 2:
            rates.remove(max(rates))
            rates.remove(min(rates))
        geo_means[algo] = gmean(rates)
    return geo_means

def plot_means(means: dict[str, dict[str, float]], output_dir: str, mode: str) -> None:
    """Plot geomean means of miss rates for each algorithm and application."""
    linestyles = itertools.cycle(["-", "--", "-.", ":", (0, (3, 1, 1, 1)), (0, (5, 1)), (0, (5, 5)), (0, (3, 5, 1, 5))])
    colors = plt.cm.tab10(np.linspace(0, 1, len(means)))
    
    with plt.style.context(['science', 'ieee']):
        fig, ax = plt.subplots(figsize=(7, 5))
        
        for (algo, app_means), color in zip(means.items(), colors):
            applications = list(app_means.keys())
            mean_values = list(app_means.values())
            ax.plot(applications, mean_values, linewidth=1.25, linestyle=next(linestyles), label=algo, color=color)

        ax.set_xlabel("Applications", fontsize=14)
        ax.set_ylabel("Geomean Mean of Miss Ratio", fontsize=14)
        ax.grid(linestyle="--", alpha=0.7)
        ax.set_xticklabels(applications, rotation=45, ha='right')
        
        legend = ax.legend(loc='best', frameon=True, fontsize=10, ncol=2, fancybox=True, framealpha=0.8)
        
        fig.tight_layout(pad=2)
        suffix = mode if mode in ["decode", "load", "filtered"] else ""
        fig.savefig(os.path.join(output_dir, f"geomean_mean_plot_{suffix}.pdf"), bbox_inches="tight")
        plt.close(fig)
        print(f"Plot is saved to {os.path.join(output_dir, f'geomean_mean_plot_{suffix}.pdf')}")

def log_means(means: dict[str, dict[str, float]], log_file: str, mode: str) -> None:
    """Log geomean means to a file."""
    with open(log_file, 'w') as f:
        for algo, app_means in means.items():
            for app, mean in app_means.items():
                f.write(f"{algo}, {app}, {mean:.4f}\n")
    print(f"Geomean means logged to {log_file}")

def extract_app_name(filename: str) -> str:
    """Extract application name by removing number prefix."""
    base_name = os.path.basename(filename)
    match = re.match(r'^\d+\.(.+?)\.champsimtrace', base_name)
    return match.group(1) if match else base_name.split('.')[0]

def run(directory: str, mode: str):
    """Read all CSV files in the directory, calculate geomean means, and plot."""
    means = defaultdict(dict)
    
    if mode == "full":
        csv_files = glob.glob(os.path.join(directory, "*.xz_decode_ins.csv")) + \
                    glob.glob(os.path.join(directory, "*.gz_decode_ins.csv"))
        suffix = "decode"
    elif mode == "focus":
        csv_files = glob.glob(os.path.join(directory, "*.xz_load_ins.csv")) + \
                    glob.glob(os.path.join(directory, "*.gz_load_ins.csv"))
        suffix = "load"
    elif mode == "residual":
        csv_files = glob.glob(os.path.join(directory, "*.xz_filtered_ins.csv")) + \
                    glob.glob(os.path.join(directory, "*.gz_filtered_ins.csv"))
        suffix = "filtered"
    else:
        print("Invalid mode. Choose from 'full', 'focus', or 'residual'.")
        sys.exit(1)
    
    for filepath in csv_files:
        app_name = extract_app_name(filepath)
        miss_rates = parse_csv_file(filepath)
        app_means = calculate_geo_mean(miss_rates)
        
        for algo, mean in app_means.items():
            means[algo][app_name] = mean
    
    plot_means(means, directory, suffix)
    log_means(means, os.path.join(directory, f"geomean_means_{suffix}.log"), suffix)

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    
    if len(sys.argv) < 3:
        print("Usage: python script.py <path_to_directory> <mode>")
        sys.exit(1)
    
    directory_path = sys.argv[1]
    mode = sys.argv[2]  # "full", "focus", or "residual"
    run(directory_path, mode)