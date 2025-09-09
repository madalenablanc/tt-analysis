#!/usr/bin/env python3

import subprocess, sys

# Ensure grid proxy (mirrors fase1_data structure)
command = "voms-proxy-init --rfc --voms cms --valid 172:00"
subprocess.run(command, shell=True, executable="/bin/bash")

import os, json, signal
import ROOT

# ---------- Environment / XRootD client ----------
os.environ.setdefault("X509_USER_PROXY", f"/tmp/x509up_u{os.getuid()}")
os.environ.setdefault("XrdSecGSISRVNAMES", "*")

# Enable multithreading
ROOT.EnableImplicitMT()

# ---------- Parameters ----------
line_number   = -1   # -1 => process all; >=0 => only that line index
input_list    = "DY_MuTau_skimmed_2018.txt"  # paths to phase0 DY outputs (one per line)
output_prefix = "/eos/user/m/mblancco/samples_2018_mutau/fase1_dy/DY_2018_UL_skimmed_MuTau_cuts_"
resume_path   = ".mutau_phase1_dy_resume.json"
overwrite     = True
prefix        = "/eos/user/m/mblancco/samples_2018_mutau/fase0_DY/"

print("Processing DY - phase1\n")
print(f"Output directory: {os.path.dirname(output_prefix)}")

# ---------- Muon scale factors (from POG tables) ----------
sf_decl = r"""
#include <vector>
#include <array>
#include <string>
#include <fstream>
#include <cmath>

static std::vector<std::array<double,6>> sfNeg, sfPos, sfHigh, sfIdIso;

std::vector<std::array<double,6>> readSF(const std::string &path) {
    std::vector<std::array<double,6>> table; table.reserve(128);
    std::ifstream f(path);
    double e0,e1,p0,p1,corr,syst;
    while (f >> e0 >> e1 >> p0 >> p1 >> corr >> syst) {
        table.push_back({e0,e1,p0,p1,corr,syst});
    }
    return table;
}

void loadSF() {
    static bool loaded = false; if (loaded) return; loaded = true;
    sfNeg  = readSF("/eos/user/m/mblancco/tau_analysis/POGCorrections/muon_neg18_lowTrig.txt");
    sfPos  = readSF("/eos/user/m/mblancco/tau_analysis/POGCorrections/muon_pos18_lowTrig.txt");
    sfHigh = readSF("/eos/user/m/mblancco/tau_analysis/POGCorrections/muon_18_highTrig.txt");
    sfIdIso= readSF("/eos/user/m/mblancco/tau_analysis/POGCorrections/muon_IdISO.txt");
}

double MuonTrigSF(double pt, double eta, double charge, int flag=0) {
    loadSF();
    if (pt < 35.0 || std::abs(eta) > 2.4) return 1.0;
    const auto &table = pt < 200.0 ? (charge < 0 ? sfNeg : sfPos) : sfHigh;
    double sf = 1.0;
    for (const auto &row : table) {
        if (std::abs(eta) > row[0] && std::abs(eta) <= row[1] &&
            pt > row[2] && pt <= row[3]) {
            sf = (flag == 0) ? row[4] : row[5];
            break;
        }
    }
    return sf;
}

double MuonIDISOSF(double pt, double eta, int flag=0) {
    loadSF();
    if (pt < 35.0 || std::abs(eta) > 2.4) return 1.0;
    double sf = 1.0;
    for (const auto &row : sfIdIso) {
        if (std::abs(eta) > row[0] && std::abs(eta) <= row[1] &&
            pt > row[2] && pt <= row[3]) {
            sf = (flag == 0) ? row[4] : row[5];
            break;
        }
    }
    return sf;
}

double muRecoSF(double pt, double eta, int flag=0) {
    double sf = 1.0; double a = std::abs(eta);
    if (flag == 0) {
        if (a > 2.1 && pt < 200.0) return 0.999;
        if (pt > 200.0) {
            double p = pt / std::sin(2.0 * std::atan(std::exp(-eta)));
            if (p > 100 && p < 150)  sf = a < 1.6 ? 0.9948 : 0.993;
            if (p > 150 && p < 200)  sf = a < 1.6 ? 0.9950 : 0.990;
            if (p > 200 && p < 300)  sf = a < 1.6 ? 0.994  : 0.988;
            if (p > 300 && p < 400)  sf = a < 1.6 ? 0.9914 : 0.981;
            if (p > 400 && p < 600)  sf = a < 1.6 ? 0.993  : 0.983;
            if (p > 600 && p < 1500) sf = a < 1.6 ? 0.991  : 0.978;
            if (p > 1500 && p < 3500) sf= a < 1.6 ? 1.0    : 0.98;
        }
    } else {
        if (pt < 200.0) {
            if (a < 0.9)        sf = 0.000382;
            else if (a < 1.2)   sf = 0.000522;
            else if (a < 2.1)   sf = 0.000831;
            else                sf = 0.001724;
        } else {
            double p = pt / std::sin(2.0 * std::atan(std::exp(-eta)));
            if (a < 1.6) {
                if (p < 300)          sf = 0.001;
                else if (p < 400)     sf = 0.0009;
                else if (p < 600)     sf = 0.002;
                else if (p < 1500)    sf = 0.004;
                else                  sf = 0.1;
            } else {
                if (p < 300)          sf = 0.001;
                else if (p < 400)     sf = 0.002;
                else if (p < 600)     sf = 0.003;
                else if (p < 1500)    sf = 0.006;
                else                  sf = 0.03;
            }
        }
    }
    return sf;
}
"""

ROOT.gInterpreter.Declare(sf_decl)

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

# Keep generator_weight for MC
columns = [
    "mu_id", "tau_id1", "tau_id2", "tau_id3", "mu_pt", "tau_pt",
    "mu_charge", "tau_charge", "mu_eta", "tau_eta",
    "mu_n", "tau_n", "mu_phi", "tau_phi", "mu_mass", "tau_mass",
    "sist_mass", "acop", "sist_pt", "sist_rap", "met_pt", "met_phi",
    "jet_pt", "jet_eta", "jet_phi", "jet_mass", "jet_btag",
    "weight", "n_b_jet", "generator_weight",
    # SFs and their systematics
    "mu_trig_sf", "mu_idiso_sf", "mu_reco_sf",
    "mu_trig_syst", "mu_idiso_syst", "mu_reco_syst",
    # optional combined weight (no XS here)
    "event_weight"
]

# ---------- Main loop ----------
for file_idx, idx in enumerate(indices):
    path = lines[idx]  # relative path or PFN
    out_file = f"{output_prefix}{idx}.root"

    print(f"\n📦 Processing file {file_idx+1} of {total_files}:")
    print(f" → {path}")

    # Skip if already completed (unless overwrite)
    if not overwrite and (os.path.exists(out_file) or idx in resume.get("done", [])):
        print(f"⏭️  Skipping index {idx} (already done).")
        continue

    # Build input path: phase0 outputs are plain EOS paths
    input_path = prefix + path if not path.startswith("root://") and not path.startswith("/") else path

    print(f"📦 Input : {input_path}")
    print(f"📤 Output: {out_file}")

    try:
        # Create RDataFrame on phase0 tree
        df = ROOT.RDataFrame("tree", input_path)

        # Add deltaR from existing four-vector components
        df_added = (
            df
            .Define("muon", "TLorentzVector mu; mu.SetPtEtaPhiM(mu_pt, mu_eta, mu_phi, mu_mass); return mu;")
            .Define("tau",  "TLorentzVector t; t.SetPtEtaPhiM(tau_pt, tau_eta, tau_phi, tau_mass); return t;")
            .Define("delta_r", "return tau.DeltaR(muon);")
        )

        # Apply cuts identical to open_DY.cpp
        df_filtered = (
            df_added
            .Filter("mu_id >= 3", "Muon ID (tight enough)")
            .Filter("tau_id1 > 63", "Tau VSjet")
            .Filter("tau_id2 > 7",  "Tau VSe")
            .Filter("tau_id3 > 1",  "Tau VSmu")
            .Filter("delta_r > 0.4", "Delta R separation")
            .Filter("fabs(tau_eta) < 2.4 && fabs(mu_eta) < 2.4", "Acceptance")
            .Filter("mu_pt > 35. && tau_pt > 100.", "pT thresholds")
            .Filter("mu_charge * tau_charge < 0", "Opposite-sign")
            # DY muon SFs (nominal + syst) and simple event weight
            .Define("mu_trig_sf",  "MuonTrigSF(mu_pt, mu_eta, mu_charge, 0)")
            .Define("mu_idiso_sf", "MuonIDISOSF(mu_pt, mu_eta, 0)")
            .Define("mu_reco_sf",  "muRecoSF(mu_pt, mu_eta, 0)")
            .Define("mu_trig_syst",  "MuonTrigSF(mu_pt, mu_eta, mu_charge, 1)")
            .Define("mu_idiso_syst", "MuonIDISOSF(mu_pt, mu_eta, 1)")
            .Define("mu_reco_syst",  "muRecoSF(mu_pt, mu_eta, 1)")
            .Define("event_weight", "(double)generator_weight * mu_trig_sf * mu_idiso_sf * mu_reco_sf")
        )

        # Create output directory if needed
        out_dir = os.path.dirname(out_file)
        if out_dir and not out_dir.startswith("root://"):
            os.makedirs(out_dir, exist_ok=True)

        # Snapshot selected columns
        df_filtered.Snapshot("tree", out_file, columns)

        print(f"✅ Done. Output saved to:\n{out_file}")

        # Update resume state
        if idx not in resume["done"]:
            resume["done"].append(idx)
        save_resume(resume_path, resume)

    except Exception as e:
        print(f"❌ Failed to process {path}: {e}")
        continue

print("✅ DY phase1 cuts completed for requested files.")
