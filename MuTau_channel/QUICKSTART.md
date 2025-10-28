# Quick Start Guide - Python BDT on lxplus

Choose the method that works best for you:

## Method 1: LCG Setup (RECOMMENDED - Fastest)

Uses CERN's pre-compiled packages from CVMFS. Much faster!

```bash
cd /eos/home-m/mblancco/tau_analysis/MuTau_channel/
chmod +x setup_bdt_simple.sh
./setup_bdt_simple.sh
```

**Activate:**
```bash
source /eos/home-m/mblancco/activate_bdt_lcg.sh
```

✅ **Advantages:**
- Very fast (no compilation)
- Uses CERN's optimized packages
- Minimal disk usage

---

## Method 2: Virtual Environment (Full Control)

Creates isolated Python environment in EOS.

```bash
cd /eos/home-m/mblancco/tau_analysis/MuTau_channel/
chmod +x setup_bdt_environment.sh
./setup_bdt_environment.sh
```

**Activate:**
```bash
source /eos/home-m/mblancco/python_venv/bdt_mutau/bin/activate
```

✅ **Advantages:**
- Full control over package versions
- Isolated environment
- Can upgrade packages easily

⚠️ **Note:** Installation can be slow (~10-15 min)

---

## Method 3: Manual Install (If scripts fail)

```bash
# Load LCG environment
source /cvmfs/sft.cern.ch/lcg/views/LCG_105/x86_64-el9-gcc13-opt/setup.sh

# Install only missing packages
mkdir -p /eos/home-m/mblancco/python_packages_extra
pip install --target=/eos/home-m/mblancco/python_packages_extra --no-cache-dir \
    xgboost uproot awkward seaborn

# Add to path
export PYTHONPATH="/eos/home-m/mblancco/python_packages_extra:$PYTHONPATH"
```

**To make permanent, add to ~/.bashrc:**
```bash
echo 'source /cvmfs/sft.cern.ch/lcg/views/LCG_105/x86_64-el9-gcc13-opt/setup.sh' >> ~/.bashrc
echo 'export PYTHONPATH="/eos/home-m/mblancco/python_packages_extra:$PYTHONPATH"' >> ~/.bashrc
```

---

## Quick Test

After activation, test it works:

```bash
python -c "import xgboost; print('XGBoost:', xgboost.__version__)"
python -c "import uproot; print('uproot:', uproot.__version__)"
```

If both print version numbers, you're ready to go!

---

## Run BDT Workflow

```bash
# 1. Train BDT
python train_bdt.py

# 2. Apply to samples
python apply_bdt.py --input your_file.root --output output.pkl

# 3. Make plots
python plot_bdt_output.py --signal signal.pkl --dy dy.pkl --log
```

---

## Troubleshooting

### Script gets stuck during installation
- Press Ctrl+C
- Try **Method 1 (LCG)** instead - it's much faster
- Or use **Method 3 (Manual)**

### "No module named X" error
```bash
# Make sure environment is activated
source /eos/home-m/mblancco/activate_bdt_lcg.sh

# Check PYTHONPATH
echo $PYTHONPATH
```

### Network timeout
```bash
# Use shorter timeout
pip install --timeout=30 --retries=1 xgboost
```

### Still getting AFS quota errors?
Make sure you're installing to EOS:
```bash
# Should show EOS path, not /afs/
pip show xgboost | grep Location
```

---

## Recommended: Method 1 (LCG)

**For most users, use Method 1 (LCG setup)**:
- ✅ Fastest installation (< 2 minutes)
- ✅ Uses CERN's optimized packages
- ✅ Minimal storage required
- ✅ Most reliable on lxplus

Just run:
```bash
./setup_bdt_simple.sh
source /eos/home-m/mblancco/activate_bdt_lcg.sh
python train_bdt.py
```

Done! 🎉
