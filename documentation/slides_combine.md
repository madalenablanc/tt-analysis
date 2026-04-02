---
title: "MuTau channel: Combine tool"
date: "26/03/2026"
theme: "Madrid"
colortheme: "default"
fontsize: 10pt
aspectratio: 169
---

# Overview

### Goal
Set a 95% CL upper limit on the signal strength $r = \sigma_\text{obs} / \sigma_\text{SM}$ for the exclusive $\gamma\gamma \to \mu\tau$ process using the CMS Combine tool.

### Full pipeline
```
save_shapes.cpp      mutau_mass_shape.txt        combine
(shape histograms) --> (datacard + workspace) --> (AsymptoticLimits)
        |                                               |
MuTau_shapes.root                    higgsCombineMuTau.AsymptoticLimits.mH120.root
                                                        |
                                          plot_single_limit.py
                                                        |
                                          MuTau_limits.png / limits.json
```

---

# Step 1: save_shapes.cpp


Produces `MuTau_shapes.root` -- the input ROOT file with per-sample invariant mass histograms that Combine reads as shapes.

### Input samples
| Sample | File |
|--------|------|
| Data | `Data_2018_UL_MuTau_nano_merged_proton_vars.root` |
| DY | `DY_2018_UL_MuTau_nano_merged_pileup_protons.root` |
| ttbar | `ttjets_2018_UL_MuTau_nano_merged_pileup_protons.root` |
| QCD | `QCD_2018_UL_MuTau_nano_merged_proton_vars_new.root` |
| Signal | `MuTau_sinal_SM_2018_july.root` |

---

# Step 1: save_shapes.cpp

### What it does
1. Loops over all events; requires `sist_mass > 0`
2. For MC: requires protons on both arms (`xi1 > 0 && xi2 > 0`)
3. Fills invariant mass histogram `m_X` for each sample (30 bins, 0-1200 GeV)
4. Also fills 7 other kinematic histograms per sample (`acop`, `sist_pt`, `sist_rap`, `tau_pt`, `met_pt`, mass matching, rapidity matching)
5. Applies MC normalization after the loop:
   - DY: `Scale(1.004e-4)` = $L \times \sigma / \sum w$
   - ttbar: `Scale(1.0)` (already normalized via 0.15 factor in Phase1)
   - Data, QCD: weight = 1.0 (data-driven)

### Output
- `MuTau_shapes.root` containing `m_data`, `m_sinal`, `m_dy`, `m_ttjets`, `m_qcd`

---

# Step 2: datacard (mutau_mass_shape.txt)

### Channel and process structure
```
imax 1   # 1 channel: mutau
jmax 3   # 3 backgrounds: DY, ttbar, QCD
kmax *   # all nuisance parameters
```

### Shape mapping (all from `../MuTau_shapes.root`)
| Process | Histogram | Index |
|---------|-----------|-------|
| `data_obs` | `m_data` | -- |
| `sinal` | `m_sinal` | 0 (signal) |
| `dy` | `m_dy` | 1 |
| `ttjets` | `m_ttjets` | 2 |
| `qcd` | `m_qcd` | 3 |

All rates set to `-1` -> Combine reads normalization directly from histogram integrals.

---

# Step 2: systematic uncertainties

### Lognormal (lnN) uncertainties
| Source | Signal | DY | ttbar | QCD |
|--------|--------|----|-------|-----|
| Luminosity | 1.5% | 1.5% | 1.5% | -- |
| tau ID vs jet | 5.0% | 4.0% | 4.0% | -- |
| tau ID vs e | 2.5% | 2.5% | 2.5% | -- |
| tau ID vs mu | 3.1% | 3.2% | 2.6% | -- |
| Muon trigger | 0.5% | 0.07% | 0.07% | -- |
| Muon ID+Iso | 0.02% | 0.04% | 0.04% | -- |
| Muon reco | 0.06% | 0.05% | 0.05% | -- |
| Proton xi | 50% | 50% | 50% | 50% |

### Floating normalizations (rateParam, range [0, 5])
- `beta_dy` for DY, constrained by `dy_norm lnN 1.30`
- `beta_tt` for ttbar, constrained by `ttjets_norm lnN 1.50`
- `beta_qcd` for QCD, constrained by `qcd_norm lnN 1.50`

### MC statistical uncertainty
- `autoMCStats 10` -- Barlow-Beeston-lite per bin

---

# Step 3: running 

### Commands
```bash
# Convert datacard to RooWorkspace
text2workspace.py datacards/mutau_mass_shape.txt \
    -o datacards/mutau_mass_shape_workspace.root \
    -m 120

# Compute asymptotic limits at 95% CL
combine -M AsymptoticLimits datacards/mutau_mass_shape_workspace.root \
    -m 120 -n MuTau --cl 0.95
```

### Output file
`higgsCombineMuTau.AsymptoticLimits.mH120.root`



| `quantileExpected` | Meaning |
|--------------------|---------|
| -1 | Observed |
| 0.5 | Expected median |
| 0.16 / 0.84 | Expected $\pm 1\sigma$ |
| 0.025 / 0.975 | Expected $\pm 2\sigma$ |

---

# Step 4: plot_single_limit.py

### purpose
Reads the output and produces a summary text file and limit plot.

### sage
```bash
python3 datacards/plot_single_limit.py \
    higgsCombineMuTau.AsymptoticLimits.mH120.root \
    --mass 120 --output datacards/MuTau_limits
```

### what it does
1. Reads quantiles from the `limit` tree
2. Writes `MuTau_limits.txt` -- plain-text table of all six values
3. Draws a TMultiGraph:
   - Yellow band: $\pm 2\sigma$ expected
   - Green band: $\pm 1\sigma$ expected
   - Dashed black line: expected median
   - Black point: observed
4. Saves as `MuTau_limits.png` and `MuTau_limits.pdf`

---

# Results: 95% CL upper limits on $r$

### method: AsymptoticLimits, mH = 120 GeV

| Estimate | Limit on $r = \sigma / \sigma_\text{SM}$ |
|----------|------------------------------------------|
| Expected $-2\sigma$ | 3156 |
| Expected $-1\sigma$ | 3987 |
| **Expected (median)** | **4000** |
| Expected $+1\sigma$ | 8050 |
| Expected $+2\sigma$ | 12131 |
| **Observed** | **13891** |

The large values of $r$ indicate the analysis is not yet sensitive to the SM signal -- the current selection and statistics allow excluding signal cross sections $\sim 4000\times$ the SM prediction at 95% CL.

---

# Results: limit plot

![MuTau limits](datacards/MuTau_limits.png)

