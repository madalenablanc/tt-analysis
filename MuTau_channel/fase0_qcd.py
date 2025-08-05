import ROOT
import math
import os

ROOT.EnableImplicitMT()

# === Parameters ===
prefix = "root://cms-xrd-global.cern.ch//"
input_list = "QCD_2018_UL.txt"
output_prefix = "/eos/user/m/mblancco/samples_2018_mutau/QCD_2018_UL_skimmed_MuTau_nano_"
lumi_file = "dadosluminosidade.txt"

# === Load certified luminosity info ===
def load_lumi_list(filename):
    certified = {}
    with open(filename) as f:
        lines = f.readlines()
    current_run = None
    for line in lines:
        numbers = list(map(int, line.strip().split()))
        if not numbers:
            continue
        if numbers[0] > 50000:
            current_run = numbers[0]
            certified[current_run] = []
            pairs = numbers[1:]
        else:
            pairs = numbers
        for i in range(0, len(pairs), 2):
            if i + 1 < len(pairs):
                certified[current_run].append((pairs[i], pairs[i + 1]))
    return certified

certified_lumis = load_lumi_list(lumi_file)

# === Generate C++ code for lumi filtering ===
cpp_lumi_filter_code = """
bool lumi_filter(ULong64_t run, ULong64_t lumi) {
    std::map<ULong64_t, std::vector<std::pair<ULong64_t, ULong64_t>>> good;
"""
for run, ranges in certified_lumis.items():
    cpp_lumi_filter_code += f"    good[{run}] = {{"
    cpp_lumi_filter_code += ", ".join([f"{{{start}, {end}}}" for (start, end) in ranges])
    cpp_lumi_filter_code += "};\n"
cpp_lumi_filter_code += """
    auto it = good.find(run);
    if (it == good.end()) return false;
    for (const auto &range : it->second) {
        if (lumi >= range.first && lumi <= range.second)
            return true;
    }
    return false;
}
"""
ROOT.gInterpreter.Declare(cpp_lumi_filter_code)

# === Load input file list ===
with open(input_list) as f:
    lines = [line.strip() for line in f if line.strip()]

# === Process all files ===
for i, line in enumerate(lines):
    input_file = prefix + line
    output_file = f"{output_prefix}{i}.root"

    print(f"📦 Processing file {i + 1}/{len(lines)}")
    print(f"    Input : {input_file}")
    print(f"    Output: {output_file}")

    if not os.path.exists(os.path.dirname(output_file)):
        print(f"❌ Skipping: Output path does not exist: {output_file}")
        continue

    try:
        df = ROOT.RDataFrame("Events", input_file)
        df_filtered = (
            df.Filter("lumi_filter(run, luminosityBlock)", "Certified lumi")
              .Filter("HLT_IsoMu24 == 1", "HLT Muon trigger")
              .Filter("Muon_pt.size() > 0 && Tau_pt.size() > 0", "Muon and tau present")
              .Filter("Muon_mvaId[0] >= 3")
              .Filter("Tau_idDeepTau2017v2p1VSjet[0] >= 63")
              .Filter("Tau_idDeepTau2017v2p1VSe[0] >= 7")
              .Filter("Tau_idDeepTau2017v2p1VSmu[0] >= 1")
        )

        df_defs = (
            df_filtered
            .Define("muon_pt", "Muon_pt[0]")
            .Define("tau_pt", "Tau_pt[0]")
            .Define("muon_eta", "Muon_eta[0]")
            .Define("tau_eta", "Tau_eta[0]")
            .Define("muon_phi", "Muon_phi[0]")
            .Define("tau_phi", "Tau_phi[0]")
            .Define("muon_mass", "Muon_mass[0]")
            .Define("tau_mass", "Tau_mass[0]")
            .Define("muon_charge", "Muon_charge[0]")
            .Define("tau_charge", "Tau_charge[0]")
            .Define("muon_id", "Muon_mvaId[0]")
            .Define("tau_id1", "Tau_idDeepTau2017v2p1VSjet[0]")
            .Define("tau_id2", "Tau_idDeepTau2017v2p1VSe[0]")
            .Define("tau_id3", "Tau_idDeepTau2017v2p1VSmu[0]")
            .Define("muon_n", "int(Muon_pt.size())")
            .Define("tau_n", "int(Tau_pt.size())")

            .Define("muon", "TLorentzVector mu; mu.SetPtEtaPhiM(muon_pt, muon_eta, muon_phi, muon_mass); return mu;")
            .Define("tau", "TLorentzVector t; t.SetPtEtaPhiM(tau_pt, tau_eta, tau_phi, tau_mass); return t;")
            .Define("sistema", "return muon + tau;")
            .Define("sist_mass", "sistema.M()")
            .Define("sist_pt", "sistema.Pt()")
            .Define("sist_rap", "sistema.Rapidity()")
            .Define("acop", """
                double dphi = fabs(muon_phi - tau_phi);
                if (dphi > M_PI) dphi = 2*M_PI - dphi;
                return fabs(dphi)/M_PI;
            """)

            .Define("met_pt", "MET_pt")
            .Define("met_phi", "MET_phi")

            .Define("jet_pt", "Jet_pt.size() > 0 ? Jet_pt[0] : -1")
            .Define("jet_eta", "Jet_eta.size() > 0 ? Jet_eta[0] : -999")
            .Define("jet_phi", "Jet_phi.size() > 0 ? Jet_phi[0] : -999")
            .Define("jet_mass", "Jet_mass.size() > 0 ? Jet_mass[0] : -999")
            .Define("jet_btag", "Jet_btagDeepB.size() > 0 ? Jet_btagDeepB[0] : -999")
            .Define("n_b_jet", """
                int btag = 0;
                for (auto b : Jet_btagDeepB)
                    if (b > 0.4506) btag++;
                return btag;
            """)

            .Define("generator_weight", "1.0")
            .Define("weight", "1.0")
        )

        columns = [
            "muon_id", "tau_id1", "tau_id2", "tau_id3", "muon_pt", "tau_pt",
            "muon_charge", "tau_charge", "muon_eta", "tau_eta",
            "muon_n", "tau_n", "muon_phi", "tau_phi", "muon_mass", "tau_mass",
            "sist_mass", "acop", "sist_pt", "sist_rap", "met_pt", "met_phi",
            "jet_pt", "jet_eta", "jet_phi", "jet_mass", "jet_btag",
            "weight", "n_b_jet", "generator_weight"
        ]

        df_defs.Snapshot("tree", output_file, columns)
        print("✅ Done.")
    except Exception as e:
        print(f"❌ Failed to process {input_file}: {e}")
