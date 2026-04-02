#!/usr/bin/env python3
"""
Check generator_weight values in the processed files to understand normalization.
"""

import ROOT

# Files to check (merged files after proton mixing)
files = {
    "DY": "/eos/user/m/mblancco/samples_2018_mutau/DY_2018_UL_MuTau_nano_merged_pileup_protons.root",
    "ttbar": "/eos/user/m/mblancco/samples_2018_mutau/ttjets_2018_UL_MuTau_nano_merged_pileup_protons.root",
}

# Cross-sections and sum of weights from original samples
LUMI_PB = 54900
XSEC_DY = 6077.22
XSEC_TTBAR = 831.76
SUM_W_DY = 3.323e12      # From check_mc_normalization.py output
SUM_W_TTBAR = 6.108e11   # From check_mc_normalization.py output

PROTON_ACCEPTANCE = 0.245

print("=" * 70)
print("Weight Analysis")
print("=" * 70)

for name, filepath in files.items():
    print(f"\n--- {name} ---")
    print(f"File: {filepath}")

    f = ROOT.TFile.Open(filepath)
    if not f or f.IsZombie():
        print("ERROR: Cannot open file")
        continue

    tree = f.Get("tree")
    if not tree:
        print("ERROR: Cannot find tree")
        f.Close()
        continue

    n_events = tree.GetEntries()
    print(f"N events: {n_events}")

    # Check available branches
    branches = [b.GetName() for b in tree.GetListOfBranches()]
    print(f"Branches: {', '.join(branches[:20])}...")

    # Get statistics on weight-related branches
    for branch_name in ["weight", "generator_weight", "event_weight"]:
        if branch_name in branches:
            tree.Draw(f"{branch_name}>>h_{branch_name}_{name}", "", "goff")
            h = ROOT.gDirectory.Get(f"h_{branch_name}_{name}")
            if h:
                print(f"  {branch_name}:")
                print(f"    Mean: {h.GetMean():.6g}")
                print(f"    StdDev: {h.GetStdDev():.6g}")
                print(f"    Sum: {h.GetMean() * n_events:.6g}")
                print(f"    Min: {h.GetMinimum():.6g}, Max: {h.GetMaximum():.6g}")

    f.Close()

print("\n" + "=" * 70)
print("EXPECTED NORMALIZATION")
print("=" * 70)

print("\nCorrect per-event weight formula:")
print("  w = (L × σ / Σgen_w) × generator_weight × muon_SFs × proton_acc")
print()

scale_dy = LUMI_PB * XSEC_DY / SUM_W_DY
scale_ttbar = LUMI_PB * XSEC_TTBAR / SUM_W_TTBAR

print(f"DY scale factor (L×σ/Σw): {scale_dy:.6e}")
print(f"ttbar scale factor (L×σ/Σw): {scale_ttbar:.6e}")
print()

# Average generator weight
avg_gen_w_dy = SUM_W_DY / 195510810  # N_events from our calculation
avg_gen_w_ttbar = SUM_W_TTBAR / 304895029

print(f"Average generator_weight for DY: {avg_gen_w_dy:.0f}")
print(f"Average generator_weight for ttbar: {avg_gen_w_ttbar:.0f}")
print()

# Expected average weight per event (before muon SFs)
avg_w_dy = scale_dy * avg_gen_w_dy * PROTON_ACCEPTANCE
avg_w_ttbar = scale_ttbar * avg_gen_w_ttbar * PROTON_ACCEPTANCE

print(f"Expected average weight per event (before muon SFs):")
print(f"  DY: {scale_dy:.6e} × {avg_gen_w_dy:.0f} × {PROTON_ACCEPTANCE} = {avg_w_dy:.4f}")
print(f"  ttbar: {scale_ttbar:.6e} × {avg_gen_w_ttbar:.0f} × {PROTON_ACCEPTANCE} = {avg_w_ttbar:.4f}")
print()
print(f"Ratio ttbar/DY: {avg_w_ttbar/avg_w_dy:.3f}")
print()

print("=" * 70)
print("WHAT YOUR FILES CURRENTLY HAVE")
print("=" * 70)
print()
print("DY weight branch = generator_weight × muon_SFs × 0.245")
print("  (includes raw generator_weight, which is ~17000 on average)")
print()
print("ttbar weight branch = 0.15 × muon_SFs × 0.245")
print("  (generator_weight is IGNORED, hardcoded 0.15 factor)")
print()
print("This inconsistency is why the normalization is confusing!")
print()

print("=" * 70)
print("RECOMMENDED FIX")
print("=" * 70)
print()
print("Option 1: Use generator_weight properly for BOTH samples")
print("  - Apply scale factor = L×σ/Σw to each event")
print("  - weight = scale × generator_weight × muon_SFs × proton_acc")
print()
print("Option 2: Use FLAT normalization for BOTH samples (current approach)")
print("  - Ignore generator_weight entirely")
print("  - Apply empirical scale factors at plotting stage")
print("  - DY scale ≈ 1.0, ttbar scale ≈ 0.15-0.16")
print()
print("Your current MuTauPlots_after.C with Scale(1) for both + PROTON_ACCEPTANCE=0.245")
print("is effectively using Option 2 for ttbar but NOT for DY (DY still has generator_weight).")
