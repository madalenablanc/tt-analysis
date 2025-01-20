import ROOT
import math
import os

# Open the ROOT file
#filename = "/eos/user/m/mblancco/samples_2018_tautau/fase0_2/ttJetscode_GammaGammaTauTau_SignalMC_SM_18UL_23k_NANOAODSIM_fase0_with_xi_.root"  # Replace with the correct path to your file
#filename ="/eos/user/m/mblancco/samples_2018_tautau/fase1/GammaGammaTauTau_SignalMC_SM_18UL_23k_NANOAODSIM_fase1_with_xi_qcdcode.root"

filename="/eos/user/m/mblancco/samples_2018_tautau/fase0_2/ttJetscode_GammaGammaTauTau_SignalMC_SM_18UL_23k_NANOAODSIM_fase0_with_xi_and_deltaphi.root"

file = ROOT.TFile.Open(filename)

if not file or file.IsZombie():
    print(f"Error: Could not open file {filename}")
    exit(1)

tree_name = "tree"  # Replace with the actual TTree name
tree = file.Get(tree_name)

if not tree:
    print(f"Error: Could not find TTree '{tree_name}' in file {filename}")
    file.Close()
    exit(1)
    


# Create histograms
hist_tau0_pt = ROOT.TH1F("tau0_pt", "Tau+ pT; pT [GeV]; Events", 100, 0, 900)
hist_tau1_pt = ROOT.TH1F("tau1_pt", "Tau- pT; pT [GeV]; Events", 100, 0, 900)
hist_tau0_eta = ROOT.TH1F("tau0_eta", "Tau+ Eta; #eta; Events", 100, -3, 3)
hist_tau1_eta = ROOT.TH1F("tau1_eta", "Tau- Eta; #eta; Events", 100, -3, 3)
hist_sist_mass = ROOT.TH1F("sist_mass", "Invariant Mass of Tau Pair; Mass [GeV]; Events", 100, 0, 1000)
hist_sist_rap = ROOT.TH1F("sist_rap", "Rapidity of Tau Pair; Rapidity; Events", 200, -3, 3)
hist_delta_phi = ROOT.TH1F("delta_phi", "Delta Phi Between Tau+ and Tau-; #Delta#phi; Events", 500, 3.02, 3.18)
hist_xi = ROOT.TH1F("xi", "Proton Energy Loss; #xi; Events", 500, 0, 0.3)
hist_tau0_phi = ROOT.TH1F("tau0_phi", "Tau0 Phi",100, -4, 4)
hist_tau1_phi = ROOT.TH1F("tau1_phi", "Tau1 Phi",100, -4, 4)
hist_ttpair_inv_mass=ROOT.TH1F("invariant_mass_tt_pair", "Tau pair invariant mass",100,0, 1000 )
hist_invariant_mass=ROOT.TH1F("invariant_mass", "Proton invariant mass",100, 0, 1000 )
hist_p_rapidity=ROOT.TH1F("p_rapidity", "Proton rapidity",100,1.2, 1.2 )
hist_tt_rapidity=ROOT.TH1F("tt_rapidity", "Tau pair rapidity",100, -1.2, 1.2 )

# Loop over the tree and fill histograms
for event in tree:
    # Tau kinematics
    hist_tau0_pt.Fill(event.tau0_pt)
    hist_tau1_pt.Fill(event.tau1_pt)
    hist_tau0_eta.Fill(event.tau0_eta)
    hist_tau1_eta.Fill(event.tau1_eta)
    
    # Tau pair invariant mass and rapidity
    hist_sist_mass.Fill(event.sist_mass)
    hist_sist_rap.Fill(event.sist_rap)
    
    # Delta Phi
    # delta_phi = math.atan2(math.sin(event.tau0_phi - event.tau1_phi), math.cos(event.tau0_phi - event.tau1_phi))
    # delta_phi_2=event.tau1_phi-event.tau0_phi
    # hist_delta_phi.Fill(delta_phi_2)

    abs_delta_phi=abs(event.delta_phi)
    hist_delta_phi.Fill(abs_delta_phi)
    
    # Proton energy loss
    if hasattr(event, "proton_xi_multi_rp"):
        hist_xi.Fill(event.proton_xi_multi_rp)

    hist_tau0_phi.Fill(event.tau0_phi)
    hist_tau1_phi.Fill(event.tau1_phi)

    hist_ttpair_inv_mass.Fill(event.invariant_mass_tt_pair)
    print(event.invariant_mass)
    print(event.invariant_mass)
    hist_invariant_mass.Fill(event.invariant_mass)
    hist_p_rapidity.Fill(event.p_rapidity)
    hist_tt_rapidity.Fill(event.tt_rapidity)


# Function to save histograms without showing them
def save_histogram(hist, filename):
    canvas = ROOT.TCanvas("canvas", "", 800, 600)  # Create a canvas
    canvas.SetBatch(True)  # Enable batch mode to suppress display
    hist.Draw("HIST")  # Draw the histogram on the canvas
    canvas.SaveAs(filename)  # Save the canvas as a file
    canvas.Close()  # Close the canvas to free memory
    print(f"Saved {filename}")

dirr="/eos/user/m/mblancco/tau_analysis/plots/plots_fase0/"

if not os.path.exists(dirr):
    os.makedirs(dirr)

# Save histograms
save_histogram(hist_tau0_pt, dirr+"tau0_pt.png")
save_histogram(hist_tau1_pt, dirr+"tau1_pt.png")
save_histogram(hist_tau0_eta, dirr+"tau0_eta.png")
save_histogram(hist_tau1_eta, dirr+"tau1_eta.png")
save_histogram(hist_sist_mass, dirr+"sist_mass.png")
save_histogram(hist_sist_rap, dirr+"sist_rap.png")
save_histogram(hist_delta_phi, dirr+"delta_phi_new.png")
save_histogram(hist_xi, dirr+"proton_xi.png")
save_histogram(hist_tau0_phi, dirr+"tau0_phi.png")
save_histogram(hist_tau1_phi, dirr+"tau1_phi.png")
save_histogram(hist_ttpair_inv_mass, dirr+"tt_inv_mass.png")
save_histogram(hist_invariant_mass, dirr+"p_inv_mass.png")
save_histogram(hist_tt_rapidity, dirr+"tt_rapidity.png")
save_histogram(hist_p_rapidity, dirr+"p_rapidity.png")


# Close the ROOT file
file.Close()
print("Analysis complete. Histograms saved without displaying.")
