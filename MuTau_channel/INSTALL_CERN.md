# Installation Guide for CERN lxplus

This guide shows how to install the Python BDT framework on CERN lxplus/EOS without hitting AFS quota limits.

## Problem

Installing Python packages to AFS (`~/.local`) causes quota errors:
```
ERROR: Could not install packages due to an OSError: [Errno 122] Disk quota exceeded: '/afs/cern.ch/user/m/mblancco/.local/...'
```

## Solution

Install packages to your **EOS home directory** which has much more space.

---

## Method 1: Automated Setup (Recommended)

### Step 1: Run the Setup Script

```bash
cd /eos/home-m/mblancco/tau_analysis/MuTau_channel/
chmod +x setup_bdt_environment.sh
./setup_bdt_environment.sh
```

This will:
- Create a Python virtual environment in `/eos/home-m/mblancco/python_venv/bdt_mutau/`
- Install all required packages (xgboost, scikit-learn, matplotlib, etc.)
- Verify the installation
- Create an activation script

### Step 2: Activate the Environment

Every time you want to use the BDT framework:

```bash
source /eos/home-m/mblancco/python_venv/bdt_mutau/bin/activate
```

Or use the shortcut:

```bash
source /eos/home-m/mblancco/activate_bdt_env.sh
```

### Step 3: Run Your Scripts

```bash
# Now you can run the BDT scripts
python train_bdt.py
python apply_bdt.py --input file.root --output output.pkl
python plot_bdt_output.py --dy dy.pkl --signal signal.pkl
```

### Step 4: Deactivate When Done

```bash
deactivate
```

---

## Method 2: Manual Setup

If you prefer manual control:

### Create Virtual Environment in EOS

```bash
# Create directory for virtual environments
mkdir -p /eos/home-m/mblancco/python_venv

# Create virtual environment
python3 -m venv /eos/home-m/mblancco/python_venv/bdt_mutau

# Activate it
source /eos/home-m/mblancco/python_venv/bdt_mutau/bin/activate
```

### Install Packages

```bash
# Upgrade pip first
pip install --upgrade pip

# Install packages one by one (or use requirements_bdt.txt)
pip install xgboost
pip install scikit-learn
pip install numpy pandas
pip install matplotlib seaborn
pip install uproot awkward
```

Or all at once:

```bash
pip install -r requirements_bdt.txt
```

---

## Method 3: Use pip with --target (Alternative)

Install directly to a custom directory without virtual environment:

```bash
# Create custom directory
mkdir -p /eos/home-m/mblancco/python_packages

# Install packages there
pip install --target=/eos/home-m/mblancco/python_packages \
    xgboost scikit-learn numpy pandas matplotlib seaborn uproot awkward

# Add to PYTHONPATH (add to ~/.bashrc to make permanent)
export PYTHONPATH="/eos/home-m/mblancco/python_packages:$PYTHONPATH"
```

---

## Add to Your ~/.bashrc (Optional)

Make activation easier by adding an alias to your `~/.bashrc`:

```bash
# Edit ~/.bashrc
nano ~/.bashrc

# Add this line:
alias activate_bdt='source /eos/home-m/mblancco/python_venv/bdt_mutau/bin/activate'

# Save and reload
source ~/.bashrc
```

Now you can just type `activate_bdt` to activate the environment!

---

## Verification

After installation, verify everything works:

```bash
# Activate environment
source /eos/home-m/mblancco/python_venv/bdt_mutau/bin/activate

# Test imports
python -c "import xgboost; print('XGBoost version:', xgboost.__version__)"
python -c "import sklearn; print('scikit-learn version:', sklearn.__version__)"
python -c "import uproot; print('uproot version:', uproot.__version__)"
python -c "import matplotlib; print('matplotlib version:', matplotlib.__version__)"

# If all print version numbers, you're good to go!
```

---

## Troubleshooting

### "python3: command not found"

Load a Python module first:

```bash
# On lxplus
module load python/3.9
# or
module load python/3.11
```

### "No module named venv"

Install python3-venv (you might need admin help) or use Method 3 with `--target`.

### Still getting quota errors?

Check your disk usage:

```bash
# Check AFS quota
fs quota ~

# Check EOS quota
eos quota /eos/home-m/mblancco
```

Make sure you're installing to EOS, not AFS!

### Permission denied on EOS

Make sure the directory is writable:

```bash
chmod -R u+w /eos/home-m/mblancco/python_venv
```

---

## Storage Locations

| Location | Quota | Speed | Use For |
|----------|-------|-------|---------|
| AFS (`~/.local`) | ~10 GB | Fast | Small files only ⚠️ |
| EOS (`/eos/home-m/...`) | 1 TB | Medium | Python packages ✅ |
| Work area | Large | Fast | Temporary files |

---

## Complete Workflow on lxplus

```bash
# 1. Login to lxplus
ssh mblancco@lxplus.cern.ch

# 2. Navigate to work directory
cd /eos/home-m/mblancco/tau_analysis/MuTau_channel/

# 3. Setup environment (first time only)
chmod +x setup_bdt_environment.sh
./setup_bdt_environment.sh

# 4. Activate environment (every session)
source /eos/home-m/mblancco/python_venv/bdt_mutau/bin/activate

# 5. Run your analysis
python train_bdt.py

# 6. Apply BDT
python apply_bdt.py --input /eos/home-m/mblancco/samples_2018_mutau/DY_2018_UL_MuTau_nano_merged_pileup_protons.root --output bdt_scores_dy.pkl

# 7. Create plots
python plot_bdt_output.py --dy bdt_scores_dy.pkl --signal bdt_scores_signal.pkl --log

# 8. Deactivate when done
deactivate
```

---

## Notes

- Virtual environment is ~300 MB (well within EOS quota)
- Installation takes ~5-10 minutes
- Environment persists between sessions
- Can be shared across multiple projects
- No ROOT installation required (uproot handles ROOT files)

---

## Questions?

If you encounter issues, check:

1. Are you installing to EOS? (`/eos/home-m/mblancco/...`)
2. Is the virtual environment activated? (should see `(bdt_mutau)` in prompt)
3. Do you have write permissions? (`ls -la /eos/home-m/mblancco/`)

The setup script should handle most common issues automatically!
