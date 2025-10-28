#!/bin/bash
# Setup script for BDT Python environment on CERN infrastructure
# Installs packages to EOS to avoid AFS quota issues

# Set your EOS home directory (edit if needed)
EOS_HOME="/eos/home-m/mblancco"

# Create Python virtual environment in EOS
VENV_DIR="${EOS_HOME}/python_venv/bdt_mutau"

echo "=========================================="
echo "Setting up BDT Python Environment"
echo "=========================================="
echo ""
echo "Installation directory: ${VENV_DIR}"
echo ""

# Create directory if it doesn't exist
mkdir -p "${EOS_HOME}/python_venv"

# Check if virtual environment already exists
if [ -d "$VENV_DIR" ]; then
    echo "Virtual environment already exists at ${VENV_DIR}"
    echo "To recreate it, delete the directory first:"
    echo "  rm -rf ${VENV_DIR}"
    echo ""
    read -p "Do you want to use the existing environment? (y/n) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Exiting..."
        exit 1
    fi
else
    # Create virtual environment
    echo "Creating virtual environment..."
    python3 -m venv "$VENV_DIR"

    if [ $? -ne 0 ]; then
        echo "ERROR: Failed to create virtual environment"
        echo "Make sure python3-venv is installed"
        exit 1
    fi
fi

# Activate virtual environment
echo "Activating virtual environment..."
source "${VENV_DIR}/bin/activate"

# Upgrade pip (with timeout)
echo ""
echo "Upgrading pip..."
pip install --upgrade pip --timeout=30 --no-cache-dir 2>/dev/null || echo "Warning: pip upgrade failed, continuing with existing version"

# Install packages one by one with progress
echo ""
echo "Installing required packages..."
echo "This may take a few minutes..."
echo ""

# Use --no-cache-dir to avoid disk issues and --timeout to prevent hanging
PIP_OPTS="--no-cache-dir --timeout=60 --retries=2"

echo "[1/8] Installing numpy..."
pip install $PIP_OPTS "numpy>=1.21.0,<2.0.0" || { echo "ERROR: Failed to install numpy"; exit 1; }

echo "[2/8] Installing pandas..."
pip install $PIP_OPTS "pandas>=1.3.0,<2.0.0" || { echo "ERROR: Failed to install pandas"; exit 1; }

echo "[3/8] Installing matplotlib..."
pip install $PIP_OPTS "matplotlib>=3.4.0,<4.0.0" || { echo "ERROR: Failed to install matplotlib"; exit 1; }

echo "[4/8] Installing seaborn..."
pip install $PIP_OPTS "seaborn>=0.11.0" || { echo "ERROR: Failed to install seaborn"; exit 1; }

echo "[5/8] Installing scikit-learn..."
pip install $PIP_OPTS "scikit-learn>=1.0.0,<1.4.0" || { echo "ERROR: Failed to install scikit-learn"; exit 1; }

echo "[6/8] Installing xgboost..."
pip install $PIP_OPTS "xgboost>=1.7.0,<2.0.0" || { echo "ERROR: Failed to install xgboost"; exit 1; }

echo "[7/8] Installing awkward..."
pip install $PIP_OPTS "awkward>=2.0.0,<3.0.0" || { echo "ERROR: Failed to install awkward"; exit 1; }

echo "[8/8] Installing uproot..."
pip install $PIP_OPTS "uproot>=5.0.0,<6.0.0" || { echo "ERROR: Failed to install uproot"; exit 1; }

# Check installation
echo ""
echo "=========================================="
echo "Verifying installation..."
echo "=========================================="

python3 << EOF
import sys
try:
    import xgboost
    print("✓ xgboost:", xgboost.__version__)
except ImportError:
    print("✗ xgboost: NOT INSTALLED")
    sys.exit(1)

try:
    import sklearn
    print("✓ scikit-learn:", sklearn.__version__)
except ImportError:
    print("✗ scikit-learn: NOT INSTALLED")
    sys.exit(1)

try:
    import numpy
    print("✓ numpy:", numpy.__version__)
except ImportError:
    print("✗ numpy: NOT INSTALLED")
    sys.exit(1)

try:
    import pandas
    print("✓ pandas:", pandas.__version__)
except ImportError:
    print("✗ pandas: NOT INSTALLED")
    sys.exit(1)

try:
    import matplotlib
    print("✓ matplotlib:", matplotlib.__version__)
except ImportError:
    print("✗ matplotlib: NOT INSTALLED")
    sys.exit(1)

try:
    import seaborn
    print("✓ seaborn:", seaborn.__version__)
except ImportError:
    print("✗ seaborn: NOT INSTALLED")
    sys.exit(1)

try:
    import uproot
    print("✓ uproot:", uproot.__version__)
except ImportError:
    print("✗ uproot: NOT INSTALLED")
    sys.exit(1)

try:
    import awkward
    print("✓ awkward:", awkward.__version__)
except ImportError:
    print("✗ awkward: NOT INSTALLED")
    sys.exit(1)

print("\nAll packages installed successfully!")
EOF

if [ $? -eq 0 ]; then
    echo ""
    echo "=========================================="
    echo "Setup Complete!"
    echo "=========================================="
    echo ""
    echo "To use this environment in the future, run:"
    echo "  source ${VENV_DIR}/bin/activate"
    echo ""
    echo "Or add this to your ~/.bashrc:"
    echo "  alias activate_bdt='source ${VENV_DIR}/bin/activate'"
    echo ""
    echo "Then you can activate with:"
    echo "  activate_bdt"
    echo ""

    # Create activation script
    ACTIVATE_SCRIPT="${EOS_HOME}/activate_bdt_env.sh"
    cat > "$ACTIVATE_SCRIPT" << 'SCRIPT_END'
#!/bin/bash
# Activate BDT Python environment
source /eos/home-m/mblancco/python_venv/bdt_mutau/bin/activate
echo "BDT Python environment activated!"
echo "Python: $(which python)"
echo "Pip: $(which pip)"
SCRIPT_END
    chmod +x "$ACTIVATE_SCRIPT"

    echo "Created activation script: ${ACTIVATE_SCRIPT}"
    echo "You can also run: source ${ACTIVATE_SCRIPT}"
    echo ""
else
    echo ""
    echo "ERROR: Some packages failed to install"
    exit 1
fi
