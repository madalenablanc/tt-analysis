import ROOT
import math

ROOT.EnableImplicitMT()

# === Parameters ===
line_number = 0  # <-- CHANGE this to index of line in QCD_2018_UL.txt
prefix = "root://cms-xrd-global.cern.ch//"
input_list = "QCD_2018_UL.txt"
output_prefix = "/eos/user/m/mblancco/samples_2018_mutau/QCD_2018_UL_skimmed_MuTau_nano_"
lumi_file = "dadosluminosidade.txt"

# === Load certified luminosity info from dadosluminosidade.txt ===
def load_lumi_list(filename):
    certified = {}
    with open(filename) as f:
        lines = f.readlines()

    current_run = None
    for line in lines:
        numbers = list(map(int, line.strip().split()))
        if not numbers:
            continue
        if numbers[0] > 50000:  # new run
            current_run = numbers[0]
            certified[current_run] = []
            pairs = numbers[1:]
        else:
            pairs = numbers

        for i in range(0, len(pairs), 2):
            if i + 1 < len(pairs):
                certified[current_run].append((pairs[i], pairs[i+1]))
    return certified



certified_lumis = load_lumi_list(lumi_file)

# === Generate C++ code for lumi filter ===
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

# === Read file list ===
with open(input_list) as f:
    lines = [line.strip() for line in f if line.strip()]

if line_number >= len(lines):
    raise IndexError("Line number exceeds number of files.")

input_file = prefix + lines[line_number]
output_file = f"{output_prefix}{line_number}.root"

print(f"📦 Input file: {input_file}")
print(f"📤 Output file: {output_file}")

# === Create RDataFrame ===
df = ROOT.RDataFrame("Events", input_file)

# === Apply event selection ===
df_filtered = (
    df.Filter("lumi_filter(run, luminosityBlock)", "Certified lumi")
      .Filter("HLT_IsoMu24 == 1", "HLT Muon trigger")
      .Filter("Muon_pt.size() > 0 && Tau_pt.size() > 0", "Muon and tau present")
      .Filter("Muon_mvaId[0] >= 3", "Muon ID")
      .Filter("Tau_idDeepTau2017v2p1VSjet[0] >= 63", "Tau VSjet ID")
      .Filter("Tau_idDeepTau2017v2p1VSe[0] >= 7", "Tau VSe ID")
      .Filter("Tau_idDeepTau2017v2p1VSmu[0] >= 1", "Tau VSmu ID")
)

# === Define physics variables ===
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

# === Columns to save ===
columns = [
    "muon_id", "tau_id1", "tau_id2", "tau_id3", "muon_pt", "tau_pt",
    "muon_charge", "tau_charge", "muon_eta", "tau_eta",
    "muon_n", "tau_n", "muon_phi", "tau_phi", "muon_mass", "tau_mass",
    "sist_mass", "acop", "sist_pt", "sist_rap", "met_pt", "met_phi",
    "jet_pt", "jet_eta", "jet_phi", "jet_mass", "jet_btag",
    "weight", "n_b_jet", "generator_weight"
]

# === Save to file ===
df_defs.Snapshot("tree", output_file, columns)

print("✅ QCD MuTau skimming done with certified lumi filtering.")
