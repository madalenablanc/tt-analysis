#!/usr/bin/python

import subprocess, sys

command = "voms-proxy-init --rfc --voms cms --valid 172:00"

subprocess.run(command, shell = True, executable="/bin/bash")


import ROOT
import math
import os
import json
import signal
import sys

ROOT.EnableImplicitMT()

# === Parameters ===
line_number   = -1   # -1 => process all; >=0 => only that line index
input_list    = "DY_2018_UL.txt"
prefix        = "root://cms-xrd-global.cern.ch//"
output_prefix = "/eos/user/m/mblancco/samples_2018_mutau/fase0_DY/DY_2018_UL_skimmed_MuTau_nano_"
resume_path   = ".mutau_phase0_dy_resume.json"
overwrite     = True

# ---------- Resume helpers ----------
def load_resume(path):
    try:
        with open(path) as f:
            return json.load(f)
    except Exception:
        return {"done": []}

def save_resume(path, state):
    tmp = path + ".tmp"
    with open(tmp, "w") as f:
        json.dump(state, f, indent=2, sort_keys=True)
    os.replace(tmp, path)

resume = load_resume(resume_path)

def _sig_handler(signum, frame):
    print("\n📝 Caught signal, writing resume file…")
    save_resume(resume_path, resume)
    sys.exit(1)

signal.signal(signal.SIGINT, _sig_handler)
signal.signal(signal.SIGTERM, _sig_handler)

# === Load list of files ===
with open(input_list) as f:
    lines = [l.strip() for l in f if l.strip()]

if line_number >= 0 and line_number >= len(lines):
    raise IndexError(f"line_number {line_number} >= number of files {len(lines)}")

indices = range(len(lines)) if line_number < 0 else [line_number]

# Output columns (DY includes generator info)
columns = [
    "mu_id", "tau_id1", "tau_id2", "tau_id3", "mu_pt", "tau_pt",
    "mu_charge", "tau_charge", "mu_eta", "tau_eta",
    "mu_n", "tau_n", "mu_phi", "tau_phi", "mu_mass", "tau_mass",
    "sist_mass", "acop", "sist_pt", "sist_rap", "met_pt", "met_phi",
    "jet_pt", "jet_eta", "jet_phi", "jet_mass", "jet_btag",
    "weight", "n_b_jet", "generator_weight", "tau_decay", "tau_genmatch"
]

total_files = len(indices)
for file_idx, i in enumerate(indices):
    line = lines[i]
    input_file = prefix + line if not line.startswith("root://") else line
    output_file = f"{output_prefix}{i}.root"

    print(f"\n📦 Processing file {file_idx+1}/{total_files} (index {i}):")
    print(f"    Input : {input_file}")
    print(f"    Output: {output_file}")

    # Skip if already completed (unless overwrite)
    if not overwrite and (os.path.exists(output_file) or i in resume.get("done", [])):
        print(f"⏭️  Skipping index {i} (already done).")
        continue

    # Ensure output directory exists
    out_dir = os.path.dirname(output_file)
    if out_dir and not out_dir.startswith("root://"):
        os.makedirs(out_dir, exist_ok=True)

    try:
        df = ROOT.RDataFrame("Events", input_file)

        df = df.Filter("HLT_IsoMu24 == 1", "Passes muon trigger") \
               .Filter("Muon_pt.size() > 0 && Tau_pt.size() > 0", "Has Muon and Tau")

        df = (
            df.Define("mu_pt", "Muon_pt[0]")
              .Define("mu_eta", "Muon_eta[0]")
              .Define("mu_phi", "Muon_phi[0]")
              .Define("mu_mass", "Muon_mass[0]")
              .Define("mu_charge", "Muon_charge[0]")
              .Define("mu_id", "Muon_mvaId[0]")
              .Define("mu_n", "int(Muon_pt.size())")

              .Define("tau_pt", "Tau_pt[0]")
              .Define("tau_eta", "Tau_eta[0]")
              .Define("tau_phi", "Tau_phi[0]")
              .Define("tau_mass", "Tau_mass[0]")
              .Define("tau_charge", "Tau_charge[0]")
              .Define("tau_id1", "Tau_idDeepTau2017v2p1VSjet[0]")
              .Define("tau_id2", "Tau_idDeepTau2017v2p1VSe[0]")
              .Define("tau_id3", "Tau_idDeepTau2017v2p1VSmu[0]")
              .Define("tau_n", "int(Tau_pt.size())")

              .Define("muon", "TLorentzVector mu; mu.SetPtEtaPhiM(mu_pt, mu_eta, mu_phi, mu_mass); return mu;")
              .Define("tau",  "TLorentzVector t; t.SetPtEtaPhiM(tau_pt, tau_eta, tau_phi, tau_mass); return t;")
              .Define("sist", "return muon + tau;")
              .Define("sist_mass", "sist.M()")
              .Define("sist_pt",   "sist.Pt()")
              .Define("sist_rap",  "sist.Rapidity()")
              .Define("acop", """
                  double dphi = fabs(mu_phi - tau_phi);
                  if (dphi > M_PI) dphi = 2 * M_PI - dphi;
                  return fabs(dphi) / M_PI;
              """)

              .Define("met_pt", "MET_pt")
              .Define("met_phi", "MET_phi")

              .Define("jet_pt", "Jet_pt.size() > 0 ? Jet_pt[0] : -1")
              .Define("jet_eta", "Jet_eta.size() > 0 ? Jet_eta[0] : -999")
              .Define("jet_phi", "Jet_phi.size() > 0 ? Jet_phi[0] : -999")
              .Define("jet_mass", "Jet_mass.size() > 0 ? Jet_mass[0] : -999")
              .Define("jet_btag", "Jet_btagDeepB.size() > 0 ? Jet_btagDeepB[0] : -999")
              .Define("n_b_jet", """
                  int b = 0;
                  for (auto j : Jet_btagDeepB)
                      if (j > 0.4506) b++;
                  return b;
              """)

              .Define("generator_weight", "Generator_weight")
              .Define("weight", "1.0")
              .Define("tau_decay", "Tau_decayMode[0]")
              .Define("tau_genmatch", "Tau_genPartFlav[0]")
        )

        df.Snapshot("tree", output_file, columns)
        print("✅ Done.")

        # Update resume state
        if i not in resume["done"]:
            resume["done"].append(i)
        save_resume(resume_path, resume)

    except Exception as e:
        print(f"❌ Failed on file {i}: {e}")
        continue
