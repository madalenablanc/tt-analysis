#!/usr/bin/env python3
"""
BDT Training Script for MuTau Channel using XGBoost
Based on TMVA configuration from TauTau channel
"""

import numpy as np
import pandas as pd
import pickle
import json
from pathlib import Path
import matplotlib.pyplot as plt
import seaborn as sns

try:
    import uproot
except ImportError:
    print("Warning: uproot not available, falling back to ROOT")
    import ROOT

import xgboost as xgb
from sklearn.model_selection import train_test_split
from sklearn.metrics import roc_curve, auc, roc_auc_score
import warnings
warnings.filterwarnings('ignore')


def load_data_uproot(file_path, tree_name='tree', max_events=None):
    """Load data using uproot (faster, no ROOT dependency)"""
    print(f"Loading {file_path} using uproot...")

    with uproot.open(file_path) as file:
        tree = file[tree_name]

        # Define branches to read
        branches = [
            'sist_acop', 'sist_pt', 'sist_mass', 'sist_rap',
            'met_pt', 'mu_pt', 'tau_pt',
            'xi_arm1_1', 'xi_arm2_1', 'weight'
        ]

        # Read data
        data = tree.arrays(branches, library="pd", entry_stop=max_events)

    return data


def load_data_root(file_path, tree_name='tree', max_events=None):
    """Load data using PyROOT (fallback method)"""
    print(f"Loading {file_path} using ROOT...")

    file = ROOT.TFile.Open(file_path)
    if not file or file.IsZombie():
        raise RuntimeError(f"Cannot open file: {file_path}")

    tree = file.Get(tree_name)
    if not tree:
        raise RuntimeError(f"Cannot find tree '{tree_name}' in {file_path}")

    # Read data into lists
    data = {
        'sist_acop': [],
        'sist_pt': [],
        'sist_mass': [],
        'sist_rap': [],
        'met_pt': [],
        'mu_pt': [],
        'tau_pt': [],
        'xi_arm1_1': [],
        'xi_arm2_1': [],
        'weight': []
    }

    n_entries = tree.GetEntries() if max_events is None else min(max_events, tree.GetEntries())

    for i in range(n_entries):
        tree.GetEntry(i)
        if i % 10000 == 0:
            print(f"  Processing event {i}/{n_entries}")

        for key in data.keys():
            if hasattr(tree, key):
                data[key].append(getattr(tree, key))
            else:
                data[key].append(0.0)

    file.Close()
    return pd.DataFrame(data)


def compute_features(df):
    """Compute derived features matching TMVA variables"""
    df = df.copy()

    # Mass matching: sist_mass - sqrt(13000^2 * xi_arm1_1 * xi_arm2_1)
    df['mass_matching'] = df['sist_mass'] - np.sqrt(13000.0**2 * df['xi_arm1_1'] * df['xi_arm2_1'])

    # Rapidity matching: sist_rap - 0.5*log(xi_arm1_1/xi_arm2_1)
    # Handle division by zero and log of negative/zero
    xi_ratio = df['xi_arm1_1'] / df['xi_arm2_1'].replace(0, np.nan)
    df['rap_matching'] = df['sist_rap'] - 0.5 * np.log(xi_ratio.clip(lower=1e-10))
    df['rap_matching'] = df['rap_matching'].fillna(0)

    return df


def prepare_data(signal_file, background_file, test_size=0.3, random_state=42):
    """Load and prepare training data"""

    # Try uproot first, fallback to ROOT
    try:
        sig_df = load_data_uproot(signal_file)
        bkg_df = load_data_uproot(background_file)
    except:
        sig_df = load_data_root(signal_file)
        bkg_df = load_data_root(background_file)

    print(f"\nLoaded {len(sig_df)} signal events")
    print(f"Loaded {len(bkg_df)} background events")

    # Compute derived features
    sig_df = compute_features(sig_df)
    bkg_df = compute_features(bkg_df)

    # Add labels
    sig_df['label'] = 1
    bkg_df['label'] = 0

    # Combine
    df = pd.concat([sig_df, bkg_df], ignore_index=True)

    # Feature list (matching TMVA variables)
    features = [
        'sist_acop',      # Central system acoplanarity
        'sist_pt',        # Total momentum
        'sist_mass',      # Invariant mass
        'sist_rap',       # System rapidity
        'met_pt',         # Missing energy
        'mu_pt',          # Muon pT
        'tau_pt',         # Tau pT
        'mass_matching',  # Invariant mass matching
        'rap_matching'    # Rapidity matching
    ]

    # Clean data
    df = df.replace([np.inf, -np.inf], np.nan)
    df = df.dropna(subset=features + ['label', 'weight'])

    print(f"\nAfter cleaning: {len(df)} total events")
    print(f"  Signal: {df['label'].sum()}")
    print(f"  Background: {(df['label'] == 0).sum()}")

    # Split into train/test
    X = df[features]
    y = df['label']
    weights = df['weight'].abs()  # Use absolute weights

    X_train, X_test, y_train, y_test, w_train, w_test = train_test_split(
        X, y, weights, test_size=test_size, random_state=random_state, stratify=y
    )

    return X_train, X_test, y_train, y_test, w_train, w_test, features


def train_bdt(X_train, y_train, w_train, X_test, y_test, w_test):
    """
    Train XGBoost BDT with parameters matching TMVA configuration:
    - NTrees=850
    - MaxDepth=3
    - AdaBoost equivalent
    """

    print("\n" + "="*60)
    print("Training XGBoost BDT")
    print("="*60)

    # XGBoost parameters matching TMVA BDT configuration
    params = {
        'max_depth': 3,                    # MaxDepth=3
        'eta': 0.1,                        # Learning rate (similar to AdaBoost)
        'objective': 'binary:logistic',    # Binary classification
        'eval_metric': 'auc',              # Area under ROC curve
        'subsample': 0.5,                  # BaggedSampleFraction=0.5
        'colsample_bytree': 1.0,           # Use all features
        'min_child_weight': 1,             # MinNodeSize
        'gamma': 0,                        # Minimum loss reduction
        'lambda': 1,                       # L2 regularization
        'alpha': 0,                        # L1 regularization
        'seed': 42,
        'tree_method': 'hist',             # Faster training
    }

    # Create DMatrix for XGBoost
    dtrain = xgb.DMatrix(X_train, label=y_train, weight=w_train)
    dtest = xgb.DMatrix(X_test, label=y_test, weight=w_test)

    # Evaluation list
    evals = [(dtrain, 'train'), (dtest, 'test')]

    # Train model
    print("\nTraining parameters:")
    for key, value in params.items():
        print(f"  {key}: {value}")

    bdt_model = xgb.train(
        params,
        dtrain,
        num_boost_round=850,  # NTrees=850
        evals=evals,
        early_stopping_rounds=50,
        verbose_eval=50
    )

    print(f"\nBest iteration: {bdt_model.best_iteration}")
    print(f"Best score: {bdt_model.best_score:.4f}")

    return bdt_model


def evaluate_model(model, X_train, y_train, w_train, X_test, y_test, w_test, features, output_dir):
    """Evaluate model and create diagnostic plots"""

    print("\n" + "="*60)
    print("Evaluating Model")
    print("="*60)

    # Create output directory
    output_dir = Path(output_dir)
    output_dir.mkdir(exist_ok=True)

    # Predictions
    dtrain = xgb.DMatrix(X_train, label=y_train, weight=w_train)
    dtest = xgb.DMatrix(X_test, label=y_test, weight=w_test)

    y_pred_train = model.predict(dtrain)
    y_pred_test = model.predict(dtest)

    # ROC curve
    fpr_train, tpr_train, _ = roc_curve(y_train, y_pred_train, sample_weight=w_train)
    fpr_test, tpr_test, _ = roc_curve(y_test, y_pred_test, sample_weight=w_test)

    auc_train = auc(fpr_train, tpr_train)
    auc_test = auc(fpr_test, tpr_test)

    print(f"\nROC AUC (train): {auc_train:.4f}")
    print(f"ROC AUC (test):  {auc_test:.4f}")

    # Plot ROC curve
    plt.figure(figsize=(8, 6))
    plt.plot(fpr_train, tpr_train, 'b-', label=f'Train (AUC = {auc_train:.3f})', linewidth=2)
    plt.plot(fpr_test, tpr_test, 'r-', label=f'Test (AUC = {auc_test:.3f})', linewidth=2)
    plt.plot([0, 1], [0, 1], 'k--', label='Random', linewidth=1)
    plt.xlabel('False Positive Rate', fontsize=12)
    plt.ylabel('True Positive Rate', fontsize=12)
    plt.title('ROC Curve - MuTau Channel BDT', fontsize=14, fontweight='bold')
    plt.legend(loc='lower right', fontsize=11)
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(output_dir / 'roc_curve.png', dpi=300)
    plt.close()
    print(f"Saved ROC curve to {output_dir / 'roc_curve.png'}")

    # Plot BDT output distribution
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))

    # Train
    bins = np.linspace(0, 1, 50)
    ax1.hist(y_pred_train[y_train == 1], bins=bins, alpha=0.6, label='Signal',
             color='blue', weights=w_train[y_train == 1], density=True)
    ax1.hist(y_pred_train[y_train == 0], bins=bins, alpha=0.6, label='Background',
             color='red', weights=w_train[y_train == 0], density=True)
    ax1.set_xlabel('BDT Output', fontsize=12)
    ax1.set_ylabel('Normalized Events', fontsize=12)
    ax1.set_title('Train Sample', fontsize=13, fontweight='bold')
    ax1.legend(fontsize=11)
    ax1.grid(True, alpha=0.3)
    ax1.set_yscale('log')

    # Test
    ax2.hist(y_pred_test[y_test == 1], bins=bins, alpha=0.6, label='Signal',
             color='blue', weights=w_test[y_test == 1], density=True)
    ax2.hist(y_pred_test[y_test == 0], bins=bins, alpha=0.6, label='Background',
             color='red', weights=w_test[y_test == 0], density=True)
    ax2.set_xlabel('BDT Output', fontsize=12)
    ax2.set_ylabel('Normalized Events', fontsize=12)
    ax2.set_title('Test Sample', fontsize=13, fontweight='bold')
    ax2.legend(fontsize=11)
    ax2.grid(True, alpha=0.3)
    ax2.set_yscale('log')

    plt.tight_layout()
    plt.savefig(output_dir / 'bdt_output_distribution.png', dpi=300)
    plt.close()
    print(f"Saved BDT distribution to {output_dir / 'bdt_output_distribution.png'}")

    # Feature importance
    importance = model.get_score(importance_type='gain')
    importance_df = pd.DataFrame({
        'feature': importance.keys(),
        'importance': importance.values()
    }).sort_values('importance', ascending=True)

    plt.figure(figsize=(10, 6))
    plt.barh(range(len(importance_df)), importance_df['importance'], color='steelblue')
    plt.yticks(range(len(importance_df)), importance_df['feature'])
    plt.xlabel('Importance (Gain)', fontsize=12)
    plt.ylabel('Feature', fontsize=12)
    plt.title('Feature Importance - MuTau Channel BDT', fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3, axis='x')
    plt.tight_layout()
    plt.savefig(output_dir / 'feature_importance.png', dpi=300)
    plt.close()
    print(f"Saved feature importance to {output_dir / 'feature_importance.png'}")

    # Save metrics
    metrics = {
        'auc_train': float(auc_train),
        'auc_test': float(auc_test),
        'n_trees': int(model.best_iteration),
        'feature_importance': {k: float(v) for k, v in importance.items()}
    }

    with open(output_dir / 'metrics.json', 'w') as f:
        json.dump(metrics, f, indent=2)
    print(f"Saved metrics to {output_dir / 'metrics.json'}")


def main():
    """Main training workflow"""

    # Configuration
    SIGNAL_FILE = "MuTau_sinal_SM_2018_july.root"
    BACKGROUND_FILE = "background_total-protons_syst.root"
    OUTPUT_DIR = "bdt_output"
    MODEL_FILE = "bdt_model_mutau.json"

    print("="*60)
    print("MuTau Channel BDT Training")
    print("="*60)

    # Load and prepare data
    X_train, X_test, y_train, y_test, w_train, w_test, features = prepare_data(
        SIGNAL_FILE, BACKGROUND_FILE
    )

    print(f"\nTraining features: {features}")
    print(f"\nTraining set size: {len(X_train)}")
    print(f"Test set size: {len(X_test)}")

    # Train BDT
    model = train_bdt(X_train, y_train, w_train, X_test, y_test, w_test)

    # Evaluate
    evaluate_model(model, X_train, y_train, w_train, X_test, y_test, w_test,
                   features, OUTPUT_DIR)

    # Save model
    model.save_model(MODEL_FILE)
    print(f"\nSaved model to {MODEL_FILE}")

    # Save feature list
    with open('bdt_features.json', 'w') as f:
        json.dump({'features': features}, f, indent=2)
    print(f"Saved feature list to bdt_features.json")

    print("\n" + "="*60)
    print("Training Complete!")
    print("="*60)


if __name__ == "__main__":
    main()
