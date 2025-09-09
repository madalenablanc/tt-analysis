import ROOT
import math
import os
import sys

# Open the ROOT file
#filename = "/eos/user/m/mblancco/samples_2018_tautau/fase0_2/ttJetscode_GammaGammaTauTau_SignalMC_SM_18UL_23k_NANOAODSIM_fase0_with_xi_.root"  # Replace with the correct path to your file
#filename ="/eos/user/m/mblancco/samples_2018_tautau/fase1/GammaGammaTauTau_SignalMC_SM_18UL_23k_NANOAODSIM_fase1_with_xi_qcdcode.root"

#filename="/eos/user/m/mblancco/samples_2018_tautau/fase0_2/ttJetscode_GammaGammaTauTau_SignalMC_SM_18UL_23k_NANOAODSIM_fase0_with_xi_and_deltaphi.root"
#filename="/eos/user/m/mblancco/samples_2018_tautau/fase0_2/ttJetscode_GammaGammaTauTau_SignalMC_SM_18UL_23k_NANOAODSIM_fase0_no_pileups.root"
#filename_bck="/eos/user/m/mblancco/samples_2018_tautau/fase0_background/QCD_2018_UL_skimmed_TauTau_nano.root"



# Function to save histograms without showing them
def save_histogram(hist, filename):
    canvas = ROOT.TCanvas("canvas", "", 800, 600)  # Create a canvas
    canvas.SetBatch(True)  # Enable batch mode to suppress display
    hist.Draw("HIST")  # Draw the histogram on the canvas
    canvas.SaveAs(filename)  # Save the canvas as a file
    canvas.Close()  # Close the canvas to free memory
    print(f"Saved {filename}")

# Helper function to check if a branch exists
def check_branch(event, branch_name):
    # if not hasattr(event, branch_name):
    #     raise RuntimeError(f"Error: Branch '{branch_name}' does not exist in the event.")
    return hasattr(event, branch_name)




def create_hist(input,vars,dest,pileup,sample_type):

    file = ROOT.TFile.Open(input)

    if not file or file.IsZombie():
        print(f"Error: Could not open file {input}")
        exit(1)

    tree_name = "tree"  # Replace with the actual TTree name
    tree = file.Get(tree_name)

    if not tree:
        print(f"Error: Could not find TTree '{tree_name}' in file {input}")
        file.Close()
        exit(1)


    if not os.path.exists(dest):
        os.makedirs(dest)


    title=""
    bins=0
    for v in vars:
        # v0=v[0].replace("'",'')
        # v1=v[1].replace("'",'')
        title=sample_type + v[1]
        bins=v[5]
        print("Title: "+title)
        # if pileup:
        #     title=sample_type+ " ;After proton pileup: "+v[1]
        #     bins=v[5]
        # else:
        #     title=sample_type+ " ;Before proton pileup: "+v[1] 
        #     bins=v[2]
        # print(v[0])
        # print(v[1])
        # print(v[2])

        hist=ROOT.TH1F(v[0],title,int(v[2]),float(v[3]),float(v[4]))
        #hist=ROOT.TH1F("tau0_pt", "Tau+ pT; pT [GeV]; Events", 80, 0, 600)

        for event in tree:
            if check_branch(event,v[0]):
                hist.Fill(getattr(event, v[0]))

            else:
                print(f"Branch '{v[0]}' does not exist. Skipping...")
                
        filenam=dest+sample_type+"-hist-"+v[0]+".png"
        save_histogram(hist,filenam)
    file.Close()

# vars=[["tau0_pt", "Tau+ pT; pT [GeV]; Events",'80', '0', '600'], ['"tau1_pt"', ' "Tau- pT; pT [GeV]; Events"', ' 80', ' 0', ' 600']]
vars=[]

f=open("vars_mutau.txt","r")
for line in f:
    vars.append(line.strip().split(","))


sample="Data"
filename_with_pileup="/eos/user/m/mblancco/samples_2018_mutau/"+sample+"_2018_UL_MuTau_nano_merged_pileup_protons.root"
filename_no_pileup="/eos/user/m/mblancco/samples_2018_mutau/"+sample+"_2018_UL_MuTau_nano_merged.root"

filename_2="/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/TauhTauh/Fundo_2018_UL_skimmed_TauTau_nano_fase1total-protons_2018.root"


# if len(sys.argv)!=4:
#     raise Exception("Number of arguments is incorrect")
# pileup=False
# filn=sys.argv[1]
# save_dir=sys.argv[2]
# pileup=sys.argv[3]

#create_hist(filename,vars,save_dir,pileup)

# create_hist(filename,vars,"/eos/user/m/mblancco/tau_analysis/plots_mutau/tt_jets;/",pileup=False)


create_hist(filename_with_pileup,vars,"/eos/user/m/mblancco/tau_analysis/plots/mutau/",pileup=True,sample_type=sample+"-with-pileup")
create_hist(filename_no_pileup,vars,"/eos/user/m/mblancco/tau_analysis/plots/mutau/",pileup=False,sample_type=sample+"-no-pileup")

# # Create histograms
# hist_tau0_pt = ROOT.TH1F("tau0_pt", "Tau+ pT; pT [GeV]; Events", 80, 0, 600)
# hist_tau1_pt = ROOT.TH1F("tau1_pt", "Tau- pT; pT [GeV]; Events", 80, 0, 600)
# hist_tau0_eta = ROOT.TH1F("tau0_eta", "Tau+ Eta; #eta; Events", 80, -3, 3)
# hist_tau1_eta = ROOT.TH1F("tau1_eta", "Tau- Eta; #eta; Events", 80, -3, 3)
# hist_sist_mass = ROOT.TH1F("sist_mass", "Invariant Mass of Tau Pair; Mass [GeV]; Events", 75, 0, 1000)
# hist_sist_rap = ROOT.TH1F("sist_rap", "Rapidity of Tau Pair; Rapidity; Events", 75, -2, 2)
# hist_delta_phi = ROOT.TH1F("delta_phi", "Delta Phi Between Tau+ and Tau-; #Delta#phi; Events", 200, 3.06, 3.16)
# hist_xi = ROOT.TH1F("xi", "Proton Energy Loss; Proton energy loss #xi; Events", 100, 0, 0.2)
# hist_tau0_phi = ROOT.TH1F("tau0_phi", "Tau0 Phi; #phi; Events",100, -4, 4)
# hist_tau1_phi = ROOT.TH1F("tau1_phi", "Tau1 Phi; #phi, No pileup; Events",100, -4, 4)
# hist_ttpair_inv_mass=ROOT.TH1F("invariant_mass_tt_pair", "Tau pair invariant mass; Tau pair invariant mass [GeV]; Events",60,0, 1000 )
# hist_invariant_mass=ROOT.TH1F("p_invariant_mass", "Proton pair invariant mass; Proton pair invariant mass [GeV]; Events",45, 300, 2000 )
# hist_p_rapidity=ROOT.TH1F("p_rapidity", "Proton pair rapidity; Proton pair rapidity; Events",75,-1.5,1.5 )
# hist_tt_rapidity=ROOT.TH1F("tt_rapidity", "Tau pair rapidity; Tau pair rapidity; Events",75,-3,3)
# hist_diff_mass=ROOT.TH1F("p-t mass","p-t mass ",100,-100,2000)
# hist_diff_rapidity=ROOT.TH1F("p-t rapidity","p-t rapidity",100,-10,10)



# # Loop over the tree and fill histograms
# for event in tree:
#     # Tau kinematics
#     check_branch(event, "tau0_pt")
#     hist_tau0_pt.Fill(event.tau0_pt)

#     check_branch(event, "tau1_pt")
#     hist_tau1_pt.Fill(event.tau1_pt)

#     check_branch(event, "tau0_eta")
#     hist_tau0_eta.Fill(event.tau0_eta)

#     check_branch(event, "tau1_eta")
#     hist_tau1_eta.Fill(event.tau1_eta)

#     # Tau pair invariant mass and rapidity
#     check_branch(event, "sist_mass")
#     hist_sist_mass.Fill(event.sist_mass)

#     check_branch(event, "sist_rap")
#     hist_sist_rap.Fill(event.sist_rap)

#     # Delta Phi
#     check_branch(event, "delta_phi")
#     abs_delta_phi = abs(event.delta_phi)
#     hist_delta_phi.Fill(abs_delta_phi)

#     # Proton energy loss
#     if hasattr(event, "proton_xi_multi_rp"):  # Optional variable
#         hist_xi.Fill(event.proton_xi_multi_rp)

#     # Tau phi
#     check_branch(event, "tau0_phi")
#     hist_tau0_phi.Fill(event.tau0_phi)

#     check_branch(event, "tau1_phi")
#     hist_tau1_phi.Fill(event.tau1_phi)

#     # Invariant mass and rapidity
#     check_branch(event, "invariant_mass_tt_pair")
#     hist_ttpair_inv_mass.Fill(event.invariant_mass_tt_pair)

#     check_branch(event, "p_invariant_mass")
#     print(event.p_invariant_mass)
#     hist_invariant_mass.Fill(event.p_invariant_mass)

#     check_branch(event, "p_rapidity")
#     hist_p_rapidity.Fill(event.p_rapidity)

#     # Difference in mass and rapidity
#     check_branch(event, "invariant_mass_tt_pair")
#     check_branch(event, "p_invariant_mass")
#     hist_diff_mass.Fill(event.p_invariant_mass - event.invariant_mass_tt_pair)

#     check_branch(event, "tt_rapidity")
#     hist_diff_rapidity.Fill(event.p_rapidity - event.tt_rapidity)




# dirr="/eos/user/m/mblancco/tau_analysis/plots/plots_fase0_no_pileup/"

# if not os.path.exists(dirr):
#     os.makedirs(dirr)

# Save histograms
# save_histogram(hist_tau0_pt, dirr+"tau0_pt.png")
# save_histogram(hist_tau1_pt, dirr+"tau1_pt.png")
# save_histogram(hist_tau0_eta, dirr+"tau0_eta.png")
# save_histogram(hist_tau1_eta, dirr+"tau1_eta.png")
# save_histogram(hist_sist_mass, dirr+"sist_mass.png")
# save_histogram(hist_sist_rap, dirr+"sist_rap.png")
# save_histogram(hist_delta_phi, dirr+"delta_phi_new.png")
# save_histogram(hist_xi, dirr+"proton_xi.png")
# save_histogram(hist_tau0_phi, dirr+"tau0_phi.png")
# save_histogram(hist_tau1_phi, dirr+"tau1_phi.png")
# save_histogram(hist_ttpair_inv_mass, dirr+"tt_inv_mass.png")
# save_histogram(hist_invariant_mass, dirr+"p_inv_mass.png")
# save_histogram(hist_tt_rapidity, dirr+"tt_rapidity.png")
# save_histogram(hist_p_rapidity, dirr+"p_rapidity.png")
# save_histogram(hist_diff_mass,dirr+"p-t mass.png")
# save_histogram(hist_diff_rapidity,dirr+"p-t rapidity.png")


# Close the ROOT file

print("Analysis complete. Histograms saved without displaying.")