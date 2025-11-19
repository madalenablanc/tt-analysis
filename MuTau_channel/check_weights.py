import ROOT

# Check the QCD merged file
f = ROOT.TFile.Open("/eos/home-m/mblancco/samples_2018_mutau/QCD_2018_UL_MuTau_nano_merged_proton_vars.root")
tree = f.Get("tree")

print("QCD FILE ANALYSIS:")
print(f"Total events: {tree.GetEntries()}")

# Check if there are ANY charge or proton variables
if tree.GetBranch("mu_charge") and tree.GetBranch("tau_charge"):
    ss = tree.GetEntries("mu_charge * tau_charge > 0")
    os = tree.GetEntries("mu_charge * tau_charge < 0")
    print(f"Same-sign: {ss}")
    print(f"Opposite-sign: {os}")
    print(f"NOTE: Should be ALL same-sign!")

# Check proton variables  
if tree.GetBranch("xi_arm1_1") and tree.GetBranch("xi_arm2_1"):
    has_xi = tree.GetEntries("xi_arm1_1 >= 0 && xi_arm2_1 >= 0")
    print(f"Has both xi >= 0: {has_xi}")

if tree.GetBranch("pps_has_both_arms"):
    has_pps = tree.GetEntries("pps_has_both_arms == 1")
    print(f"Has pps_has_both_arms: {has_pps}")
