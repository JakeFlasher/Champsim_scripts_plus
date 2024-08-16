import os
import re
from collections import defaultdict
import numpy as np
import matplotlib.pyplot as plt
import scienceplots
from scipy.stats import hmean
import pandas as pd
import seaborn as sns
import sys

def parse_log_file(filepath: str) -> dict[str, list[float]]:
    """Parse a log file to extract miss rates for each algorithm."""
    miss_rates = defaultdict(list)
    
    with open(filepath, 'r') as f:
        for line in f:
            algo, app, rate = line.strip().split(', ')
            miss_rates[app].append(float(rate))
    
    return miss_rates

def calculate_harmonic_means(miss_rates: dict[str, list[float]]) -> dict[str, float]:
    """Calculate harmonic mean of miss rates across all algorithms for each application."""
    harmonic_means = {}
    for app, rates in miss_rates.items():
        harmonic_means[app] = hmean(rates)
    return harmonic_means

def calculate_ratios(decode_means: dict[str, float], load_means: dict[str, float]) -> dict[str, float]:
    """Calculate the ratio of mean_load to mean_decode for each application."""
    ratios = {}
    for app in decode_means:
        if app in load_means:
            ratios[app] = load_means[app] / decode_means[app]
    return ratios

def read_csv_ratios(filepath: str) -> dict[str, float]:
    """Read the baseline ratios from a CSV file."""
    df = pd.read_csv(filepath)
    ratios = df['decode'] / df['load']
    return dict(zip(df['app'], ratios))

def plot_ratios(harmonic_ratios: dict[str, float], geomean_ratios: dict[str, float], baseline_ratios: dict[str, float], output_file: str) -> None:
    """Plot the ratios for each application with enhanced visuals."""
    sns.set(style="whitegrid")
    applications = list(harmonic_ratios.keys())
    harmonic_values = list(harmonic_ratios.values())
    geomean_values = [geomean_ratios[app] for app in applications]
    baseline_values = [baseline_ratios[app] for app in applications]

    x = np.arange(len(applications))

    with plt.style.context(['science', 'ieee']):
        plt.figure(figsize=(10, 6))
        
        # Bar plot
        bars1 = plt.bar(x - 0.2, harmonic_values, width=0.2, label='Harmonic Ratios', color=sns.color_palette("Paired")[0])
        bars2 = plt.bar(x, geomean_values, width=0.2, label='Geometric Ratios', color=sns.color_palette("Paired")[1])
        bars3 = plt.bar(x + 0.2, baseline_values, width=0.2, label='Shrinkage Ratios', color=sns.color_palette("Paired")[2])

        # Add numbers on bars
        for bar in bars1:
            yval = bar.get_height()
            plt.text(bar.get_x() + bar.get_width()/2.0, yval, f'{yval:.2f}', ha='center', va='bottom', fontsize=8)
        for bar in bars2:
            yval = bar.get_height()
            plt.text(bar.get_x() + bar.get_width()/2.0, yval, f'{yval:.2f}', ha='center', va='bottom', fontsize=8)
        for bar in bars3:
            yval = bar.get_height()
            plt.text(bar.get_x() + bar.get_width()/2.0, yval, f'{yval:.2f}', ha='center', va='bottom', fontsize=8)

        # Line plot
        plt.plot(x, harmonic_values, marker='o', color='blue', linestyle='--')
        plt.plot(x, geomean_values, marker='o', color='green', linestyle='--')
        plt.plot(x, baseline_values, marker='o', color='red', linestyle='--')

        plt.xlabel('Applications')
        plt.ylabel('Ratio (Load / Decode)')
        plt.title('Comparison of Load/Decode Ratios')
        plt.xticks(x, applications, rotation=45, ha='right')
        plt.legend()
        plt.tight_layout()
        plt.savefig(output_file)
        plt.close()
        print(f"Plot saved as {output_file}")

def process_log_files(directory: str, output_file: str) -> None:
    """Process log files and plot the ratios."""
    decode_log = os.path.join(directory, "harmonic_means_decode.log")
    load_log = os.path.join(directory, "harmonic_means_load.log")
    baseline_csv = os.path.join(directory, "size_ratios.csv")
    
    decode_rates = parse_log_file(decode_log)
    load_rates = parse_log_file(load_log)
    
    decode_means = calculate_harmonic_means(decode_rates)
    load_means = calculate_harmonic_means(load_rates)
    
    harmonic_ratios = calculate_ratios(decode_means, load_means)

    decode_log = decode_log.replace("harmonic", "geomean")
    load_log = load_log.replace("harmonic", "geomean")
    
    decode_rates = parse_log_file(decode_log)
    load_rates = parse_log_file(load_log)
    
    decode_means = calculate_harmonic_means(decode_rates)
    load_means = calculate_harmonic_means(load_rates)
    
    geomean_ratios = calculate_ratios(decode_means, load_means)
    
    baseline_ratios = read_csv_ratios(baseline_csv)
    
    plot_ratios(harmonic_ratios, geomean_ratios, baseline_ratios, output_file)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python ratio.py <path_to_directory>")
        sys.exit(1)
    
    directory_path = sys.argv[1]
    output_file = os.path.join(directory_path, "combined_ratio_plot.pdf")
    process_log_files(directory_path, output_file)