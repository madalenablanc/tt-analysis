#!/usr/bin/env python3
"""
Script to open a ROOT file from the input list and print all proton-related variables.
"""
import os
import sys
import ROOT

# Enable multi-threading for better performance
ROOT.EnableImplicitMT()

# Configuration
input_list = "Data_MuTau_phase0_2018.txt"

# XRootD configuration
os.environ.setdefault("X509_USER_PROXY", f"/tmp/x509up_u{os.getuid()}")
os.environ.setdefault("XrdSecGSISRVNAMES", "*")
os.environ.setdefault("XRD_REQUESTTIMEOUT", "60")
os.environ.setdefault("XRD_CONNECTIONWINDOW", "15")
os.environ.setdefault("XRD_STREAMTIMEOUT", "120")

# Read the first file from the input list
print(f"Reading input list from: {input_list}")
with open(input_list) as f:
    lines = [ln.strip() for ln in f if ln.strip()]

if not lines:
    print("ERROR: No files found in input list!")
    sys.exit(1)

# Get the first file path
store_path = lines[0]
print(f"\nFirst file in list: {store_path}")

# Build the full XRootD URL
redirector = "root://cms-xrd-global.cern.ch"
if store_path.startswith("root://"):
    file_url = store_path
else:
    file_url = f"{redirector}//{store_path.lstrip('/')}"

print(f"Opening file: {file_url}")
print("=" * 80)

# Open the ROOT file
tfile = ROOT.TFile.Open(file_url)
if not tfile or tfile.IsZombie():
    print(f"ERROR: Could not open file: {file_url}")
    sys.exit(1)

print(f"✅ Successfully opened file!")
print()

# Get the Events tree
tree = tfile.Get("Events")
if not tree:
    print("ERROR: Could not find 'Events' tree in file!")
    tfile.Close()
    sys.exit(1)

print(f"Found 'Events' tree with {tree.GetEntries()} entries")
print()

# Get all branch names
branches = [branch.GetName() for branch in tree.GetListOfBranches()]

# Search for proton-related variables
proton_keywords = ['Proton', 'proton', 'PPS', 'pps', 'forward', 'Forward']

print("=" * 80)
print("SEARCHING FOR PROTON-RELATED VARIABLES")
print("=" * 80)

proton_branches = []
for branch_name in branches:
    for keyword in proton_keywords:
        if keyword in branch_name:
            proton_branches.append(branch_name)
            break

if proton_branches:
    print(f"\n✅ Found {len(proton_branches)} proton-related branches:\n")
    for branch in sorted(proton_branches):
        branch_obj = tree.GetBranch(branch)
        branch_type = branch_obj.GetClassName() if branch_obj.GetClassName() else branch_obj.GetTitle()
        print(f"  • {branch}")
        print(f"    Type: {branch_type}")

    print("\n" + "=" * 80)
    print("PRINTING VALUES FROM FIRST EVENT")
    print("=" * 80)

    # Read first event
    tree.GetEntry(0)

    for branch in sorted(proton_branches):
        try:
            value = getattr(tree, branch)
            print(f"\n{branch}:")

            # Check if it's a collection (vector)
            if hasattr(value, 'size'):
                print(f"  Size: {value.size()}")
                if value.size() > 0:
                    print(f"  Values: {[value[i] for i in range(min(value.size(), 10))]}")
                    if value.size() > 10:
                        print(f"  ... (showing first 10 of {value.size()} values)")
            else:
                print(f"  Value: {value}")
        except Exception as e:
            print(f"  Error reading value: {e}")
else:
    print("\n⚠️  No proton-related variables found in this file.")
    print("\nSearched for keywords:", ', '.join(proton_keywords))

    # Optionally show all available branches
    print(f"\n📋 Total branches available: {len(branches)}")
    response = input("\nWould you like to see all branch names? (y/n): ")
    if response.lower() == 'y':
        print("\nAll available branches:")
        for i, branch in enumerate(sorted(branches), 1):
            print(f"  {i:4d}. {branch}")

print("\n" + "=" * 80)

# Clean up
tfile.Close()
print("\n✅ Done!")
