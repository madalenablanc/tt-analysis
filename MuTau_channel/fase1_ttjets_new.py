import uproot
import numpy as np
import awkward as ak
from concurrent.futures import ProcessPoolExecutor
import math

# ----- Load SF files -----
muon_neg18_lowTrig = np.loadtxt("/eos/user/m/mblancco/tau_analysis/POGCorrections/muon_neg18_lowTrig.txt")
muon_pos18_lowTrig = np.loadtxt("/eos/user/m/mblancco/tau_analysis/POGCorrections/muon_pos18_lowTrig.txt")
muon_18_highTrig = np.loadtxt("/eos/user/m/mblancco/tau_analysis/POGCorrections/muon_18_highTrig.txt")
muon_IdISO = np.loadtxt("/eos/user/m/mblancco/tau_analysis/POGCorrections/muon_IdISO.txt")

# ----- Scale factor functions -----
def MuonTrigSF(pt, eta, charge, p_tot, flag=0):
    sf = 0
    table = None
    if pt < 200:
        table = muon_neg18_lowTrig if charge < 0 else muon_pos18_lowTrig
    else:
        table = muon_18_highTrig

    for row in table:
        eta_min, eta_max, pt_min, pt_max, corr, syst = row
        if abs(eta) > eta_min and abs(eta) <= eta_max and pt > pt_min and pt <= pt_max:
            sf = corr if flag == 0 else syst
    if abs(eta) > 2.4 or pt < 35.:
        sf = 1.
    return sf

def MuonIDISOSF(pt, eta, charge, p_tot, flag=0):
    sf = 0
    for row in muon_IdISO:
        eta_min, eta_max, pt_min, pt_max, corr, syst = row
        if abs(eta) > eta_min and abs(eta) <= eta_max and pt > pt_min and pt <= pt_max:
            sf = corr if flag == 0 else syst
    if abs(eta) > 2.4 or pt < 35.:
        sf = 1.
    return sf

def muRecoSF(pt, eta, flag=0):
    sf = 1
    if abs(eta) > 2.1 and pt < 200 and flag == 0:
        sf = 0.999
    p_tot = pt / np.sin(2 * np.arctan(np.exp(-eta)))
    if pt > 200 and flag == 0:
        if 100 < p_tot < 150: sf = 0.9948 if abs(eta) < 1.6 else 0.993
        if 150 < p_tot < 200: sf = 0.9950 if abs(eta) < 1.6 else 0.990
        if 200 < p_tot < 300: sf = 0.994 if abs(eta) < 1.6 else 0.988
        if 300 < p_tot < 400: sf = 0.9914 if abs(eta) < 1.6 else 0.981
        if 400 < p_tot < 600: sf = 0.993 if abs(eta) < 1.6 else 0.983
        if 600 < p_tot < 1500: sf = 0.991 if abs(eta) < 1.6 else 0.978
        if 1500 < p_tot < 3500: sf = 1.0 if abs(eta) < 1.6 else 0.98
    if flag == 1:
        if pt < 200:
            if abs(eta) < 0.9: sf = 0.000382
            elif 0.9 <= abs(eta) < 1.2: sf = 0.000522
            elif 1.2 <= abs(eta) < 2.1: sf = 0.000831
            else: sf = 0.001724
        else:
            if abs(eta) < 1.6:
                if p_tot < 300: sf = 0.001
                elif 300 <= p_tot < 400: sf = 0.0009
                elif 400 <= p_tot < 600: sf = 0.002
                elif 600 <= p_tot < 1500: sf = 0.004
                else: sf = 0.1
            else:
                if p_tot < 300: sf = 0.001
                elif 300 <= p_tot < 400: sf = 0.002
                elif 400 <= p_tot < 600: sf = 0.003
                elif 600 <= p_tot < 1500: sf = 0.006
                else: sf = 0.03
    return sf

# ----- Event processing -----
def process_event(event):
    mu_pt, mu_eta, mu_phi, mu_mass, mu_charge = event['mu_pt'], event['mu_eta'], event['mu_phi'], event['mu_mass'], event['mu_charge']
    tau_pt, tau_eta, tau_phi, tau_mass, tau_charge = event['tau_pt'], event['tau_eta'], event['tau_phi'], event['tau_mass'], event['tau_charge']

    # Selection
    dR = np.sqrt((mu_eta - tau_eta)**2 + (mu_phi - tau_phi)**2)
    if mu_pt < 0 or tau_pt < 0 or dR < 0.4 or abs(mu_eta) > 2.4 or abs(tau_eta) > 2.4:
        return None

    mu_trig_sf = MuonTrigSF(mu_pt, mu_eta, mu_charge, mu_pt / np.sin(2*np.arctan(np.exp(-mu_eta))))
    mu_idiso_sf = MuonIDISOSF(mu_pt, mu_eta, mu_charge, mu_pt / np.sin(2*np.arctan(np.exp(-mu_eta))))
    mu_reco_sf = muRecoSF(mu_pt, mu_eta)

    weight_sample = 0.15
    weight = weight_sample * mu_trig_sf * mu_idiso_sf * mu_reco_sf

    syst_mu_trig = MuonTrigSF(mu_pt, mu_eta, mu_charge, mu_pt / np.sin(2*np.arctan(np.exp(-mu_eta))), flag=1)
    syst_mu_idiso = MuonIDISOSF(mu_pt, mu_eta, mu_charge, mu_pt / np.sin(2*np.arctan(np.exp(-mu_eta))), flag=1)
    syst_mu_reco = muRecoSF(mu_pt, mu_eta, flag=1)

    return {
        'mu_pt': mu_pt, 'tau_pt': tau_pt,
        'mu_eta': mu_eta, 'tau_eta': tau_eta,
        'mu_phi': mu_phi, 'tau_phi': tau_phi,
        'mu_mass': mu_mass, 'tau_mass': tau_mass,
        'weight': weight,
        'syst_mu_trig': syst_mu_trig,
        'syst_mu_idiso': syst_mu_idiso,
        'syst_mu_reco': syst_mu_reco,
        'mu_charge': mu_charge, 'tau_charge': tau_charge
    }

# ----- Parallel processing of ROOT file -----
def process_file(filename):
    tree = uproot.open(filename)["tree"]
    events = tree.arrays(library="ak")
    
    results = []
    with ProcessPoolExecutor() as executor:
        futures = [executor.submit(process_event, row) for row in ak.zip(events)]
        for f in futures:
            res = f.result()
            if res is not None:
                results.append(res)
    
    return results

# ----- Main execution -----
if __name__ == "__main__":
    filename = "/eos/user/m/mblancco/samples_2018_mutau/ttjets_2018.root"
    processed_events = process_file(filename)
    print(f"Processed {len(processed_events)} events")
