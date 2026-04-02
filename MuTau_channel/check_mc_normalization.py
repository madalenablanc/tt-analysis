#!/usr/bin/env python3
"""
Script to calculate MC normalization factors from original NanoAOD samples.

Formula: Scale = (Luminosity × Cross-section) / Sum_of_generator_weights

Run this on lxplus where you have access to the original MC samples via XRootD.
"""

import subprocess

# Initialize grid proxy first (like fase0 scripts)
command = "voms-proxy-init --rfc --voms cms --valid 172:00"
subprocess.run(command, shell=True, executable="/bin/bash")

import ROOT
import os

# Enable multithreading
ROOT.EnableImplicitMT()

# 2018 luminosity
LUMINOSITY = 54.9  # fb^-1 = 54900 pb^-1
LUMINOSITY_PB = 54900  # pb^-1

# Cross-sections (pb) from official CMS recommendations
# DYJetsToLL_M-50 aMC@NLO: 6077.22 pb (NNLO)
# TTJets aMC@NLO: 831.76 pb (NNLO)
XSEC_DY = 6077.22
XSEC_TTBAR = 831.76

# XRootD redirector for CMS data (double slash like fase0 scripts)
XROOTD_REDIRECTOR = "root://cms-xrd-global.cern.ch//"

# Sample list files (relative paths from this script's directory)
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DY_LIST = os.path.join(SCRIPT_DIR, "DY_2018_UL.txt")
TTBAR_LIST = os.path.join(SCRIPT_DIR, "ttJets_2018_UL.txt")


def read_file_list(list_path, max_files=None):
    """Read sample paths from a text file."""
    with open(list_path) as f:
        paths = [ln.strip() for ln in f if ln.strip() and not ln.startswith('#')]
    if max_files:
        paths = paths[:max_files]
    return paths


def get_sum_weights_from_file(filepath):
    """Get sum of generator weights from a single NanoAOD file's Runs tree."""
    try:
        f = ROOT.TFile.Open(filepath)
        if not f or f.IsZombie():
            print(f"  ERROR: Cannot open {filepath}")
            return None, None

        # NanoAOD stores sum of weights in the Runs tree
        runs = f.Get("Runs")
        if not runs:
            f.Close()
            return None, None

        sum_weights = 0.0
        n_events = 0
        for entry in runs:
            if hasattr(entry, 'genEventSumw'):
                sum_weights += entry.genEventSumw
            if hasattr(entry, 'genEventCount'):
                n_events += entry.genEventCount

        f.Close()
        return sum_weights, n_events
    except Exception as e:
        print(f"  ERROR: Exception opening {filepath}: {e}")
        return None, None


def get_total_sum_weights(file_list, xrootd_prefix=XROOTD_REDIRECTOR, max_files=None):
    """
    Sum generator weights from all files in the list.

    Args:
        file_list: List of /store/... paths
        xrootd_prefix: XRootD redirector URL
        max_files: Maximum number of files to process (for testing)

    Returns:
        total_sum_weights, total_n_events
    """
    total_sum_w = 0.0
    total_n_events = 0
    n_success = 0
    n_failed = 0

    files_to_process = file_list[:max_files] if max_files else file_list

    for i, path in enumerate(files_to_process):
        if i % 10 == 0:
            print(f"  Processing file {i+1}/{len(files_to_process)}...")

        full_path = xrootd_prefix + path
        sum_w, n_events = get_sum_weights_from_file(full_path)

        if sum_w is not None:
            total_sum_w += sum_w
            total_n_events += n_events
            n_success += 1
        else:
            n_failed += 1

    print(f"  Processed {n_success} files successfully, {n_failed} failed")
    return total_sum_w, total_n_events


def calculate_scale_factor(sum_weights, xsec_pb, lumi_pb=LUMINOSITY_PB):
    """
    Calculate the scale factor to normalize MC to data.

    Scale = (Luminosity × Cross-section) / Sum_of_weights
    """
    if sum_weights is None or sum_weights == 0:
        return None

    n_expected = lumi_pb * xsec_pb
    scale = n_expected / sum_weights
    return scale


if __name__ == "__main__":
    print("=" * 70)
    print("MC Normalization Factor Calculator")
    print("=" * 70)
    print(f"Luminosity: {LUMINOSITY} fb^-1 = {LUMINOSITY_PB} pb^-1")
    print()

    # Configuration
    # Set to None to process all files, or a number to limit (for testing)
    MAX_FILES = None  # e.g., 10 for testing, None for full calculation

    samples = {
        "DY": {
            "list_file": DY_LIST,
            "xsec": XSEC_DY,
            "description": "DYJetsToLL_M-50 (aMC@NLO)",
        },
        "ttbar": {
            "list_file": TTBAR_LIST,
            "xsec": XSEC_TTBAR,
            "description": "TTJets (aMC@NLO)",
        }
    }

    results = {}

    for name, info in samples.items():
        print("=" * 70)
        print(f"Processing {info['description']} ({name})")
        print("=" * 70)
        print(f"Sample list: {info['list_file']}")
        print(f"Cross-section: {info['xsec']} pb")
        print()

        if not os.path.exists(info['list_file']):
            print(f"ERROR: File list not found: {info['list_file']}")
            continue

        # Read file list
        file_list = read_file_list(info['list_file'])
        print(f"Found {len(file_list)} files in the list")

        if MAX_FILES:
            print(f"Processing only first {MAX_FILES} files (testing mode)")
        print()

        # Get sum of weights
        print("Reading Runs tree from original NanoAOD files...")
        sum_w, n_events = get_total_sum_weights(file_list, max_files=MAX_FILES)

        if sum_w is not None and sum_w > 0:
            print()
            print(f"Total N events (genEventCount): {n_events:,.0f}")
            print(f"Total sum of weights (genEventSumw): {sum_w:,.2f}")

            # Calculate scale factor
            n_expected = LUMINOSITY_PB * info['xsec']
            scale = calculate_scale_factor(sum_w, info['xsec'])

            print()
            print(f"N_expected = L × σ = {LUMINOSITY_PB} × {info['xsec']} = {n_expected:,.0f}")
            print(f"Scale factor = N_expected / sum_weights = {scale:.6f}")

            results[name] = {
                'n_events': n_events,
                'sum_weights': sum_w,
                'scale': scale,
            }
        else:
            print("ERROR: Could not get weights from files")
        print()

    # Summary
    print("=" * 70)
    print("SUMMARY")
    print("=" * 70)
    for name, res in results.items():
        print(f"{name}:")
        print(f"  N events:    {res['n_events']:,.0f}")
        print(f"  Sum weights: {res['sum_weights']:,.2f}")
        print(f"  Scale factor: {res['scale']:.6f}")
        print()

    print("=" * 70)
    print("HOW TO USE THESE FACTORS:")
    print("=" * 70)
    print()
    print("In your plotting macro, the total weight for each MC event should be:")
    print()
    print("  weight = Scale × PROTON_ACCEPTANCE × muon_SFs")
    print()
    print("Where PROTON_ACCEPTANCE = 0.245 (probability of protons on both arms)")
    print()
    print("For DY (using flat normalization, not generator_weight):")
    print(f"  h_dy->Scale(DY_SCALE);  // where DY_SCALE = {results.get('DY', {}).get('scale', 'N/A')}")
    print()
    print("For ttbar (using flat normalization):")
    print(f"  h_ttbar->Scale(TTBAR_SCALE);  // where TTBAR_SCALE = {results.get('ttbar', {}).get('scale', 'N/A')}")
    print()
    print("NOTE: If your event_weight already includes 0.15 for ttbar (as in fase1_ttjets.py),")
    print("you need to adjust accordingly.")
    print("=" * 70)
