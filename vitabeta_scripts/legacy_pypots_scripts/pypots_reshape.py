import pandas as pd
import numpy as np
from sklearn.preprocessing import StandardScaler
from pypots.imputation import SAITS  # Ensure you have pypots version==0.3.1 installed
from pypots.utils.metrics import calc_mae
import joblib
import matplotlib.pyplot as plt
import os

# Define constants
DATA_PATH = "/root/developing/champsim_traces_dev/gap/IPC_single_instr/500M/torch_data_v1/bfs-10.trace.gz.csv"
# DATA_PATH = "test.csv"
SCALER_PATH = 'scaler.pkl'
SAVE_PATH = 'test_path'

def load_data(data_path):
    """ Load the dataset from a CSV file. """
    if not os.path.exists(data_path):
        raise FileNotFoundError(f"File not found: {data_path}")
    
    df = pd.read_csv(data_path, index_col=0)
    if "ipc" not in df.columns:
        raise ValueError("Column 'IPC' not found in the dataset")
    
    return df

def data_preview(df):
    """ Print a preview of the dataset and basic statistics. """
    print("==== Data Preview (First 5 Rows) ====")
    print(df.head(), "\n")

def preprocess_data(df):
    """ Preprocess the dataset: drop unnecessary columns, scale and reshape. """
    # Extract number of samples and steps
    num_steps = 64  # This should be the correct division
    num_samples = round(len(df) / num_steps)  # This should be the correct division

    # Drop columns that are not needed
    # df_cleaned = df.drop(['year', 'month', 'day', 'hour', 'wd', 'station'], axis=1)
    df_cleaned = df
    # Feature scaling
    scaler = StandardScaler()
    X_scaled = scaler.fit_transform(df_cleaned.to_numpy())
    print(X_scaled)

    # Save the scaler for future use
    joblib.dump(scaler, SCALER_PATH)
    # Identify missing 'ipc' values
    missing_ipc_mask = df['ipc'].isna()
    print(missing_ipc_mask)
    # Reshape the data to (samples, steps, features)
    X_reshaped = X_scaled.reshape(num_samples, num_steps, 5)
    
    return X_reshaped, scaler, df_cleaned.columns, missing_ipc_mask

def replace_missing_ipc(df_original, imputation_inverse, missing_ipc_mask):
    """ Replace missing 'ipc' values in the original DataFrame with imputed values """
    # Extract the imputed 'ipc' values
    # imputed_ipc = imputation_inverse[:, -1]  # Assuming 'ipc' is the last column after inverse_transform
    num_points = len(df_original['ipc'])
    imputed_ipc = imputation_inverse[:num_points, 4]

    # Replace missing 'ipc' values in the original DataFrame
    df_original.loc[missing_ipc_mask, 'ipc'] = imputed_ipc[missing_ipc_mask.values]

    return df_original

def train_saits_model(X, num_steps, num_features):
    """ Train the SAITS model using the given dataset. """
    dataset = {"X": X}
    print(X.shape) 
    # Initialize SAITS model
    """
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
        patience=10,
        num_workers=4,
        # device=['cuda:0', 'cuda:1', 'cuda:2', 'cuda:3'], 
        device=['cuda:0'],
        saving_path=SAVE_PATH
    )
    """
    print(num_steps)
    print(num_features)
    saits = SAITS(n_steps=num_steps, n_features=num_features, n_layers=2, d_model=256, n_heads=4, d_k=64, d_v=64,batch_size=16384, d_ffn=128, dropout=0.1, epochs=100, patience=10, num_workers=4, device=['cuda:0', 'cuda:1', 'cuda:3', 'cuda:5'])
    # Train the model
    saits.fit(dataset)
    imputation = saits.impute(dataset)
    saits.save("saved_models/saits_bfs-10.pypots")  # save the model for future use
    # Make predictions (impute missing values)
    # imputation = saits.predict(dataset)
    
    return imputation

def inverse_transform_imputation(imputation, scaler, num_features):
    """ Inverse transform the imputed values to the original scale. """
    # imputation_reshaped = imputation["imputation"].reshape(-1, num_features)
    imputation_reshaped = imputation.reshape(-1, num_features)
    imputation_inverse = scaler.inverse_transform(imputation_reshaped)

    
    return imputation_inverse

def plot_results(imputation, df_origin):
    """ Plot the original and imputed data. """
    # Creating a time series for the x-axis
    # time_series = pd.date_range(start="2013-03-01", end="2013-04-30 23:00:00", freq='H')
    time_series = df_origin.index

    # Ensure the length matches for plotting
    num_points = min(len(imputation), len(df_origin['ipc']))
    # num_points = min(len(time_series), len(imputation), len(df_origin['ipc']))
    
    plt.figure(figsize=(60,40))
    plt.plot(time_series[:num_points], imputation[:num_points, 4], label='Imputed ipc')
    plt.plot(time_series[:num_points], df_origin['ipc'][:num_points], label='Original ipc')
    plt.legend()
    plt.xlabel('Instr index')
    plt.ylabel('ipc')
    plt.title('ipc: Original vs Imputed')
    plt.xticks(rotation=45)  # Rotate x-axis labels for better readability
    plt.tight_layout()
    plt.savefig("test.pdf")
    # plt.show()

def main():
    """ Main function to run the script. """
    # Load and preprocess the data
    df_origin = load_data(DATA_PATH)
        # Data preview and description
    data_preview(df_origin)

    X, scaler, feature_columns, mask = preprocess_data(df_origin)
    
    num_samples = X.shape[0]
    num_steps = X.shape[1]
    num_features = X.shape[2]

    # Train the SAITS model and perform imputation
    imputation = train_saits_model(X, num_steps, num_features)

    # Inverse transform the imputed data to original scale
    # print(imputation)
    imputation_inverse = inverse_transform_imputation(imputation, scaler, num_features)
    # imputation_inverse = imputation 
    # print(imputation_inverse)
        # Replace missing 'ipc' values in the original DataFrame
    df_imputed = replace_missing_ipc(df_origin, imputation_inverse, mask)

    # Save the imputed DataFrame to CSV
    df_imputed.to_csv('imputed_data.csv', index=True)
    # Plot the results
    plot_results(imputation_inverse, df_origin)

if __name__ == "__main__":
    main()
