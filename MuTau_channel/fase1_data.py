#!/usr/bin/env python3


import subprocess, sys

command = "voms-proxy-init --rfc --voms cms --valid 172:00"

subprocess.run(command, shell = True, executable="/bin/bash")
import os, time, json, signal, sys
import ROOT

# ---------- Environment / XRootD client ----------
os.environ.setdefault("X509_USER_PROXY", f"/tmp/x509up_u{os.getuid()}")
os.environ.setdefault("XrdSecGSISRVNAMES", "*")

# Enable multithreading
ROOT.EnableImplicitMT()

# ---------- Parameters ----------
line_number   = -1   # -1 => process all; >=0 => only that line index
input_list    = "Data_QCD_MuTau_skimmed_2018.txt"  # one /store/... (or full PFN) per line
output_prefix = "/eos/user/m/mblancco/samples_2018_mutau/fase1_data_proton_vars/Data_2018_UL_skimmed_MuTau_cuts_"
lumi_file     = "dadosluminosidade.txt"
resume_path   = ".mutau_phase1_qcd_resume.json"
overwrite     = True
prefix        = "/eos/home-m/mblancco/samples_2018_mutau/fase0_mutau_proton_vars/"

print("Processing Data - phase1\n")
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
    print("\n📝 Caught signal, writing resume file…")
    save_resume(resume_path, resume)
    sys.exit(1)

signal.signal(signal.SIGINT, _sig_handler)
signal.signal(signal.SIGTERM, _sig_handler)


# ---------- Read input list ----------
with open(input_list) as f:
    lines = [ln.strip() for ln in f if ln.strip()]

if line_number >= 0 and line_number >= len(lines):
    raise IndexError(f"line_number {line_number} >= number of files {len(lines)}")

indices = range(len(lines)) if line_number < 0 else [line_number]
total_files = len(indices)


columns = [
    "muon_id", "tau_id1", "tau_id2", "tau_id3", "muon_pt", "tau_pt",
    "muon_charge", "tau_charge", "muon_eta", "tau_eta",
    "muon_n", "tau_n", "muon_phi", "tau_phi", "muon_mass", "tau_mass",
    "sist_mass", "acop", "sist_pt", "sist_rap", "met_pt", "met_phi",
    "jet_pt", "jet_eta", "jet_phi", "jet_mass", "jet_btag",
    "weight", "n_b_jet",
    # Proton collections (from phase 0)
    "nproton_multi", "nproton_single",
    "proton_multi_xi", "proton_multi_arm", "proton_multi_t",
    "proton_multi_thetaX", "proton_multi_thetaY",
    "proton_multi_time", "proton_multi_timeUnc",
    "proton_single_xi"
]

# ---------- Main loop ----------
for file_idx, idx in enumerate(indices):
    path = lines[idx]  # "/store/...root" or full PFN
    out_file = f"{output_prefix}{idx}.root"

    print(f"\n📦 Processing file {file_idx+1} of {total_files}:")
    print(f" → {path}")

    # Skip if already completed (unless overwrite)
    if not overwrite and (os.path.exists(out_file) or idx in resume.get("done", [])):
        print(f"⏭️  Skipping index {idx} (already done).")
        continue

    # Build input path
    input_path = prefix + path if not path.startswith("root://") else path
    
    print(f"📦 Input : {input_path}")
    print(f"📤 Output: {out_file}")

    try:
        # Create RDataFrame
        df = ROOT.RDataFrame("tree", input_path)


        df_added=(
            df

            .Define("muon", "TLorentzVector mu; mu.SetPtEtaPhiM(muon_pt, muon_eta, muon_phi, muon_mass); return mu;")
            .Define("tau", "TLorentzVector t; t.SetPtEtaPhiM(tau_pt, tau_eta, tau_phi, tau_mass); return t;")
            .Define("delta_r","""
                double dr=tau.DeltaR(muon);
                return dr;
            """)

        )

        # Apply event selection with lumi filtering
        df_filtered = (
            # df.Filter("lumi_filter(run, luminosityBlock)", "Certified lumi")
            #   .Filter("HLT_IsoMu24 == 1", "HLT single-muon 2018")
              df_added
              # muon_pt and tau_pt are scalars from phase 0, not vectors - no .size() needed
              .Filter("muon_id > 3", "Muon ID (> Medium)")
              .Filter("tau_id1 > 63", "Tau VSjet")
              .Filter("tau_id2   > 7",  "Tau VSe")
              .Filter("tau_id3  > 1",  "Tau VSmu")
              .Filter("muon_pt > 35. && tau_pt > 100.", "pT thresholds")
              .Filter("muon_charge * tau_charge < 0", "Opposite sign")
              .Filter("delta_r>0.4", "Delta R accepance")
              .Filter("fabs(tau_eta)<2.4 && fabs(muon_eta)<2.4","Geometrical acceptance")
        )

        # Define derived variables
        # df_defs = (
        #     df_filtered
        #     .Define("mu_pt", "Muon_pt[0]")
        #     .Define("tau_pt", "Tau_pt[0]")
        #     .Define("mu_eta", "Muon_eta[0]")
        #     .Define("tau_eta", "Tau_eta[0]")
        #     .Define("mu_phi", "Muon_phi[0]")
        #     .Define("tau_phi", "Tau_phi[0]")
        #     .Define("mu_mass", "Muon_mass[0]")
        #     .Define("tau_mass", "Tau_mass[0]")
        #     .Define("mu_charge", "Muon_charge[0]")
        #     .Define("tau_charge", "Tau_charge[0]")
        #     .Define("mu_id", "Muon_mvaId[0]")
        #     .Define("tau_id1", "Tau_idDeepTau2017v2p1VSjet[0]")
        #     .Define("tau_id2", "Tau_idDeepTau2017v2p1VSe[0]")
        #     .Define("tau_id3", "Tau_idDeepTau2017v2p1VSmu[0]")
        #     .Define("mu_n", "int(Muon_pt.size())")
        #     .Define("tau_n", "int(Tau_pt.size())")

        #     .Define("muon", "TLorentzVector m; m.SetPtEtaPhiM(mu_pt, mu_eta, mu_phi, mu_mass); return m;")
        #     .Define("tau", "TLorentzVector t; t.SetPtEtaPhiM(tau_pt, tau_eta, tau_phi, tau_mass); return t;")
        #     .Define("sist", "return muon + tau;")
        #     .Define("sist_mass", "sist.M()")
        #     .Define("sist_pt", "sist.Pt()")
        #     .Define("sist_rap", "sist.Rapidity()")
        #     .Define("acop", """
        #         double dphi = fabs(mu_phi - tau_phi);
        #         if (dphi > M_PI) dphi = 2 * M_PI - dphi;
        #         return fabs(dphi) / M_PI;
        #     """)

        #     .Define("met_pt", "MET_pt")
        #     .Define("met_phi", "MET_phi")

        #     .Define("jet_pt", "Jet_pt.size() > 0 ? Jet_pt[0] : -1")
        #     .Define("jet_eta", "Jet_eta.size() > 0 ? Jet_eta[0] : -999")
        #     .Define("jet_phi", "Jet_phi.size() > 0 ? Jet_phi[0] : -999")
        #     .Define("jet_mass", "Jet_mass.size() > 0 ? Jet_mass[0] : -999")
        #     .Define("jet_btag", "Jet_btagDeepB.size() > 0 ? Jet_btagDeepB[0] : -999")
        #     .Define("n_b_jet", """
        #         int count = 0;
        #         for (auto b : Jet_btagDeepB)
        #             if (b > 0.4506) count++;
        #         return count;
        #     """)

        #     # Data weights (no generator info)
        #     .Define("weight", "1.0")
        # )

        # Create output directory if needed
        out_dir = os.path.dirname(out_file)
        if out_dir and not out_dir.startswith("root://"):
            os.makedirs(out_dir, exist_ok=True)

        # Snapshot
        df_filtered.Snapshot("tree", out_file, columns)

        print(f"✅ Done. Output saved to:\n{out_file}")

        # Update resume state
        if idx not in resume["done"]:
            resume["done"].append(idx)
        save_resume(resume_path, resume)

    except Exception as e:
        print(f"❌ Failed to process {path}: {e}")
        continue

print("✅ DATA phase1 cuts completed for requested files.")