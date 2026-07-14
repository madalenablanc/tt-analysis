---
title: "MuTau signal processing: all steps in sinal.py"
date: "2026-04-25"
theme: "Madrid"
colortheme: "default"
fontsize: 10pt
aspectratio: 169
---

# Input and output files

**Input ntuple** (`ntp1` tree):
```
/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/SignalSamples/MuTau/
  GammaGammaTauTau_2018_UL_MuTau_SMandBSMweights_ntuplesfromminiJuly.root
```
299,979 events. Contains branches: `mu_pt`, `tau_pt`, `proton_xi`, `proton_arm`, `proton_thx`, `weight_sm[102]`, `bsm_weights[102]`, etc.

**Output** (`tree` tree):
```
MuTau_sinal_SM_2018_july.root
```
Flat tree with ~60 scalar branches + vector branches (`proton_xi`, `proton_arm`, `proton_thx`, `proton_thy`) + `weights_bsm_sf[102]`.

---

# Auxiliary files loaded at startup

| File | Purpose |
|------|---------|
| `muon_neg18_lowTrig.txt` | Muon trigger SF table (negative charge, $p_T < 200$ GeV) |
| `muon_pos18_lowTrig.txt` | Muon trigger SF table (positive charge, $p_T < 200$ GeV) |
| `muon_18_highTrig.txt` | Muon trigger SF table ($p_T > 200$ GeV, both charges) |
| `muon_IdISO.txt` | Muon ID+ISO SF table |
| `reco_charactersitics_version1.root` | $\xi$ systematic uncertainty vs. $\xi$ graph (arm 0 and arm 1), fitted with `pol20` |
| `pixelEfficiencies_multiRP_reMiniAOD.root` | Pixel multi-RP reconstruction efficiency 2D histograms |
| `pixelEfficiencies_radiation_reMiniAOD.root` | Pixel radiation damage efficiency 2D histograms |

All SF tables are $(\eta, p_T)$-binned. The $\xi$ systematics graph is fit to a degree-20 polynomial used to compute $\xi$ up/down shifts per event.

---

# Muon scale factor functions

**`muon_trig_sf(pt, eta, charge, p_tot, flag=0)`**

- $p_T < 200$ GeV, negative charge $\to$ `muon_neg18_lowTrig.txt`
- $p_T < 200$ GeV, positive charge $\to$ `muon_pos18_lowTrig.txt`
- $p_T > 200$ GeV $\to$ `muon_18_highTrig.txt`
- Default `flag=0` returns central value; `flag=1` returns systematic uncertainty
- Falls back to SF$=1$ if $|\eta|>2.4$ or $p_T<35$ GeV

**`muon_idiso_sf(pt, eta, charge, p_tot, flag=0)`**

- Looks up `muon_IdISO.txt` for all charges
- Falls back to SF$=1$ if $|\eta|>2.4$ or $p_T<35$ GeV

**`muon_reco_sf(pt, eta, flag=0)`**

- Hard-coded table; depends on $p_\text{tot} = p_T / \sin(2\arctan(e^{-\eta}))$
- Low-$p_T$ ($p_T < 200$ GeV): only correction for $|\eta|>2.1$ ($\text{SF}=0.999$)
- High-$p_T$ ($p_T > 200$ GeV): binned in $p_\text{tot}$ and $|\eta|$ (values 0.978--0.9950)
- `flag=1` returns systematic uncertainty values (small: $\sim 0.001$--$0.004$)

---

# Step 0: normalization weight (all events)

Computed **before any selection**, for every event:

$$w_\text{norm} = \frac{L \times w_\text{SM}[0]}{N_\text{gen}} = \frac{54900 \times w_\text{SM}[0]}{4{,}000{,}000}$$

- $w_\text{SM}[0]$: per-event LHE reweighting weight (SM point, index 0 out of 102 weights)
- $N_\text{gen} = 4{,}000{,}000$: original generated sample size (before skimming)
- The ntuple on disk has 299,979 events (after skimming); the weights already encode the reduction
- The denominator 4M and the stored weights are set up so $\sum w_\text{norm} \approx L \times \sigma_\text{SM}$

Cutflow at step 0 records: raw count and $\sum w_\text{SM}[0]$.

---

# Step 1 — non-empty $\mu$/$\tau$ collections

**Cut:** `len(e_charge) > 0` AND `len(tau_charge) > 0`

Rejects events with no reconstructed muon or tau candidate.

- No new weight applied
- Cutflow records: $\sum w_\text{norm}$ for passing events
- Also computes $\Delta\phi(\mu,\tau)$, $\Delta\eta(\mu,\tau)$, $\Delta R(\mu,\tau)$ for use in step 2

---

# Step 2 — baseline ID + $\eta$ + $\Delta R$: muon SFs introduced

**Cuts (all must pass simultaneously):**

| Cut | Value |
|-----|-------|
| Tau ID (vs jets) | `tau_id_full[0] > 0.5` |
| Muon ID | `e_id[0] > 0.5` |
| Tau $|\eta|$ | $< 2.4$ |
| Muon $|\eta|$ | $< 2.4$ |
| $\Delta R(\mu,\tau)$ | $> 0.4$ |

**Muon SFs computed and combined:**
$$w_\mu = \text{SF}_\text{trig}(p_T, \eta, q, p_\text{tot}) \times \text{SF}_\text{ID+ISO}(p_T, \eta) \times \text{SF}_\text{reco}(p_T, \eta)$$

$$w_\text{factor} = w_\text{norm} \times w_\mu$$

This `weight_factor` is the running weight for **all subsequent steps**.

---

# Step 2 — diagnostic SF decomposition

At step 2, four partial sums are tracked to show the effect of each SF:

| Sum | Meaning |
|-----|---------|
| $\sum w_\text{norm}$ | Before any muon SF |
| $\sum w_\text{norm} \times \text{SF}_\text{trig}$ | After trigger SF only |
| $\sum w_\text{norm} \times \text{SF}_\text{trig} \times \text{SF}_\text{ID+ISO}$ | After trigger + ID+ISO |
| $\sum w_\text{factor}$ | Full muon weight |

Printed at end of run to show the relative impact of each correction.

---

# Step 3 — kinematic cuts ($p_T$)

**Cuts:**

$$p_T^\tau > 100 \text{ GeV} \quad \text{AND} \quad p_T^\mu > 35 \text{ GeV}$$

- No new weight factor
- Cutflow records $\sum w_\text{factor}$
- Histograms for $\mu$ $p_T$, $\tau$ $p_T$, multiplicity, acoplanarity, system $p_T$, system mass, $\Delta R$ are filled at step 6 (after proton cut) — not here

---

# Step 4 — opposite-sign charge

**Cut:** $q_\mu \times q_\tau < 0$

Selects events where muon and tau have opposite electric charge, as expected from $\gamma\gamma \to \mu\tau$.

- No new weight factor
- Cutflow records $\sum w_\text{factor}$

---

# Step 5 — $\eta$ repeat check

**Cut:** $|\eta_\tau| < 2.4$ AND $|\eta_\mu| < 2.4$

This repeats the $\eta$ condition already applied in step 2. In practice, all events passing step 2 also pass step 5. The cutflow entry exists as a cross-check — it should be identical to step 4.

At this point, the $\xi$ systematic variations are also computed and stored:

$$\xi_\text{up/dw} = \xi \pm \delta\xi(\xi), \quad \delta\xi = \texttt{xi\_sist\_inter.Eval}(\xi)$$

where $\delta\xi(\xi)$ comes from the fitted `pol20` systematics function.

---

# Step 6 — $\geq 1$ proton per arm (PPS selection)

**Cut:** at least one reconstructed proton with arm=0 AND at least one with arm=1.

Uses `proton_arm_index(arm, 0)` and `proton_arm_index(arm, 1)` — returns the list index of the first proton in each arm, or $-1$ if absent.

- No new weight factor; this is a **hard cut**, not a weight
- Selects only events with signal-like double-arm proton topology
- Before this cut, the xi multiplicity table is filled: counts events with 0, 1, 2, or $>2$ proton xi values (weighted by `weight_factor`)
- Histograms (mu $p_T$, tau $p_T$, multiplicity, acop, system $p_T$, system mass, $\Delta R$) are filled here

---

# Step 7 — $\theta_x$ fiducial cut (xangle-dependent)

**Cut:** proton $\theta_x$ within acceptance for both arms, using xangle-dependent limits.

The crossing angle `xangle` is assigned from the event's fractional position in the sample (proxy for luminosity-weighted era):

$$\text{xangle} = 130^\circ \text{ to } 160^\circ \quad (\text{based on } i/N_\text{total})$$

Two TF1 functions (`limit_fun_arm_0`, `limit_fun_arm_1`) compute the $\theta_x$ upper limit as a function of $\xi$ and the crossing angle. An event is rejected if:

$$\theta_x^\text{arm0} > \text{limit}_0(\xi_0, \text{xangle}) \quad \text{OR} \quad \theta_x^\text{arm1} > \text{limit}_1(\xi_1, \text{xangle})$$

- No new weight
- Cutflow records $\sum w_\text{factor}$

---

# Step 8 — track $(x,y)$ fiducial cut (era-dependent)

**Cut:** pixel track positions $(x_1, y_1, x_2, y_2)$ for both arms must be within the active sensor region.

The fiducial windows differ by data-taking era, assigned by event fraction:

| Fraction range | Era |
|----------------|-----|
| $\leq 0.21$ | 2018A |
| $0.21$--$0.29$ | 2018B1 |
| $0.29$--$0.37$ | 2018B2 |
| $0.37$--$0.50$ | 2018C |
| $0.50$--$0.77$ | 2018D1 |
| $> 0.77$ | 2018D2 |

Each era has separate $[x_\text{min}, x_\text{max}] \times [y_\text{min}, y_\text{max}]$ windows for strips 1 and 2 of both arms. Events outside these windows are rejected.

- No new weight factor yet
- Cutflow records $\sum w_\text{factor}$

---

# Step 9 — pixel radiation damage weight

After the track fiducial cut, the radiation damage efficiency is applied as a multiplicative weight.

$$w_\text{rad} = \varepsilon^\text{multi}_\text{arm0}(x_1, y_1) \times \varepsilon^\text{rad}_\text{arm0}(x_1, y_1) \times \varepsilon^\text{multi}_\text{arm1}(x_1, y_1) \times \varepsilon^\text{rad}_\text{arm1}(x_1, y_1)$$

Each $\varepsilon$ is a 2D histogram value at the proton track position $(x_1, y_1)$:

| Root file | Histogram type | Era histograms |
|-----------|---------------|----------------|
| `pixelEfficiencies_multiRP_reMiniAOD.root` | Multi-RP reco efficiency | `h45_220_2018X_all_2D`, `h56_220_2018X_all_2D` |
| `pixelEfficiencies_radiation_reMiniAOD.root` | Radiation damage efficiency | `h45_210_2018X_all_2D`, `h56_210_2018X_all_2D` |

- `h45` = arm 0 (sector 45), `h56` = arm 1 (sector 56)
- Histograms selected per era (same era boundaries as step 8)

**Final weight written to tree:**
$$w_\text{final} = w_\text{factor} \times w_\text{rad} = w_\text{norm} \times w_\mu \times w_\text{rad}$$

---

# Step 10 — tree fill

All branches written to the output tree:

| Branch group | Contents |
|-------------|----------|
| Kinematics | `mu_pt`, `tau_pt`, `mu_eta`, `tau_eta`, `sist_mass`, `sist_pt`, `sist_rap`, `acop`, `met_pt`, `M_e_met` |
| ID / charge | `mu_id`, `mu_charge`, `tau_id_full`, `tau_id`, `tau_id_antimu`, `tau_id_antie`, `tau_id_antij`, `tau_decay` |
| Proton | `xi_arm1_1/2`, `xi_arm2_1/2`, `n_protons_arm0/1`, `pps_has_arm0/1/both` |
| Proton tracks | `thx1/2`, `thy1/2`, `x1/2`, `y1/2`, `t1/2`, `trackx1/2_1/2`, `tracky1/2_1/2`, `trackrpid1/2`, `trackthx/y1/2` |
| $\xi$ systematics | `xi_arm1_1_up/dw`, `xi_arm2_1_up/dw` |
| Muon systematics | `syst_mu_trig`, `syst_mu_idiso`, `syst_mu_reco` |
| Weights | `weight` $= w_\text{final}$, `weights_bsm_sf[102]` |
| Jet | `jet_pt`, `jet_eta`, `jet_phi`, `jet_mass`, `jet_btag`, `n_b_jet` |

---

# BSM weights stored alongside SM

At step 6, the 102-element `bsm_weights` array is normalized to the SM cross-section:

```python
sm_cs = bsm_weights[51]          # index 51 = SM reference point
bsm_weights[p] = bsm_weights[p] / sm_cs   # for p in 0..101
```

The normalized array is stored in `weights_bsm_sf[102]`. Multiplying any entry by `weight` gives the expected yield for a different BSM coupling scenario without re-running the selection.

---

# Complete weight flow summary

```
For every event i in ntp1:

  STEP 0   w_norm = 54900 * w_SM[0] / (4000 * 1000)
           [all generated events]

  STEP 1   cut: len(mu) > 0 AND len(tau) > 0

  STEP 2   cut: tau_id_full>0.5, mu_id>0.5,
                |eta_mu|<2.4, |eta_tau|<2.4, dR>0.4
           w_factor = w_norm * SF_trig * SF_ID+ISO * SF_reco

  STEP 3   cut: pT_tau > 100 GeV, pT_mu > 35 GeV

  STEP 4   cut: q_mu * q_tau < 0  (opposite sign)

  STEP 5   cut: |eta| < 2.4 repeat  [xi syst shifts computed]

  STEP 6   cut: >=1 proton per arm
           [BSM weights normalized, histograms filled]

  STEP 7   cut: theta_x fiducial (xangle-dependent)

  STEP 8   cut: track (x,y) fiducial (era-dependent)

  STEP 9   w_rad = eff_multi_arm0 * eff_rad_arm0
                 * eff_multi_arm1 * eff_rad_arm1
           w_final = w_factor * w_rad

  STEP 10  out.Fill()   [weight = w_final]
```
