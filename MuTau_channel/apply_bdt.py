#!/usr/bin/env python3
"""
Apply trained BDT to data samples for MuTau Channel
Creates histograms of BDT response for signal, background, and data
"""

import numpy as np
import pandas as pd
import json
from pathlib import Path
import argparse

try:
    import uproot
except ImportError:
    print("Warning: uproot not available, falling back to ROOT")
    import ROOT

import xgboost as xgb
import warnings
warnings.filterwarnings('ignore')


def load_data_uproot(file_path, tree_name='tree', max_events=None):
    """Load data using uproot"""
    print(f"Loading {file_path} using uproot...")

    with uproot.open(file_path) as file:
        tree = file[tree_name]

        branches = [
            'sist_acop', 'sist_pt', 'sist_mass', 'sist_rap',
            'met_pt', 'mu_pt', 'tau_pt',
            'xi_arm1_1', 'xi_arm2_1', 'weight'
        ]

        data = tree.arrays(branches, library="pd", entry_stop=max_events)

    return data


def load_data_root(file_path, tree_name='tree', max_events=None):
    """Load data using PyROOT"""
    print(f"Loading {file_path} using ROOT...")

    file = ROOT.TFile.Open(file_path)
    if not file or file.IsZombie():
        raise RuntimeError(f"Cannot open file: {file_path}")

    tree = file.Get(tree_name)
    if not tree:
        raise RuntimeError(f"Cannot find tree '{tree_name}' in {file_path}")

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
    """Compute derived features matching training"""
    df = df.copy()

    # Mass matching
    df['mass_matching'] = df['sist_mass'] - np.sqrt(13000.0**2 * df['xi_arm1_1'] * df['xi_arm2_1'])

    # Rapidity matching
    xi_ratio = df['xi_arm1_1'] / df['xi_arm2_1'].replace(0, np.nan)
    df['rap_matching'] = df['sist_rap'] - 0.5 * np.log(xi_ratio.clip(lower=1e-10))
    df['rap_matching'] = df['rap_matching'].fillna(0)

    return df


def apply_bdt(model, input_file, features, output_file):
    """Apply BDT to input file and save results"""

    print(f"\n{'='*60}")
    print(f"Applying BDT to: {input_file}")
    print(f"{'='*60}")

    # Load data
    try:
        df = load_data_uproot(input_file)
    except:
        df = load_data_root(input_file)

    print(f"Loaded {len(df)} events")

    # Compute features
    df = compute_features(df)

    # Clean data
    df = df.replace([np.inf, -np.inf], np.nan)

    # Keep track of original indices before dropping NaN
    original_length = len(df)
    df_clean = df.dropna(subset=features).copy()
    print(f"After cleaning: {len(df_clean)}/{original_length} events")

    # Prepare features
    X = df_clean[features]

    # Make predictions
    dmatrix = xgb.DMatrix(X)
    bdt_scores = model.predict(dmatrix)

    # Add BDT scores to dataframe
    df_clean['bdt_score'] = bdt_scores

    # Save to pickle for easy loading
    output_path = Path(output_file)
    df_clean.to_pickle(output_path)
    print(f"Saved BDT scores to {output_path}")

    # Save to ROOT file as well (for compatibility)
    root_output = output_path.with_suffix('.root')
    save_to_root(df_clean, root_output)

    return df_clean


def save_to_root(df, output_file):
    """Save dataframe with BDT scores to ROOT file"""
    try:
        import ROOT
        from array import array

        print(f"Saving to ROOT file: {output_file}")

        # Create ROOT file and tree
        root_file = ROOT.TFile(str(output_file), "RECREATE")
        tree = ROOT.TTree("tree", "Tree with BDT scores")

        # Create branches
        branches = {}
        for col in df.columns:
            branches[col] = array('f', [0.])
            tree.Branch(col, branches[col], f"{col}/F")

        # Fill tree
        for idx, row in df.iterrows():
            for col in df.columns:
                branches[col][0] = float(row[col])
            tree.Fill()

        # Write and close
        tree.Write()
        root_file.Close()
        print(f"Saved ROOT file: {output_file}")

    except Exception as e:
        print(f"Warning: Could not save ROOT file: {e}")


def main():
    parser = argparse.ArgumentParser(description='Apply BDT to MuTau data samples')
    parser.add_argument('--model', default='bdt_model_mutau.json',
                        help='Path to trained BDT model')
    parser.add_argument('--features', default='bdt_features.json',
                        help='Path to features JSON file')
    parser.add_argument('--input', required=True,
                        help='Input ROOT file')
    parser.add_argument('--output', required=True,
                        help='Output pickle file')

    args = parser.parse_args()

    print("="*60)
    print("MuTau Channel - Apply BDT")
    print("="*60)

    # Load model
    print(f"\nLoading model from {args.model}")
    model = xgb.Booster()
    model.load_model(args.model)

    # Load features
    print(f"Loading features from {args.features}")
    with open(args.features, 'r') as f:
        feature_config = json.load(f)
        features = feature_config['features']

    print(f"Features: {features}")

    # Apply BDT
    df_result = apply_bdt(model, args.input, features, args.output)

    print(f"\nBDT Score Statistics:")
    print(df_result['bdt_score'].describe())

    print("\n" + "="*60)
    print("Application Complete!")
    print("="*60)


if __name__ == "__main__":
    main()
