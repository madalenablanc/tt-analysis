import ROOT
import math

# Enable multithreading
ROOT.EnableImplicitMT()

# --- Parameters ---
input_list_path = "ttJets_2018_UL.txt"
prefix = "root://cms-xrd-global.cern.ch//"
output_prefix = "/eos/user/m/mblancco/samples_2018_mutau/ttjets_2018_UL_skimmed_MuTau_nano_"

print("Processing ttjets - phase0\n")
print("output directory:/eos/user/m/mblancco/samples_2018_mutau/")

# --- Get all input files ---
with open(input_list_path) as f:
    lines = [line.strip() for line in f.readlines() if line.strip()]

total_files = len(lines)

for line_number, input_file in enumerate(lines):
    print(f"\n📦 Processing file {line_number+1} of {total_files}:")
    print(f" → {input_file}")

    input_path = prefix + input_file
    output_path = output_prefix + str(line_number) + ".root"

    try:
        # --- Create RDataFrame ---
        df = ROOT.RDataFrame("Events", input_path)

        # --- Apply filters ---
        df_filtered = (
            df.Filter("HLT_IsoMu24 == 1", "Trigger passed")
              .Filter("Muon_pt.size() > 0 && Tau_pt.size() > 0", "At least one muon and one tau")
        )

        # --- Define derived variables ---
        df_defs = (
            df_filtered
            .Define("mu_pt", "Muon_pt[0]")
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

            .Define("generator_weight", "Generator_weight")
            .Define("weight", "1.0")
            .Define("tau_decay", "Tau_decayMode[0]")
            .Define("tau_genmatch", "Tau_genPartFlav[0]")

            .Define("muon", "TLorentzVector mu; mu.SetPtEtaPhiM(mu_pt, mu_eta, mu_phi, mu_mass); return mu;")
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
        )

        # --- Columns to output ---
        columns = [
            "mu_id", "tau_id1", "tau_id2", "tau_id3", "mu_pt", "tau_pt",
            "mu_charge", "tau_charge", "mu_eta", "tau_eta",
            "mu_n", "tau_n", "mu_phi", "tau_phi", "mu_mass", "tau_mass",
            "sist_mass", "acop", "sist_pt", "sist_rap", "met_pt", "met_phi",
            "jet_pt", "jet_eta", "jet_phi", "jet_mass", "jet_btag",
            "weight", "n_b_jet", "generator_weight", "tau_decay", "tau_genmatch"
        ]

        # --- Snapshot ---
        df_defs.Snapshot("tree", output_path, columns)

        print(f"✔ Done. Output saved to:\n{output_path}")

    except Exception as e:
        print(f"❌ Failed to process {input_file}: {e}")
