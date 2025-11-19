#!/usr/bin/env python3
"""
Check proton acceptance - what fraction of events have both arms?
This could explain the factor of 2 if only 50% of events pass proton requirements.
"""

import ROOT

# Check a fase0 file (before proton filtering in fase1)
fase0_file = "/eos/user/m/mblancco/samples_2018_mutau/fase0_with_proton_vars/Data_2018_UL_skimmed_MuTau_nano_0.root"

# Check the merged fase1 files (after proton filtering)
data_file = "/eos/home-m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_proton_vars.root"
qcd_file = "/eos/home-m/mblancco/samples_2018_mutau/QCD_2018_UL_MuTau_nano_merged_proton_vars.root"

print("="*60)
print("PROTON ACCEPTANCE ANALYSIS")
print("="*60)

# Check fase0 file
try:
    print("\n1. FASE 0 OUTPUT (before proton filtering):")
    print(f"   File: {fase0_file}")

    f0 = ROOT.TFile.Open(fase0_file)
    if not f0 or f0.IsZombie():
        print("   ERROR: Cannot open file")
    else:
        tree0 = f0.Get("tree")
        if not tree0:
            tree0 = f0.Get("tree_out")

        if tree0:
            total = tree0.GetEntries()
            valid_mass = tree0.GetEntries("sist_mass >= 0")

            # Check proton variables
            has_pps = tree0.GetEntries("pps_has_both_arms == 1")
            has_xi = tree0.GetEntries("xi_arm1_1 >= 0 && xi_arm2_1 >= 0")

            print(f"   Total events: {total}")
            print(f"   Valid mass (>=0): {valid_mass} ({100*valid_mass/total:.1f}%)")
            print(f"   pps_has_both_arms==1: {has_pps} ({100*has_pps/total:.1f}%)")
            print(f"   xi_arm1_1>=0 && xi_arm2_1>=0: {has_xi} ({100*has_xi/total:.1f}%)")
            print(f"   Ratio pps/xi: {has_pps/has_xi if has_xi > 0 else 'N/A':.3f}")

            # Check with valid mass
            has_pps_mass = tree0.GetEntries("pps_has_both_arms == 1 && sist_mass >= 0")
            has_xi_mass = tree0.GetEntries("xi_arm1_1 >= 0 && xi_arm2_1 >= 0 && sist_mass >= 0")

            print(f"\n   With valid mass cut:")
            print(f"   pps_has_both_arms==1: {has_pps_mass} ({100*has_pps_mass/valid_mass:.1f}%)")
            print(f"   xi_arm1_1>=0 && xi_arm2_1>=0: {has_xi_mass} ({100*has_xi_mass/valid_mass:.1f}%)")

        f0.Close()
except Exception as e:
    print(f"   ERROR: {e}")

# Check Data file
try:
    print("\n2. DATA FILE (after fase1 filtering):")
    print(f"   File: {data_file}")

    fdata = ROOT.TFile.Open(data_file)
    if not fdata or fdata.IsZombie():
        print("   ERROR: Cannot open file")
    else:
        tdata = fdata.Get("tree")

        if tdata:
            total_data = tdata.GetEntries()
            valid_mass_data = tdata.GetEntries("sist_mass >= 0")

            print(f"   Total events: {total_data}")
            print(f"   Valid mass (>=0): {valid_mass_data} ({100*valid_mass_data/total_data:.1f}%)")

            # Check what passed the filters
            os_data = tdata.GetEntries("sist_mass >= 0")  # All should be OS already

            print(f"   Events with sist_mass>=0: {os_data}")

        fdata.Close()
except Exception as e:
    print(f"   ERROR: {e}")

# Check QCD file
try:
    print("\n3. QCD FILE (after fase1 filtering):")
    print(f"   File: {qcd_file}")

    fqcd = ROOT.TFile.Open(qcd_file)
    if not fqcd or fqcd.IsZombie():
        print("   ERROR: Cannot open file")
    else:
        tqcd = fqcd.Get("tree")

        if tqcd:
            total_qcd = tqcd.GetEntries()
            valid_mass_qcd = tqcd.GetEntries("sist_mass >= 0")

            print(f"   Total events: {total_qcd}")
            print(f"   Valid mass (>=0): {valid_mass_qcd} ({100*valid_mass_qcd/total_qcd:.1f}%)")

            # Check what passed the filters
            ss_qcd = tqcd.GetEntries("sist_mass >= 0")  # All should be SS already

            print(f"   Events with sist_mass>=0: {ss_qcd}")

        fqcd.Close()
except Exception as e:
    print(f"   ERROR: {e}")

print("\n" + "="*60)
print("INTERPRETATION:")
print("="*60)
print("If pps_has_both_arms / xi filter shows ~50% acceptance:")
print("  → The factor of 2 compensates for proton requirements")
print("  → This is CORRECT for PPS analysis")
print("\nIf the ratio is close to 100%:")
print("  → The issue is elsewhere (merging, input files, etc.)")
print("="*60)