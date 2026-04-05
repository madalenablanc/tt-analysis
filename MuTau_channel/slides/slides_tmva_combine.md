---
title: "MuTau channel: TMVA multi-method training & Combine results"
date: "02/04/2026"
theme: "Madrid"
colortheme: "default"
fontsize: 10pt
aspectratio: 169
---

# Overview

### Goal
Compare three MVA discriminants (BDT, Likelihood, Fisher) trained on the MuTau channel and propagate them to 95% CL upper limits using the CMS Combine tool.

### Signal process
Exclusive $\gamma\gamma \to \tau^+\tau^-$ (μτ final state), $\sigma_\text{SM} \approx 1.34\ \text{fb}$, $\mathcal{L} = 54.9\ \text{fb}^{-1}$, 2018 UL.

### Full pipeline
```
TMVAClassification.C  -->  run_bdt_all.sh  -->  save_shapes.cpp
   (train 3 methods)       (apply to all        (shapes ROOT file)
                            samples)                    |
                                               mutau_bdt_shape.txt
                                               mutau_likelihood_shape.txt
                                                        |
                                                    combine
                                                        |
                                               mva_comparison.cpp
                                              (plots + KS test)
```

---

# TMVA: input variables

### 9 discriminating variables used in training

| Variable | Description |
|----------|-------------|
| `sist_rap` | Central system rapidity |
| `acop` | Acoplanarity |
| `sist_pt` | System $p_T$ |
| `mu_pt` | Muon $p_T$ |
| `tau_pt` | Tau $p_T$ |
| `sist_mass` | Invariant mass $m_X$ |
| `sist_mass - 13000·√(ξ₁ξ₂)` | Mass matching condition |
| `met_pt` | Missing $E_T$ |
| `sist_rap - 0.5·ln(ξ₁/ξ₂)` | Rapidity matching condition |

### Training sample
- Signal: `MuTau_sinal_SM_2018_july.root`, $n_\text{train} = 6500$, weight $= 0.00021$
- Background: `background_total-protons_syst.root`, $n_\text{train} = 15000$, weight $= 104$

---

# TMVA: methods trained

### Three methods enabled simultaneously

| Method | Type | Key parameters |
|--------|------|----------------|
| **BDT** | Adaptive Boost | NTrees=850, MaxDepth=3, AdaBoostBeta=0.5 |
| **Likelihood** | 1D PDF (naive Bayes) | Spline2 interpolation, NSmooth=1 |
| **Fisher** | Linear discriminant | Decorrelated input variables |

### Why this combination?
- BDT: best non-linear discrimination, main analysis method
- Likelihood: fast, interpretable, useful cross-check
- Fisher: near-instant training, linear baseline

### Weight files produced
```
dataset/weights/TMVAClassification_BDT.weights.xml
dataset/weights/TMVAClassification_Likelihood.weights.xml
dataset/weights/TMVAClassification_Fisher.weights.xml
```

---

# TMVA: overtraining check (KS test)

### Method
KS test comparing train vs test distributions for signal and background.
A p-value $> 0.01$ means no significant overtraining.

### Results

| Method | Signal KS p-value | Background KS p-value | Status |
|--------|-------------------|-----------------------|--------|
| BDT | -- | -- | -- |
| Likelihood | -- | -- | -- |
| Fisher | -- | -- | -- |

*(fill in from `./mva_comparison` terminal output)*

---

# TMVA: signal vs background separation — BDT

![BDT separation](tmva_separation_BDT.png){ width=90% }

---

# TMVA: signal vs background separation — Likelihood

![Likelihood separation](tmva_separation_Likelihood.png){ width=90% }

---

# TMVA: signal vs background separation — Fisher

![Fisher separation](tmva_separation_Fisher.png){ width=90% }

---

# TMVA: MVA output distributions

### BDT output
![BDT output](mva_BDT.png){ width=90% }

---

# TMVA: MVA output distributions

### Likelihood output
![Likelihood output](mva_Likelihood.png){ width=90% }

---

# TMVA: MVA output distributions

### Fisher output
![Fisher output](mva_Fisher.png){ width=90% }

---

# TMVA: method comparison (S/√B)

### Integrated S/√B per method (from `./mva_comparison` output)

| Method | Signal integral | Background integral | S/√B |
|--------|----------------|---------------------|------|
| BDT | -- | -- | -- |
| Likelihood | -- | -- | -- |
| Fisher | -- | -- | -- |

*(fill in from terminal output)*

### Interpretation
- Higher S/√B → better expected sensitivity
- BDT expected to outperform the linear methods (Fisher, Likelihood)
- Likelihood useful as a cross-check given its simpler assumptions

---

# Combine: BDT shape analysis (previous result)

### Datacard: `mutau_bdt_shape.txt`
Uses `bdt_data`, `bdt_sinal`, `bdt_dy`, `bdt_ttjets`, `bdt_qcd` from `MuTau_shapes.root`.

### Results: 95% CL upper limits on $r = \sigma / \sigma_\text{SM}$

| Estimate | Limit on $r$ |
|----------|-------------|
| Expected $-2\sigma$ | 3156 |
| Expected $-1\sigma$ | 3987 |
| **Expected (median)** | **4000** |
| Expected $+1\sigma$ | 8050 |
| Expected $+2\sigma$ | 12131 |
| **Observed** | **13891** |

---

# Combine: Likelihood shape analysis (new)

### Datacard: `mutau_likelihood_shape.txt`
Uses `lk_data`, `lk_sinal`, `lk_dy`, `lk_ttjets`, `lk_qcd` from `MuTau_shapes.root`.

### Results: 95% CL upper limits on $r = \sigma / \sigma_\text{SM}$

| Estimate | Limit on $r$ |
|----------|-------------|
| Expected $-2\sigma$ | 2093.75 |
| Expected $-1\sigma$ | 3195.80 |
| **Expected (median)** | **4000.00** |
| Expected $+1\sigma$ | 7539.61 |
| Expected $+2\sigma$ | 10537.86 |
| **Observed** | **5225.67** |

### Additional tests
| Test | Result |
|------|--------|
| Significance | $0.044\sigma$ |
| GoodnessOfFit (saturated) | $t = 16.92$ |

---

# Combine: method comparison

### Expected median limit on $r$ at 95% CL

| Discriminant | Expected median $r$ | Observed $r$ |
|-------------|---------------------|-------------|
| BDT | 4000 | 13891 |
| Likelihood | -- | -- |
| Fisher | -- | -- |

### Comments
- The large values of $r$ reflect the current low sensitivity: the analysis excludes $\sigma \sim 4000 \times \sigma_\text{SM}$ at 95% CL
- Sensitivity is limited by statistics and large backgrounds (QCD, DY)
- The BDT is expected to give the tightest limit; Likelihood and Fisher serve as cross-checks
- Improving sensitivity requires: tighter kinematic cuts, more data, or BSM signal hypotheses

---

# Summary & next steps

### What was done
- Extended TMVA training from BDT-only to **BDT + Likelihood + Fisher**
- Applied all three methods to signal, DY, ttbar, QCD, and data samples
- Produced shapes for Combine and ran AsymptoticLimits for BDT and Likelihood
- Implemented overtraining checks (KS test), data/MC ratio panels, and S/√B plots

### SM signal reference
$$\sigma_\text{SM}(\gamma\gamma \to \tau^+\tau^-, \mu\tau) \approx 1.34\ \text{fb}, \quad \mathcal{L} = 54.9\ \text{fb}^{-1}$$

### Next steps
- [ ] Fill in KS test p-values and S/√B table from `./mva_comparison` output
- [ ] Run Combine for Likelihood and Fisher channels
- [ ] Compare limits across all three methods
- [ ] Consider BSM signal hypotheses (scalar masses 400–1400 GeV)
- [ ] Add post-fit plots once FitDiagnostics is run
