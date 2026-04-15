---
title: "MuTau channel: Combine tool"
date: "13/04/2026"
theme: "Madrid"
colortheme: "default"
fontsize: 10pt
aspectratio: 169
---

# Overview

### .
Set a 95% CL upper limit on the signal strength $r = \sigma_\text{obs} / \sigma_\text{SM}$ 

### data pipeline (BDT-based)
```
save_shapes.cpp   TMVAClassification.C   TMVAClassificationApplication.C
(kinematic hists) --> (BDT training)   --> (BDT score per event)
        |                                           |
MuTau_shapes.root                    mutau_bdt_shape.txt  -->  mutau_bdt_workspace.root
                                                                       |
                                                              combine (Limits, Significance,
                                                              FitDiagnostics, GoF, Impacts)
```

 corrections applied: **per-arm proton selection** and **signal cross-section normalization** (both bugs fixed before final results).

---

# Bug fix 1: signal proton selection

### Problem
The signal code required exactly two total proton entries, discarding events with $\geq 3$ pileup protons.

### Fix
Replaced with a per-arm requirement: **at least one valid proton in arm 0 AND at least one in arm 1**.

### results

|  | Before  | After  | Change |
|--------|-----------|-----------|--------|
| Selected signal events (unweighted) | 14,085 | 21,398 | +7,313 |
| Final weighted signal yield | 0.0633 | 0.0794 | +25% |

---

# bug  2: signal cross-section normalization

### problem
The signal was normalized as:
$$w_\text{sample} = \frac{54900 \times w_\text{SM}[0]}{4000 \times 1000}$$
The denominator $4{,}000{,}000$ has no physical basis in the sample.


Opened the upstream ntuple (`GammaGammaTauTau_2018_UL_MuTau_SMandBSMweights_ntuplesfromminiJuly.root`):
- Total events: **299,979** (no upstream filter)
- $\sum w_\text{SM}[0]$ over all events: **402.661**

### fix
Correct formula using $\sigma_\text{SM}^\text{rwgt51} = 0.0047\ \text{pb}$:
$$w_\text{sample} = \frac{54900 \times 0.0047 \times w_\text{SM}[0]}{402.661}$$


---

# bug  2: signal cross-section normalization (cont)

### problem
The signal was normalized as:
$$w_\text{sample} = \frac{54900 \times w_\text{SM}[0]}{4000 \times 1000}$$
The denominator $4{,}000{,}000$ has no physical basis in the sample.

### verification
$$\sigma_\text{implied} = \frac{\sum w_\text{sample}}{L} = \frac{258.03}{54900} = 0.0047\ \text{pb} = 4.7\ \text{fb} \quad \checkmark$$


---

#  1: save\_shapes.cpp

Produces `MuTau_shapes.root` -- input ROOT file with per-sample kinematic histograms that Combine reads as shapes.

### Input samples
| Sample | File |
|--------|------|
| Data | `Data_2018_UL_MuTau_nano_merged_proton_vars.root` |
| DY | `DY_2018_UL_MuTau_nano_merged_pileup_protons.root` |
| ttbar | `ttjets_2018_UL_MuTau_nano_merged_pileup_protons.root` |
| QCD | `QCD_2018_UL_MuTau_nano_merged_proton_vars_new.root` |
| Signal | `MuTau_sinal_SM_2018_july.root` |

### What it does
1. Requires `sist_mass > 0`; for MC: protons on both arms (`xi1 > 0 && xi2 > 0`)
2. Fills invariant mass (`m_X`, 30 bins, 0--1200 GeV) and 7 other kinematic histograms per sample
3. Applies MC normalization (DY: `Scale(1.004e-4)`, ttbar: `Scale(1.0)`, data/QCD: weight = 1)

---

#  2: TMVA  (TMVAClassification.C)

### Variable list
Central-system rapidity, acoplanarity, total system $p_T$, muon $p_T$, tau $p_T$, invariant mass, PPS mass-matching term, MET, rapidity-matching term.



### BDT scan results (corrected normalization, $\sigma_\text{SM} = 4.7$ fb)

| Scan | Config | Exp. $-2\sigma$ | Exp. $-1\sigma$ | **Exp. median** | Exp. $+1\sigma$ | Exp. $+2\sigma$ | Observed |
|------|--------|---------|---------|---------|---------|---------|---------|
| **A** | AdaBoost, $N=600$, $d=2$ | **0.79** | **1.20** | **2.04** | **3.76** | **6.18** | **6.03** |
| B | AdaBoost, $N=850$, $d=3$ | 0.80 | 1.23 | 2.13 | 3.95 | 6.44 | 4.73 |
| C | GradBoost, $N=600$, $d=2$ | 48.8 | 69.9 | 111.5 | 194.2 | 337.9 | 219.4 |

---

# TVMA pt2

### Methods trained
Likelihood, Fisher, BDT -- **BDT selected as best discriminator**.

---

#  3: datacard (mutau\_bdt\_shape.txt)

### channel and process structure
```
imax 1   # 1 channel: mutau
jmax 3   # 3 backgrounds: DY, ttbar, QCD
kmax *   # all nuisance parameters
```

### shape mapping (all from `../MuTau_shapes.root`)
| Process | Histogram | Index |
|---------|-----------|-------|
| `data_obs` | `bdt_data` | -- |
| `sinal` | `bdt_sinal` | 0 (signal) |
| `dy` | `bdt_dy` | 1 |
| `ttjets` | `bdt_ttjets` | 2 |
| `qcd` | `bdt_qcd` | 3 |

all rates set to `-1` $\to$ Combine reads normalization directly from histogram integrals.

 yield: **1,708 events** (protons on both arms).

---

# 3: systematic uncertainties

### Lognormal (lnN) uncertainties
| source | signal | dY | ttbar | QCD |
|--------|--------|----|-------|-----|
| Luminosity | 1.5% | 1.5% | 1.5% | -- |
| tau ID vs jet | 5.0% | 4.0% | 4.0% | -- |
| tau ID vs e | 2.5% | 2.5% | 2.5% | -- |
| tau ID vs mu | 3.1% | 3.2% | 2.6% | -- |
| Muon trigger | 0.5% | 0.07% | 0.07% | -- |
| Muon ID+Iso | 0.02% | 0.04% | 0.04% | -- |
| Muon reco | 0.06% | 0.05% | 0.05% | -- |
| Proton $\xi$ (`xiProton`) | 50% | 50% | 50% | 50% |


---

# 3: systematic uncertainties (cont)

### Floating normalization factors (rateParam, range [0, 5])
- `beta_dy` for DY, constrained by `dy_norm lnN 1.30`
- `beta_tt` for ttbar, constrained by `ttjets_norm lnN 1.50`
- `beta_qcd` for QCD, constrained by `qcd_norm lnN 1.50`

---

#  4: running Combine

```bash
##Convert datacard to RooWorkspace
text2workspace.py mutau_bdt_shape.txt -o mutau_bdt_workspace.root -m 120

##Asymptotic 95% CL limits
combine -M AsymptoticLimits mutau_bdt_workspace.root -m 120 -n MuTau_bdt --cl 0.95

##Observed and expected significance
combine -M Significance mutau_bdt_workspace.root -m 120 -n MuTau_bdt --significance
combine -M Significance mutau_bdt_workspace.root -m 120 -n MuTau_bdt_exp -t -1 --expectSignal=1

##Fit diagnostics
combine -M FitDiagnostics mutau_bdt_workspace.root -m 120 -n MuTau_bdt \
    --saveShapes --saveWithUncertainties --rMin 0 --rMax 500 --robustFit 1

##Best-fit signal strength(singles)
combine -M MultiDimFit mutau_bdt_workspace.root -m 120 -n MuTau_bdt_singles \
    --algo singles --rMin 0 --rMax 500 --robustFit 1

##Goodness of fit (observed+toys)
combine -M GoodnessOfFit mutau_bdt_workspace.root -m 120 -n MuTau_bdt_gof --algo saturated
combine -M GoodnessOfFit mutau_bdt_workspace.root -m 120 -n MuTau_bdt_gof_toys \
    --algo saturated -t 200 --toysFrequentist
```

---

#  5: impacts

```bash
combineTool.py -M Impacts -d mutau_bdt_workspace.root -m 120 \
    --doInitialFit --robustFit 1 --rMin 0 --rMax 500

combineTool.py -M Impacts -d mutau_bdt_workspace.root -m 120 \
    --doFits --robustFit 1 --rMin 0 --rMax 500

combineTool.py -M Impacts -d mutau_bdt_workspace.root -m 120 -o impacts_MuTau_bdt.json

plotImpacts.py -i impacts_MuTau_bdt.json -o impacts_MuTau_bdt
```

---

# results: 95% CL upper limits on $r$

### Method: AsymptoticLimits, BDT shape, mH = 120 GeV (Scan A, corrected normalization)

| Estimate | $r = \sigma / \sigma_\text{SM}$ | Absolute $\sigma_{95}$ |
|----------|--------------------------------|------------------------|
| Expected $-2\sigma$ | -- | -- |
| Expected $-1\sigma$ | -- | -- |
| **Expected (median)** | **2.04** | **$\sim$9.6 fb** |
| Expected $+1\sigma$ | -- | -- |
| Expected $+2\sigma$ | -- | -- |
| **Observed** | **6.03** | **$\sim$28.3 fb** |

**mass-shape cross-check:** expected $r < 22.75$, observed $r < 45.0$ (mass as discriminant).

The BDT result is $\sim$11$\times$ better than the mass-shape result

---

# significance and best-fit signal strength

### after normalization fix ($\sigma_\text{SM} = 4.7$ fb)

| Quantity | Result |
|----------|--------|
| Observed local significance | $2.41\sigma$ | 
| Expected significance at $r=1$ | $1.62\sigma$ |
| Best-fit signal strength | $r = 2.13\;^{+1.92}_{-1.17}$ |

**before fix (wrong normalization):** expected significance was $\sim 0.046\sigma$, best-fit $r \sim 100$ -- those numbers were artifacts of the 47$\times$ normalization underestimate.

**now:** The mild excess ($2.41\sigma$) is consistent with a statistical fluctuation

---

# goodness of fit and nuisance impacts

### saturated GoF (after  fix)
| Quantity | Value |
|----------|-------|
| Observed test statistic | 13.86 |
| p-value | 0.51 |

Observed value is well within the toy distribution-> the fitted model is compatible with data.

### dominant nuisance impacts on $r$

| Nuisance / group | Approx. effect on $r$ | 
|-----------------|----------------------|
| `xiProton` | $+49\;/\;-33$ | 
| `beta_qcd` | $-24\;/\;+37$ |
| `beta_dy` | $-22\;/\;+31$ |
| `beta_tt` | $-21\;/\;+27$ | 
| Muon / tau efficiency | small to moderate | 


---

# Summary and next steps


### corrected results (Scan A BDT, $\sigma_\text{SM} = 4.7$ fb)

| Quantity | Value |
|----------|-------|
| Expected limit (median) | $r < 2.04$ |
| Observed limit | $r < 6.03$ |
| Observed significance | $2.41\sigma$ |
| Expected significance | $1.62\sigma$ |
| Best-fit $r$ | $2.13\;^{+1.92}_{-1.17}$ |
| GoF p-value | 0.51 |

Scan B gives expected $r < 2.13$ (+4\% vs A) -- Scan A robustness confirmed.


---

# Summary and next steps (cont)

### What was achieved
- **bug 1 fixed:** proton selection corrected to per-arm logic ($\geq$1 per PPS arm): +25% signal yield
- **bug 2 fixed:** signal normalization corrected to $\sigma_\text{SM} = 4.7$ fb (was 47$\times$ too small)
- BDT trained and scanned (A, B, C); Scan A selected ; Scan C rejected (unstable)
- statistical validation: limits, significance, fit diagnostics, GoF, impacts


### Possible improvement items
1. **Post-fit shape validation**: identify which BDT bins drive the $2.41\sigma$ excess
2. **Proton modeling**: `xiProton` is the dominant nuisance -- refine proton $\xi$ uncertainty ?
3. **Background normalization**: assess DY, QCD, ttbar with control regions ?
