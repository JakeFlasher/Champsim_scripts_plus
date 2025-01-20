import pandas as pd
import numpy as np
from sklearn.preprocessing import StandardScaler
from pypots.imputation import SAITS  # Ensure you have pypots version==0.3.1 installed
import joblib
import matplotlib.pyplot as plt
import os

# Define constants
DATA_PATH = "test.csv"
# DATA_PATH = "/root/developing/champsim_traces_dev/gap/IPC_single_instr/500M/torch_data_v1/bfs-10.trace.gz.csv"
SCALER_PATH = 'scaler.pkl'
SAVE_PATH = 'test_path'

def load_data(data_path):
    """ Load the dataset from a CSV file. """
    if not os.path.exists(data_path):
        raise FileNotFoundError(f"File not found: {data_path}")
    
    df = pd.read_csv(data_path, index_col=0)
    if "ipc" not in df.columns:
        raise ValueError("Column 'ipc' not found in the dataset")
    
    return df

def data_preview(df):
    """ Print a preview of the dataset and basic statistics. """
    print("==== Data Preview (First 5 Rows) ====")
    print(df.head(), "\n")

def preprocess_data(df):
    """ Preprocess the dataset: identify missing 'ipc' values, scale, and reshape. """
    # Keep a copy of the original DataFrame
    df_original = df.copy()
    
    # Identify missing 'ipc' values
    missing_ipc_mask = df['ipc'].isna()
    
    # Separate features and target
    X = df.drop(columns=['ipc']).to_numpy()
    y = df['ipc'].to_numpy()
    
    # Combine features and target for scaling
    data = np.hstack([X, y.reshape(-1, 1)])
    
    # Feature scaling
    scaler = StandardScaler()
    data_scaled = scaler.fit_transform(data)
    
    # Extract scaled features and target
    X_scaled = data_scaled[:, :-1]
    y_scaled = data_scaled[:, -1]
    
    # Prepare data for SAITS
    # Since SAITS expects data in shape (samples, time steps, features)
    # For simplicity, consider each data point as a single time step
    X_saits = np.hstack([X_scaled, y_scaled.reshape(-1, 1)])
    X_saits = X_saits.reshape(-1, 1, X_saits.shape[1])
    
    return X_saits, scaler, missing_ipc_mask, df_original

def train_saits_model(X, num_steps, num_features):
    """ Train the SAITS model using the given dataset. """
    dataset = {"X": X}
    
    # Initialize SAITS model
    saits = SAITS(
        n_steps=num_steps,
        n_features=num_features,
        n_layers=2, 
        d_model=256, 
        d_ffn=128, 
        n_heads=4, 
        d_k=64, 
        d_v=64, 
        batch_size=128,
        epochs=200,
        patience=50,
        num_workers=4,
        device=['cuda:0','cuda:1', 'cuda:3', 'cuda:5'],
        saving_path=SAVE_PATH
    )
    
    # Train the model
    saits.fit(dataset)
    
    # Make predictions (impute missing values)
    imputation = saits.predict(dataset)
    
    return imputation

def inverse_transform_imputation(imputation, scaler):
    """ Inverse transform the imputed values to the original scale. """
    # Reshape imputation to 2D array
    imputation_reshaped = imputation["imputation"].reshape(-1, imputation["imputation"].shape[-1])

    # Inverse transform
    imputation_inverse = scaler.inverse_transform(imputation_reshaped)

    return imputation_inverse

def replace_missing_ipc(df_original, imputation_inverse, missing_ipc_mask):
    """ Replace missing 'ipc' values in the original DataFrame with imputed values """
    # Extract the imputed 'ipc' values
    imputed_ipc = imputation_inverse[:, -1]  # Assuming 'ipc' is the last column after inverse_transform

    # Replace missing 'ipc' values in the original DataFrame
    df_original.loc[missing_ipc_mask, 'ipc'] = imputed_ipc[missing_ipc_mask.values]

    return df_original

def plot_results(df_imputed, df_original):
    """ Plot the original and imputed 'ipc' values. """
    # Creating a time series for the x-axis
    time_series = df_imputed.index

    # Ensure the length matches for plotting
    num_points = min(len(df_imputed), len(df_original))
    
    plt.figure(figsize=(15,8))
    plt.plot(time_series[:num_points], df_imputed['ipc'][:num_points], label='Imputed ipc', alpha=0.7)
    plt.plot(time_series[:num_points], df_original['ipc'][:num_points], label='Original ipc', alpha=0.7)
    plt.legend()
    plt.xlabel('Instruction Index')
    plt.ylabel('ipc')
    plt.title('ipc: Original vs Imputed')
    plt.xticks(rotation=45)  # Rotate x-axis labels for better readability
    plt.tight_layout()
    plt.savefig("ipc_imputation.pdf")
    # plt.show()

def main():
    """ Main function to run the script. """
    # Load and preprocess the data
    df_origin = load_data(DATA_PATH)
    # Data preview
    data_preview(df_origin)

    # Preprocess the data
    X_saits, scaler, missing_ipc_mask, df_original = preprocess_data(df_origin)

    num_samples = X_saits.shape[0]
    num_steps = X_saits.shape[1]  # Should be 1
    num_features = X_saits.shape[2]

    # Train the SAITS model and perform imputation
    imputation = train_saits_model(X_saits, num_steps, num_features)

    # Inverse transform the imputed data to original scale
    imputation_inverse = inverse_transform_imputation(imputation, scaler)

    # Replace missing 'ipc' values in the original DataFrame
    df_imputed = replace_missing_ipc(df_original, imputation_inverse, missing_ipc_mask)

    # Save the imputed DataFrame to CSV
    df_imputed.to_csv('imputed_data.csv', index=True)

    # Plot the results
    plot_results(df_imputed, df_origin)

if __name__ == "__main__":
    main()
