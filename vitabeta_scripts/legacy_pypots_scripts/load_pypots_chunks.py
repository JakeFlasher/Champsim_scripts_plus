import argparse
import pandas as pd
import numpy as np
from sklearn.preprocessing import StandardScaler
import joblib
import matplotlib.pyplot as plt
import matplotlib as mpl
import os
import logging

# Set up basic logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Import the imputation models
from pypots.imputation import SAITS, ModernTCN, TimeMixer, ImputeFormer, TEFN
from pypots.imputation.fits import FITS
from pypots.optim import Adam
from pypots.utils.metrics import calc_mae

def parse_arguments():
    parser = argparse.ArgumentParser(description='Time Series Imputation Script')
    parser.add_argument('--data_path', type=str, required=True, help='Path to the input CSV data file')
    parser.add_argument('--model', type=str, required=True,
                        choices=['SAITS', 'ModernTCN', 'TimeMixer', 'ImputeFormer', 'FITS', 'TEFN', 'CSAI'],
                        help='Imputation model to use')
    parser.add_argument('--save_path', type=str, default='results', help='Directory to save results and models')
    parser.add_argument('--device', type=str, default='cuda:0', help='Device, default: cuda:0')
    parser.add_argument('--scaler_path', type=str, default='scaler.pkl', help='Path to save the scaler')
    parser.add_argument('--model_path', type=str, default=None, help='Path to the pre-trained model file')
    parser.add_argument('--plot_results', action='store_true', help='Flag to plot the results')
    parser.add_argument('--chunk_size', type=int, default=500000, help='Number of rows per chunk')
    args = parser.parse_args()
    return args

def process_chunk(chunk, scaler=None, time_steps=128):
    """ Process a single data chunk: scaling, reshaping, and identifying missing values. """
    # Identify missing 'ipc' values
    missing_ipc_mask = chunk['ipc'].isna()

    # Separate features and target
    X = chunk.drop(columns=['ipc']).to_numpy()
    y = chunk['ipc'].to_numpy()

    # Combine features and target for scaling
    data = np.hstack([X, y.reshape(-1, 1)])

    # Feature scaling
    if scaler is None:
        scaler = StandardScaler()
        data_scaled = scaler.fit_transform(data)
    else:
        data_scaled = scaler.transform(data)

    # Extract scaled features and target
    num_features = data_scaled.shape[1]
    data_scaled = data_scaled.reshape(-1, num_features)

    # Store original indices for later mapping
    original_indices = chunk.index.to_numpy()
    return data_scaled, missing_ipc_mask, original_indices

def impute_chunk(model, data_scaled, time_steps):
    """ Impute missing values in a chunk using the provided model. """
    num_samples = data_scaled.shape[0] // time_steps
    truncated_length = num_samples * time_steps
    data_scaled = data_scaled[:truncated_length]

    data_scaled = data_scaled.reshape(num_samples, time_steps, -1)
    dataset = {"X": data_scaled}

    # Perform imputation
    imputation = model.predict(dataset)
    imputed_values = imputation["imputation"].reshape(-1, data_scaled.shape[2])
    return imputed_values

def main():
    """ Main function to run the script. """
    args = parse_arguments()
    data_path = args.data_path
    model_name = args.model
    save_path = args.save_path
    scaler_path = args.scaler_path
    device = args.device
    model_path = args.model_path
    time_steps = 64  # Set time_steps as per your requirement
    chunk_size = args.chunk_size  # Number of rows per chunk

    if not os.path.exists(save_path):
        os.makedirs(save_path)

    # Initialize an empty DataFrame to store imputed results
    df_imputed_list = []

    # Initialize scaler
    scaler = StandardScaler()

    # Load model if model path is provided
    model = None
    if model_path and os.path.exists(model_path):
        # Initialize the model parameters (you need to adjust num_steps and num_features)
        # Assuming num_features is known or can be derived from data
        num_features = None  # Placeholder, will be set after processing the first chunk
        model = initialize_model(time_steps, num_features, model_name, save_path, device)
        model.load(model_path)
        logger.info(f"Model loaded from {model_path}")
    else:
        # If model is not provided, initialize it after processing the first chunk
        pass

    # Process data in chunks
    data_iterator = pd.read_csv(data_path, index_col=0, chunksize=chunk_size, engine="pyarrow")
    chunk_number = 0

    for chunk in data_iterator:
        logger.info(f"Processing chunk {chunk_number}")
        chunk_number += 1

        # Data preview
        # data_preview(chunk)

        # Preprocess the chunk
        data_scaled, missing_ipc_mask, original_indices = process_chunk(chunk, scaler if chunk_number > 1 else None, time_steps=time_steps)

        if chunk_number == 1:
            num_features = data_scaled.shape[1]
            # Fit scaler on first chunk
            scaler.fit(data_scaled)
            # Save the scaler
            joblib.dump(scaler, os.path.join(save_path, scaler_path))
            # Initialize the model
            if model is None:
                model = initialize_model(time_steps, num_features, model_name, save_path, device)
                if model_path and os.path.exists(model_path):
                    model.load(model_path)
                    logger.info(f"Model loaded from {model_path}")
                else:
                    # Train the model on the first chunk
                    num_samples = data_scaled.shape[0] // time_steps
                    truncated_length = num_samples * time_steps
                    data_scaled_chunk = data_scaled[:truncated_length]
                    data_scaled_chunk = data_scaled_chunk.reshape(num_samples, time_steps, -1)
                    dataset_chunk = {"X": data_scaled_chunk}
                    model = train_model(model, dataset_chunk)

        # Transform data using the scaler
        data_scaled = scaler.transform(data_scaled)

        # Impute the chunk
        imputed_values = impute_chunk(model, data_scaled, time_steps)

        # Inverse transform the imputed data to original scale
        imputation_inverse = scaler.inverse_transform(imputed_values)

        # Replace missing 'ipc' values in the original chunk
        df_chunk_imputed = chunk.copy()
        imputed_ipc = imputation_inverse[:, -1]
        missing_ipc_mask_flat = missing_ipc_mask.values
        df_chunk_imputed.loc[missing_ipc_mask_flat, 'ipc'] = imputed_ipc[missing_ipc_mask_flat]

        # Append the imputed chunk to the list
        df_imputed_list.append(df_chunk_imputed)

        # Clear variables to free memory
        del chunk, data_scaled, missing_ipc_mask, original_indices, imputed_values, imputation_inverse, df_chunk_imputed

    # Concatenate all imputed chunks
    df_imputed = pd.concat(df_imputed_list)

    # Save the imputed DataFrame to CSV
    output_csv = os.path.join(save_path, f'imputed_data_{model_name}.csv')
    df_imputed.to_csv(output_csv, index=True)
    logger.info(f"Imputed data saved to {output_csv}")

    # Plot the results if the flag is set
    if args.plot_results:
        df_original_full = pd.read_csv(data_path, index_col=0)
        plot_results(df_imputed, df_original_full, save_path, model_name)

    print(f"Imputation with model {model_name} completed.")
    print(f"Imputed data saved to {output_csv}")

if __name__ == "__main__":
    main()
