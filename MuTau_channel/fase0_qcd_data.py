#!/usr/bin/env python3
import os, time, json, signal, sys
import ROOT

# ---------- Environment / XRootD client ----------
os.environ.setdefault("X509_USER_PROXY", f"/tmp/x509up_u{os.getuid()}")
os.environ.setdefault("XrdSecGSISRVNAMES", "*")

# Enable multithreading
ROOT.EnableImplicitMT()

# ---------- Parameters ----------
line_number   = -1   # -1 => process all; >=0 => only that line index
input_list    = "Data_MuTau_phase0_2018.txt"  # one /store/... (or full PFN) per line
output_prefix = "/eos/user/m/mblancco/samples_2018_mutau/fase0_with_proton_vars/Data_2018_UL_skimmed_MuTau_nano_"
lumi_file     = "dadosluminosidade.txt"
resume_path   = ".mutau_phase0_resume.json"
overwrite     = True
prefix        = "root://cms-xrd-global.cern.ch//"

print("Processing QCD/Data - phase0 with lumi filtering\n")
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

# ---------- Load certified luminosity info ----------
def load_lumi_list(filename):
    certified = {}
    with open(filename) as f:
        lines = f.readlines()
    current_run = None
    for line in lines:
        parts = line.strip().split()
        if not parts:
            continue
        nums = list(map(int, parts))
        if nums[0] > 50000:
            current_run = nums[0]
            certified[current_run] = []
            pairs = nums[1:]
        else:
            pairs = nums
        for i in range(0, len(pairs), 2):
            if i + 1 < len(pairs):
                certified[current_run].append((pairs[i], pairs[i+1]))
    return certified

certified_lumis = load_lumi_list(lumi_file)

# Create C++ lumi filter function (static for thread safety)
cpp_lumi = [
    'bool lumi_filter(ULong64_t run, ULong64_t lumi) {',
    '  static std::map<ULong64_t, std::vector<std::pair<ULong64_t, ULong64_t>>> good = {'
]
cpp_pairs = []
for run, ranges in certified_lumis.items():
    pairs = ", ".join([f"{{{a}, {b}}}" for (a, b) in ranges])
    cpp_pairs.append(f"    {{{run}, {{{pairs}}}}}")

cpp_lumi.append(",\n".join(cpp_pairs))
cpp_lumi += [
    '  };',
    '  auto it = good.find(run);',
    '  if (it == good.end()) return false;',
    '  for (const auto &rg : it->second) {',
    '    if (lumi >= rg.first && lumi <= rg.second) return true;',
    '  }',
    '  return false;',
    '}'
]
ROOT.gInterpreter.Declare("\n".join(cpp_lumi))

# ---------- Read input list ----------
with open(input_list) as f:
    lines = [ln.strip() for ln in f if ln.strip()]

if line_number >= 0 and line_number >= len(lines):
    raise IndexError(f"line_number {line_number} >= number of files {len(lines)}")

indices = range(len(lines)) if line_number < 0 else [line_number]
total_files = len(indices)
processing_times = []

# ---------- Output columns (Data - no MC variables) ----------
columns = [
    "mu_id", "tau_id1", "tau_id2", "tau_id3", "mu_pt", "tau_pt",
    "mu_charge", "tau_charge", "mu_eta", "tau_eta",
    "mu_n", "tau_n", "mu_phi", "tau_phi", "mu_mass", "tau_mass",
    "sist_mass", "acop", "sist_pt", "sist_rap", "met_pt", "met_phi",
    "jet_pt", "jet_eta", "jet_phi", "jet_mass", "jet_btag",
    "proton_xi", "proton_arm", "proton_thx", "proton_thy", "n_pu",
    "weight", "n_b_jet"
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

    start_time = time.time()
    try:
        # Create RDataFrame
        df = ROOT.RDataFrame("Events", input_path)

        # Apply event selection with lumi filtering
        df_filtered = (
            df.Filter("lumi_filter(run, luminosityBlock)", "Certified lumi")
              .Filter("HLT_IsoMu24 == 1", "HLT single-muon 2018")
              .Filter("Muon_pt.size() > 0 && Tau_pt.size() > 0", "Muon and tau present")
            #   .Filter("Muon_mvaId[0] >= 3", "Muon ID (>= Medium)")
            #   .Filter("Tau_idDeepTau2017v2p1VSjet[0] >= 63", "Tau VSjet")
            #   .Filter("Tau_idDeepTau2017v2p1VSe[0]   >= 7",  "Tau VSe")
            #   .Filter("Tau_idDeepTau2017v2p1VSmu[0]  >= 1",  "Tau VSmu")
            #   .Filter("Muon_pt[0] > 35. && Tau_pt[0] > 100.", "pT thresholds")
            #   .Filter("Muon_charge[0] * Tau_charge[0] < 0", "Opposite sign")
        )

        # Define derived variables
        df_defs = (
            df_filtered
            .Define("mu_pt", "Muon_pt[0]")
            .Define("tau_pt", "Tau_pt[0]")
            .Define("mu_eta", "Muon_eta[0]")
            .Define("tau_eta", "Tau_eta[0]")
            .Define("mu_phi", "Muon_phi[0]")
            .Define("tau_phi", "Tau_phi[0]")
            .Define("mu_mass", "Muon_mass[0]")
            .Define("tau_mass", "Tau_mass[0]")
            .Define("mu_charge", "Muon_charge[0]")
            .Define("tau_charge", "Tau_charge[0]")
            .Define("mu_id", "Muon_mvaId[0]")
            .Define("tau_id1", "Tau_idDeepTau2017v2p1VSjet[0]")
            .Define("tau_id2", "Tau_idDeepTau2017v2p1VSe[0]")
            .Define("tau_id3", "Tau_idDeepTau2017v2p1VSmu[0]")
            .Define("mu_n", "int(Muon_pt.size())")
            .Define("tau_n", "int(Tau_pt.size())")

            .Define("muon", "TLorentzVector m; m.SetPtEtaPhiM(mu_pt, mu_eta, mu_phi, mu_mass); return m;")
            .Define("tau", "TLorentzVector t; t.SetPtEtaPhiM(tau_pt, tau_eta, tau_phi, tau_mass); return t;")
            .Define("sist", "return muon + tau;")
            .Define("sist_mass", "sist.M()")
            .Define("sist_pt", "sist.Pt()")
            .Define("sist_rap", "sist.Rapidity()")
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
                int count = 0;
                for (auto b : Jet_btagDeepB)
                    if (b > 0.4506) count++;
                return count;
            """)
            .Define("proton_available", "Proton_multiRP_xi.size() > 0")
            .Define("proton_xi", "proton_available ? Proton_multiRP_xi[0] : -999.")
            .Define("proton_arm", "proton_available ? Proton_multiRP_arm[0] : -1.")
            .Define("proton_thx", "proton_available ? Proton_multiRP_thetaX[0] : -999.")
            .Define("proton_thy", "proton_available ? Proton_multiRP_thetaY[0] : -999.")
            .Define("n_pu", "double(Proton_multiRP_xi.size())")

            # Data weights (no generator info)
            .Define("weight", "1.0")
        )

        # Create output directory if needed
        out_dir = os.path.dirname(out_file)
        if out_dir and not out_dir.startswith("root://"):
            os.makedirs(out_dir, exist_ok=True)

        # Snapshot
        df_defs.Snapshot("tree", out_file, columns)

        elapsed = time.time() - start_time
        processing_times.append(elapsed)
        print(f"✅ Done in {elapsed:.1f} s. Output saved to:\n{out_file}")

        # Update resume state
        if idx not in resume["done"]:
            resume["done"].append(idx)
        save_resume(resume_path, resume)

    except Exception as e:
        print(f"❌ Failed to process {path}: {e}")
        continue

if processing_times:
    avg_time = sum(processing_times) / len(processing_times)
    print(f"⏱️ Average processing time over {len(processing_times)} files: {avg_time:.1f} s")
else:
    print("⏱️ No files processed successfully; average time unavailable.")

print("✅ QCD/Data phase-0 skimming completed for requested files.")
