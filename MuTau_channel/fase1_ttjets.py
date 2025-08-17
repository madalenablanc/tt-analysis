import ROOT, os, numpy as np, math

ROOT.EnableImplicitMT(1)  # Must be before any RDataFrame is created :contentReference[oaicite:3]{index=3}

# === Load correction tables as numpy arrays
sf_neg = np.loadtxt("/eos/user/m/mblancco/tau_analysis/POGCorrections/muon_neg18_lowTrig.txt")
sf_pos = np.loadtxt("/eos/user/m/mblancco/tau_analysis/POGCorrections/muon_pos18_lowTrig.txt")
sf_high = np.loadtxt("/eos/user/m/mblancco/tau_analysis/POGCorrections/muon_18_highTrig.txt")
sf_idiso = np.loadtxt("/eos/user/m/mblancco/tau_analysis/POGCorrections/muon_IdISO.txt")

# === Python functions decorated to compile with numba
@ROOT.Numba.Declare(['float', 'float', 'float', 'float', 'int'], 'float')
def MuonTrigSF(pt, eta, charge, p_tot, flag):
    sf = 1.0
    abs_eta = abs(eta)
    if pt < 35 or abs_eta > 2.4:
        return 1.0
    tbl = sf_high if pt >= 200 else (sf_pos if charge > 0 else sf_neg)
    for e0,e1,p0,p1,corr,syst in tbl:
        if abs_eta > e0 and abs_eta <= e1 and pt > p0 and pt <= p1:
            sf = corr if flag == 0 else syst
    return sf

@ROOT.Numba.Declare(['float', 'float', 'float', 'float', 'int'], 'float')
def MuonIDISOSF(pt, eta, charge, p_tot, flag):
    sf = 1.0
    abs_eta = abs(eta)
    if pt < 35 or abs_eta > 2.4:
        return 1.0
    for e0,e1,p0,p1,corr,syst in sf_idiso:
        if abs_eta > e0 and abs_eta <= e1 and pt > p0 and pt <= p1:
            sf = corr if flag == 0 else syst
    return sf

@ROOT.Numba.Declare(['float','float','int'],'float')
def muRecoSF(pt, eta, flag):
    abs_eta = abs(eta)
    p_tot = pt / math.sin(2 * math.atan(math.exp(-eta)))
    if flag == 0:
        if pt < 200 and abs_eta > 2.1:
            return 0.999
        if pt > 200:
            # replicate your logic bins from original C++
            if 100 < p_tot < 150:
                return 0.9948 if abs_eta < 1.6 else 0.993
            if 150 < p_tot < 200:
                return 0.9950 if abs_eta < 1.6 else 0.990
            if 200 < p_tot < 300:
                return 0.994 if abs_eta < 1.6 else 0.988
            if 300 < p_tot < 400:
                return 0.9914 if abs_eta < 1.6 else 0.981
            if 400 < p_tot < 600:
                return 0.993 if abs_eta < 1.6 else 0.983
            if 600 < p_tot < 1500:
                return 0.991 if abs_eta < 1.6 else 0.978
            if 1500 < p_tot < 3500:
                return 1.0 if abs_eta < 1.6 else 0.98
    else:
        if pt < 200:
            if abs_eta < 0.9: return 0.000382
            elif abs_eta < 1.2: return 0.000522
            elif abs_eta < 2.1: return 0.000831
            else: return 0.001724
        else:
            if p_tot < 300: return 0.001
            elif p_tot < 400: return 0.0009 if abs_eta < 1.6 else 0.002
            elif p_tot < 600: return 0.002 if abs_eta < 1.6 else 0.003
            elif p_tot < 1500: return 0.004 if abs_eta < 1.6 else 0.006
            else: return 0.1 if abs_eta < 1.6 else 0.03
    return 1.0

# === Batch processing over all listed files
base_path = "/eos/user/m/mblancco/samples_2018_mutau/"
output_dir = os.path.join(base_path, "ficheiros_fase1")
weight_sample = 0.15

with open("/eos/user/m/mblancco/samples_2018_mutau/samples_ttjets_skimmed_new.txt") as f:
    files = [ln.strip() for ln in f if ln.strip()]

for idx, fname in enumerate(files, start=1):
    infile = os.path.join(base_path, fname)
    if not os.path.exists(infile):
        print("Skipping missing:", infile)
        continue
    print(f"Processing {fname} ({idx}/{len(files)})")

    rdf = ROOT.RDataFrame("tree", infile)
    rdf = rdf.Define("p_tot", "mu_pt / sin(2*atan(exp(-mu_eta)))") \
        .Define("mu_trig_sf", f"{weight_sample} * Numba::MuonTrigSF(mu_pt, mu_eta, mu_charge, p_tot, 0)") \
        .Define("mu_idiso_sf", "Numba::MuonIDISOSF(mu_pt, mu_eta, mu_charge, p_tot, 0)") \
        .Define("mu_reco_sf", "Numba::muRecoSF(mu_pt, mu_eta, 0)") \
        .Define("total_weight", "mu_trig_sf * mu_idiso_sf * mu_reco_sf") \
        .Define("deltaR", "sqrt(pow(mu_eta - tau_eta,2) + pow(mu_phi - tau_phi,2))") \
        .Define("deltaphi", "fabs(mu_phi - tau_phi) > M_PI ? fabs(mu_phi - tau_phi - 2*M_PI) : fabs(mu_phi - tau_phi)") \
        .Define("sist_acop", "fabs(deltaphi)/M_PI") 

    rdf_f = rdf.Filter("mu_id > 3") \
              .Filter("tau_id1 > 63 && tau_id2 > 7 && tau_id3 > 1") \
              .Filter("deltaR > 0.4") \
              .Filter("fabs(mu_eta) < 2.4 && fabs(tau_eta) < 2.4") \
              .Filter("mu_pt > 35 && tau_pt > 100") \
              .Filter("mu_charge * tau_charge < 0")

    outfile = os.path.join(output_dir, f"TESTS_ttjets_output_{idx}.root")
    rdf_f.Snapshot("tree_out", outfile)
    print("Saved:", outfile)
