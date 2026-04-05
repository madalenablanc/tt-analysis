# MuTau Channel: Status Update

January 2026

---

# 1. Summary

This week I investigated a normalization issue identified by Jonathan in the MC samples with pileup proton mixing.

**Key findings:**
- Found and fixed a bug in the proton merging code
- Measured the proton acceptance from data: **P = 24.5%**
- Clarified the complete weight structure for all samples

---

# 2. The Proton Mixing Procedure

## Why we need it

- MC samples (DY, ttbar) have no PPS proton information
- Real data has pileup protons from ~30-40 collisions per bunch crossing
- We add fake pileup protons to MC to match data

## The proton pool

**File:** `proton_pool_2018.root`

- Contains real protons from 2018 PPS data
- Each proton has: arm (0 or 1) and xi (momentum loss)
- Created from data with signal triggers but no kinematic cuts
- Protons are essentially random pileup

## Assignment algorithm

For each MC event:

1. **Always assign 1 proton per arm** (xi_arm1_1, xi_arm2_1)

2. **Probabilistically add 2nd protons:**

| Config | Probability | Description |
|--------|-------------|-------------|
| P11 | 8.0% | 1+1 protons |
| P12 | 2.0% | 1+2 protons |
| P21 | 2.0% | 2+1 protons |
| P22 | 0.5% | 2+2 protons |

---

# 3. The Bug Found

## Original code (WRONG)

```python
# weight was set to fixed value, discarding event_weight
weight = 0.13
```

The `event_weight` from phase1 (containing generator weights and muon scale factors) was **lost**.

## Fixed code (CORRECT)

```python
# Now multiplies event_weight by proton acceptance
weight = event_weight * 0.13
```

---

# 4. Proton Acceptance Measurement

## What is it?

The fraction of events with at least 1 proton detected on **both** arms.

## My measurement

**Sample:** `Data_2018_UL_MuTau_nano_merged_proton_vars.root`

| Category | Events | Fraction |
|----------|--------|----------|
| No protons on either arm | 1,849 | 26.5% |
| Protons only on arm 0 | 1,600 | 23.0% |
| Protons only on arm 1 | 1,813 | 26.0% |
| **Protons on BOTH arms** | **1,708** | **24.5%** |

**Total events:** 6,970
**Measured acceptance:** P = 0.245 (24.5%)

## Comparison with previous value

- Matteo's value: **0.13 (13%)**
- My measurement: **0.245 (24.5%)**

The difference may be due to different event selection or data sample.

**Question:** Which value should we use?

---

# 5. Complete Weight Structure

## Data
```
weight = 1.0 (no corrections)
```

## Drell-Yan (DY)
```
Phase 1:     event_weight = generator_weight * muon_SFs
Proton mix:  weight = event_weight * 0.13
Plotting:    final = weight * 1.81
```

## ttbar
```
Phase 1:     event_weight = 0.15 * muon_SFs  (0.15 already included!)
Proton mix:  weight = event_weight * 0.13
Plotting:    final = weight * 1.0
```

## QCD
```
Data-driven from same-sign selection
weight = custom normalization
```

---

# 6. Cross-Section Normalization

| Sample | Factor | Formula | Where applied |
|--------|--------|---------|---------------|
| DY | 1.81 | N_MC / (L * sigma) | Plotting |
| ttbar | 0.15 | N_MC / (L * sigma) | Phase 1 |

---

# 7. Current Issues

| Issue | Status |
|-------|--------|
| Proton mixing overwrites weights | FIXED |
| event_weight lost after mixing | FIXED |
| Proton acceptance value (0.13 vs 0.245) | TO DISCUSS |
| DY normalization in plotting | TO CHECK |

---

# 8. Next Steps

1. **Clarify proton acceptance value** with Jonathan/Matteo

2. **Re-run proton mixing** for DY and ttbar with fixed code

3. **Verify plotting code** applies correct normalizations

4. **Generate new comparison plots**

5. **Cross-check results** with Jonathan

---

# 9. Questions for Discussion

1. Which proton acceptance should we use: 0.13 or 0.245?

2. What selection was used for Matteo's 0.13 measurement?

3. Should we re-measure from a different stage sample?

---

# Backup: Code for Measuring Acceptance

```cpp
// In ROOT
TFile* f = TFile::Open("Data_merged_proton_vars.root");
TTree* tree = (TTree*)f->Get("tree");

Long64_t total = tree->GetEntries();
Long64_t good = tree->GetEntries(
    "Sum$(proton_multi_arm==0)>=1 && Sum$(proton_multi_arm==1)>=1"
);

double P = (double)good / (double)total;
cout << "Acceptance P = " << P << endl;
```

---

# Backup: Files Used

| File | Description |
|------|-------------|
| `fase0_data_mutau.py` | Phase 0 processing |
| `fase1_data.py` | Phase 1 selection |
| `fase1_dy.py` | Phase 1 for DY |
| `fase1_ttjets.py` | Phase 1 for ttbar |
| `merge_pp_mutau.py` | Proton mixing |
| `plot_m.py` | Plotting |
| `check_proton_acceptance.py` | Acceptance measurement |
