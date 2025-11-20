#!/bin/bash
# Script to run diagnostic plots on lxplus
# Usage: bash run_diagnostics.sh

echo "=========================================="
echo "Running diagnostic plots for MuTau analysis"
echo "=========================================="
echo ""

# Make sure we're in the right directory
#cd /afs/cern.ch/user/m/mblancco/tau_analysis/MuTau_channel || exit 1

# Run the diagnostic plot script
echo "Generating diagnostic plots..."
python3 diagnostic_plots.py

if [ $? -eq 0 ]; then
    echo ""
    echo "=========================================="
    echo "SUCCESS! Plots created in diagnostic_plots/"
    echo "=========================================="
    echo ""
    echo "Generated plots:"
    ls -lh diagnostic_plots/
    echo ""
    echo "You can now present SIMPLE_PRESENTATION.pdf to your advisors."
    echo "The plots are referenced in the presentation."
else
    echo ""
    echo "ERROR: Failed to generate plots"
    echo "Check the error messages above"
    exit 1
fi
