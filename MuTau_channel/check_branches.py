#!/usr/bin/env python3
"""Quick script to check what branches exist in each file"""

import ROOT

files = {
    "Data": "/eos/user/m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged.root",
    "QCD": "/eos/user/m/mblancco/samples_2018_mutau/QCD_2018_UL_MuTau_nano_merged.root",
    "DY": "/eos/user/m/mblancco/samples_2018_mutau/DY_2018_UL_MuTau_nano_merged_pileup_protons.root",
    "ttjets": "/eos/user/m/mblancco/samples_2018_mutau/ttjets_2018_UL_MuTau_nano_merged_pileup_protons.root",
}

for name, path in files.items():
    print(f"\n{'='*60}")
    print(f"{name}: {path}")
    print('='*60)

    f = ROOT.TFile.Open(path)
    if not f or f.IsZombie():
        print("ERROR: Cannot open file")
        continue

    tree = f.Get("tree")
    if not tree:
        print("ERROR: No tree found")
        continue

    print(f"Branches ({tree.GetNbranches()}):")
    branches = [tree.GetListOfBranches().At(i).GetName() for i in range(tree.GetNbranches())]

    # Group by category
    proton_branches = [b for b in branches if 'proton' in b.lower() or 'pps' in b.lower() or 'xi_' in b or 'n_pu' in b]
    tau_branches = [b for b in branches if 'tau' in b.lower() and 'id' in b.lower()]
    mu_branches = [b for b in branches if 'mu' in b.lower() and ('id' in b.lower() or 'pt' in b.lower() or 'eta' in b.lower())]

    print("\n  Proton-related:")
    for b in sorted(proton_branches):
        print(f"    - {b}")

    print("\n  Tau ID:")
    for b in sorted(tau_branches):
        print(f"    - {b}")

    print("\n  Muon:")
    for b in sorted(mu_branches)[:10]:  # Just first 10
        print(f"    - {b}")

    f.Close()

print("\n" + "="*60)
