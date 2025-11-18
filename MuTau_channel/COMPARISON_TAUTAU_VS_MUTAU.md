# Comparison: TauTau vs MuTau Channel Processing

## Key Finding: TauTau Does NOT Have This Problem

The factor of 2 issue in MuTau channel **does not exist** in TauTau channel because TauTau uses **C++ processing** that applies selections directly on the tree without the intermediate proton variable definitions that cause the MuTau inconsistency.

---

## Processing Architecture Comparison

### TauTau Channel (C++)

**Phase 0:** Does NOT exist - works directly from NanoAOD
**Phase 1:** C++ code (`nanotry_fase1_*.cpp`) reads NanoAOD directly

```cpp
// Data: nanotry_fase1_Dados.cpp line 142
if(tree->GetLeaf("tau0_charge")->GetValue(0) *
   tree->GetLeaf("tau1_charge")->GetValue(0) < 0.) {
    // Opposite sign - NO PROTON FILTER HERE
}

// QCD: nanotry_fase1_QCD.cpp line 142
if(tree->GetLeaf("tau0_charge")->GetValue(0) *
   tree->GetLeaf("tau1_charge")->GetValue(0) > 0.) {
    // Same sign - NO PROTON FILTER HERE
}
```

**Plotting:** C++ code (`plot_m.cpp`) applies weights:
```cpp
// Line 238: Data weight
w_data = 1;

// Line 258: QCD weight
w_qcd = 1;
```

**Result:** Both use `weight = 1.0` - no factor of 2 needed!

---

### MuTau Channel (Python)

**Phase 0:** Python (`fase0_qcd_data.py`) creates derived proton variables:
```python
.Define("pps_has_both_arms", "int(n_protons_arm0 > 0 && n_protons_arm1 > 0)")
.Define("xi_arm1_1", "proton_first_xi(...)")
.Define("xi_arm2_1", "proton_first_xi(...)")
```

**Phase 1:** Python (`fase1_*.py`) applies **different** proton filters:
```python
# Data: fase1_data.py line 135 + 138
.Filter("mu_charge * tau_charge < 0", "Opposite sign")
.Filter("pps_has_both_arms == 1", "Proton requirement")

# QCD: fase1_qcd.py line 137 + 140
.Filter("mu_charge * tau_charge > 0", "Same sign")
.Filter("xi_arm1_1 >= 0 && xi_arm2_1 >= 0", "Proton requirement")
```

**Plotting:** Python (`plot_m.py`) compensates with weights:
```python
# Line 258: QCD weight
w_qcd = 2.0

# Line 361: Data weight
weight = 2.0
```

**Result:** Factor of 2 needed to compensate for inconsistent proton selection!

---

## Why TauTau Doesn't Have This Problem

### 1. **No Intermediate Proton Variables**

TauTau Phase 1 does NOT create `pps_has_both_arms` or separate `xi_arm*` variables. It reads directly from NanoAOD's `Proton_multiRP_*` branches when needed.

### 2. **Proton Filtering Done in Plotting Code**

In TauTau, proton requirements are applied **during plotting** (plot_m.cpp), not during Phase 1 processing. This means:
- Data and QCD go through **identical** Phase 1 selection
- Only difference is charge sign (by design)
- No opportunity for inconsistent proton filters

### 3. **C++ vs Python Architecture**

**TauTau (C++):**
```
NanoAOD → Phase 1 (C++) → Merged Files → plot_m.cpp
          ↑
          Same cuts for Data/QCD except charge
```

**MuTau (Python):**
```
NanoAOD → Phase 0 (Python) → Phase 1 (Python) → Merged Files → plot_m.py
          ↑                    ↑
          Creates pps vars    Different proton filters! ✗
```

---

## Detailed Weight Comparison

### TauTau plot_m.cpp

```cpp
// Line 237-238: Data
w_data = tree_data->GetLeaf("weight")->GetValue(0);
w_data = 1;  // Override to 1.0

// Line 257-258: QCD
w_qcd = tree_qcd->GetLeaf("weight")->GetValue(0);
w_qcd = 1;  // Override to 1.0

// Line 296-297: DY
w_dy = tree_dy->GetLeaf("weight")->GetValue(0);
w_dy = 1.81 * 0.13;  // Apply physics weight

// Line 333-334: ttjets
w_ttjets = tree_ttjets->GetLeaf("weight")->GetValue(0);
w_ttjets = 0.15 * 0.13;  // Apply physics weight
```

**Key observation:** Data and QCD both use `w = 1.0`. The 0.13 factor is **not applied** to them - only to MC samples (DY, ttjets).

---

### MuTau plot_m.py (Current)

```python
# Line 258: QCD
w_qcd = 2.0  # ← Compensating for proton selection difference

# Line 361: Data
weight = 2.0  # ← Compensating for proton selection difference

# Lines 287-289: DY
w_dy = get_value(event, "weight", default=1.0)
w_dy = 1.81  # WITHOUT 0.13 factor

# Lines 314-315: ttjets
w_ttjets = get_value(event, "weight", default=1.0)
w_ttjets = 0.15  # WITHOUT 0.13 factor
```

**Key observations:**
1. MuTau uses `2.0` for Data/QCD (bug compensation)
2. MuTau does NOT multiply by 0.13 for DY/ttjets (you followed my advice to leave it out)

---

## The 0.13 Factor Mystery

Looking at TauTau:
```cpp
double w_dy = 1.81 * 0.13;      // = 0.2353
double w_ttjets = 0.15 * 0.13;  // = 0.0195
```

This 0.13 factor is the **PPS acceptance/efficiency** mentioned in [merge_pp_mutau.py:16](../MuTau_channel/merge_pp_mutau.py#L16):
```python
FIXED_WEIGHT = 0.13  # related to probabilities on the pps
```

**Why TauTau applies it but MuTau doesn't:**
- TauTau MC samples don't have proton information → need to simulate it → apply 0.13
- MuTau uses `merge_pp_mutau.py` which **already embeds** the 0.13 into the tree's weight branch
- So MuTau should NOT apply 0.13 again (would be double-counting)

**Conclusion:** You were right to leave out 0.13 in MuTau!

---

## Selection Cuts Comparison

| Cut | TauTau Data | TauTau QCD | MuTau Data | MuTau QCD |
|-----|-------------|------------|------------|-----------|
| **ID thresholds** | tau_id1 > 64<br>tau_id2 > 8<br>tau_id3 > 2 | tau_id1 > 64<br>tau_id2 > 8<br>tau_id3 > 2 | mu_id >= 3<br>tau_id1 > 63<br>tau_id2 > 7<br>tau_id3 > 1 | mu_id >= 3<br>tau_id1 > 63<br>tau_id2 > 7<br>tau_id3 > 1 |
| **pT thresholds** | tau0_pt > 100<br>tau1_pt > 100 | tau0_pt > 100<br>tau1_pt > 100 | mu_pt > 35<br>tau_pt > 100 | mu_pt > 35<br>tau_pt > 100 |
| **Charge** | Opposite sign < 0 | Same sign > 0 | Opposite sign < 0 | Same sign > 0 |
| **Geometry** | abs(eta) < 2.4 | abs(eta) < 2.4 | abs(eta) < 2.4 | abs(eta) < 2.4 |
| **ΔR** | > 0.4 | > 0.4 | > 0.4 | > 0.4 |
| **Protons** | ❌ None (applied later) | ❌ None (applied later) | ✅ pps_has_both_arms == 1 | ✅ xi_arm1_1/2_1 >= 0 |

**Critical difference:** TauTau applies proton cuts during plotting, MuTau applies them during Phase 1 with **inconsistent criteria**.

---

## How MuTau Should Work (Following TauTau Model)

### Option 1: Remove Proton Filters from Phase 1 (Best Match to TauTau)

**Change fase1_data.py and fase1_qcd.py:**
```python
# Remove these lines:
# .Filter("pps_has_both_arms == 1", "...")
# .Filter("xi_arm1_1 >= 0 && xi_arm2_1 >= 0", "...")
```

**Add proton filtering in plot_m.py:**
```python
for event in tree_data:
    mass = get_value(event, "sist_mass", default=-1.0)
    if mass < 0:
        continue

    # Add proton check here
    xi1 = get_value(event, "xi_arm1_1")
    xi2 = get_value(event, "xi_arm2_1")
    if xi1 < 0 or xi2 < 0:
        continue

    weight = 1.0  # No compensation needed!
    # ... fill histograms
```

**Pros:**
- Matches TauTau architecture exactly
- Maximum flexibility (can change proton cuts without reprocessing)
- No factor of 2 compensation

**Cons:**
- Larger intermediate files (more events)
- Slower plotting (more events to loop over)

---

### Option 2: Make Phase 1 Filters Consistent (Minimal Change)

**Change fase1_data.py line 138:**
```python
# From:
.Filter("pps_has_both_arms == 1", "...")
# To:
.Filter("xi_arm1_1 >= 0 && xi_arm2_1 >= 0", "...")
```

**Update plot_m.py:**
```python
w_qcd = 1.0   # Line 258
weight = 1.0  # Line 361
```

**Pros:**
- Minimal code changes
- Smaller files (proton filtering already done)
- No factor of 2 compensation

**Cons:**
- Still different from TauTau architecture
- Less flexible (need to reprocess to change proton cuts)

---

## Summary: Why the Difference Exists

| Aspect | TauTau | MuTau | Reason for Difference |
|--------|--------|-------|----------------------|
| **Language** | C++ | Python | Historical / developer preference |
| **Architecture** | 1-stage (Phase1→Plot) | 2-stage (Phase0→Phase1→Plot) | MuTau adds proton variables in Phase 0 |
| **Proton filtering** | In plotting code | In Phase 1 | Different design choices |
| **Data/QCD consistency** | ✅ Identical except charge | ❌ Different proton filters | Bug in MuTau implementation |
| **Weights** | Both = 1.0 | Both = 2.0 | MuTau compensating for bug |
| **0.13 factor** | Applied to MC | Not applied | MuTau embeds it during Phase 0 merge |

---

## Recommendation

**Follow the TauTau model:** Remove proton filtering from Phase 1 and apply it consistently during plotting. This:
1. Eliminates the factor of 2 compensation
2. Makes Data and QCD processing identical (except charge)
3. Provides flexibility to adjust proton selection without reprocessing
4. Aligns MuTau with the proven TauTau architecture

However, this requires:
- Reprocessing Data and QCD from Phase 1 (or using Phase 0 output)
- Updating plot_m.py to add proton filtering
- More CPU time during plotting (but more flexibility)

If reprocessing is not feasible right now, use **Option 2** as a short-term fix.
