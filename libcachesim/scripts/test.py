import os
import itertools
from collections import defaultdict
import numpy as np
import matplotlib.pyplot as plt
import scienceplots
import logging
from typing import List, Dict, Tuple
import re
import sys

logger = logging.getLogger("plot_mrc_size")

def parse_csv_file(filepath: str) -> Dict[str, List[Tuple[int, float]]]:
    """Parse the CSV file to extract and sort MRC data by cache size."""
    mrc_dict = defaultdict(list)
    
    with open(filepath, 'r') as f:
        for line in f:
            if line.startswith("result"):
                match = re.search(r'([\w\-.\(\):]+)\s+cache size\s+(\d+),.*miss ratio\s+([\d.]+)', line)
                if match:
                    algo = match.group(1)
                    cache_size = int(match.group(2))
                    miss_ratio = float(match.group(3))
                    mrc_dict[algo].append((cache_size, miss_ratio))
    
    # Sort each algorithm's data by cache size
    for algo in mrc_dict:
        mrc_dict[algo].sort()
    
    return mrc_dict

def plot_mrc_size(
    mrc_dict: Dict[str, List[Tuple[int, float]]],
    filepath: str
) -> None:
    """Plot the miss ratio from the computation."""
    linestyles = itertools.cycle(["-", "--", "-.", ":", (0, (3, 1, 1, 1)), (0, (5, 1)), (0, (5, 5)), (0, (3, 5, 1, 5))])
    colors = plt.cm.tab10(np.linspace(0, 1, len(mrc_dict)))
    
    with plt.style.context(['science', 'ieee']):
        fig, ax = plt.subplots(figsize=(6, 4))
        
        for (algo, mrc), color in zip(mrc_dict.items(), colors):
            ax.plot(
                [x[0] for x in mrc],
                [x[1] for x in mrc],
                linewidth=1.25,
                linestyle=next(linestyles),
                label=algo,
                color=color
            )

        ax.set_xlabel("Cache Size", fontsize=14)
        ax.set_xscale("log")
        ax.set_ylabel("Miss Ratio", fontsize=14)
        ax.grid(linestyle="--", alpha=0.7)
        ax.set_title("Cache Miss Ratio vs. Cache Size", fontsize=16)
        
        # Highlighted legend box
        legend = ax.legend(loc='best', frameon=True, fontsize=10, ncol=2, fancybox=True, framealpha=0.8)
        
        # Save the plot
        output_dir = os.path.dirname(filepath)
        dataname = os.path.splitext(os.path.basename(filepath))[0]
        fig.tight_layout(pad=2)
        fig.savefig(os.path.join(output_dir, f"{dataname}.pdf"), bbox_inches="tight")
        plt.close(fig)
        print(f"Plot is saved to {os.path.join(output_dir, f'{dataname}.pdf')}")

def run(filepath: str):
    """Function to parse a CSV file and plot MRC."""
    mrc_dict = parse_csv_file(filepath)
    plot_mrc_size(mrc_dict, filepath)

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    
    if len(sys.argv) < 2:
        print("Usage: python script.py <path_to_csv>")
        sys.exit(1)
    
    file_path = sys.argv[1]
    run(file_path)
