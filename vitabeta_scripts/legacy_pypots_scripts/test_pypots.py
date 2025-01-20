import numpy as np
from sklearn.preprocessing import StandardScaler
from pygrinder import mcar
from pypots.data import load_specific_dataset
from pypots.imputation import SAITS
from pypots.utils.metrics import calc_mae

# Data preprocessing. Tedious, but PyPOTS can help. 🤓
data = load_specific_dataset('physionet_2012')  # PyPOTS will automatically download and extract it.
X = data['train_X']
num_samples = len(X)
X = StandardScaler().fit_transform(X.reshape(-1, X.shape[-1])).reshape(X.shape)
X_ori = X  # keep X_ori for validation
X = mcar(X, 0.1)  # randomly hold out 10% observed values as ground truth
dataset = {"X": X}  # X for model input
print(X.shape)  # (7671, 48, 37), 7671 samples, 48 time steps, 37 features

# initialize the model
saits = SAITS(
    n_steps=48,
    n_features=37,
    n_layers=2,
    d_model=256,
    d_ffn=128,
    n_heads=4,
    d_k=64,
    d_v=64,
    dropout=0.1,
    epochs=10,
    saving_path="examples/saits", # set the path for saving tensorboard logging file and model checkpoint
    model_saving_strategy="best", # only save the model with the best validation performance
)

# train the model. Here I consider the train dataset only, and evaluate on it, because ground truth is not visible to the model.
saits.fit(dataset)
# impute the originally-missing values and artificially-missing values
imputation = saits.impute(dataset)
# calculate mean absolute error on the ground truth (artificially-missing values)
indicating_mask = np.isnan(X) ^ np.isnan(X_ori)  # indicating mask for imputation error calculation
mae = calc_mae(imputation, np.nan_to_num(X_ori), indicating_mask)  # calculate mean absolute error on the ground truth (artificially-missing values)

# the best model has been already saved, but you can still manually save it with function save_model() as below
saits.save(saving_path="examples/saits/manually_saved_saits_model")
# you can load the saved model into a new initialized model
saits.load("examples/saits/manually_saved_saits_model.pypots")
