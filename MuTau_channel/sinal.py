#!/usr/bin/env python3

import math
from array import array
from pathlib import Path

import ROOT


ROOT.ROOT.EnableImplicitMT()
ROOT.gROOT.SetBatch(True)


BASE_DIR = Path(__file__).resolve().parent
EMU_DIR = (BASE_DIR / "../POGCorrections").resolve()


def load_sf_table(path):
    table = []
    with open(path) as handle:
        for line in handle:
            stripped = line.strip()
            if not stripped or stripped.startswith("#"):
                continue
            parts = stripped.split()
            if len(parts) < 6:
                continue
            table.append(tuple(float(value) for value in parts[:6]))
    return table


MUON_NEG18_LOWTRIG = load_sf_table(EMU_DIR / "muon_neg18_lowTrig.txt")
MUON_POS18_LOWTRIG = load_sf_table(EMU_DIR / "muon_pos18_lowTrig.txt")
MUON_18_HIGHTRIG = load_sf_table(EMU_DIR / "muon_18_highTrig.txt")
MUON_IDISO = load_sf_table(EMU_DIR / "muon_IdISO.txt")


def muon_trig_sf(pt, eta, charge, p_tot, flag=0):
    print(f"val func.   pt= {pt} eta= {eta} carga= {charge} ptot= {p_tot}")
    sf = 0.0

    if pt < 200.0 and charge < 0.0:
        for eta_min, eta_max, pt_min, pt_max, corr, syst in MUON_NEG18_LOWTRIG:
            print(eta_min, eta_max, pt_min, pt_max, corr, syst)
            if abs(eta) > eta_min and abs(eta) <= eta_max and pt > pt_min and pt <= pt_max:
                sf = corr if flag == 0 else syst

    if pt < 200.0 and charge > 0.0:
        for eta_min, eta_max, pt_min, pt_max, corr, syst in MUON_POS18_LOWTRIG:
            if abs(eta) > eta_min and abs(eta) <= eta_max and pt > pt_min and pt <= pt_max:
                sf = corr if flag == 0 else syst

    if pt > 200.0:
        for eta_min, eta_max, pt_min, pt_max, corr, syst in MUON_18_HIGHTRIG:
            if abs(eta) > eta_min and abs(eta) <= eta_max and pt > pt_min and pt <= pt_max:
                sf = corr if flag == 0 else syst

    if abs(eta) > 2.4 or pt < 35.0:
        sf = 1.0
    return sf


def muon_idiso_sf(pt, eta, charge, p_tot, flag=0):
    sf = 0.0
    for eta_min, eta_max, pt_min, pt_max, corr, syst in MUON_IDISO:
        if abs(eta) > eta_min and abs(eta) <= eta_max and pt > pt_min and pt <= pt_max:
            sf = corr if flag == 0 else syst
    if abs(eta) > 2.4 or pt < 35.0:
        sf = 1.0
    return sf


def mu_reco_sf(pt, eta, flag=0):
    sf = 1.0
    if abs(eta) > 2.1 and pt < 200.0 and flag == 0:
        sf = 0.999
    if pt > 200.0 and flag == 0:
        p_tot = pt / math.sin(2.0 * math.atan(math.exp(-eta)))
        if 100.0 < p_tot < 150.0 and abs(eta) < 1.6:
            sf = 0.9948
        if 100.0 < p_tot < 150.0 and abs(eta) > 1.6:
            sf = 0.993
        if 150.0 < p_tot < 200.0 and abs(eta) < 1.6:
            sf = 0.9950
        if 150.0 < p_tot < 200.0 and abs(eta) > 1.6:
            sf = 0.99
        if 200.0 < p_tot < 300.0 and abs(eta) < 1.6:
            sf = 0.994
        if 200.0 < p_tot < 300.0 and abs(eta) > 1.6:
            sf = 0.988
        if 300.0 < p_tot < 400.0 and abs(eta) < 1.6:
            sf = 0.9914
        if 300.0 < p_tot < 400.0 and abs(eta) > 1.6:
            sf = 0.981
        if 400.0 < p_tot < 600.0 and abs(eta) < 1.6:
            sf = 0.993
        if 400.0 < p_tot < 600.0 and abs(eta) > 1.6:
            sf = 0.983
        if 600.0 < p_tot < 1500.0 and abs(eta) < 1.6:
            sf = 0.991
        if 600.0 < p_tot < 1500.0 and abs(eta) > 1.6:
            sf = 0.978
        if 1500.0 < p_tot < 3500.0 and abs(eta) < 1.6:
            sf = 1.0
        if 1500.0 < p_tot < 3500.0 and abs(eta) > 1.6:
            sf = 0.98

    if pt < 200.0 and flag == 1:
        if abs(eta) < 0.9:
            sf = 0.000382
        if 0.9 <= abs(eta) < 1.2:
            sf = 0.000522
        if 1.2 <= abs(eta) < 2.1:
            sf = 0.000831
        if abs(eta) >= 2.1:
            sf = 0.001724

    if pt > 200.0 and flag == 1:
        p_tot = pt / math.sin(2.0 * math.atan(math.exp(-eta)))

        if abs(eta) < 1.6 and p_tot < 300.0:
            sf = 0.001
        if abs(eta) < 1.6 and 300.0 <= p_tot < 400.0:
            sf = 0.0009
        if abs(eta) < 1.6 and 400.0 <= p_tot < 600.0:
            sf = 0.002
        if abs(eta) < 1.6 and 600.0 <= p_tot < 1500.0:
            sf = 0.004
        if abs(eta) < 1.6 and p_tot >= 1500.0:
            sf = 0.1

        if abs(eta) > 1.6 and p_tot < 300.0:
            sf = 0.001
        if abs(eta) > 1.6 and 300.0 <= p_tot < 400.0:
            sf = 0.002
        if abs(eta) > 1.6 and 400.0 <= p_tot < 600.0:
            sf = 0.003
        if abs(eta) > 1.6 and 600.0 <= p_tot < 1500.0:
            sf = 0.006
        if abs(eta) > 1.6 and p_tot >= 1500.0:
            sf = 0.03

    return sf


def get_hist(file_handle, name, cache):
    hist = cache.get(name)
    if hist is None:
        hist = file_handle.Get(name)
        if not hist:
            raise RuntimeError(f"Histogram '{name}' not found in {file_handle.GetName()}")
        cache[name] = hist
    return hist


def proton_first(values, arms, target_arm, occurrence):
    seen = 0
    for arm_value, xi_value in zip(arms, values):
        if int(arm_value) == target_arm:
            if seen == occurrence:
                return float(xi_value)
            seen += 1
    return -999.0


def proton_count(arms, target_arm):
    return sum(1 for value in arms if int(value) == target_arm)


def main():
    muon_neg18_lowTrig = open(EMU_DIR / "muon_neg18_lowTrig.txt")
    muon_pos18_lowTrig = open(EMU_DIR / "muon_pos18_lowTrig.txt")
    muon_18_highTrig = open(EMU_DIR / "muon_18_highTrig.txt")
    muon_IdISO = open(EMU_DIR / "muon_IdISO.txt")
    muon_neg18_lowTrig.close()
    muon_pos18_lowTrig.close()
    muon_18_highTrig.close()
    muon_IdISO.close()

    proton_sist_1 = ROOT.TFile.Open("/eos/home-m/mblancco/tau_analysis/TauTau_Channel/POGCorrections/reco_charactersitics_version1.root")
    xi_sist_1 = proton_sist_1.Get("2018_TS1_TS2/multi rp-0/xi/g_systematics_vs_xi")
    xi_sist_inter_1 = ROOT.TF1("xi_sist_inter_1", "pol20", 0, 10)
    xi_sist_inter_1.SetParLimits(0, -0.8, 0.1)
    xi_sist_inter_1.SetParLimits(1, 0.1, 1.0)
    xi_sist_1.Fit(xi_sist_inter_1.GetName())
    xi_sist_2 = proton_sist_1.Get("2018_TS1_TS2/multi rp-1/xi/g_systematics_vs_xi")
    xi_sist_inter_2 = ROOT.TF1("xi_sist_inter_2", "pol20", 0, 10)
    xi_sist_inter_2.SetParLimits(0, -1.0, 0.1)
    xi_sist_inter_2.SetParLimits(1, 0.1, 1.0)
    xi_sist_2.Fit(xi_sist_inter_2.GetName())

    ele = ROOT.TLorentzVector()
    tau = ROOT.TLorentzVector()
    nu = ROOT.TLorentzVector()

    fundo = ROOT.TFile.Open("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/SignalSamples/MuTau/GammaGammaTauTau_2018_UL_MuTau_SMandBSMweights_ntuplesfromminiJuly.root")
    ntp1 = fundo.Get("ntp1")

    output = ROOT.TFile("./MuTau_sinal_SM_2018_july.root", "RECREATE")
    out = ROOT.TTree("tree", "")

    scalar_specs = [
        ("mu_id", "mu_id/D"),
        ("tau_id1", "tau_id1/D"),
        ("tau_id2", "tau_id2/D"),
        ("tau_id3", "tau_id3/D"),
        ("mu_pt", "mu_pt/D"),
        ("tau_pt", "tau_pt/D"),
        ("mu_charge", "mu_charge/D"),
        ("tau_charge", "tau_charge/D"),
        ("mu_eta", "mu_eta/D"),
        ("tau_eta", "tau_eta/D"),
        ("mu_n", "mu_n/D"),
        ("tau_n", "tau_n/D"),
        ("mu_phi", "mu_phi/D"),
        ("tau_phi", "tau_phi/D"),
        ("mu_mass", "mu_mass/D"),
        ("tau_mass", "tau_mass/D"),
        ("sist_mass", "sist_mass/D"),
        ("acop", "acop/D"),
        ("sist_pt", "sist_pt/D"),
        ("sist_rap", "sist_rap/D"),
        ("met_pt", "met_pt/D"),
        ("met_phi", "met_phi/D"),
        ("jet_pt", "jet_pt/D"),
        ("jet_eta", "jet_eta/D"),
        ("jet_phi", "jet_phi/D"),
        ("jet_mass", "jet_mass/D"),
        ("jet_btag", "jet_btag/D"),
        ("n_b_jet", "n_b_jet/D"),
        ("n_pu", "n_pu/D"),
        ("xi_arm1_1", "xi_arm1_1/D"),
        ("xi_arm1_2", "xi_arm1_2/D"),
        ("xi_arm2_1", "xi_arm2_1/D"),
        ("xi_arm2_2", "xi_arm2_2/D"),
        ("n_protons_arm0", "n_protons_arm0/D"),
        ("n_protons_arm1", "n_protons_arm1/D"),
        ("pps_has_arm0", "pps_has_arm0/D"),
        ("pps_has_arm1", "pps_has_arm1/D"),
        ("pps_has_both_arms", "pps_has_both_arms/D"),
        ("weight", "weight/D"),
        ("M_e_met", "M_e_met/D"),
        ("syst_mu_trig", "syst_mu_trig/D"),
        ("syst_mu_idiso", "syst_mu_idiso/D"),
        ("syst_mu_reco", "syst_mu_reco/D"),
        ("xi_arm1_1_up", "xi_arm1_1_up/D"),
        ("xi_arm1_1_dw", "xi_arm1_1_dw/D"),
        ("xi_arm2_1_up", "xi_arm2_1_up/D"),
        ("xi_arm2_1_dw", "xi_arm2_1_dw/D"),
        ("tau_id_full", "tau_id_full/D"),
        ("tau_id", "tau_id/D"),
        ("tau_id_antimu", "tau_id_antimu/D"),
        ("tau_id_antie", "tau_id_antie/D"),
        ("tau_id_antij", "tau_id_antij/D"),
        ("tau_decay", "tau_decay/D"),
        ("tau_energy", "tau_energy/D"),
        ("mu_energy", "mu_energy/D"),
        ("thy1", "thy1/D"),
        ("thy2", "thy2/D"),
        ("thx1", "thx1/D"),
        ("thx2", "thx2/D"),
        ("y1", "y1/D"),
        ("y2", "y2/D"),
        ("x1", "x1/D"),
        ("x2", "x2/D"),
        ("t1", "t1/D"),
        ("t2", "t2/D"),
        ("arm1", "arm1/D"),
        ("arm2", "arm2/D"),
        ("time1", "time1/D"),
        ("time2", "time2/D"),
        ("trackx1_1", "trackx1_1/D"),
        ("trackx1_2", "trackx1_2/D"),
        ("trackx2_1", "trackx2_1/D"),
        ("trackx2_2", "trackx2_2/D"),
        ("tracky1_1", "tracky1_1/D"),
        ("tracky1_2", "tracky1_2/D"),
        ("tracky2_1", "tracky2_1/D"),
        ("tracky2_2", "tracky2_2/D"),
        ("trackrpid1_1", "trackrpid1_1/D"),
        ("trackrpid1_2", "trackrpid1_2/D"),
        ("trackrpid2_1", "trackrpid2_1/D"),
        ("trackrpid2_2", "trackrpid2_2/D"),
        ("trackthx1_1", "trackthx1_1/D"),
        ("trackthx1_2", "trackthx1_2/D"),
        ("trackthx2_1", "trackthx2_1/D"),
        ("trackthx2_2", "trackthx2_2/D"),
        ("trackthy1_1", "trackthy1_1/D"),
        ("trackthy1_2", "trackthy1_2/D"),
        ("trackthy2_1", "trackthy2_1/D"),
        ("trackthy2_2", "trackthy2_2/D"),
    ]

    scalars = {name: array("d", [0.0]) for name, _ in scalar_specs}
    for name, leaf in scalar_specs:
        out.Branch(name, scalars[name], leaf)

    weights_bsm_sf = ROOT.std.vector("float")()
    out.Branch("weights_bsm_sf", weights_bsm_sf)

    vector_branches = {
        "proton_xi": ROOT.std.vector("float")(),
        "proton_arm": ROOT.std.vector("float")(),
        "proton_thx": ROOT.std.vector("float")(),
        "proton_thy": ROOT.std.vector("float")(),
    }
    for name, branch in vector_branches.items():
        out.Branch(name, branch)

    histo_e_pt = ROOT.TH1D("histo_e_pt", "histo_e_pt", 50, 0, 500)
    histo_tau_pt = ROOT.TH1D("histo_tau_pt", "histo_tau_pt", 50, 0, 500)
    histo_n_tau = ROOT.TH1D("histo_n_tau", "histo_n_tau", 50, 0, 10)
    histo_n_eletron = ROOT.TH1D("histo_n_eletron", "histo_n_eletron", 50, 0, 10)
    histo_acop = ROOT.TH1D("histo_acop", "histo_acop", 50, 0, 1)
    histo_Pt = ROOT.TH1D("histo_Pt", "histo_Pt", 50, 0, 1000)
    histo_Mt = ROOT.TH1D("histo_Mt", "histo_Mt", 50, 0, 1000)
    histo_DR = ROOT.TH1D("histo_DR", "histo_DR", 50, 0, 5)

    n_id = 0.0
    n_charge = 0.0
    n_pt = 0.0
    n_eta = 0.0
    n_rec = 0.0
    n_p_up_xi = 0.0
    n_p_dw_xi = 0.0
    n_p_rad = 0.0

    PI = math.pi

    rad_multi = ROOT.TFile.Open("pixelEfficiencies_multiRP_reMiniAOD.root")
    rad_damage = ROOT.TFile.Open("pixelEfficiencies_radiation_reMiniAOD.root")
    hist_cache_multi = {}
    hist_cache_rad = {}

    entries = ntp1.GetEntries()
    weight_sample = 0.0
    for i in range(entries):
        ntp1.GetEntry(i)

        e_pt = ntp1.mu_pt
        tau_pt = ntp1.tau_pt
        e_charge = ntp1.mu_charge
        tau_charge = ntp1.tau_charge
        e_id = ntp1.mu_id
        tau_id_full = ntp1.tau_id_full
        tau_eta = ntp1.tau_eta
        e_eta = ntp1.mu_eta
        tau_phi = ntp1.tau_phi
        e_phi = ntp1.mu_phi
        tau_energy = ntp1.tau_energy
        e_energy = ntp1.mu_energy
        met_pt = ntp1.met_pt
        met_phi = ntp1.met_phi
        xi = ntp1.proton_xi
        thy = ntp1.proton_thy
        thx = ntp1.proton_thx
        y_vals = ntp1.proton_y
        x_vals = ntp1.proton_x
        t_vals = ntp1.proton_t
        arm = ntp1.proton_arm
        Tempo = ntp1.proton_time
        trackx1 = ntp1.proton_trackx1
        trackx2 = ntp1.proton_trackx2
        tracky1 = ntp1.proton_tracky1
        tracky2 = ntp1.proton_tracky2
        trackrpid1 = ntp1.proton_trackrpid1
        trackrpid2 = ntp1.proton_trackrpid2
        trackthx1 = ntp1.proton_trackthx1
        trackthx2 = ntp1.proton_trackthx2
        trackthy1 = ntp1.proton_trackthy1
        trackthy2 = ntp1.proton_trackthy2
        tau_id = ntp1.tau_id
        tau_id_antimu = ntp1.tau_id_antimu
        tau_id_antie = ntp1.tau_id_antie
        tau_id_antij = ntp1.tau_id_antij
        tau_decay = ntp1.tau_decay
        jet_pt = ntp1.jet_pt
        jet_eta = ntp1.jet_eta
        jet_phi = ntp1.jet_phi
        jet_mass = ntp1.jet_mass
        jet_btag = ntp1.jet_btag
        gen_proton_xi = ntp1.gen_proton_xi
        weight_sm = ntp1.weight_sm
        bsm_weights = ntp1.bsm_weights

        if len(e_charge) == 0 or len(tau_charge) == 0:
            continue

        weight_sample = 54900.0 * weight_sm[0] / (4000.0 * 1000.0)

        phi_e = e_phi[0]
        phi_tau = tau_phi[0]

        if e_phi[0] < 0.0:
            phi_e = e_phi[0] + 2 * PI
        if tau_phi[0] < 0.0:
            phi_tau = tau_phi[0] + 2 * PI

        delta_phi = abs(phi_e - phi_tau)
        acop = delta_phi / PI

        eta_e = e_eta[0]
        eta_tau = tau_eta[0]
        delta_eta = abs(eta_e - eta_tau)
        angular_dist = math.sqrt(delta_phi * delta_phi + delta_eta * delta_eta)
        histo_DR.Fill(angular_dist)

        if tau_id_full[0] > 0.5 and e_id[0] > 0.5 and abs(tau_eta[0]) < 2.4 and abs(e_eta[0]) < 2.4 and angular_dist > 0.4:
            mu_trig = muon_trig_sf(e_pt[0], e_eta[0], e_charge[0], e_pt[0] / math.sin(2.0 * math.atan(math.exp(-e_eta[0]))))
            mu_idiso = muon_idiso_sf(e_pt[0], e_eta[0], e_charge[0], e_pt[0] / math.sin(2.0 * math.atan(math.exp(-e_eta[0]))))
            mu_reco = mu_reco_sf(e_pt[0], e_eta[0])

            weight_factor = weight_sample * mu_trig * mu_idiso * mu_reco
            n_id += weight_factor

            if tau_pt[0] > 100.0 and e_pt[0] > 35.0:
                n_pt += weight_factor

                if float(e_charge[0]) * float(tau_charge[0]) < 0.0:
                    n_charge += weight_factor

                    if abs(tau_eta[0]) < 2.4 and abs(e_eta[0]) < 2.4:
                        n_eta += weight_factor

                        mu_trig_syst = muon_trig_sf(e_pt[0], e_eta[0], e_charge[0], e_pt[0] / math.sin(2.0 * math.atan(math.exp(-e_eta[0]))), 1)
                        mu_idiso_syst = muon_idiso_sf(e_pt[0], e_eta[0], e_charge[0], e_pt[0] / math.sin(2.0 * math.atan(math.exp(-e_eta[0]))), 1)
                        mu_reco_syst = mu_reco_sf(e_pt[0], e_eta[0], 1)

                        scalars["syst_mu_trig"][0] = mu_trig_syst
                        scalars["syst_mu_idiso"][0] = mu_idiso_syst
                        scalars["syst_mu_reco"][0] = mu_reco_syst

                        if len(xi) != 2:
                            continue

                        n_rec += weight_factor

                        histo_e_pt.Fill(e_pt[0])
                        histo_tau_pt.Fill(tau_pt[0])
                        histo_n_tau.Fill(len(tau_pt))
                        histo_n_eletron.Fill(len(e_pt))

                        if acop <= 1.0:
                            histo_acop.Fill(acop)
                        else:
                            acop = 2 - acop
                            delta_phi = 2 * PI - delta_phi
                            histo_acop.Fill(acop)

                        ele.SetPtEtaPhiE(e_pt[0], e_eta[0], e_phi[0], e_energy[0])
                        tau.SetPtEtaPhiE(tau_pt[0], tau_eta[0], tau_phi[0], tau_energy[0])
                        nu.SetPtEtaPhiE(met_pt[0], 0.0, met_phi[0], met_pt[0])
                        Pt = (ele + tau).Pt()
                        print(Pt)
                        Mt = (ele + tau).M()
                        m_e_met = (ele + nu).Mt()
                        histo_Pt.Fill(Pt)
                        histo_Mt.Fill(Mt)

                        print(f"tamanho bsm_weights: {len(bsm_weights)}")
                        sm_cs = bsm_weights[51]
                        for p in range(102):
                            bsm_weights[p] = bsm_weights[p] / sm_cs

                        weights_bsm_sf.clear()
                        for value in bsm_weights:
                            weights_bsm_sf.push_back(value)

                        xangle = 0.0
                        fraction = float(i) / float(entries)

                        if fraction <= 0.134:
                            xangle = 130.0
                        if 0.134 <= fraction <= 0.149:
                            xangle = 131.0
                        if 0.149 <= fraction <= 0.166:
                            xangle = 132.0
                        if 0.166 <= fraction <= 0.185:
                            xangle = 133.0
                        if 0.185 < fraction <= 0.205:
                            xangle = 134.0
                        if 20.5 <= fraction <= 0.227:
                            xangle = 135.0
                        if 0.227 <= fraction <= 0.251:
                            xangle = 136.0
                        if 0.251 <= fraction <= 0.277:
                            xangle = 137.0
                        if 0.277 < fraction <= 0.303:
                            xangle = 138.0
                        if 0.303 <= fraction <= 0.331:
                            xangle = 139.0
                        if 0.331 <= fraction <= 0.362:
                            xangle = 140.0
                        if 0.362 <= fraction <= 0.39:
                            xangle = 141.0
                        if 0.39 <= fraction <= 0.42:
                            xangle = 142.0
                        if 0.42 <= fraction <= 0.45:
                            xangle = 143.0
                        if 0.45 <= fraction <= 0.488:
                            xangle = 144.0
                        if 0.488 <= fraction <= 0.521:
                            xangle = 145.0
                        if 0.521 <= fraction < 0.556:
                            xangle = 146.0
                        if 0.556 <= fraction <= 0.595:
                            xangle = 147.0
                        if 0.595 <= fraction <= 0.633:
                            xangle = 148.0
                        if 0.633 <= fraction <= 0.673:
                            xangle = 149.0
                        if 0.673 <= fraction <= 0.713:
                            xangle = 150.0
                        if 0.713 <= fraction <= 0.748:
                            xangle = 151.0
                        if 0.748 <= fraction <= 0.783:
                            xangle = 152.0
                        if 0.783 <= fraction <= 0.816:
                            xangle = 153.0
                        if 0.816 <= fraction <= 0.849:
                            xangle = 154.0
                        if 0.849 <= fraction <= 0.879:
                            xangle = 155.0
                        if 0.879 <= fraction <= 0.901:
                            xangle = 156.0
                        if 0.901 <= fraction <= 0.916:
                            xangle = 157.0
                        if 0.916 <= fraction <= 0.924:
                            xangle = 158.0
                        if 0.924 <= fraction <= 0.925:
                            xangle = 159.0
                        if 0.925 <= fraction <= 1.00:
                            xangle = 160.0

                        limit_fun_arm_0 = ROOT.TF1("limit_fun_arm_0", "-(8.44219E-07*[xangle]-0.000100957)+(([xi]<(0.000247185*[xangle]+0.101599))*-(1.40289E-05*[xangle]-0.00727237)+([xi]>=(0.000247185*[xangle]+0.101599))*-(0.000107811*[xangle]-0.0261867))*([xi]-(0.000247185*[xangle]+0.101599))", 0, 1)
                        limit_fun_arm_1 = ROOT.TF1("limit_fun_arm_1", "-(-4.74758E-07*[xangle]+3.0881E-05)+(([xi]<(0.000727859*[xangle]+0.0722653))*-(2.43968E-05*[xangle]-0.0085461)+([xi]>=(0.000727859*[xangle]+0.0722653))*-(7.19216E-05*[xangle]-0.0148267))*([xi]-(0.000727859*[xangle]+0.0722653))", 0, 1)

                        limit_fun_arm_0.SetParameter("xangle", xangle)
                        limit_fun_arm_1.SetParameter("xangle", xangle)
                        limit_fun_arm_0.SetParameter("xi", xi[0])
                        limit_fun_arm_1.SetParameter("xi", xi[1])

                        print(f"upper limit arm 0: {-limit_fun_arm_0.Eval(0.5)} upper limit arm 1: {-limit_fun_arm_1.Eval(0.5)}")

                        if thx[0] > -limit_fun_arm_0.Eval(0.5) or thx[1] > -limit_fun_arm_1.Eval(0.5):
                            continue

                        print("#######################ehi")
                        n_p_up_xi += weight_factor
                        print(f"x1: {trackx1[0]} x2: {trackx2[0]} y1: {tracky1[0]} y2: {tracky2[0]} xi_1: {xi[0]} xi_2: {xi[1]}")

                        if fraction <= 0.21:
                            if trackx1[0] < 2.71 or trackx1[0] > 17.927:
                                continue
                            if tracky1[0] < -11.589 or tracky1[0] > 3.698:
                                continue
                            if trackx2[0] < 2.278 or trackx2[0] > 24.62:
                                continue
                            if tracky2[0] < -10.898 or tracky2[0] > 4.398:
                                continue
                            if trackx1[1] < 3.0 or trackx1[1] > 18.498:
                                continue
                            if tracky1[1] < -11.298 or tracky1[1] > 4.098:
                                continue
                            if trackx2[1] < 2.42 or trackx2[1] > 25.045:
                                continue
                            if tracky2[1] < -10.398 or tracky2[1] > 5.098:
                                continue

                        if 0.21 < fraction <= 0.29:
                            if trackx1[0] < 2.85 or trackx1[0] > 17.927:
                                continue
                            if tracky1[0] < -11.589 or tracky1[0] > 3.698:
                                continue
                            if trackx2[0] < 2.42 or trackx2[0] > 24.62:
                                continue
                            if tracky2[0] < -10.798 or tracky2[0] > 4.298:
                                continue
                            if trackx1[1] < 3.0 or trackx1[1] > 18.07:
                                continue
                            if tracky1[1] < -11.198 or tracky1[1] > 4.098:
                                continue
                            if trackx2[1] < 2.42 or trackx2[1] > 25.045:
                                continue
                            if tracky2[1] < -10.398 or tracky2[1] > 5.098:
                                continue

                        if 0.29 < fraction <= 0.37:
                            if trackx1[0] < 2.562 or trackx1[0] > 17.64:
                                continue
                            if tracky1[0] < -11.098 or tracky1[0] > 4.198:
                                continue
                            if trackx2[0] < 2.135 or trackx2[0] > 24.62:
                                continue
                            if tracky2[0] < -11.398 or tracky2[0] > 3.798:
                                continue
                            if trackx1[1] < 3.0 or trackx1[1] > 17.931:
                                continue
                            if tracky1[1] < -10.498 or tracky1[1] > 4.698:
                                continue
                            if trackx2[1] < 2.279 or trackx2[1] > 24.76:
                                continue
                            if tracky2[1] < -10.598 or tracky2[1] > 4.498:
                                continue

                        if 0.37 < fraction <= 0.50:
                            if trackx1[0] < 2.564 or trackx1[0] > 17.93:
                                continue
                            if tracky1[0] < -11.098 or tracky1[0] > 4.198:
                                continue
                            if trackx2[0] < 2.278 or trackx2[0] > 24.62:
                                continue
                            if tracky2[0] < -11.398 or tracky2[0] > 3.698:
                                continue
                            if trackx1[1] < 3.0 or trackx1[1] > 17.931:
                                continue
                            if tracky1[1] < -10.498 or tracky1[1] > 4.698:
                                continue
                            if trackx2[1] < 2.279 or trackx2[1] > 24.76:
                                continue
                            if tracky2[1] < -10.598 or tracky2[1] > 4.398:
                                continue

                        if 0.50 < fraction <= 0.77:
                            if trackx1[0] < 2.847 or trackx1[0] > 17.93:
                                continue
                            if tracky1[0] < -11.098 or tracky1[0] > 4.098:
                                continue
                            if trackx2[0] < 2.278 or trackx2[0] > 24.62:
                                continue
                            if tracky2[0] < -11.398 or tracky2[0] > 3.698:
                                continue
                            if trackx1[1] < 3.0 or trackx1[1] > 17.931:
                                continue
                            if tracky1[1] < -10.498 or tracky1[1] > 4.698:
                                continue
                            if trackx2[1] < 2.279 or trackx2[1] > 24.76:
                                continue
                            if tracky2[1] < -10.598 or tracky2[1] > 4.398:
                                continue

                        if fraction > 0.77:
                            if trackx1[0] < 2.847 or trackx1[0] > 17.931:
                                continue
                            if tracky1[0] < -11.598 or tracky1[0] > 4.498:
                                continue
                            if trackx2[0] < 2.278 or trackx2[0] > 24.62:
                                continue
                            if tracky2[0] < -11.598 or tracky2[0] > 3.398:
                                continue
                            if trackx1[1] < 3.0 or trackx1[1] > 17.931:
                                continue
                            if tracky1[1] < -9.998 or tracky1[1] > 4.698:
                                continue
                            if trackx2[1] < 2.279 or trackx2[1] > 24.76:
                                continue
                            if tracky2[1] < -10.598 or tracky2[1] > 3.898:
                                continue

                        n_p_dw_xi += weight_factor

                        weight = 1.0

                        if fraction <= 0.21:
                            raddamage56_match = get_hist(rad_multi, "Pixel/2018/2018A/h56_220_2018A_all_2D", hist_cache_multi)
                            raddamage56 = get_hist(rad_damage, "Pixel/2018/2018A/h56_210_2018A_all_2D", hist_cache_rad)
                            weight *= raddamage56.GetBinContent(raddamage56.FindBin(trackx1[1], tracky1[1]))
                            weight *= raddamage56_match.GetBinContent(raddamage56_match.FindBin(trackx1[1], tracky1[1]))
                            raddamage45_match = get_hist(rad_multi, "Pixel/2018/2018A/h45_220_2018A_all_2D", hist_cache_multi)
                            raddamage45 = get_hist(rad_damage, "Pixel/2018/2018A/h45_210_2018A_all_2D", hist_cache_rad)
                            weight *= raddamage45.GetBinContent(raddamage45.FindBin(trackx1[0], tracky1[0]))
                            weight *= raddamage45_match.GetBinContent(raddamage45_match.FindBin(trackx1[0], tracky1[0]))

                        if 0.21 < fraction < 0.29:
                            raddamage56_match = get_hist(rad_multi, "Pixel/2018/2018B1/h56_220_2018B1_all_2D", hist_cache_multi)
                            raddamage56 = get_hist(rad_damage, "Pixel/2018/2018B1/h56_210_2018B1_all_2D", hist_cache_rad)
                            weight *= raddamage56.GetBinContent(raddamage56.FindBin(trackx1[1], tracky1[1]))
                            weight *= raddamage56_match.GetBinContent(raddamage56_match.FindBin(trackx1[1], tracky1[1]))
                            raddamage45_match = get_hist(rad_multi, "Pixel/2018/2018B1/h45_220_2018B1_all_2D", hist_cache_multi)
                            raddamage45 = get_hist(rad_damage, "Pixel/2018/2018B1/h45_210_2018B1_all_2D", hist_cache_rad)
                            weight *= raddamage45.GetBinContent(raddamage45.FindBin(trackx1[0], tracky1[0]))
                            weight *= raddamage45_match.GetBinContent(raddamage45_match.FindBin(trackx1[0], tracky1[0]))

                        if 0.29 < fraction <= 0.37:
                            raddamage56_match = get_hist(rad_multi, "Pixel/2018/2018B2/h56_220_2018B2_all_2D", hist_cache_multi)
                            raddamage56 = get_hist(rad_damage, "Pixel/2018/2018B2/h56_210_2018B2_all_2D", hist_cache_rad)
                            weight *= raddamage56.GetBinContent(raddamage56.FindBin(trackx1[1], tracky1[1]))
                            weight *= raddamage56_match.GetBinContent(raddamage56_match.FindBin(trackx1[1], tracky1[1]))
                            raddamage45_match = get_hist(rad_multi, "Pixel/2018/2018B2/h45_220_2018B2_all_2D", hist_cache_multi)
                            raddamage45 = get_hist(rad_damage, "Pixel/2018/2018B2/h45_210_2018B2_all_2D", hist_cache_rad)
                            weight *= raddamage45.GetBinContent(raddamage45.FindBin(trackx1[0], tracky1[0]))
                            weight *= raddamage45_match.GetBinContent(raddamage45_match.FindBin(trackx1[0], tracky1[0]))

                        if 0.37 < fraction <= 0.50:
                            raddamage56_match = get_hist(rad_multi, "Pixel/2018/2018C/h56_220_2018C_all_2D", hist_cache_multi)
                            raddamage56 = get_hist(rad_damage, "Pixel/2018/2018C/h56_210_2018C_all_2D", hist_cache_rad)
                            weight *= raddamage56.GetBinContent(raddamage56.FindBin(trackx1[1], tracky1[1]))
                            weight *= raddamage56_match.GetBinContent(raddamage56_match.FindBin(trackx1[1], tracky1[1]))
                            raddamage45_match = get_hist(rad_multi, "Pixel/2018/2018C/h45_220_2018C_all_2D", hist_cache_multi)
                            raddamage45 = get_hist(rad_damage, "Pixel/2018/2018C/h45_210_2018C_all_2D", hist_cache_rad)
                            weight *= raddamage45.GetBinContent(raddamage45.FindBin(trackx1[0], tracky1[0]))
                            weight *= raddamage45_match.GetBinContent(raddamage45_match.FindBin(trackx1[0], tracky1[0]))

                        if 0.50 < fraction <= 0.77:
                            raddamage56_match = get_hist(rad_multi, "Pixel/2018/2018D1/h56_220_2018D1_all_2D", hist_cache_multi)
                            raddamage56 = get_hist(rad_damage, "Pixel/2018/2018D1/h56_210_2018D1_all_2D", hist_cache_rad)
                            weight *= raddamage56.GetBinContent(raddamage56.FindBin(trackx1[1], tracky1[1]))
                            weight *= raddamage56_match.GetBinContent(raddamage56_match.FindBin(trackx1[1], tracky1[1]))
                            raddamage45_match = get_hist(rad_multi, "Pixel/2018/2018D1/h45_220_2018D1_all_2D", hist_cache_multi)
                            raddamage45 = get_hist(rad_damage, "Pixel/2018/2018D1/h45_210_2018D1_all_2D", hist_cache_rad)
                            weight *= raddamage45.GetBinContent(raddamage45.FindBin(trackx1[0], tracky1[0]))
                            weight *= raddamage45_match.GetBinContent(raddamage45_match.FindBin(trackx1[0], tracky1[0]))

                        if fraction > 0.77:
                            raddamage56_match = get_hist(rad_multi, "Pixel/2018/2018D2/h56_220_2018D2_all_2D", hist_cache_multi)
                            raddamage56 = get_hist(rad_damage, "Pixel/2018/2018D2/h56_210_2018D2_all_2D", hist_cache_rad)
                            weight *= raddamage56.GetBinContent(raddamage56.FindBin(trackx1[1], tracky1[1]))
                            weight *= raddamage56_match.GetBinContent(raddamage56_match.FindBin(trackx1[1], tracky1[1]))
                            raddamage45_match = get_hist(rad_multi, "Pixel/2018/2018D2/h45_220_2018D2_all_2D", hist_cache_multi)
                            raddamage45 = get_hist(rad_damage, "Pixel/2018/2018D2/h45_210_2018D2_all_2D", hist_cache_rad)
                            weight *= raddamage45.GetBinContent(raddamage45.FindBin(trackx1[0], tracky1[0]))
                            weight *= raddamage45_match.GetBinContent(raddamage45_match.FindBin(trackx1[0], tracky1[0]))

                        print(f"weight rad {weight}")
                        n_p_rad += weight * weight_factor

                        for branch in vector_branches.values():
                            branch.clear()
                        for value in xi:
                            vector_branches["proton_xi"].push_back(float(value))
                        for value in arm:
                            vector_branches["proton_arm"].push_back(float(value))
                        for value in thx:
                            vector_branches["proton_thx"].push_back(float(value))
                        for value in thy:
                            vector_branches["proton_thy"].push_back(float(value))

                        arm0_count = float(proton_count(arm, 0))
                        arm1_count = float(proton_count(arm, 1))
                        scalars["n_protons_arm0"][0] = arm0_count
                        scalars["n_protons_arm1"][0] = arm1_count
                        scalars["pps_has_arm0"][0] = 1.0 if arm0_count > 0 else 0.0
                        scalars["pps_has_arm1"][0] = 1.0 if arm1_count > 0 else 0.0
                        scalars["pps_has_both_arms"][0] = 1.0 if (arm0_count > 0 and arm1_count > 0) else 0.0

                        xi_arm1_1_val = proton_first(xi, arm, 0, 0)
                        xi_arm1_2_val = proton_first(xi, arm, 0, 1)
                        xi_arm2_1_val = proton_first(xi, arm, 1, 0)
                        xi_arm2_2_val = proton_first(xi, arm, 1, 1)

                        scalars["xi_arm1_1"][0] = xi_arm1_1_val
                        scalars["xi_arm1_2"][0] = xi_arm1_2_val
                        scalars["xi_arm2_1"][0] = xi_arm2_1_val
                        scalars["xi_arm2_2"][0] = xi_arm2_2_val

                        if xi_arm1_1_val >= 0:
                            shift_1 = xi_sist_inter_1.Eval(xi_arm1_1_val)
                            scalars["xi_arm1_1_up"][0] = xi_arm1_1_val + shift_1
                            scalars["xi_arm1_1_dw"][0] = xi_arm1_1_val - shift_1
                        else:
                            scalars["xi_arm1_1_up"][0] = -999.0
                            scalars["xi_arm1_1_dw"][0] = -999.0

                        if xi_arm2_1_val >= 0:
                            shift_2 = xi_sist_inter_2.Eval(xi_arm2_1_val)
                            scalars["xi_arm2_1_up"][0] = xi_arm2_1_val + shift_2
                            scalars["xi_arm2_1_dw"][0] = xi_arm2_1_val - shift_2
                        else:
                            scalars["xi_arm2_1_up"][0] = -999.0
                            scalars["xi_arm2_1_dw"][0] = -999.0

                        scalars["mu_pt"][0] = e_pt[0]
                        scalars["tau_pt"][0] = tau_pt[0]
                        scalars["mu_charge"][0] = e_charge[0]
                        scalars["tau_charge"][0] = tau_charge[0]
                        scalars["mu_eta"][0] = e_eta[0]
                        scalars["tau_eta"][0] = tau_eta[0]
                        scalars["mu_phi"][0] = e_phi[0]
                        scalars["tau_phi"][0] = tau_phi[0]
                        scalars["mu_id"][0] = e_id[0]
                        scalars["tau_id_full"][0] = tau_id_full[0]
                        scalars["tau_id1"][0] = tau_id[0]
                        scalars["tau_id2"][0] = tau_id_antie[0]
                        scalars["tau_id3"][0] = tau_id_antimu[0]
                        scalars["tau_id"][0] = tau_id[0]
                        scalars["tau_id_antimu"][0] = tau_id_antimu[0]
                        scalars["tau_id_antie"][0] = tau_id_antie[0]
                        scalars["tau_id_antij"][0] = tau_id_antij[0]
                        scalars["tau_decay"][0] = tau_decay[0]
                        scalars["mu_energy"][0] = e_energy[0]
                        scalars["tau_energy"][0] = tau_energy[0]
                        scalars["mu_n"][0] = float(len(e_pt))
                        scalars["tau_n"][0] = float(len(tau_pt))
                        scalars["mu_mass"][0] = ele.M()
                        scalars["tau_mass"][0] = tau.M()
                        scalars["acop"][0] = acop
                        scalars["sist_pt"][0] = Pt
                        scalars["sist_mass"][0] = Mt
                        scalars["sist_rap"][0] = (ele + tau).Rapidity()
                        scalars["met_phi"][0] = met_phi[0]
                        scalars["met_pt"][0] = met_pt[0]
                        scalars["M_e_met"][0] = m_e_met
                        if len(jet_pt) > 0:
                            scalars["jet_pt"][0] = jet_pt[0]
                            scalars["jet_eta"][0] = jet_eta[0]
                            scalars["jet_phi"][0] = jet_phi[0]
                            scalars["jet_mass"][0] = jet_mass[0]
                            scalars["jet_btag"][0] = jet_btag[0]
                        else:
                            scalars["jet_pt"][0] = -1.0
                            scalars["jet_eta"][0] = -999.0
                            scalars["jet_phi"][0] = -999.0
                            scalars["jet_mass"][0] = -999.0
                            scalars["jet_btag"][0] = -999.0

                        jet_b_n = sum(1 for value in jet_btag if value > 0.5)
                        scalars["n_b_jet"][0] = float(jet_b_n)

                        scalars["n_pu"][0] = 0.0

                        scalars["weight"][0] = weight_factor * weight

                        scalars["thy1"][0] = thy[0]
                        scalars["thy2"][0] = thy[1]
                        scalars["thx1"][0] = thx[0]
                        scalars["thx2"][0] = thx[1]
                        scalars["y1"][0] = y_vals[0]
                        scalars["y2"][0] = y_vals[1]
                        scalars["x1"][0] = x_vals[0]
                        scalars["x2"][0] = x_vals[1]
                        scalars["t1"][0] = t_vals[0]
                        scalars["t2"][0] = t_vals[1]
                        scalars["arm1"][0] = arm[0]
                        scalars["arm2"][0] = arm[1]
                        scalars["time1"][0] = Tempo[0]
                        scalars["time2"][0] = Tempo[1]
                        scalars["trackx1_1"][0] = trackx1[0]
                        scalars["trackx1_2"][0] = trackx1[1]
                        scalars["trackx2_1"][0] = trackx2[0]
                        scalars["trackx2_2"][0] = trackx2[1]
                        scalars["tracky1_1"][0] = tracky1[0]
                        scalars["tracky1_2"][0] = tracky1[1]
                        scalars["tracky2_1"][0] = tracky2[0]
                        scalars["tracky2_2"][0] = tracky2[1]
                        scalars["trackrpid1_1"][0] = trackrpid1[0]
                        scalars["trackrpid1_2"][0] = trackrpid1[1]
                        scalars["trackrpid2_1"][0] = trackrpid2[0]
                        scalars["trackrpid2_2"][0] = trackrpid2[1]
                        scalars["trackthx1_1"][0] = trackthx1[0]
                        scalars["trackthx1_2"][0] = trackthx1[1]
                        scalars["trackthx2_1"][0] = trackthx2[0]
                        scalars["trackthx2_2"][0] = trackthx2[1]
                        scalars["trackthy1_1"][0] = trackthy1[0]
                        scalars["trackthy1_2"][0] = trackthy1[1]
                        scalars["trackthy2_1"][0] = trackthy2[0]
                        scalars["trackthy2_2"][0] = trackthy2[1]

                        out.Fill()

    canvases = [ROOT.TCanvas(f"c{idx+1}", "", 700, 800) for idx in range(8)]
    histo_e_pt.Draw("histo")
    histo_tau_pt.Draw("histo")
    histo_n_tau.Draw("histo")
    histo_n_eletron.Draw("histo")
    histo_acop.Draw("histo")
    histo_Pt.Draw("histo")
    histo_Mt.Draw("histo")
    histo_DR.Draw("histo")

    print("os acontecimentos já foram moltiplicados por o sample weight")
    print(f"O número total de acontecimentos é {ntp1.GetEntries() * weight_sample}")
    print(f"O número de partículas com 1e e 1th é {n_id}")
    print(f"O n  mero de part  culas com tau pt e electron pt    {n_pt}")
    print(f"O número de partículas com o produto das cargas positivos é {n_charge}")
    print(f"O número de partículas com 2 prot reconstruidos é {n_rec}")
    print(f"O n  mero de part  culas com 2 prot reconstruidos que passam o up_xi cut é   {n_p_up_xi}")
    print(f"O n  mero de part  culas com 2 prot reconstruidos que passam o dw_xi cut      {n_p_dw_xi}")
    print(f"O n  mero de part  culas com 2 prot reconstruidos que passam o radiation damage cut       {n_p_rad}")

    output.Write()
    output.Close()
    proton_sist_1.Close()
    rad_multi.Close()
    rad_damage.Close()
    fundo.Close()


if __name__ == "__main__":
    main()
