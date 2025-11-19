#!/usr/bin/env python3
"""
Check the distribution of tau ID values in fase0 output
to understand why fase1 cuts are so aggressive
"""

import ROOT

fase0_file = "/eos/user/m/mblancco/samples_2018_mutau/fase0_with_proton_vars/Data_2018_UL_skimmed_MuTau_nano_0.root"

print("=" * 80)
print("TAU ID DISTRIBUTION ANALYSIS")
print("=" * 80)

try:
    f = ROOT.TFile.Open(fase0_file)
    if not f or f.IsZombie():
        print("ERROR: Cannot open file")
    else:
        tree = f.Get("tree")
        if not tree:
            print("ERROR: Cannot get tree")
        else:
            total = tree.GetEntries()
            print(f"\nTotal events in fase0: {total:,}")

            # Check tau ID distributions
            print(f"\n1. TAU ID DISTRIBUTIONS:")

            # VSjet (tau_id1)
            print(f"\n   tau_id1 (VSjet):")
            for threshold in [0, 1, 2, 4, 8, 16, 32, 63, 64, 127]:
                n = tree.GetEntries(f"tau_id1 > {threshold}")
                print(f"      > {threshold:3d}: {n:8,} ({100*n/total:5.2f}%)")

            # VSe (tau_id2)
            print(f"\n   tau_id2 (VSe):")
            for threshold in [0, 1, 2, 4, 7, 8, 15]:
                n = tree.GetEntries(f"tau_id2 > {threshold}")
                print(f"      > {threshold:3d}: {n:8,} ({100*n/total:5.2f}%)")

            # VSmu (tau_id3)
            print(f"\n   tau_id3 (VSmu):")
            for threshold in [0, 1, 2, 4, 8, 15]:
                n = tree.GetEntries(f"tau_id3 > {threshold}")
                print(f"      > {threshold:3d}: {n:8,} ({100*n/total:5.2f}%)")

            # Combined (current fase1 requirements)
            print(f"\n2. COMBINED TAU ID (fase1 requirements):")
            n_all = tree.GetEntries("tau_id1 > 63 && tau_id2 > 7 && tau_id3 > 1")
            print(f"   tau_id1 > 63 && tau_id2 > 7 && tau_id3 > 1: {n_all:,} ({100*n_all/total:.4f}%)")

            # Check with other cuts applied
            print(f"\n3. WITH OTHER FASE1 CUTS:")
            base = "mu_id >= 3"
            n = tree.GetEntries(base)
            print(f"   After mu_id >= 3: {n:,}")

            base += " && tau_id1 > 63 && tau_id2 > 7 && tau_id3 > 1"
            n = tree.GetEntries(base)
            print(f"   + tau ID cuts: {n:,}")

            base += " && mu_pt > 35. && tau_pt > 100."
            n = tree.GetEntries(base)
            print(f"   + pT cuts: {n:,}")

            base += " && pps_has_both_arms == 1"
            n = tree.GetEntries(base)
            print(f"   + proton requirement: {n:,}")

            # Check if the issue is with how tau_id is stored
            print(f"\n4. CHECKING TAU ID STORAGE:")
            print(f"   Drawing tau_id1 histogram...")
            h1 = ROOT.TH1F("h_tau_id1", "tau_id1 distribution", 256, 0, 256)
            tree.Draw("tau_id1 >> h_tau_id1", "", "goff")
            print(f"   Mean tau_id1: {h1.GetMean():.2f}")
            print(f"   RMS tau_id1: {h1.GetRMS():.2f}")

            # Show which bins have entries
            print(f"\n   Non-zero bins in tau_id1:")
            for i in range(1, 257):
                if h1.GetBinContent(i) > 0:
                    val = i - 1  # bin 1 = value 0
                    count = int(h1.GetBinContent(i))
                    if count > 100:  # Only show bins with significant entries
                        print(f"      Value {val:3d}: {count:8,} events")

        f.Close()

except Exception as e:
    print(f"ERROR: {e}")
    import traceback
    traceback.print_exc()

print("\n" + "=" * 80)
print("INTERPRETATION:")
print("=" * 80)
print("If most taus have tau_id1 = 0 or very low values:")
print("  → Fase0 saved low-quality taus (no ID cuts applied)")
print("  → Fase1 tau_id1 > 63 cut rejects almost everything")
print("  → THIS IS THE BUG!")
print("")
print("Solution: Apply tau ID cuts in fase0, or use looser cuts in fase1")
print("=" * 80)