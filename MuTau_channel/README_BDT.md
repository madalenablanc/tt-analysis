# Python BDT Framework for MuTau Channel

Modern Python implementation of BDT training and application using XGBoost, replacing TMVA C++ workflow.

## Features

- **XGBoost BDT** - State-of-the-art gradient boosting (superior to TMVA)
- **Pure Python** - No C++ compilation needed
- **Easy to modify** - Simple parameter tuning and feature engineering
- **Publication-quality plots** - Matplotlib/Seaborn visualizations
- **Compatible with existing workflow** - Can read/write ROOT files

## Installation

```bash
cd /Users/utilizador/cernbox/tau_analysis/MuTau_channel/

# Install dependencies
pip install -r requirements_bdt.txt

# Or with conda:
conda install -c conda-forge xgboost scikit-learn matplotlib seaborn uproot awkward
```

## Quick Start

### 1. Train the BDT

```bash
python train_bdt.py
```

**Input files (edit in script if needed):**
- `MuTau_sinal_SM_2018_july.root` - Signal
- `background_total-protons_syst.root` - Combined background (DY + QCD + TTJets)

**Outputs:**
- `bdt_model_mutau.json` - Trained XGBoost model
- `bdt_features.json` - List of features used
- `bdt_output/roc_curve.png` - ROC curve
- `bdt_output/bdt_output_distribution.png` - BDT score distributions
- `bdt_output/feature_importance.png` - Feature importance plot
- `bdt_output/metrics.json` - Performance metrics

### 2. Apply BDT to Samples

Apply the trained model to each sample:

```bash
# DY sample
python apply_bdt.py \
  --input /path/to/DY_2018_UL_MuTau_nano_merged_pileup_protons.root \
  --output bdt_scores_dy.pkl

# QCD sample
python apply_bdt.py \
  --input /path/to/QCD_2018_UL_MuTau_nano_merged_proton_vars.root \
  --output bdt_scores_qcd.pkl

# TTJets sample
python apply_bdt.py \
  --input /path/to/ttjets_2018_UL_MuTau_nano_merged_pileup_protons.root \
  --output bdt_scores_ttjets.pkl

# Signal sample
python apply_bdt.py \
  --input MuTau_sinal_SM_2018_july.root \
  --output bdt_scores_signal.pkl

# Data sample
python apply_bdt.py \
  --input /path/to/Data_2018_UL_MuTau_nano_merged_proton_vars.root \
  --output bdt_scores_data.pkl
```

**Note:** ROOT files are also created automatically (e.g., `bdt_scores_dy.root`)

### 3. Create Plots

```bash
python plot_bdt_output.py \
  --dy bdt_scores_dy.pkl \
  --qcd bdt_scores_qcd.pkl \
  --ttjets bdt_scores_ttjets.pkl \
  --signal bdt_scores_signal.pkl \
  --data bdt_scores_data.pkl \
  --output-dir bdt_plots \
  --log
```

**Outputs:**
- `bdt_plots/bdt_output_stacked.png` - Stacked histogram (data vs MC)
- `bdt_plots/bdt_signal_vs_background.png` - Signal vs background separation
- `bdt_plots/bdt_summary.txt` - Summary statistics

## BDT Configuration

The BDT uses parameters matching the TMVA TauTau configuration:

```python
params = {
    'max_depth': 3,              # MaxDepth=3 (from TMVA)
    'eta': 0.1,                  # Learning rate
    'subsample': 0.5,            # BaggedSampleFraction=0.5
    'n_estimators': 850,         # NTrees=850 (from TMVA)
}
```

### Input Features (9 variables, matching TMVA):

1. `sist_acop` - Central system acoplanarity
2. `sist_pt` - Total momentum (GeV)
3. `sist_mass` - Invariant mass (GeV)
4. `sist_rap` - System rapidity
5. `met_pt` - Missing energy (GeV)
6. `mu_pt` - Muon pT (GeV)
7. `tau_pt` - Tau pT (GeV)
8. `mass_matching` - `sist_mass - sqrt(13000² × xi_arm1_1 × xi_arm2_1)` (GeV)
9. `rap_matching` - `sist_rap - 0.5 × log(xi_arm1_1 / xi_arm2_1)`

## Customization

### Modify BDT Parameters

Edit `train_bdt.py`:

```python
params = {
    'max_depth': 5,              # Increase tree depth
    'eta': 0.05,                 # Decrease learning rate
    'subsample': 0.8,            # Increase sample fraction
    # ... add more parameters
}

# Change number of trees
num_boost_round=1000
```

### Add/Remove Features

Edit feature list in `train_bdt.py`:

```python
features = [
    'sist_acop',
    'sist_pt',
    # Add new feature:
    'new_variable_name',
    # Comment out to remove:
    # 'tau_pt',
]
```

### Change Weights

Modify sample weights in `plot_bdt_output.py`:

```python
# Adjust in create_stacked_histogram()
bkg_colors = {
    'ttjets': '#2ecc71',  # Green
    'qcd': '#e74c3c',     # Red
    'dy': '#f39c12'       # Orange
}
```

## Advantages Over TMVA

1. **No compilation** - Instant modifications, no waiting for compilation
2. **Better performance** - XGBoost typically outperforms TMVA BDT
3. **Modern tools** - Easy hyperparameter tuning, cross-validation
4. **Better plots** - Matplotlib for publication-quality figures
5. **Easier debugging** - Python error messages vs C++ segfaults
6. **Integration** - Works seamlessly with your existing Python scripts

## Comparison with TMVA Workflow

| Step | TMVA (C++) | Python (XGBoost) |
|------|-----------|------------------|
| Training | `root -l TMVAClassification.C` | `python train_bdt.py` |
| Application | Edit file, recompile, run 5× | Single command per sample |
| Plotting | Compile C++ scripts | `python plot_bdt_output.py` |
| Modify params | Edit .C file, recompile | Edit .py file, run |
| Debug | Segfaults, cryptic errors | Clear Python errors |

## File Structure

```
MuTau_channel/
├── train_bdt.py              # Training script
├── apply_bdt.py              # Application script
├── plot_bdt_output.py        # Visualization script
├── requirements_bdt.txt      # Python dependencies
├── README_BDT.md            # This file
├── bdt_model_mutau.json     # Trained model (generated)
├── bdt_features.json        # Feature list (generated)
├── bdt_output/              # Training outputs (generated)
│   ├── roc_curve.png
│   ├── bdt_output_distribution.png
│   ├── feature_importance.png
│   └── metrics.json
└── bdt_plots/               # Final plots (generated)
    ├── bdt_output_stacked.png
    ├── bdt_signal_vs_background.png
    └── bdt_summary.txt
```

## Troubleshooting

### "No module named 'uproot'"
```bash
pip install uproot awkward
```

### "No module named 'xgboost'"
```bash
pip install xgboost
```

### ROOT file reading issues
If uproot fails, the scripts will automatically fall back to PyROOT. Make sure ROOT is properly installed and Python can find it.

### Memory issues with large files
Add `max_events` parameter in `load_data_uproot()` to limit events:
```python
df = load_data_uproot(file_path, max_events=100000)
```

## Performance Tips

1. **Use uproot** - Much faster than PyROOT for large files
2. **Cache results** - BDT scores are saved as pickle files for reuse
3. **Parallel processing** - Apply BDT to multiple files in parallel:
   ```bash
   python apply_bdt.py --input file1.root --output out1.pkl &
   python apply_bdt.py --input file2.root --output out2.pkl &
   wait
   ```

## Contact

For questions about this Python BDT framework, check the original TMVA implementation in:
- `/Users/utilizador/cernbox/tau_analysis/TauTau_Channel/TMVAClassification.C`

The Python version replicates the TMVA configuration while providing modern tools and better usability.
