#!/usr/bin/env python3

import os
import sys

import ROOT


# -------- Hardcoded configuration (paths, weight, probabilities) --------
PROTON_FILE = "/eos/cms/store/group/phys_smp/Exclusive_DiTau/proton_pool_2018/proton_pool_2018.root"
# Use your merged MuTau phase1 file here (tree name will be auto-detected between 'tree' and 'tree_out')
INPUT_FILE = "/eos/user/m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged.root"
OUTPUT_FILE = "/eos/user/m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_pileup_protons.root"

# Fixed weight to store in 'weight' branch
FIXED_WEIGHT = 0.13 # related too probabilities on the pps

# Probabilities matching the tau-tau C++
P11 = 0.08
P12 = 0.02
P21 = 0.02
P22 = 0.005


def ensure_branch(tree, name):
    b = tree.GetListOfBranches().FindObject(name)
    return b is not None



def _get_tree(f, names=("tree", "tree_out")):
    for n in names:
        t = f.Get(n)
        if t:
            return t
    return None


def main():
    # Open input files
    f_protons = ROOT.TFile.Open(PROTON_FILE)
    if not f_protons or f_protons.IsZombie():
        print(f"ERROR: Could not open proton pool file: {PROTON_FILE}", file=sys.stderr)
        sys.exit(1)
    f_in = ROOT.TFile.Open(INPUT_FILE)
    if not f_in or f_in.IsZombie():
        print(f"ERROR: Could not open input file: {INPUT_FILE}", file=sys.stderr)
        sys.exit(1)

    t_protons = _get_tree(f_protons, ("tree",))
    t_in = _get_tree(f_in)
    if not t_protons:
        print("ERROR: Proton pool tree 'tree' not found", file=sys.stderr)
        sys.exit(1)
    if not t_in:
        print("ERROR: Input tree not found (tried 'tree' and 'tree_out')", file=sys.stderr)
        sys.exit(1)

    # Basic checks
    n_protons = t_protons.GetEntries()
    n_mc = t_in.GetEntries()
    if n_protons < 3 * n_mc:
        print(f"ERROR: Not enough protons in pool ({n_protons}) for {n_mc} events. Need at least {3*n_mc}.", file=sys.stderr)
        sys.exit(1)

    # Fractions from probabilities
    total = P11 + P12 + P21 + P22
    frac11 = P11 / total
    frac12 = P12 / total
    frac21 = P21 / total

    # Prepare output
    out_dir = os.path.dirname(OUTPUT_FILE)
    if out_dir:
        os.makedirs(out_dir, exist_ok=True)

    f_out = ROOT.TFile(OUTPUT_FILE, "RECREATE")
    t_out = ROOT.TTree("tree", "MuTau with merged pileup protons")

    # Helper: buffer creation (float/double as 'd', int as 'i')
    from array import array

    # Expected MuTau branches (will be filled only if present in input)
    spec = [
        ("mu_id", "i", 0),
        ("tau_id1", "i", 0),
        ("tau_id2", "i", 0),
        ("tau_id3", "i", 0),
        ("mu_pt", "f", -1.0), ("tau_pt", "f", -1.0),
        ("mu_charge", "i", 0), ("tau_charge", "i", 0),
        ("mu_eta", "f", -999.0), ("tau_eta", "f", -999.0),
        ("mu_n", "i", 0), ("tau_n", "i", 0),
        ("mu_phi", "f", -999.0), ("tau_phi", "f", -999.0),
        ("mu_mass", "f", 0.0), ("tau_mass", "f", 0.0),
        ("sist_mass", "d", -1.0), ("acop", "d", -1.0),
        ("sist_pt", "d", -1.0), ("sist_rap", "d", -999.0),
        ("met_pt", "f", -1.0), ("met_phi", "f", -999.0),
        ("jet_pt", "f", -1.0), ("jet_eta", "f", -999.0),
        ("jet_phi", "f", -999.0), ("jet_mass", "f", -1.0),
        ("jet_btag", "f", -1.0),
        ("n_b_jet", "i", 0),
        ("generator_weight", "f", 1.0),
    ]

    # Input type overrides so SetBranchAddress matches the real input branch type
    input_type_override = {
        # IDs come as UChar_t in your input trees
        "mu_id": 'B',
        "tau_id1": 'B',
        "tau_id2": 'B',
        "tau_id3": 'B',
    }

    # Build input readers and output branches
    inputs = {}
    outputs = {}
    def _leaf_code(typ_code):
        return {
            'i': 'I',
            'f': 'F',
            'd': 'D',
            'b': 'B',
            'B': 'B',
        }.get(typ_code, 'D')

    for name, typ, default in spec:
        # Output buffer
        buf = array(typ, [default])
        outputs[name] = buf
        # Create output branch with matching leaf code
        leaf_type = _leaf_code(typ)
        t_out.Branch(name, buf, f"{name}/{leaf_type}")
        # If exists in input, bind a reader; else keep default per-event
        if ensure_branch(t_in, name):
            # Input buffer must match the input branch C++ type
            in_code = input_type_override.get(name, typ)
            in_default = 0 if in_code in ('b', 'B') else default
            inbuf = array(in_code, [in_default])
            inputs[name] = inbuf
            t_in.SetBranchAddress(name, inbuf)

    # Fixed weight branch (override whatever is in input)
    weight = array("d", [float(FIXED_WEIGHT)])
    t_out.Branch("weight", weight, "weight/D")

    # Proton xi branches
    xi_arm1_1 = array("d", [-1.0])
    xi_arm1_2 = array("d", [-1.0])
    xi_arm2_1 = array("d", [-1.0])
    xi_arm2_2 = array("d", [-1.0])
    t_out.Branch("`xi_arm1_1`", xi_arm1_1, "xi_arm1_1/D")
    t_out.Branch("xi_arm1_2", xi_arm1_2, "xi_arm1_2/D")
    t_out.Branch("xi_arm2_1", xi_arm2_1, "xi_arm2_1/D")
    t_out.Branch("xi_arm2_2", xi_arm2_2, "xi_arm2_2/D")

    # Proton leaf handles
    has_arm = ensure_branch(t_protons, "proton_arm")
    has_xi = ensure_branch(t_protons, "proton_xi")
    if not (has_arm and has_xi):
        print("ERROR: proton tree missing required branches 'proton_arm' and/or 'proton_xi'", file=sys.stderr)
        sys.exit(1)

    # Loop and fill
    n_rndm = 0
    for i in range(n_mc):
        t_in.GetEntry(i)

        # Copy inputs (if bound)
        for name, inbuf in inputs.items():
            outputs[name][0] = inbuf[0]

        # Reset xi
        xi_arm1_1[0] = -1.0
        xi_arm1_2[0] = -1.0
        xi_arm2_1[0] = -1.0
        xi_arm2_2[0] = -1.0

        mc_frac = float(i) / float(n_mc) if n_mc > 0 else 0.0

        # Always pick at least one proton per arm
        while True:
            if n_rndm >= n_protons:
                # Safety wrap-around (should not happen due to pre-check)
                n_rndm = 0
            t_protons.GetEntry(n_rndm)
            n_rndm += 1
            arm = int(t_protons.GetLeaf("proton_arm").GetValue())
            xi = float(t_protons.GetLeaf("proton_xi").GetValue())
            if arm == 0 and xi_arm1_1[0] < 0: xi_arm1_1[0] = xi
            if arm == 1 and xi_arm2_1[0] < 0: xi_arm2_1[0] = xi
            if xi_arm1_1[0] >= 0 and xi_arm2_1[0] >= 0:
                break

        # Possibly add a second proton in arm 2
        if (mc_frac > frac11 and mc_frac < (frac11 + frac12)) or (mc_frac > (frac11 + frac12 + frac21)):
            while True:
                if n_rndm >= n_protons:
                    n_rndm = 0
                t_protons.GetEntry(n_rndm)
                n_rndm += 1
                if int(t_protons.GetLeaf("proton_arm").GetValue()) == 1:
                    xi_arm2_2[0] = float(t_protons.GetLeaf("proton_xi").GetValue())
                    break

        # Possibly add a second proton in arm 1
        if (mc_frac > (frac11 + frac12) and mc_frac < (frac11 + frac12 + frac21)) or (mc_frac > (frac11 + frac12 + frac21)):
            while True:
                if n_rndm >= n_protons:
                    n_rndm = 0
                t_protons.GetEntry(n_rndm)
                n_rndm += 1
                if int(t_protons.GetLeaf("proton_arm").GetValue()) == 0:
                    xi_arm1_2[0] = float(t_protons.GetLeaf("proton_xi").GetValue())
                    break

        # weight is fixed per event (already set)
        t_out.Fill()

    print("Enrichment completed.")
    f_out.Write()
    f_out.Close()


if __name__ == "__main__":
    main()
