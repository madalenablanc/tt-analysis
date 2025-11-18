#!/usr/bin/env python3
"""
Diagnostic script to check event counts at each stage of the MuTau analysis.
This helps identify where events are being lost or miscounted.
"""

import ROOT
import os

# File paths to check
files_to_check = {
    "Data (merged)": "/eos/home-m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged.root",
    "Data (with protons)": "/eos/home-m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_proton_vars.root",
    "QCD (merged)": "/eos/home-m/mblancco/samples_2018_mutau/QCD_2018_UL_MuTau_nano_merged_proton_vars.root",
    "DY (merged)": "/eos/home-m/mblancco/samples_2018_mutau/DY_2018_UL_MuTau_nano_merged_pileup_protons.root",
    "ttjets (merged)": "/eos/home-m/mblancco/samples_2018_mutau/ttjets_2018_UL_MuTau_nano_merged_pileup_protons.root",
}

def check_file(path, label):
    """Check event counts and key variables in a ROOT file."""
    print(f"\n{'='*60}")
    print(f"Checking: {label}")
    print(f"Path: {path}")
    print(f"{'='*60}")

    if not os.path.exists(path):
        print(f"❌ File does not exist!")
        return

    f = ROOT.TFile.Open(path)
    if not f or f.IsZombie():
        print(f"❌ Cannot open file!")
        return

    # Try to get the tree
    tree = f.Get("tree")
    if not tree:
        tree = f.Get("tree_out")

    if not tree:
        print(f"❌ No tree found (tried 'tree' and 'tree_out')")
        f.Close()
        return

    n_entries = tree.GetEntries()
    print(f"✅ Total entries: {n_entries}")

    # Check how many pass basic cuts
    if tree.GetBranch("sist_mass"):
        n_valid_mass = tree.GetEntries("sist_mass >= 0")
        print(f"   Entries with valid mass (>=0): {n_valid_mass} ({100*n_valid_mass/n_entries:.1f}%)")

    # Check proton variables if they exist
    if tree.GetBranch("xi_arm1_1") and tree.GetBranch("xi_arm2_1"):
        n_both_protons = tree.GetEntries("xi_arm1_1 >= 0 && xi_arm2_1 >= 0")
        print(f"   Entries with both protons: {n_both_protons} ({100*n_both_protons/n_entries:.1f}%)")

    if tree.GetBranch("pps_has_both_arms"):
        n_pps_both = tree.GetEntries("pps_has_both_arms == 1")
        print(f"   Entries with pps_has_both_arms==1: {n_pps_both} ({100*n_pps_both/n_entries:.1f}%)")

    # Check weight branch
    if tree.GetBranch("weight"):
        tree.Draw("weight>>h_weight", "", "goff")
        h = ROOT.gDirectory.Get("h_weight")
        if h:
            print(f"   Weight: mean={h.GetMean():.4f}, entries={h.GetEntries():.0f}")

    # Check if there are any selection criteria that might affect counts
    if tree.GetBranch("mu_charge") and tree.GetBranch("tau_charge"):
        n_opposite_sign = tree.GetEntries("mu_charge * tau_charge < 0")
        n_same_sign = tree.GetEntries("mu_charge * tau_charge > 0")
        print(f"   Opposite sign: {n_opposite_sign} ({100*n_opposite_sign/n_entries:.1f}%)")
        print(f"   Same sign: {n_same_sign} ({100*n_same_sign/n_entries:.1f}%)")

    f.Close()

def main():
    print("MuTau Analysis Event Count Diagnostic")
    print("=" * 60)

    for label, path in files_to_check.items():
        try:
            check_file(path, label)
        except Exception as e:
            print(f"ERROR checking {label}: {e}")

    print(f"\n{'='*60}")
    print("Summary:")
    print("If Data and QCD appear at half their expected values,")
    print("check for:")
    print("1. Duplicate event removal (might be removing too many)")
    print("2. Different proton selection criteria (pps_has_both_arms vs xi >= 0)")
    print("3. Charge sign selection (opposite vs same sign)")
    print("4. Weight factors applied incorrectly")
    print("=" * 60)

if __name__ == "__main__":
    main()
