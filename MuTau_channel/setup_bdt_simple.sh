#!/bin/bash
# Simplified setup using LCG (CERN's curated Python stack)
# Much faster than pip install, already compiled for lxplus

echo "=========================================="
echo "Setting up BDT Environment (LCG method)"
echo "=========================================="
echo ""

# Use LCG_105 which has Python 3.11 and most packages we need
LCG_VIEW="/cvmfs/sft.cern.ch/lcg/views/LCG_105/x86_64-el9-gcc13-opt"

if [ ! -d "$LCG_VIEW" ]; then
    echo "ERROR: LCG view not found at $LCG_VIEW"
    echo "Trying alternative LCG version..."
    LCG_VIEW="/cvmfs/sft.cern.ch/lcg/views/LCG_104/x86_64-centos7-gcc11-opt"
fi

if [ ! -d "$LCG_VIEW" ]; then
    echo "ERROR: No LCG view found. Are you on lxplus?"
    exit 1
fi

echo "Using LCG view: $LCG_VIEW"
echo ""

# Source LCG environment
source ${LCG_VIEW}/setup.sh

echo "Python version:"
python --version
echo ""

# Check what's available
echo "Checking available packages..."

python << 'EOF'
import sys

packages = {
    'numpy': 'numpy',
    'pandas': 'pandas',
    'matplotlib': 'matplotlib',
    'sklearn': 'scikit-learn',
    'ROOT': 'ROOT (PyROOT)'
}

available = []
missing = []

for module, name in packages.items():
    try:
        exec(f"import {module}")
        version = eval(f"{module}.__version__")
        available.append(f"✓ {name}: {version}")
    except ImportError:
        missing.append(f"✗ {name}: NOT FOUND")

for pkg in available:
    print(pkg)
for pkg in missing:
    print(pkg)

if missing:
    print("\nSome packages missing, will install with pip...")
    sys.exit(1)
else:
    print("\nAll basic packages available in LCG!")
    sys.exit(0)
EOF

LCG_STATUS=$?

# Install only missing packages to EOS
EOS_HOME="/eos/home-m/mblancco"
EXTRA_PACKAGES="${EOS_HOME}/python_packages_extra"

if [ $LCG_STATUS -ne 0 ]; then
    echo ""
    echo "Installing missing packages to ${EXTRA_PACKAGES}..."
    mkdir -p "$EXTRA_PACKAGES"

    # Install only what's missing (xgboost, uproot, awkward, seaborn)
    pip install --target="$EXTRA_PACKAGES" --no-cache-dir --timeout=60 \
        xgboost uproot awkward seaborn 2>&1 | grep -v "WARNING: There was an error"

    export PYTHONPATH="${EXTRA_PACKAGES}:${PYTHONPATH}"
fi

# Verify everything works
echo ""
echo "=========================================="
echo "Verifying installation..."
echo "=========================================="

python << 'EOF'
import sys
extra_path = "/eos/home-m/mblancco/python_packages_extra"
if extra_path not in sys.path:
    sys.path.insert(0, extra_path)

packages = {
    'xgboost': 'XGBoost',
    'sklearn': 'scikit-learn',
    'numpy': 'NumPy',
    'pandas': 'Pandas',
    'matplotlib': 'Matplotlib',
    'seaborn': 'Seaborn',
    'uproot': 'uproot',
    'awkward': 'awkward'
}

all_ok = True
for module, name in packages.items():
    try:
        mod = __import__(module)
        version = getattr(mod, '__version__', 'unknown')
        print(f"✓ {name}: {version}")
    except ImportError as e:
        print(f"✗ {name}: NOT INSTALLED")
        all_ok = False

if all_ok:
    print("\n✓ All packages installed successfully!")
else:
    print("\n✗ Some packages failed to install")
    sys.exit(1)
EOF

if [ $? -eq 0 ]; then
    # Create activation script
    ACTIVATE_SCRIPT="${EOS_HOME}/activate_bdt_lcg.sh"
    cat > "$ACTIVATE_SCRIPT" << EOF
#!/bin/bash
# Activate BDT environment using LCG
source ${LCG_VIEW}/setup.sh
export PYTHONPATH="${EXTRA_PACKAGES}:\${PYTHONPATH}"
echo "BDT Environment activated (LCG method)!"
echo "Python: \$(which python)"
EOF
    chmod +x "$ACTIVATE_SCRIPT"

    echo ""
    echo "=========================================="
    echo "Setup Complete!"
    echo "=========================================="
    echo ""
    echo "To use this environment, run:"
    echo "  source ${ACTIVATE_SCRIPT}"
    echo ""
    echo "Or add to your scripts:"
    echo "  source ${LCG_VIEW}/setup.sh"
    echo "  export PYTHONPATH=${EXTRA_PACKAGES}:\${PYTHONPATH}"
    echo ""
else
    echo ""
    echo "ERROR: Setup failed"
    exit 1
fi
