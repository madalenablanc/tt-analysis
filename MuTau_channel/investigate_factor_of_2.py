#!/usr/bin/env python3
"""
Investigate the factor of 2 discrepancy by checking:
1. How many events pass lumi filter in fase0
2. How many events pass HLT trigger
3. Whether these combine to give factor of 2
"""

import ROOT

# Check a fase0 output file
fase0_file = "/eos/user/m/mblancco/samples_2018_mutau/fase0_with_proton_vars/Data_2018_UL_skimmed_MuTau_nano_0.root"

print("=" * 80)
print("INVESTIGATING FACTOR OF 2 DISCREPANCY")
print("=" * 80)

try:
    print(f"\nOpening fase0 file: {fase0_file}")
    f = ROOT.TFile.Open(fase0_file)
    if not f or f.IsZombie():
        print("ERROR: Cannot open file")
    else:
        tree = f.Get("tree")
        if not tree:
            print("ERROR: Cannot get tree")
        else:
            total = tree.GetEntries()
            print(f"\n1. TOTAL EVENTS IN FASE0 OUTPUT: {total:,}")
            print(f"   (These have already passed lumi + HLT filters)")

            # Check proton acceptance
            has_both = tree.GetEntries("pps_has_both_arms == 1")
            has_xi = tree.GetEntries("xi_arm1_1 >= 0 && xi_arm2_1 >= 0")

            print(f"\n2. PROTON ACCEPTANCE:")
            print(f"   pps_has_both_arms==1: {has_both:,} ({100*has_both/total:.2f}%)")
            print(f"   xi >= 0 in both arms: {has_xi:,} ({100*has_xi/total:.2f}%)")

            # Check charge split BEFORE any other cuts
            os_all = tree.GetEntries("mu_charge * tau_charge < 0")
            ss_all = tree.GetEntries("mu_charge * tau_charge > 0")

            print(f"\n3. CHARGE SPLIT (no other cuts):")
            print(f"   Opposite sign: {os_all:,} ({100*os_all/total:.2f}%)")
            print(f"   Same sign: {ss_all:,} ({100*ss_all/total:.2f}%)")

            # Check charge split WITH protons
            os_pps = tree.GetEntries("mu_charge * tau_charge < 0 && pps_has_both_arms == 1")
            ss_pps = tree.GetEntries("mu_charge * tau_charge > 0 && pps_has_both_arms == 1")

            print(f"\n4. CHARGE SPLIT WITH PROTONS (pps_has_both_arms==1):")
            print(f"   Opposite sign: {os_pps:,} ({100*os_pps/has_both:.2f}% of events with protons)")
            print(f"   Same sign: {ss_pps:,} ({100*ss_pps/has_both:.2f}% of events with protons)")

            # Now apply ALL fase1 cuts step by step
            print(f"\n5. APPLYING FASE1 CUTS SEQUENTIALLY:")

            base_cut = "pps_has_both_arms == 1"
            n = tree.GetEntries(base_cut)
            print(f"   Starting with protons: {n:,}")

            base_cut += " && mu_id >= 3"
            n = tree.GetEntries(base_cut)
            print(f"   + Muon ID: {n:,}")

            base_cut += " && tau_id1 > 63 && tau_id2 > 7 && tau_id3 > 1"
            n = tree.GetEntries(base_cut)
            print(f"   + Tau ID: {n:,}")

            base_cut += " && mu_pt > 35. && tau_pt > 100."
            n = tree.GetEntries(base_cut)
            print(f"   + pT thresholds: {n:,}")

            # Calculate delta_r manually (can't use it directly)
            base_cut += " && fabs(mu_eta) < 2.4 && fabs(tau_eta) < 2.4"
            n = tree.GetEntries(base_cut)
            print(f"   + Eta acceptance: {n:,}")

            # Final split by charge
            os_final = tree.GetEntries(base_cut + " && mu_charge * tau_charge < 0")
            ss_final = tree.GetEntries(base_cut + " && mu_charge * tau_charge > 0")

            print(f"\n6. FINAL COUNTS (before delta_r cut):")
            print(f"   Opposite sign (Data): {os_final:,}")
            print(f"   Same sign (QCD): {ss_final:,}")

            print(f"\n7. EXPECTED VS ACTUAL:")
            print(f"   If old method gave every event protons (100% acceptance):")
            print(f"   - Expected Data: {os_all:,} OS events")
            print(f"   - Expected QCD: {ss_all:,} SS events")
            print(f"   ")
            print(f"   New method with real protons ({100*has_both/total:.1f}% acceptance):")
            print(f"   - After all cuts, Data: {os_final:,}")
            print(f"   - After all cuts, QCD: {ss_final:,}")
            print(f"   ")
            print(f"   Reduction factor: {os_all/os_final:.2f}x for Data") if os_final > 0 else None
            print(f"   Reduction factor: {ss_all/ss_final:.2f}x for QCD") if ss_final > 0 else None

        f.Close()

except Exception as e:
    print(f"ERROR: {e}")
    import traceback
    traceback.print_exc()

print("\n" + "=" * 80)
print("CONCLUSION:")
print("=" * 80)
print("If the reduction factor is ~2x, then weight=2.0 compensates for:")
print("  (a) Some systematic cut difference between old and new processing")
print("  (b) A bug in how events are being counted or processed")
print("  (c) An intentional correction factor")
print("")
print("Run this script on lxplus to see the actual numbers!")
print("=" * 80)