#!/usr/bin/env python3
"""
Script to measure the proton acceptance from real data.
Calculates the fraction of events with at least 1 proton on each arm (arm 0 and arm 1).
This gives us the P = 0.13 (or ~0.20) factor used in proton merging.
"""

import ROOT
import sys

# Path to your data file with proton information
DATA_FILE = "/eos/user/m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_proton_vars.root"

def count_protons_per_arm(tree):
    """
    Count how many events have at least 1 proton on both arms.

    Returns:
        tuple: (total_events, events_with_both_arms, fraction)
    """
    total_events = 0
    events_with_both_arms = 0

    # Counters for different proton multiplicities
    multiplicity_counts = {
        (0, 0): 0,  # No protons on either arm
        (0, 1): 0,  # Only arm 1 has protons
        (1, 0): 0,  # Only arm 0 has protons
        (1, 1): 0,  # Both arms have at least 1 proton
    }

    n_events = tree.GetEntries()
    print(f"Processing {n_events} events...")

    for i in range(n_events):
        tree.GetEntry(i)

        # Progress indicator
        if i % 100000 == 0:
            print(f"  Processed {i}/{n_events} events ({100.0*i/n_events:.1f}%)")

        total_events += 1

        # Check if the tree has multiRP proton branches
        # Two possible naming conventions:
        # 1. nProton_multiRP, Proton_multiRP_arm
        # 2. proton_multi_arm (already a vector)

        has_arm0 = False
        has_arm1 = False

        # Try to access proton arm information
        try:
            # Method 1: Check if we have proton_multi_arm vector
            if hasattr(tree, 'proton_multi_arm'):
                proton_arms = tree.proton_multi_arm
                for arm in proton_arms:
                    if arm == 0:
                        has_arm0 = True
                    if arm == 1:
                        has_arm1 = True

            # Method 2: Check if we have Proton_multiRP_arm
            elif hasattr(tree, 'Proton_multiRP_arm'):
                proton_arms = tree.Proton_multiRP_arm
                for j in range(len(proton_arms)):
                    arm = proton_arms[j]
                    if arm == 0:
                        has_arm0 = True
                    if arm == 1:
                        has_arm1 = True

            # Method 3: Use GetLeaf (more general but slower)
            else:
                # Try to find the branch with proton arm information
                leaf_arm = tree.GetLeaf("proton_multi_arm")
                if not leaf_arm:
                    leaf_arm = tree.GetLeaf("Proton_multiRP_arm")

                if leaf_arm:
                    n_protons = leaf_arm.GetLen()
                    for j in range(n_protons):
                        arm = int(leaf_arm.GetValue(j))
                        if arm == 0:
                            has_arm0 = True
                        if arm == 1:
                            has_arm1 = True

        except Exception as e:
            print(f"Warning: Could not read proton information at event {i}: {e}")
            continue

        # Count multiplicities
        key = (1 if has_arm0 else 0, 1 if has_arm1 else 0)
        multiplicity_counts[key] += 1

        # Check if we have at least 1 proton on BOTH arms
        if has_arm0 and has_arm1:
            events_with_both_arms += 1

    fraction = events_with_both_arms / total_events if total_events > 0 else 0.0

    return total_events, events_with_both_arms, fraction, multiplicity_counts


def main():
    print("=" * 80)
    print("Proton Acceptance Calculator")
    print("=" * 80)
    print(f"\nOpening data file: {DATA_FILE}")

    # Open the file
    f = ROOT.TFile.Open(DATA_FILE)
    if not f or f.IsZombie():
        print(f"ERROR: Could not open file: {DATA_FILE}", file=sys.stderr)
        print("\nPlease update DATA_FILE path in the script to point to your data file.")
        sys.exit(1)

    # Get the tree
    tree = f.Get("tree")
    if not tree:
        print("ERROR: Could not find 'tree' in the file", file=sys.stderr)
        sys.exit(1)

    print(f"Found tree with {tree.GetEntries()} events\n")

    # List available branches to help debug
    print("Available proton-related branches:")
    branches = tree.GetListOfBranches()
    for i in range(branches.GetEntries()):
        branch_name = branches.At(i).GetName()
        if "proton" in branch_name.lower() or "Proton" in branch_name:
            print(f"  - {branch_name}")
    print()

    # Calculate acceptance
    total, with_both, fraction, multiplicities = count_protons_per_arm(tree)

    # Print results
    print("\n" + "=" * 80)
    print("RESULTS")
    print("=" * 80)
    print(f"\nTotal events analyzed: {total:,}")
    print(f"Events with ≥1 proton on both arms: {with_both:,}")
    print(f"\nProton acceptance P = {fraction:.4f} ({fraction*100:.2f}%)")

    print("\n--- Detailed Breakdown ---")
    print(f"No protons on either arm:      {multiplicities[(0,0)]:,} ({100.0*multiplicities[(0,0)]/total:.2f}%)")
    print(f"Protons only on arm 0:         {multiplicities[(1,0)]:,} ({100.0*multiplicities[(1,0)]/total:.2f}%)")
    print(f"Protons only on arm 1:         {multiplicities[(0,1)]:,} ({100.0*multiplicities[(0,1)]/total:.2f}%)")
    print(f"Protons on BOTH arms:          {multiplicities[(1,1)]:,} ({100.0*multiplicities[(1,1)]/total:.2f}%)")

    print("\n" + "=" * 80)
    print("INTERPRETATION")
    print("=" * 80)
    print(f"\nThe measured proton acceptance is P = {fraction:.4f}")
    print("\nThis is the factor that should be used as FIXED_WEIGHT in merge_pp_mutau.py")

    if abs(fraction - 0.13) < 0.03:
        print(f"✓ Close to Matteo's value of 0.13 (difference: {abs(fraction-0.13):.4f})")
    elif abs(fraction - 0.20) < 0.03:
        print(f"✓ Close to Jonathan's estimate of ~0.20 (difference: {abs(fraction-0.20):.4f})")
    else:
        print(f"⚠ Different from both 0.13 and 0.20 - may depend on event selection cuts")

    print("\nNote: The exact value depends on:")
    print("  - Which triggers were used")
    print("  - What event selection cuts were applied")
    print("  - Which data-taking period")
    print("  - Detector conditions and calibration")

    f.Close()


if __name__ == "__main__":
    main()
