# pip install uproot awkward vector numpy
import numpy as np, awkward as ak, uproot, vector
vector.register_awkward()

in_file  = "/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/ETau/Dados_2018_UL_skimmed_ETau_nano_fase0_merge.root"
treepath = "analyzer/ntp1"
out_file = "/eos/user/m/mblancco/samples_2018_etau/fase1_etau/dados_fase1_skimmed_ETau_merged_new.root"

branches = [
  # leptons
  "e_pt","e_eta","e_phi","e_energy","e_id","e_charge",
  "tau_pt","tau_eta","tau_phi","tau_energy","tau_charge",
  "tau_id","tau_id_antimu","tau_id_antie","tau_id_antij","tau_id_full",
  # jets/MET (optional to save)
  "met_pt","met_phi","jet_pt","jet_eta","jet_phi","jet_mass","jet_btag",
  # PPS
  "proton_xi","proton_t"
]

with uproot.open(in_file) as f:
    tr = f[treepath]
    a  = tr.arrays(branches, how=dict, library="ak")

def lead(x): return ak.firsts(x)  # None if empty

# leading objects
e = vector.awkward.zip(
    {"pt":lead(a["e_pt"]), "eta":lead(a["e_eta"]), "phi":lead(a["e_phi"]), "E":lead(a["e_energy"])},
    with_name="Momentum4D"
)
t = vector.awkward.zip(
    {"pt":lead(a["tau_pt"]), "eta":lead(a["tau_eta"]), "phi":lead(a["tau_phi"]), "E":lead(a["tau_energy"])},
    with_name="Momentum4D"
)

# presence / multiplicity
n_e   = ak.num(a["e_pt"])
n_tau = ak.num(a["tau_pt"])
has_e_tau   = (n_e > 0) & (n_tau > 0)
exactly_one = (n_e == 1) & (n_tau == 1)

# IDs (0/1)
e_id0   = lead(a["e_id"])
tid_mu  = lead(a["tau_id_antimu"])
tid_e   = lead(a["tau_id_antie"])
tid_j   = lead(a["tau_id_antij"])
tid_full= lead(a["tau_id_full"])

pass_eid   = ak.fill_none(e_id0   > 0.5, False)
pass_tauid = ak.fill_none((tid_full>0.5) & (tid_mu>0.5) & (tid_e>0.5) & (tid_j>0.5), False)

# geometry, kinematics, charge
dr       = e.deltaR(t)
pass_geo = ak.fill_none((dr>0.4) & (abs(e.eta)<2.4) & (abs(t.eta)<2.4), False)
pass_pt  = ak.fill_none((e.pt>35) & (t.pt>100), False)

eq0 = lead(a["e_charge"])
tq0 = lead(a["tau_charge"])
pass_q   = ak.fill_none((tq0 * eq0) < 0, False)   # opposite-sign (use >0 for same-sign QCD)

# PPS: require at least two valid protons (simple, robust quality)
pxi = a["proton_xi"]
pt  = a["proton_t"]
valid_p   = np.isfinite(pxi) & np.isfinite(pt) & (pxi > 0) & (pxi < 1)
n_protons = ak.sum(valid_p, axis=1)
pass_pps  = n_protons >= 2

# final signal-like selection on DATA
mask = has_e_tau & exactly_one & pass_eid & pass_tauid & pass_geo & pass_pt & pass_q & pass_pps

# ---- Cutflow (signal-like) ----
def cnt(m): return int(ak.sum(ak.fill_none(m, False)))
print("Cutflow (signal-like, data):")
print("  All events                 :", len(a["e_pt"]))
print("  ≥1 e & ≥1 τ                :", cnt(has_e_tau))
print("  Exactly 1 e & 1 τ          :", cnt(has_e_tau & exactly_one))
print("  e ID                       :", cnt(has_e_tau & exactly_one & pass_eid))
print("  τ IDs (all)                :", cnt(has_e_tau & exactly_one & pass_eid & pass_tauid))
print("  ΔR>0.4 & |η|<2.4           :", cnt(has_e_tau & exactly_one & pass_eid & pass_tauid & pass_geo))
print("  pT: e>35, τ>100            :", cnt(has_e_tau & exactly_one & pass_eid & pass_tauid & pass_geo & pass_pt))
print("  Opposite sign              :", cnt(has_e_tau & exactly_one & pass_eid & pass_tauid & pass_geo & pass_pt & pass_q))
print("  ≥2 PPS protons             :", cnt(has_e_tau & exactly_one & pass_eid & pass_tauid & pass_geo & pass_pt & pass_q & pass_pps))
print("  ==> Selected               :", cnt(mask))

# ---- (optional) write a skim with a few outputs ----
def to_np(x): return ak.to_numpy(ak.fill_none(x[mask], 0))
p4sys = (e + t)
out = {
  "e_pt": to_np(e.pt),   "tau_pt": to_np(t.pt),
  "e_eta": to_np(e.eta), "tau_eta": to_np(t.eta),
  "e_phi": to_np(e.phi), "tau_phi": to_np(t.phi),
  "sist_mass": to_np(p4sys.mass),
  "met_pt":  ak.to_numpy(ak.fill_none(lead(a["met_pt"])[mask],  0)),
  "met_phi": ak.to_numpy(ak.fill_none(lead(a["met_phi"])[mask], 0)),
}
with uproot.recreate(out_file) as fout:
    fout["tree"] = out
print("Wrote:", out_file)
