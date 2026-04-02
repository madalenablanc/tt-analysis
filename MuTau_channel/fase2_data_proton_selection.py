#!/usr/bin/env python3
"""
Phase 2: Apply proton requirements
Requirement: At least one proton per arm
"""

import subprocess, sys

command = "voms-proxy-init --rfc --voms cms --valid 172:00"
subprocess.run(command, shell=True, executable="/bin/bash")

import os, time, json, signal
import ROOT

# ---------- Environment / XRootD client ----------
os.environ.setdefault("X509_USER_PROXY", f"/tmp/x509up_u{os.getuid()}")
os.environ.setdefault("XrdSecGSISRVNAMES", "*")

# Enable multithreading
ROOT.EnableImplicitMT()

# ---------- Parameters ----------
line_number   = -1   # -1 => process all; >=0 => only that line index
input_list    = "Data_MuTau_phase1_2018.txt"  #phase1 output files
output_prefix = "/eos/user/m/mblancco/samples_2018_mutau/fase2_data_pps/Data_2018_UL_PPS_selection_"
resume_path   = ".mutau_phase2_pps_resume.json"
overwrite     = True
prefix        = "/eos/user/m/mblancco/samples_2018_mutau/fase1_data_proton_vars/

print("Processing Data - Phase 2 (PPS Selection)\n")
print(f"Output directory: {os.path.dirname(output_prefix)}")

# ---------- Resume helpers ----------
def load_resume(path):
    try:
        with open(path) as f: return json.load(f)
    except Exception:
        return {"done": []}

def save_resume(path, state):
    tmp = path + ".tmp"
    with open(tmp, "w") as f: json.dump(state, f, indent=2, sort_keys=True)
    os.replace(tmp, path)

resume = load_resume(resume_path)

def _sig_handler(signum, frame):
    print("\nCaught signal, writing resume file...")
    save_resume(resume_path, resume)
    sys.exit(1)

signal.signal(signal.SIGINT, _sig_handler)
signal.signal(signal.SIGTERM, _sig_handler)


# process input list
with open(input_list) as f:
    lines = [ln.strip() for ln in f if ln.strip()]

if line_number >= 0 and line_number >= len(lines):
    raise IndexError(f"line_number {line_number} >= number of files {len(lines)}")

indices = range(len(lines)) if line_number < 0 else [line_number]
total_files = len(indices)


#output columns-> same as phase1 plus selected protons
columns = [
    "muon_id", "tau_id1", "tau_id2", "tau_id3", "muon_pt", "tau_pt",
    "muon_charge", "tau_charge", "muon_eta", "tau_eta",
    "muon_n", "tau_n", "muon_phi", "tau_phi", "muon_mass", "tau_mass",
    "sist_mass", "acop", "sist_pt", "sist_rap", "met_pt", "met_phi",
    "jet_pt", "jet_eta", "jet_phi", "jet_mass", "jet_btag",
    "weight", "n_b_jet",
    # Proton collections
    "nproton_multi", "nproton_single",
    "proton_multi_xi", "proton_multi_arm", "proton_multi_t",
    "proton_multi_thetaX", "proton_multi_thetaY",
    "proton_multi_time", "proton_multi_timeUnc",
    "proton_single_xi",
    # Selected protons (largest xi per arm)
    "selected_xi_arm0", "selected_xi_arm1"
]

# ---------- Main loop ----------
for file_idx, idx in enumerate(indices):
    path = lines[idx]
    out_file = f"{output_prefix}{idx}.root"

    print(f"\nProcessing file {file_idx+1} of {total_files}:")
    print(f" -> {path}")

    #skip if complete
    if not overwrite and (os.path.exists(out_file) or idx in resume.get("done", [])):
        print(f"Skipping index {idx} (already done).")
        continue

    #input
    input_path = prefix + path if not path.startswith("root://") else path

    print(f"Input : {input_path}")
    print(f"Output: {out_file}")

    try:
        df = ROOT.RDataFrame("tree", input_path)

        # PPS selection: use multiRP protons, at least one per arm, select largest xi if multiple
        df_pps = df.Filter("""
            // Require nProton_multiRP > 0
            if (nproton_multi == 0) return false;

            // Find largest xi proton in each arm
            bool has_arm0 = false;
            bool has_arm1 = false;

            for (unsigned int i = 0; i < proton_multi_arm.size(); i++) {
                if (proton_multi_arm[i] == 0) has_arm0 = true;
                if (proton_multi_arm[i] == 1) has_arm1 = true;
            }

            // Require at least one proton in each arm
            return has_arm0 && has_arm1;
        """, "PPS: >=1 multiRP proton per arm")

        # Define selected protons (largest xi per arm)
        df_selected = (
            df_pps
            .Define("selected_xi_arm0", """
                double max_xi = -1.0;
                for (unsigned int i = 0; i < proton_multi_arm.size(); i++) {
                    if (proton_multi_arm[i] == 0 && proton_multi_xi[i] > max_xi) {
                        max_xi = proton_multi_xi[i];
                    }
                }
                return max_xi;
            """)
            .Define("selected_xi_arm1", """
                double max_xi = -1.0;
                for (unsigned int i = 0; i < proton_multi_arm.size(); i++) {
                    if (proton_multi_arm[i] == 1 && proton_multi_xi[i] > max_xi) {
                        max_xi = proton_multi_xi[i];
                    }
                }
                return max_xi;
            """)
        )

        # Get event counts and proton statistics
        n_total = df.Count().GetValue()
        n_pass = df_pps.Count().GetValue()
        efficiency = 100.0 * n_pass / n_total if n_total > 0 else 0

        # Proton multiplicity statistics
        h_nproton = df.Histo1D(("h_nproton", "", 10, 0, 10), "nproton_multi")
        h_nproton_pass = df_pps.Histo1D(("h_nproton_pass", "", 10, 0, 10), "nproton_multi")

        avg_protons_all = df.Mean("nproton_multi").GetValue()
        avg_protons_pass = df_pps.Mean("nproton_multi").GetValue() if n_pass > 0 else 0

        print(f"Events: {n_total} -> {n_pass} (efficiency: {efficiency:.2f}%)")
        print(f"Average protons/event (all): {avg_protons_all:.2f}")
        print(f"Average protons/event (pass): {avg_protons_pass:.2f}")

        # Count events by proton multiplicity
        print("Proton multiplicity distribution (passed events):")
        for i in range(10):
            count = int(h_nproton_pass.GetBinContent(i+1))
            if count > 0:
                print(f"  {i} protons: {count} events")

        #output dir
        out_dir = os.path.dirname(out_file)
        if out_dir and not out_dir.startswith("root://"):
            os.makedirs(out_dir, exist_ok=True)

        #save
        df_selected.Snapshot("tree", out_file, columns)

        print(f"Done. Output saved to: {out_file}")

        #update 
        if idx not in resume["done"]:
            resume["done"].append(idx)
        save_resume(resume_path, resume)

    except Exception as e:
        print(f"Failed to process {path}: {e}")
        import traceback
        traceback.print_exc()
        continue

print("\nPhase 2 (PPS selection) completed for requested files.")