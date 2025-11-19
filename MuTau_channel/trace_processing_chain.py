#!/usr/bin/env python3
"""
Trace the full processing chain to find where events are lost
"""

import ROOT
import os
import glob

print("="*80)
print("TRACING MUTAU PROCESSING CHAIN")
print("="*80)

# 1. Check how many input files should exist
input_file_list = "Data_QCD_MuTau_skimmed_2018.txt"
if os.path.exists(input_file_list):
    with open(input_file_list) as f:
        n_input_files = len([line for line in f if line.strip()])
    print(f"\n1. INPUT FILES:")
    print(f"   Expected: {n_input_files} files (from {input_file_list})")
else:
    print(f"\n1. INPUT FILES: Cannot find {input_file_list}")
    n_input_files = 393  # From your earlier check

# 2. Check fase0 output
print(f"\n2. FASE0 OUTPUT:")
fase0_pattern = "/eos/user/m/mblancco/samples_2018_mutau/fase0_with_proton_vars/Data_2018_UL_skimmed_MuTau_nano_*.root"
print(f"   Checking: {fase0_pattern}")

# Try to count files (may not work from Mac)
try:
    # This won't work from Mac, but will work on lxplus
    fase0_files = glob.glob(fase0_pattern)
    n_fase0 = len(fase0_files)
    print(f"   Found: {n_fase0} files")

    if n_fase0 > 0:
        # Sample a few files
        sample_files = fase0_files[:min(5, n_fase0)]
        total_events = 0
        for fname in sample_files:
            try:
                f = ROOT.TFile.Open(fname)
                if f and not f.IsZombie():
                    tree = f.Get("tree")
                    if tree:
                        total_events += tree.GetEntries()
                f.Close()
            except:
                pass

        avg_events = total_events / len(sample_files) if sample_files else 0
        estimated_total = avg_events * n_fase0
        print(f"   Average events per file: {avg_events:.0f}")
        print(f"   Estimated total events: {estimated_total:.0f}")
except Exception as e:
    print(f"   Cannot access files (run this on lxplus): {e}")

# 3. Check fase1 output
print(f"\n3. FASE1 OUTPUT:")

# Data
fase1_data_pattern = "/eos/user/m/mblancco/samples_2018_mutau/fase1_data/Data_2018_UL_skimmed_MuTau_cuts_*.root"
print(f"   Data pattern: {fase1_data_pattern}")
try:
    fase1_data_files = glob.glob(fase1_data_pattern)
    n_fase1_data = len(fase1_data_files)
    print(f"   Found: {n_fase1_data} data files")

    if n_fase1_data > 0:
        total_data = 0
        for fname in fase1_data_files[:100]:  # Sample first 100
            try:
                f = ROOT.TFile.Open(fname)
                if f and not f.IsZombie():
                    tree = f.Get("tree")
                    if tree:
                        total_data += tree.GetEntries()
                f.Close()
            except:
                pass
        print(f"   Total events (first {min(n_fase1_data, 100)} files): {total_data}")
except Exception as e:
    print(f"   Cannot access files: {e}")

# QCD
fase1_qcd_pattern = "/eos/user/m/mblancco/samples_2018_mutau/fase1_qcd/QCD_2018_UL_skimmed_MuTau_cuts_*.root"
print(f"\n   QCD pattern: {fase1_qcd_pattern}")
try:
    fase1_qcd_files = glob.glob(fase1_qcd_pattern)
    n_fase1_qcd = len(fase1_qcd_files)
    print(f"   Found: {n_fase1_qcd} QCD files")

    if n_fase1_qcd > 0:
        total_qcd = 0
        for fname in fase1_qcd_files[:100]:  # Sample first 100
            try:
                f = ROOT.TFile.Open(fname)
                if f and not f.IsZombie():
                    tree = f.Get("tree")
                    if tree:
                        total_qcd += tree.GetEntries()
                f.Close()
            except:
                pass
        print(f"   Total events (first {min(n_fase1_qcd, 100)} files): {total_qcd}")
except Exception as e:
    print(f"   Cannot access files: {e}")

# 4. Check merged files
print(f"\n4. MERGED FILES:")

data_merged = "/eos/home-m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_proton_vars.root"
qcd_merged = "/eos/home-m/mblancco/samples_2018_mutau/QCD_2018_UL_MuTau_nano_merged_proton_vars.root"

try:
    f_data = ROOT.TFile.Open(data_merged)
    if f_data and not f_data.IsZombie():
        t_data = f_data.Get("tree")
        if t_data:
            print(f"   Data merged: {t_data.GetEntries()} events")
        f_data.Close()
except Exception as e:
    print(f"   Data merged: Cannot open - {e}")

try:
    f_qcd = ROOT.TFile.Open(qcd_merged)
    if f_qcd and not f_qcd.IsZombie():
        t_qcd = f_qcd.Get("tree")
        if t_qcd:
            print(f"   QCD merged: {t_qcd.GetEntries()} events")
        f_qcd.Close()
except Exception as e:
    print(f"   QCD merged: Cannot open - {e}")

# 5. Calculate expected vs actual
print(f"\n5. ANALYSIS:")
print(f"   From fase0 file #0:")
print(f"   - Total: 1,023,514 events")
print(f"   - With protons (16.9%): 172,895 events")
print(f"   - OS with protons: ~4,314 events per file")
print(f"   - SS with protons: ~168,660 events per file")
print(f"")
print(f"   If ALL {n_input_files} files processed:")
print(f"   - Expected Data (OS): ~{n_input_files * 4314:,} events")
print(f"   - Expected QCD (SS): ~{n_input_files * 168660:,} events")
print(f"")
print(f"   Actual merged:")
print(f"   - Data: 1,708 events")
print(f"   - QCD: 234 events")
print(f"")
print(f"   This represents:")
print(f"   - Data: {1708 / (n_input_files * 4314) * 100:.2f}% of expected")
print(f"   - QCD: {234 / (n_input_files * 168660) * 100:.4f}% of expected")

print("\n" + "="*80)
print("CONCLUSION:")
print("="*80)
print("The merged files contain FAR fewer events than expected.")
print("This could mean:")
print("1. Only a few input files were actually processed")
print("2. The merging step only included a subset of fase1 output files")
print("3. There are additional cuts in fase1 that we haven't accounted for")
print("\nRun this script on lxplus to check actual file counts in fase1 directories.")
print("="*80)