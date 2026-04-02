---
title: "BDT training and application"
date: "07/03/2026"
theme: "Madrid"
colortheme: "default"
fontsize: 10pt
aspectratio: 169
---

# Overview


### Samples Used
| Sample | Description | Source |
|--------|-------------|--------|
| **Data** | 2018 UL SingleMuon | Real collision data |
| **DY** | Drell-Yan Z/gamma* to tau-tau | MC simulation |
| **ttbar** | tt to tau-tau + X | MC simulation |
| **QCD** | Multi-jet background | Data-driven (same-sign) |

---

# Processing Pipeline

```
NanoAOD --> Phase0 --> Phase1 --> Merge --> Proton Mixing --> Plots
```

### Scripts

| Stage | Data | DY | ttbar | QCD |
|-------|------|-----|-------|-----|
| Phase0 | fase0_data_mutau.py | fase0_dy.py | fase0.py | - |
| Phase1 | fase1_data.py | fase1_dy.py | fase1_ttjets.py | fase1_qcd.py |


---

# Phase1: Selection cuts

| Variable | Cut | Description |
|----------|-----|-------------|
| `muon_id` | > 3 | Medium MVA ID |
| `tau_id1` (VSjet) | > 63 | VeryTight DeepTau vs jets |
| `tau_id2` (VSe) | > 7 | Tight DeepTau vs electrons |
| `tau_id3` (VSmu) | > 1 | Loose DeepTau vs muons |
| `muon_pt` | > 35 GeV | muon transverse momentum |
| `tau_pt` | > 100 GeV | tau transverse momentum |
| charge product | < 0 | opposite sign |
| Delta R | > 0.4 | Angular separation |

**Note**: QCD uses **same-sign** (SS) selection

---


# Where event weights are applied (cont)

### ttbar
```
Phase1: event_weight = 0.15 x muon_SFs  <- 0.15 included here
Proton Mixing: weight = event_weight x 0.13
Plotting: final_weight = weight x 1.0
```

### QCD (data-driven)
```
Phase1: weight=1.0 -> Plotting: final_weight=1.0
```

---

# Cross-Section normalization factors

### Formula used
```
weight = N_MC / N_expected
where N_expected = Luminosity x Cross-section
```

### Values used
| Sample | Factor | Included Where? |
|--------|--------|-----------------|
| **DY** | 1.81 | Applied in plotting |
| **ttbar** | 0.15 | Already in Phase 1 event_weight |

This does not change the outcome but should be fixed for consistency



---

# Single Lepton Plots: Cuts Applied

### Proton requirements
| Sample | Cut |
|--------|-----|
| Data/QCD | `Sum$(proton_multi_arm==0)>0 && Sum$(proton_multi_arm==1)>0` |
| MC (DY, ttbar) | `xi_arm1_1 >= 0 && xi_arm2_1 >= 0` |
| Signal | `xi_arm1_1 >= 0 && xi_arm2_1 >= 0` |

---

#  Weights applied

### Weights applied
| Sample | Weight |
|--------|--------|
| Data | None |
| QCD | None |
| DY | Proton acceptance (0.245) - xsec normalization already included |
| ttbar | Proton acceptance (0.245) x Scale(0.15) |
| Signal | `weight` branch (lumi x xsec x SFs x rad. damage) x 5000 (visualization) |

---

# Weight fix: old vs new approach

### Old weights (`MuTauPlots_weight_comparison.C`)
- MC filled with **flat** weight = `0.245` (proton acceptance, no `weight` branch)
- Then `Scale(1.81)` for DY, `Scale(0.15)` for ttbar
- No generator\_weight, no muon scale factors

### New weights (corrected)
- MC filled using **`weight` branch** = `generator_weight x muon_SFs x 0.245`
- Then `Scale(1.004e-4)` for DY (= L x sigma / Sum\_w), `Scale(1.0)` for ttbar

| | Old | New |
|--|-----|-----|
| **DY weight** | `0.245 x Scale(1.81)` | `weight branch x Scale(1.004e-4)` |
| **ttbar weight** | `0.245 x Scale(0.15)` | `weight branch x Scale(1.0)` |
| **DY Scale** | 1.81 (empirical) | 1.004e-4 (= 54900 x 6077.22 / 3.323e12) |
| **ttbar Scale** | 0.15 (empirical) | 1.0 (already ~correct via 0.15 in Phase1) |

---


# Muon pT

![Muon pT](plot_muon_pt.png)

---

# Muon eta

![Muon eta](plot_muon_eta.png)

---

# Muon mass

![Muon mass](plot_muon_mass.png)

---

# Tau pT

![Tau pT](plot_tau_pt.png)

---

# Tau eta

![Tau eta](plot_tau_eta.png)

---

# Tau mass

![Tau mass](plot_tau_mass.png)

---

# Single Distributions After Proton Merging

The following plots show individual kinematic distributions after requiring protons on both PPS arms.

### Variables shown
| Variable | Description |
|----------|-------------|
| `sist_mass` | Invariant mass m(mu,tau) |
| `acop` | Acoplanarity |
| `sist_pt` | System transverse momentum |
| `sist_rap` | System rapidity |
| `tau_pt` | Tau transverse momentum |
| `met_pt` | Missing transverse energy |

---

#  Invariant Mass (after proton requirements)

![Mass after](plot_mass_after.png)

---

#  Acoplanarity (after proton requirements)

![Acop after](plot_acop_after.png)

---

#  System pT (after proton requirements)

![pT after](plot_pt_after.png)

---

# System Rapidity (after proton requirements)

![Rapidity after](plot_rap_after.png)

---

#  Tau pT (after proton requirements)

![Tau pT after](plot_tau_pt_after.png)

---

# MET (after proton requirements)

![MET after](plot_met_after.png)

---

# 33. BDT summary: workflow

### Pipeline
```
corrs_matrix.py --> TMVAClassification.C --> TMVAClassificationApplication.C
(feature QA)        (training)               (evaluation)
```

### Files
| File | Purpose |
|------|---------|
| `corrs_matrix.py` | Correlation heatmaps from `variables2.txt` to select features |
| `TMVAClassification.C` | TMVA factory: train BDT + Likelihood + Fisher + MLP + PDERS |
| `TMVAClassificationApplication.C` | TMVA reader: apply trained weights, produce MVA score histograms |
| `dataset/weights/*.weights.xml` | Serialized trained models |

---

#  BDT summary: training inputs and variables

### Input samples
| Role | File | Tree |
|------|------|------|
| Signal | `MuTau_sinal_SM_2018_july.root` | `tree` |
| Background | `background_total-protons_syst.root` | `tree` |

### Event weights in TMVA
```cpp
dataloader->SetSignalWeightExpression("weight*weights_bsm_sf[51]");
dataloader->SetBackgroundWeightExpression("weight");
// Global scale factors:
double signalWeight = 0.00021;   double backgroundWeight = 104.;
```
---

### 9 training variables

| Variable | Description |
|----------|-------------|
| `sist_rap` | System rapidity |
| `acop` | System acoplanarity |
| `sist_pt` | System transverse momentum |
| `mu_pt`, `tau_pt` | Lepton transverse momenta |
| `sist_mass` | Invariant mass m(mu,tau) |
| `met_pt` | Missing transverse energy |
| `sist_mass - sqrt(13000^2 * xi_arm1_1 * xi_arm2_1)` | Mass matching (central vs proton) |
| `sist_rap - 0.5*log(xi_arm1_1/xi_arm2_1)` | Rapidity matching (central vs proton) |

The last two compare the central mu+tau system with the forward proton kinematics. For true exclusive events they peak at zero.

---

# BDT results: using simple classificator

Only BDT with (AdaBoost) is enabled with the default hyperparameters

TMVAClassification --> run_all_bdt.sh --> bdt_output.cpp

trains classificator, applies to each sample the results and produces corresponding histograms, plots the histogram of BDT results

---

# BDT results: using simple classificator
![](bdt_output.png)

---

#  BDT summary: configuration and output

### BDT (AdaBoost) hyperparameters
```
NTrees=850, MaxDepth=3, MinNodeSize=2.5%, AdaBoostBeta=0.5,
UseBaggedBoost, BaggedSampleFraction=0.5, GiniIndex, nCuts=20
```

### Train/test split
- 6500 signal + 15000 background for training, rest for testing
- Random split, `NormMode=NumEvents`
- No additional preselection cuts inside TMVA

---

# TMVA methods used

5 classifiers are enabled in `TMVAClassification.C` by default:

| Method | Type | Key idea |
|--------|------|----------|
| **BDT** | Boosted Decision Tree | Ensemble of 850 shallow trees (depth 3), AdaBoost, GiniIndex splitting |
| **Likelihood** | Naive Bayes | Per-variable PDF estimation (Spline2 interpolation), assumes uncorrelated inputs |
| **Fisher** | Linear discriminant | Optimal linear combination of variables maximizing S/B separation |
| **MLP** | Neural network | Multilayer perceptron with tanh activation, hidden layer of N+5 neurons, 600 training cycles |
| **PDERS** | Probability density | Adaptive range search in multi-dim space, Gaussian kernel estimator |

All five produce per-event MVA scores stored in `TMVApp_sinal.root`.

---

### Application (TMVAClassificationApplication.C)
- Reads trained weights from `dataset/weights/`
- Computes derived matching variables in the event loop from `xi_arm1_1`, `xi_arm2_1`
- Per-event `weight` branch used as tree weight
- BDT score histogram: 20 bins in [-0.8, 0.8]
- Output: `TMVApp_sinal.root` (contains `MVA_BDT`, `MVA_Likelihood`, `MVA_Fisher`, `MVA_MLP`, `MVA_PDERS`)

---

# BDT code files: TMVAClassification.C


TMVA factory script that trains the BDT classifier (and optionally other MVA methods). This is the core training step.

### Input
- Signal: `MuTau_sinal_SM_2018_july.root` (tree: `tree`)
- Background: `background_total-protons_syst.root` (tree: `tree`)

1. Registers 9 training variables (kinematic + proton matching)
2. Sets per-event weight expressions:
   - Signal: `weight * weights_bsm_sf[51]`, global scale = 0.00021
   - Background: `weight`, global scale = 104.0
3. Prepares train/test split: 6500 signal + 15000 background for training, random split
4. Books the BDT method with AdaBoost (NTrees=850, MaxDepth=3, GiniIndex, nCuts=20)
5. Calls `TrainAllMethods()`, `TestAllMethods()`, `EvaluateAllMethods()`

### Output
- `TMVA_allBkg_Mutau_2018.root` - TMVA output with training/testing histograms, ROC curves
- `dataset/weights/TMVAClassification_BDT.weights.xml` - Serialized trained BDT model

---

# BDT code files: TMVAClassificationApplication.C


Applies the trained BDT model to the **signal** sample, producing MVA score histograms for evaluation.

### Input
- ROOT file: `MuTau_sinal_SM_2018_july.root` (tree: `tree`)
- Trained weights: `dataset/weights/TMVAClassification_BDT.weights.xml`


1. Creates a TMVA Reader and registers the same 9 variables used in training
2. Reads branches: `acop`, `sist_pt`, `sist_mass`, `sist_rap`, `met_pt`, `xi_arm1_1`, `xi_arm2_1`, `weight`, `tau_pt`, `mu_pt`
3. For each event, computes the derived matching variables:
   - Mass matching: `sist_mass - sqrt(13000^2 * xi1 * xi2)`
   - Rapidity matching: `sist_rap - 0.5 * log(xi1/xi2)`
4. Skips events without valid protons on both arms (`xi <= 0`)
5. Evaluates BDT score via `reader->EvaluateMVA("BDT method")`
6. Fills BDT score histogram (20 bins, [-0.8, 0.8])

### Output
- `TMVApp_sinal.root` - Contains `MVA_BDT` histogram for signal

---

#  BDT code files: run_bdt_all.C


Standalone BDT application script that can be run on **any** sample type (signal, DY, ttbar, QCD, data). Unlike `TMVAClassificationApplication.C`, this handles the different branch naming and structure across sample types.

### Input
- Three arguments: `inputFile`, `outputFile`, `sampleType`
- Sample types: `"signal"`, `"dy"`, `"ttbar"`, `"qcd"`, `"data"`
- Trained weights: `dataset/weights/TMVAClassification_BDT.weights.xml`


1. Creates a TMVA Reader with the same 9 training variables
2. Adapts to sample-specific branch differences:
   - Data/QCD: muon pT branch = `muon_pt`, proton xi from vectors (`proton_multi_arm`, `proton_multi_xi`)
   - MC (signal/DY/ttbar): muon pT branch = `mu_pt`, proton xi from scalars (`xi_arm1_1`, `xi_arm2_1`)
3. Loops over events, requires protons on both arms (`xi1 > 0 && xi2 > 0`)
4. Computes derived matching variables, evaluates BDT score
5. Fills weighted histogram `MVA_BDT` (20 bins, [-0.8, 0.8]) using the `weight` branch

### Output
- Single ROOT file with `MVA_BDT` histogram (name given by `outputFile` argument)

---

#  BDT code files: run_bdt_all.sh


### Input
| Sample | Input file |
|--------|-----------|
| Signal | `MuTau_sinal_SM_2018_july.root` (local) |
| DY | `/eos/home-m/mblancco/samples_2018_mutau/DY_2018_UL_MuTau_nano_merged_pileup_protons.root` |
| ttbar | `/eos/home-m/mblancco/samples_2018_mutau/ttjets_2018_UL_MuTau_nano_merged_pileup_protons.root` |
| QCD | `/eos/home-m/mblancco/samples_2018_mutau/QCD_2018_UL_MuTau_nano_merged_pileup_protons.root` |
| Data | `/eos/home-m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_proton_vars.root` |

### Output
| Output file | Content |
|------------|---------|
| `TMVApp_sinal.root` | Signal BDT scores |
| `TMVApp_DY.root` | DY BDT scores |
| `TMVApp_ttjets.root` | ttbar BDT scores |
| `TMVApp_QCD.root` | QCD BDT scores |
| `TMVApp_data.root` | Data BDT scores |

---

# BDT code files: bdt_output.cpp

Plotting script that creates a stacked histogram of BDT output scores, overlaying all backgrounds, signal, and data.

### Input
- `TMVApp_DY.root` - DY BDT scores
- `TMVApp_QCD.root` - QCD BDT scores
- `TMVApp_ttjets.root` - ttbar BDT scores
- `TMVApp_sinal.root` - Signal BDT scores
- `TMVApp_data.root` - Data BDT scores

### What it does
1. Opens all five `TMVApp_*.root` files and extracts the `MVA_BDT` histogram from each
2. Rebins all histograms by factor 4 (20 bins -> 5 bins)
3. Applies normalization: DY `Scale(1.004e-4)`, Signal `Scale(5000)` for visibility
4. Builds a THStack with colored backgrounds: DY (yellow), QCD (red), ttbar (green)
5. Overlays signal (black, line width 3) and data (black points with errors)
6. Draws total background uncertainty band (gray hatched)
7. Uses log scale on Y axis, range [0.01, 10000]

### Output
- `bdt_output.png` - Stacked BDT output distribution plot

---

# Combine tool: workflow overview

### Full pipeline to compute the 95% CL upper limit

```
save_shapes.cpp         mutau_mass_shape.txt         combine
(build histograms)  -->  (datacard)            -->  (AsymptoticLimits)
       |                                                    |
MuTau_shapes.root                              higgsCombineMuTau.AsymptoticLimits.mH120.root
                                                            |
                                               plot_single_limit.py
                                                            |
                                               MuTau_limits.png / limits.json
```

### Commands run
```bash
# 1. Build shape histograms
g++ save_shapes.cpp -o save_shapes $(root-config --cflags --libs) && ./save_shapes

# 2. Convert datacard to workspace
text2workspace.py datacards/mutau_mass_shape.txt \
    -o datacards/mutau_mass_shape_workspace.root \
    -m 120

# 3. Run asymptotic limits
combine -M AsymptoticLimits datacards/mutau_mass_shape_workspace.root \
    -m 120 -n MuTau --cl 0.95

# 4. Plot result
python3 datacards/plot_single_limit.py \
    higgsCombineMuTau.AsymptoticLimits.mH120.root \
    --mass 120 --output datacards/MuTau_limits
```

---

#  Combine tool: save_shapes.cpp

### Purpose
Produces the input ROOT file (`MuTau_shapes.root`) containing the per-sample invariant mass histograms that Combine reads as shapes.

### Inputs
| Sample | File |
|--------|------|
| Data | `Data_2018_UL_MuTau_nano_merged_proton_vars.root` |
| DY | `DY_2018_UL_MuTau_nano_merged_pileup_protons.root` |
| ttbar | `ttjets_2018_UL_MuTau_nano_merged_pileup_protons.root` |
| QCD | `QCD_2018_UL_MuTau_nano_merged_proton_vars_new.root` |
| Signal | `MuTau_sinal_SM_2018_july.root` |

### What it does
1. Loops over all events in each sample
2. Requires `sist_mass > 0` and protons on both arms (`xi1 > 0 && xi2 > 0` for MC)
3. Fills histograms for 8 variables: `acop`, `sist_mass`, `sist_pt`, `sist_rap`, `tau_pt`, `met_pt`, rapidity matching, mass matching
4. Applies MC normalization scales after loops: DY `× 1.004e-4`, ttbar `× 1.0`
5. Data and QCD use weight = 1.0 (data-driven)

### Outputs
- `MuTau_shapes.root` — contains `m_data`, `m_sinal`, `m_dy`, `m_ttjets`, `m_qcd` (plus 7 other variables × 5 samples)

---

# Combine tool: datacard (mutau_mass_shape.txt)

### Structure
```
imax 1   # 1 channel: mutau
jmax 3   # 3 backgrounds: DY, ttbar, QCD
kmax *   # all nuisances
```

### Shape sources
All histograms read from `../MuTau_shapes.root`:

| Process | Histogram |
|---------|-----------|
| `data_obs` | `m_data` |
| `sinal` | `m_sinal` |
| `dy` | `m_dy` |
| `ttjets` | `m_ttjets` |
| `qcd` | `m_qcd` |

### Normalization
All rates set to `-1` → Combine reads them directly from histogram integrals.

---

# Combine tool: systematic uncertainties

### Lognormal (lnN) systematics

| Source | Signal | DY | ttbar | QCD |
|--------|--------|----|-------|-----|
| Luminosity | 1.5% | 1.5% | 1.5% | — |
| tau ID vs jet | 5.0% | 4.0% | 4.0% | — |
| tau ID vs e | 2.5% | 2.5% | 2.5% | — |
| tau ID vs mu | 3.1% | 3.2% | 2.6% | — |
| Muon trigger | 0.5% | 0.07% | 0.07% | — |
| Muon ID+Iso | 0.02% | 0.04% | 0.04% | — |
| Muon reco | 0.06% | 0.05% | 0.05% | — |
| Proton xi | 50% | 50% | 50% | 50% |

### Normalization floats (rateParam)
- `beta_dy`, `beta_tt`, `beta_qcd` — free floating in [0, 5], constrained by `dy_norm` (30%), `ttjets_norm` (50%), `qcd_norm` (50%)

### Statistical
- `autoMCStats 10` — Barlow-Beeston-lite for MC statistical uncertainties

---

# Combine tool: results

### Method: AsymptoticLimits at 95% CL, mH = 120 GeV

| Estimate | Limit on signal strength $r$ |
|----------|------------------------------|
| Expected −2σ | 3156 |
| Expected −1σ | 3987 |
| **Expected (median)** | **4000** |
| Expected +1σ | 8050 |
| Expected +2σ | 12131 |
| **Observed** | **13891** |

Signal strength $r$ is defined as $\sigma_\text{obs} / \sigma_\text{SM}$.
The large values indicate the analysis is not yet sensitive to the SM signal with current statistics and selection.

---

# Combine tool: limit plot

![MuTau limits](datacards/MuTau_limits.png)

---

# Combine tool: plot_single_limit.py

### Purpose
Reads the Combine output ROOT file and produces a summary text file and a ROOT-based limit plot.

### Input
- `higgsCombineMuTau.AsymptoticLimits.mH120.root` — Combine output tree `limit`
- `--mass 120` — x-axis mass value
- `--output MuTau_limits` — output file prefix

### What it does
1. Iterates over entries in the `limit` tree, identifies each quantile by `quantileExpected`:
   - `−1` → observed, `0.5` → median expected, `0.16/0.84` → ±1σ, `0.025/0.975` → ±2σ
2. Writes a plain-text summary table (`MuTau_limits.txt`)
3. Draws a TMultiGraph with yellow (±2σ) and green (±1σ) bands, dashed expected line, and observed point
4. Saves as PNG and PDF

### Outputs
- `MuTau_limits.txt` — plain-text table of all six limit values
- `MuTau_limits.png` / `MuTau_limits.pdf` — limit plot