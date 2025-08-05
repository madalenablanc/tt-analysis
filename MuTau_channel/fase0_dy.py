import ROOT
import math
import os

ROOT.EnableImplicitMT()

# === Parameters ===
input_list = "DY_2018_UL.txt"
prefix = "root://cms-xrd-global.cern.ch//"
output_prefix = "/eos/user/m/mblancco/samples_2018_mutau/DY_2018_UL_skimmed_MuTau_nano_"

# === Load list of files ===
with open(input_list) as f:
    lines = [l.strip() for l in f if l.strip()]

for i, line in enumerate(lines):
    input_file = prefix + line
    output_file = f"{output_prefix}{i}.root"

    print(f"📦 Processing file {i+1}/{len(lines)}:")
    print(f"    Input : {input_file}")
    print(f"    Output: {output_file}")

    if not os.path.exists(os.path.dirname(output_file)):
        print(f"❌ Output directory doesn't exist: {os.path.dirname(output_file)}. Skipping...")
        continue

    try:
        df = ROOT.RDataFrame("Events", input_file)

        df = df.Filter("HLT_IsoMu24 == 1", "Passes muon trigger") \
               .Filter("Muon_pt.size() > 0 && Tau_pt.size() > 0", "Has Muon and Tau")

        df = (
            df.Define("muon_pt", "Muon_pt[0]")
              .Define("muon_eta", "Muon_eta[0]")
              .Define("muon_phi", "Muon_phi[0]")
              .Define("muon_mass", "Muon_mass[0]")
              .Define("muon_charge", "Muon_charge[0]")
              .Define("muon_id", "Muon_mvaId[0]")
              .Define("muon_n", "int(Muon_pt.size())")

              .Define("tau_pt", "Tau_pt[0]")
              .Define("tau_eta", "Tau_eta[0]")
              .Define("tau_phi", "Tau_phi[0]")
              .Define("tau_mass", "Tau_mass[0]")
              .Define("tau_charge", "Tau_charge[0]")
              .Define("tau_id1", "Tau_idDeepTau2017v2p1VSjet[0]")
              .Define("tau_id2", "Tau_idDeepTau2017v2p1VSe[0]")
              .Define("tau_id3", "Tau_idDeepTau2017v2p1VSmu[0]")
              .Define("tau_n", "int(Tau_pt.size())")

              .Define("muon", "TLorentzVector mu; mu.SetPtEtaPhiM(muon_pt, muon_eta, muon_phi, muon_mass); return mu;")
              .Define("tau",  "TLorentzVector t; t.SetPtEtaPhiM(tau_pt, tau_eta, tau_phi, tau_mass); return t;")
              .Define("sist", "return muon + tau;")
              .Define("sist_mass", "sist.M()")
              .Define("sist_pt",   "sist.Pt()")
              .Define("sist_rap",  "sist.Rapidity()")
              .Define("acop", """
                  double dphi = fabs(muon_phi - tau_phi);
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

        columns = [
            "muon_id", "tau_id1", "tau_id2", "tau_id3", "muon_pt", "tau_pt",
            "muon_charge", "tau_charge", "muon_eta", "tau_eta",
            "muon_n", "tau_n", "muon_phi", "tau_phi", "muon_mass", "tau_mass",
            "sist_mass", "acop", "sist_pt", "sist_rap", "met_pt", "met_phi",
            "jet_pt", "jet_eta", "jet_phi", "jet_mass", "jet_btag",
            "weight", "n_b_jet", "generator_weight", "tau_decay", "tau_genmatch"
        ]

        df.Snapshot("tree", output_file, columns)
        print("✅ Done.")

    except Exception as e:
        print(f"❌ Failed on file {i}: {e}")
