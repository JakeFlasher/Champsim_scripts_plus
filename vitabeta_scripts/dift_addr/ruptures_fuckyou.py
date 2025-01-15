import pandas as pd
import numpy as np
import ruptures as rpt
import matplotlib.pyplot as plt

def load_data(csv_file):
    """Load the IPC values from the CSV file."""
    df = pd.read_csv(csv_file)
    if 'ipc' not in df.columns:
        raise ValueError("Column 'ipc' not found in the dataset")
    return df

def preprocess_ipc(df):
    """Preprocess the IPC data."""
    ipc_values = df['ipc'].fillna(method='ffill').fillna(method='bfill').to_numpy()
    return ipc_values

def detect_change_points(ipc_values, model='rbf', penalty=3):
    """Detect change points in the IPC values using Ruptures."""
    # Choose an appropriate model based on data characteristics
    algo = rpt.Pelt(model=model).fit(ipc_values)
    change_points = algo.predict(pen=penalty)
    return change_points

def save_change_points(change_points, output_file):
    """Save the detected change points to a log file."""
    with open(output_file, 'w') as f:
        for cp in change_points[:-1]:  # Exclude the last point
            f.write(f"{cp}\n")

def plot_change_points(ipc_values, change_points):
    """Plot IPC values with detected change points."""
    plt.figure(figsize=(10, 6))
    plt.plot(ipc_values, label='IPC')
    for cp in change_points[:-1]:
        plt.axvline(x=cp, color='r', linestyle='--')
    plt.title('IPC Values with Detected Change Points')
    plt.xlabel('Instruction Index')
    plt.ylabel('IPC')
    plt.legend()
    plt.show()

def main():
    # Parameters
    csv_file = 'imputed_data.csv'  # Path to your CSV file
    change_point_log = 'change_points.log'
    model = 'rbf'  # Model for change point detection ('l1', 'l2', 'rbf', etc.)
    penalty = 3  # Penalty value for PELT algorithm

    # Load data
    df = load_data(csv_file)
    ipc_values = preprocess_ipc(df)

    # Detect change points
    change_points = detect_change_points(ipc_values, model=model, penalty=penalty)
    print(f"Detected change points: {change_points[:-1]}")

    # Save change points to log file
    save_change_points(change_points, change_point_log)
    print(f"Change points saved to {change_point_log}")

    # Optional: Plot change points
    plot_change_points(ipc_values, change_points)

if __name__ == "__main__":
    main()
