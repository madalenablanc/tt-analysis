#!/bin/bash
# Run BDT application on all MuTau samples
# Usage: bash run_bdt_all.sh
# Must be run from the MuTau_channel directory (where dataset/weights/ is located)

set -e

echo "========================================="
echo "  Running BDT on all MuTau samples"
echo "========================================="

echo ""
echo "--- Signal ---"
root -l -b -q 'run_bdt_all.C("/eos/user/m/mblancco/tau_analysis/MuTau_channel/MuTau_sinal_SM_2018_july.root", "TMVApp_sinal.root", "signal")'

echo ""
echo "--- DY ---"
root -l -b -q 'run_bdt_all.C("/eos/user/m/mblancco/samples_2018_mutau/DY_2018_UL_MuTau_nano_merged_pileup_protons.root", "TMVApp_DY.root", "dy")'

echo ""
echo "--- ttbar ---"
root -l -b -q 'run_bdt_all.C("/eos/user/m/mblancco/samples_2018_mutau/ttjets_2018_UL_MuTau_nano_merged_pileup_protons.root", "TMVApp_ttjets.root", "ttbar")'

echo ""
echo "--- QCD ---"
root -l -b -q 'run_bdt_all.C("/eos/user/m/mblancco/samples_2018_mutau/QCD_2018_UL_MuTau_nano_merged_proton_vars_new.root", "TMVApp_QCD.root", "qcd")'

echo ""
echo "--- Data ---"
root -l -b -q 'run_bdt_all.C("/eos/user/m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_proton_vars.root", "TMVApp_data.root", "data")'

echo ""
echo "========================================="
echo "  All done! Output files:"
echo "    TMVApp_sinal.root"
echo "    TMVApp_DY.root"
echo "    TMVApp_ttjets.root"
echo "    TMVApp_QCD.root"
echo "    TMVApp_data.root"
echo "========================================="
