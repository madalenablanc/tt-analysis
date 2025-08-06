import ROOT
import numpy as np
import math
import os

ROOT.EnableImplicitMT()  # Enable multithreading

# === CONFIGURATION ===
base_path = "/eos/user/m/mblancco/samples_2018_mutau/"
file_list_txt = "ttjets_reading.txt"
line_number = int(input("Enter line number to process: "))  # like your C++ cin >> numero_linha
output_dir = "/eos/user/m/mblancco/samples_2018_mutau/ficheiros_fase1/"
weight_sample = 0.15

# === GET ROOT FILE FROM TXT ===
with open(file_list_txt, 'r') as f:
    lines = f.readlines()
    if line_number < 1 or line_number > len(lines):
        raise ValueError(f"Line number {line_number} out of bounds (file has {len(lines)} lines)")
    input_filename = lines[line_number - 1].strip()  # strip newline
    input_file_path = os.path.join(base_path, input_filename)

# === LOAD SF TEXT FILES ===
def load_sf_file(path):
    return np.loadtxt(path)

sf_neg = load_sf_file("/eos/user/m/mblancco/tau_analysis/POGCorrections/muon_neg18_lowTrig.txt")
sf_pos = load_sf_file("/eos/user/m/mblancco/tau_analysis/POGCorrections/muon_pos18_lowTrig.txt")
sf_high = load_sf_file("/eos/user/m/mblancco/tau_analysis/POGCorrections/muon_18_highTrig.txt")
sf_idiso = load_sf_file("/eos/user/m/mblancco/tau_analysis/POGCorrections/emu_2018/muon_IdISO.txt")

def get_sf(pt, eta, sf_array, flag=0):
    abs_eta = abs(eta)
    for row in sf_array:
        eta_min, eta_max, pt_min, pt_max, corr, syst = row
        if eta_min < abs_eta <= eta_max and pt_min < pt <= pt_max:
            return corr if flag == 0 else syst
    return 1.0

def MuonTrigSF(pt, eta, charge, flag=0):
    if pt < 35 or abs(eta) > 2.4:
        return 1.0
    if pt < 200:
        return get_sf(pt, eta, sf_neg if charge < 0 else sf_pos, flag)
    else:
        return get_sf(pt, eta, sf_high, flag)

def MuonIDISOSF(pt, eta, charge, flag=0):
    if pt < 35 or abs(eta) > 2.4:
        return 1.0
    return get_sf(pt, eta, sf_idiso, flag)

def muRecoSF(pt, eta, flag=0):
    abs_eta = abs(eta)
    p_tot = pt / math.sin(2 * math.atan(math.exp(-eta)))
    sf = 1.0
    if flag == 0:
        if pt < 200 and abs_eta > 2.1:
            return 0.999
        if pt > 200:
            bins = [
                (100, 150, 0.9948, 0.993),
                (150, 200, 0.9950, 0.990),
                (200, 300, 0.994, 0.988),
                (300, 400, 0.9914, 0.981),
                (400, 600, 0.993, 0.983),
                (600, 1500, 0.991, 0.978),
                (1500, 3500, 1.0, 0.98)
            ]
            for p_min, p_max, val1, val2 in bins:
                if p_min < p_tot < p_max:
                    return val1 if abs_eta < 1.6 else val2
    elif flag == 1:
        if pt < 200:
            if abs_eta < 0.9:
                return 0.000382
            elif abs_eta < 1.2:
                return 0.000522
            elif abs_eta < 2.1:
                return 0.000831
            else:
                return 0.001724
        else:
            bins = [
                (0, 300, 0.001, 0.001),
                (300, 400, 0.0009, 0.002),
                (400, 600, 0.002, 0.003),
                (600, 1500, 0.004, 0.006),
                (1500, float("inf"), 0.1, 0.03)
            ]
            for p_min, p_max, val1, val2 in bins:
                if p_min < p_tot <= p_max:
                    return val1 if abs_eta < 1.6 else val2
    return sf

# === PREPARE DATAFRAME ===
rdf = ROOT.RDataFrame("tree", input_file_path)

rdf = rdf \
    .Define("p_tot", "mu_pt/sin(2*atan(exp(-mu_eta)))") \
    .Define("mu_trig_sf", f"({weight_sample}) * ({MuonTrigSF.__name__})(mu_pt, mu_eta, mu_charge, 0)") \
    .Define("mu_idiso_sf", f"({MuonIDISOSF.__name__})(mu_pt, mu_eta, mu_charge, 0)") \
    .Define("mu_reco_sf", f"({muRecoSF.__name__})(mu_pt, mu_eta, 0)") \
    .Define("total_weight", "mu_trig_sf * mu_idiso_sf * mu_reco_sf") \
    .Define("deltaR", "sqrt(pow(mu_eta - tau_eta, 2) + pow(muon_phi - tau_phi, 2))") \
    .Define("deltaphi", "fabs(muon_phi - tau_phi) > M_PI ? fabs(muon_phi - tau_phi - 2*M_PI) : fabs(muon_phi - tau_phi)") \
    .Define("sist_acop", "fabs(deltaphi)/M_PI") \
    .Define("sist_mass", "sqrt(pow(mu_pt + tau_pt, 2) - pow(mu_eta + tau_eta, 2))") \
    .Define("sist_rap", "0.5 * log((mu_pt + tau_pt + mu_eta + tau_eta)/(mu_pt + tau_pt - mu_eta - tau_eta))")  # Placeholder

# === FILTER EVENTS ===
rdf_filtered = rdf \
    .Filter("mu_id > 3") \
    .Filter("tau_id1 > 63 && tau_id2 > 7 && tau_id3 > 1") \
    .Filter("deltaR > 0.4") \
    .Filter("fabs(tau_eta) < 2.4 && fabs(mu_eta) < 2.4") \
    .Filter("mu_pt > 35 && tau_pt > 100") \
    .Filter("mu_charge * tau_charge < 0")

# === OUTPUT ===
output_filename = f"ttjets_2018_UL_skimmed_MuTau_nano_cortes_{line_number}.root"
output_path = os.path.join(output_dir, output_filename)

rdf_filtered.Snapshot("tree_out", output_path)

print(f"✅ Processed line {line_number}:")
print(f"   Input file: {input_file_path}")
print(f"   Output file: {output_path}")
