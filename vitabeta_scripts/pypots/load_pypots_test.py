import argparse
import pandas as pd
import numpy as np
from sklearn.preprocessing import StandardScaler
import joblib
import matplotlib.pyplot as plt
import matplotlib as mpl
import os
import logging
from pypots.imputation import TimesNet

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
                        choices=['SAITS', 'ModernTCN', 'TimeMixer', 'ImputeFormer', 'FITS', 'TEFN', 'TimesNet' ,'CSAI'],
                        help='Imputation model to use')
    parser.add_argument('--save_path', type=str, default='results', help='Directory to save results and models')
    parser.add_argument('--device', type=str, default='cuda:0', help='Device, default: cuda:0')
    parser.add_argument('--scaler_path', type=str, default='scaler.pkl', help='Path to save the scaler')
    parser.add_argument('--model_path', type=str, default=None, help='Path to the pre-trained model file')
    parser.add_argument('--plot_results', action='store_true', help='Flag to plot the results')
    args = parser.parse_args()
    return args

def load_data(data_path):
    """ Load the dataset from a CSV file. """
    if not os.path.exists(data_path):
        raise FileNotFoundError(f"File not found: {data_path}")
        
    df = pd.read_csv(data_path, index_col=0, engine="pyarrow")
    if "ipc" not in df.columns:
        raise ValueError("Column 'ipc' not found in the dataset")
        
    return df

def data_preview(df):
    """ Print a preview of the dataset and basic statistics. """
    logger.info("==== Data Preview (First 5 Rows) ====")
    logger.info(df.head())
    logger.info("\n")

def preprocess_data(df, time_steps=128):
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
    
    # Combine scaled features and target
    data_scaled = np.hstack([X_scaled, y_scaled.reshape(-1, 1)])
    
    # Reshape data to (samples, time_steps, features)
    total_length = data_scaled.shape[0]
    num_features = data_scaled.shape[1]
    num_samples = total_length // time_steps  # Integer division
    
    # Truncate excess data if total_length is not divisible by time_steps
    truncated_length = num_samples * time_steps
    data_scaled = data_scaled[:truncated_length]
    missing_ipc_mask = missing_ipc_mask.iloc[:truncated_length].reset_index(drop=True)
    df_original = df_original.iloc[:truncated_length].reset_index(drop=True)
    
    data_scaled = data_scaled.reshape(num_samples, time_steps, num_features)
    missing_ipc_mask = missing_ipc_mask.values.reshape(num_samples, time_steps)
    
    return data_scaled, scaler, missing_ipc_mask, df_original

def initialize_model(num_steps, num_features, model_name, save_path, device):
    """ Initialize the specified imputation model. """
    # Set parameters
    patience = 10
    epochs = 200
    batch_size = 128 
    adam = Adam(lr=0.001, weight_decay=1e-5)
    
    # Initialize the model based on the chosen model name
    if model_name == 'SAITS':
        model = SAITS(
            n_steps=num_steps,
            n_features=num_features,
            n_layers=2,
            d_model=256,
            d_ffn=128,
            n_heads=4,
            d_k=64,
            d_v=64,
            dropout=0.1,
            optimizer=adam,
            batch_size=4096,
            epochs=epochs,
            patience=patience,
            num_workers=4,
            saving_path=save_path,
            model_saving_strategy='best',
            device=[device],
            verbose=True
        )
    elif model_name == 'ModernTCN':
        model = ModernTCN(
            n_steps=num_steps,
            n_features=num_features,
            patch_size=8,
            patch_stride=4,
            downsampling_ratio=2,
            ffn_ratio=8,
            num_blocks=[1],
            large_size=[51],
            small_size=[5],
            dims=[64],
            small_kernel_merged=False,
            backbone_dropout=0.1,
            head_dropout=0.1,
            optimizer=adam,
            use_multi_scale=False,
            individual=False,
            apply_nonstationary_norm=False,
            batch_size=8192,
            epochs=epochs,
            patience=patience,
            num_workers=4,
            saving_path=save_path,
            model_saving_strategy='best',
            device=[device],
            verbose=True
        )
    elif model_name == 'TimeMixer':
        model = TimeMixer(
            n_steps=num_steps,
            n_features=num_features,
            n_layers=4,
            top_k=16,
            d_model=1024,
            d_ffn=512,
            dropout=0.1,
            optimizer=adam,
            batch_size=1024,
            epochs=50,
            patience=patience,
            num_workers=12,
            saving_path=save_path,
            model_saving_strategy='best',
            device=[device],
            verbose=True
        )

    elif model_name == 'TimesNet':
        model = TimesNet(
            n_steps=num_steps,
            n_features=num_features,
            batch_size=1024,
            n_layers=4,
            top_k=16,
            d_model=512,
            d_ffn=256,
            n_kernels=3,
            dropout=0.1,
            epochs=10,
            patience=patience,
            num_workers=16,
            saving_path=save_path,
            model_saving_strategy='best',
            device=[device],
            verbose=True
        )
    elif model_name == 'ImputeFormer':
        model = ImputeFormer(
            n_steps=num_steps,
            n_features=num_features,
            n_layers=4,
            d_input_embed=32,
            d_learnable_embed=96,
            d_proj=8,
            d_ffn=256,
            n_temporal_heads=4,
            dropout=0.1,
            optimizer=adam,
            batch_size=512,
            epochs=20,
            patience=patience,
            num_workers=8,
            saving_path=save_path,
            model_saving_strategy='best',
            device=[device],
            verbose=True
        )
    elif model_name == 'FITS':
        model = FITS(
            n_steps=num_steps,
            n_features=num_features,
            individual=False,
            cut_freq=2,
            batch_size=4096,
            epochs=epochs,
            patience=patience,
            num_workers=4,
            saving_path=save_path,
            model_saving_strategy='best',
            device=[device],
            verbose=True
        )
    elif model_name == 'TEFN':
        model = TEFN(
            n_steps=num_steps,
            n_features=num_features,
            n_fod=2,
            batch_size=1024,
            num_workers=4,
            patience=patience,
            model_saving_strategy="best",
            epochs=epochs,
            saving_path=save_path,
            optimizer=adam,
            device=device,
        )
    else:
        raise ValueError(f"Unsupported model '{model_name}'")
    return model

def train_model(model, dataset):
    """ Train the specified model using the given dataset. """
    model.fit(dataset)
    # After training, save the model
    model_save_path = os.path.join(model.saving_path, f"{model.__class__.__name__}_model.pypots")
    model.save(model_save_path)
    logger.info(f"Model saved to {model_save_path}")
    return model

def load_and_impute(model, dataset, model_path):
    """ Load the model from a serialized file and perform imputation. """
    model.load(model_path)
    logger.info(f"Model loaded from {model_path}")
    # Perform imputation
    imputation = model.predict(dataset)
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
    # Extract the imputed 'ipc' values (assuming 'ipc' is the last column after inverse_transform)
    imputed_ipc = imputation_inverse[:, -1]
    
    # Flatten the mask
    missing_ipc_mask_flat = missing_ipc_mask.flatten()
    imputed_ipc_flat = imputed_ipc
    
    # Replace missing 'ipc' values in the original DataFrame
    df_original.loc[missing_ipc_mask_flat, 'ipc'] = imputed_ipc_flat[missing_ipc_mask_flat]
    
    return df_original

def plot_results(df_imputed, df_original, save_path, model_name):
    """ Plot the original and imputed 'ipc' values. """
    # Configure matplotlib to avoid OverflowError
    mpl.rcParams['agg.path.chunksize'] = 10000  # or another suitable value
    
    # Creating a time series for the x-axis
    time_series = df_imputed.index
    
    # Ensure the length matches for plotting
    num_points = min(len(df_imputed), len(df_original))
    
    # For very large datasets, downsample data for plotting
    max_plot_points = 100000  # Adjust as needed
    if num_points > max_plot_points:
        step = num_points // max_plot_points
        indices = np.arange(0, num_points, step)
        time_series = time_series[indices]
        ipc_imputed = df_imputed['ipc'].iloc[indices]
        ipc_original = df_original['ipc'].iloc[indices]
    else:
        ipc_imputed = df_imputed['ipc'][:num_points]
        ipc_original = df_original['ipc'][:num_points]
    
    plt.figure(figsize=(15,8))
    plt.plot(time_series, ipc_imputed, label='Imputed ipc', alpha=0.7)
    plt.plot(time_series, ipc_original, label='Original ipc', alpha=0.7)
    plt.legend()
    plt.xlabel('Instruction Index')
    plt.ylabel('ipc')
    plt.title(f'ipc: Original vs Imputed ({model_name})')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plot_file = os.path.join(save_path, f"ipc_imputation_{model_name}.pdf")
    plt.savefig(plot_file)
    plt.close()
    logger.info(f"Plot saved to {plot_file}")

def main():
    """ Main function to run the script. """
    args = parse_arguments()
    data_path = args.data_path
    model_name = args.model
    save_path = args.save_path
    output_csv = os.path.join(save_path, f'imputed_data_{model_name}.csv')
    if os.path.exists(output_csv) and os.path.getsize(output_csv) > 0:
        print("Imputed_data exists {output_csv}.")
        return
    scaler_path = args.scaler_path
    device = args.device
    model_path = args.model_path
    time_steps = 64  # Set time steps to 64 as per your request
    
    if not os.path.exists(save_path):
        os.makedirs(save_path)
    
    # Load and preprocess the data
    df_origin = load_data(data_path)
    # Data preview
    data_preview(df_origin)
    
    # Preprocess the data
    X_model, scaler, missing_ipc_mask, df_original = preprocess_data(df_origin, time_steps=time_steps)
    
    # Save the scaler for future use
    joblib.dump(scaler, os.path.join(save_path, scaler_path))
    
    num_samples = X_model.shape[0]
    num_steps = X_model.shape[1]  # Should be 64
    num_features = X_model.shape[2]
    
    # Prepare the dataset
    dataset = {"X": X_model}
    
    # Initialize the model
    model = initialize_model(num_steps, num_features, model_name, save_path, device)
    
    if model_path and os.path.exists(model_path):
        # Load the model and perform imputation
        imputation = load_and_impute(model, dataset, model_path)
    else:
        # Train the model and perform imputation
        model = train_model(model, dataset)
        imputation = model.predict(dataset)
    
    # Inverse transform the imputed data to original scale
    imputation_inverse = inverse_transform_imputation(imputation, scaler)
    
    # Replace missing 'ipc' values in the original DataFrame
    df_imputed = replace_missing_ipc(df_original, imputation_inverse, missing_ipc_mask)
    
    # Save the imputed DataFrame to CSV
    output_csv = os.path.join(save_path, f'imputed_data_{model_name}.csv')
    df_imputed.to_csv(output_csv, index=True)
    logger.info(f"Imputed data saved to {output_csv}")
    
    # Plot the results if the flag is set
    if args.plot_results:
        plot_results(df_imputed, df_origin, save_path, model_name)
    
    print(f"Imputation with model {model_name} completed.")
    print(f"Imputed data saved to {output_csv}")

if __name__ == "__main__":
    main()
