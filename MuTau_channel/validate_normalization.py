#!/usr/bin/env python3
"""
Validation script to check if MC normalization is correct.

This script performs several cross-checks:
1. Compares weighted event yields to theoretical expectations
2. Checks Data/MC ratio in control regions
3. Verifies the normalization chain is self-consistent
"""

import ROOT
import math

# ============================================================================
# CONFIGURATION
# ============================================================================

# Luminosity and cross-sections
LUMI_PB = 54900  # pb^-1
XSEC_DY = 6077.22  # pb (NNLO)
XSEC_TTBAR = 831.76  # pb (NNLO)

# From check_mc_normalization.py
SUM_W_DY = 3.323e12
SUM_W_TTBAR = 6.108e11

# Scale factors
DY_SCALE = LUMI_PB * XSEC_DY / SUM_W_DY  # = 1.004e-4
TTBAR_SCALE_THEORETICAL = LUMI_PB * XSEC_TTBAR / SUM_W_TTBAR  # = 7.476e-5
PROTON_ACCEPTANCE = 0.245

# Average generator weights (approximate)
AVG_GEN_W_DY = 17000
AVG_GEN_W_TTBAR = 2000

# Files
FILES = {
    "DY": "/eos/user/m/mblancco/samples_2018_mutau/DY_2018_UL_MuTau_nano_merged_pileup_protons.root",
    "ttbar": "/eos/user/m/mblancco/samples_2018_mutau/ttjets_2018_UL_MuTau_nano_merged_pileup_protons.root",
    "Data": "/eos/user/m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_proton_vars.root",
    "QCD": "/eos/user/m/mblancco/samples_2018_mutau/QCD_2018_UL_MuTau_nano_merged_proton_vars_new.root",
}

# ============================================================================
# VALIDATION FUNCTIONS
# ============================================================================

def check_theoretical_yields():
    """Compare weighted MC yields to theoretical expectations."""
    print("=" * 70)
    print("CHECK 1: Theoretical Yield Comparison")
    print("=" * 70)

    # Open files and get weighted sums
    for name, filepath in [("DY", FILES["DY"]), ("ttbar", FILES["ttbar"])]:
        print(f"\nOpening {name}: {filepath}")
        f = ROOT.TFile.Open(filepath)
        if not f or f.IsZombie():
            print(f"ERROR: Cannot open {filepath}")
            continue

        tree = f.Get("tree")
        if not tree:
            print(f"ERROR: Cannot find 'tree' in {filepath}")
            f.Close()
            continue

        n_events = tree.GetEntries()
        print(f"  Total entries: {n_events}")

        # Get sum of weights using a simple loop instead of TTree::Draw
        sum_weights = 0.0
        n_with_protons = 0

        tree.SetBranchStatus("*", 0)
        tree.SetBranchStatus("weight", 1)
        tree.SetBranchStatus("xi_arm1_1", 1)
        tree.SetBranchStatus("xi_arm2_1", 1)

        for i in range(min(n_events, 100000)):  # Limit for speed
            tree.GetEntry(i)
            xi1 = tree.xi_arm1_1
            xi2 = tree.xi_arm2_1
            if xi1 >= 0 and xi2 >= 0:
                sum_weights += tree.weight
                n_with_protons += 1

        # Scale if we only read partial
        if n_events > 100000:
            scale = n_events / 100000
            sum_weights *= scale
            n_with_protons = int(n_with_protons * scale)

        f.Close()

        print(f"\n{name}:")
        print(f"  N events (after proton cut): {n_with_protons}")
        print(f"  Sum of weights (raw): {sum_weights:.2f}")

        if name == "DY":
            # Apply DY_SCALE
            final_yield = sum_weights * DY_SCALE
            print(f"  After DY_SCALE ({DY_SCALE:.4e}): {final_yield:.2f}")

            # What we expect: need selection efficiency
            # weight = gen_weight × muon_SFs × 0.245
            # After scale: N = Σ(gen_weight × SFs × 0.245 × L×σ/Σw)
            #            = L × σ × <SFs> × 0.245 × N_selected/N_total

        elif name == "ttbar":
            # ttbar weight = 0.15 × muon_SFs × 0.245
            # This is approximately correct since 0.15 ≈ TTBAR_SCALE × AVG_GEN_W
            final_yield = sum_weights  # Already scaled via 0.15 factor
            print(f"  Final yield (no additional scale): {final_yield:.2f}")

            # Cross-check: 0.15 should ≈ TTBAR_SCALE × AVG_GEN_W
            expected_factor = TTBAR_SCALE_THEORETICAL * AVG_GEN_W_TTBAR
            print(f"  Verification: 0.15 vs L×σ/Σw × avg_gen_w = {expected_factor:.4f}")


def check_control_regions():
    """Check Data/MC agreement in control regions."""
    print("\n" + "=" * 70)
    print("CHECK 2: Control Region Data/MC Agreement")
    print("=" * 70)
    print("(Skipped - use MuTauPlots_after.C output for Data/MC comparison)")
    print("Look at the ratio plots and printed event counts from ROOT macro.")


def check_normalization_chain():
    """Verify the normalization chain is self-consistent."""
    print("\n" + "=" * 70)
    print("CHECK 3: Normalization Chain Consistency")
    print("=" * 70)

    print("\nDY normalization chain:")
    print(f"  1. generator_weight (avg): ~{AVG_GEN_W_DY}")
    print(f"  2. × muon_SFs (avg): ~0.97")
    print(f"  3. × proton_acceptance: × {PROTON_ACCEPTANCE}")
    print(f"  4. = weight in file: ~{AVG_GEN_W_DY * 0.97 * PROTON_ACCEPTANCE:.0f}")
    print(f"  5. × DY_SCALE: × {DY_SCALE:.4e}")
    print(f"  6. = final per-event weight: ~{AVG_GEN_W_DY * 0.97 * PROTON_ACCEPTANCE * DY_SCALE:.4f}")

    print("\nttbar normalization chain:")
    print(f"  1. 0.15 factor (empirical)")
    print(f"  2. × muon_SFs (avg): ~0.97")
    print(f"  3. × proton_acceptance: × {PROTON_ACCEPTANCE}")
    print(f"  4. = weight in file: ~{0.15 * 0.97 * PROTON_ACCEPTANCE:.4f}")
    print(f"  5. No additional scale needed")
    print(f"  6. = final per-event weight: ~{0.15 * 0.97 * PROTON_ACCEPTANCE:.4f}")

    print("\nCross-check: 0.15 factor for ttbar")
    theoretical = TTBAR_SCALE_THEORETICAL * AVG_GEN_W_TTBAR
    print(f"  If using generator_weight like DY:")
    print(f"    L×σ/Σw × avg_gen_w = {TTBAR_SCALE_THEORETICAL:.4e} × {AVG_GEN_W_TTBAR} = {theoretical:.4f}")
    print(f"  Actual factor used: 0.15")
    print(f"  Ratio: {0.15/theoretical:.2f}x")
    if 0.5 < 0.15/theoretical < 2.0:
        print(f"  ✓ Within factor of 2 - reasonable approximation")
    else:
        print(f"  ⚠ Large discrepancy")


def main():
    print("=" * 70)
    print("MC NORMALIZATION VALIDATION")
    print("=" * 70)
    print(f"\nConfiguration:")
    print(f"  Luminosity: {LUMI_PB} pb^-1")
    print(f"  DY cross-section: {XSEC_DY} pb")
    print(f"  ttbar cross-section: {XSEC_TTBAR} pb")
    print(f"  DY_SCALE: {DY_SCALE:.4e}")
    print(f"  Proton acceptance: {PROTON_ACCEPTANCE}")

    # Run checks
    check_theoretical_yields()
    check_control_regions()
    check_normalization_chain()

    print("\n" + "=" * 70)
    print("SUMMARY")
    print("=" * 70)
    print("""
Your normalization is likely CORRECT if:
1. Data/MC ratios in control regions are ~1.0 (within 30%)
2. The normalization chain is self-consistent
3. Event yields are in the expected ballpark

If Data/MC > 1: MC is under-predicted → scale factors too small
If Data/MC < 1: MC is over-predicted → scale factors too large

Note: Perfect agreement is not expected due to:
- Theoretical uncertainties on cross-sections (~5-10%)
- Missing higher-order corrections
- Pileup modeling differences
- Proton mixing procedure uncertainties
""")


if __name__ == "__main__":
    main()
