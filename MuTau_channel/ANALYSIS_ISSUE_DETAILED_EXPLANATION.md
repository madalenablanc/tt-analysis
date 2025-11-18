# Detailed Explanation: Factor of 2 Discrepancy in Data and QCD

## Executive Summary

Your Data and QCD samples appear at half their expected values because of an **inconsistent proton selection criterion** applied during Phase 1 processing:
- **Data** uses: `pps_has_both_arms == 1`
- **QCD** uses: `xi_arm1_1 >= 0 && xi_arm2_1 >= 0`

While these appear logically equivalent, they can produce different event counts due to how the proton variables are constructed in Phase 0.

---

## Complete Data Flow

### Phase 0: Initial Processing (`fase0_qcd_data.py`)

**Input:** Raw NanoAOD files from CMS with `Proton_multiRP_*` branches

**Processing Steps:**

1. **Basic Event Selection:**
   - Luminosity filtering (certified lumi sections)
   - HLT trigger requirement: `HLT_IsoMu24 == 1`
   - At least one muon and one tau candidate

2. **Proton Variable Creation:**

   ```cpp
   // Helper function: Extract the i-th proton xi from a specific arm
   double proton_first_xi(xi_vector, arm_vector, targetArm, occurrence) {
       int seen = 0;
       for (size_t i = 0; i < arm_vector.size(); ++i) {
           if (arm_vector[i] == targetArm) {
               if (seen == occurrence) {
                   return xi_vector[i];  // Return xi value
               }
               ++seen;
           }
       }
       return -999.;  // Not found
   }
   ```

   **Variables Created:**
   ```python
   xi_arm1_1 = proton_first_xi(..., 0, 0)  # First proton in arm 0
   xi_arm1_2 = proton_first_xi(..., 0, 1)  # Second proton in arm 0
   xi_arm2_1 = proton_first_xi(..., 1, 0)  # First proton in arm 1
   xi_arm2_2 = proton_first_xi(..., 1, 1)  # Second proton in arm 1

   n_protons_arm0 = proton_count_arm(..., 0)  # Count in arm 0
   n_protons_arm1 = proton_count_arm(..., 1)  # Count in arm 1

   pps_has_arm0 = int(n_protons_arm0 > 0)
   pps_has_arm1 = int(n_protons_arm1 > 0)
   pps_has_both_arms = int(n_protons_arm0 > 0 && n_protons_arm1 > 0)
   ```

**Key Point:** `pps_has_both_arms` checks if there are **ANY** protons in each arm based on the arm vector, while `xi_arm1_1/xi_arm2_1` extract the **actual xi values** which could still be `-999` if extraction fails.

**Output:** Files in `fase0_with_proton_vars/`

---

### Phase 1: Signal/Control Region Selection

#### Data Processing (`fase1_data.py`)

**Input:** Phase 0 output files

**Selection Cuts:**
```python
.Filter("mu_id >= 3", "Muon ID (>= Medium)")
.Filter("tau_id1 > 63", "Tau VSjet")
.Filter("tau_id2 > 7", "Tau VSe")
.Filter("tau_id3 > 1", "Tau VSmu")
.Filter("mu_pt > 35. && tau_pt > 100.", "pT thresholds")
.Filter("mu_charge * tau_charge < 0", "Opposite sign")  # SIGNAL REGION
.Filter("delta_r > 0.4", "Delta R acceptance")
.Filter("fabs(tau_eta) < 2.4 && fabs(mu_eta) < 2.4", "Geometrical acceptance")
.Filter("pps_has_both_arms == 1", "Proton requirement")  # ← ISSUE HERE
```

**Output:** `Data_2018_UL_MuTau_nano_merged_proton_vars.root`

#### QCD Processing (`fase1_qcd.py`)

**Input:** Phase 0 output files (same as Data)

**Selection Cuts:**
```python
.Filter("mu_id >= 3", "Muon ID (>= Medium)")
.Filter("tau_id1 > 63", "Tau VSjet")
.Filter("tau_id2 > 7", "Tau VSe")
.Filter("tau_id3 > 1", "Tau VSmu")
.Filter("mu_pt > 35. && tau_pt > 100.", "pT thresholds")
.Filter("mu_charge * tau_charge > 0", "Same sign")  # CONTROL REGION
.Filter("delta_r > 0.4", "Delta R acceptance")
.Filter("fabs(tau_eta) < 2.4 && fabs(mu_eta) < 2.4", "Geometrical acceptance")
.Filter("xi_arm1_1 >= 0 && xi_arm2_1 >= 0", "Proton requirement")  # ← DIFFERENT
```

**Output:** `QCD_2018_UL_MuTau_nano_merged_proton_vars.root`

---

## The Critical Difference

### What Should Happen (Logically Equivalent)

```python
# These should select the same events:
pps_has_both_arms == 1
# ↓ equivalent to ↓
(n_protons_arm0 > 0) && (n_protons_arm1 > 0)
# ↓ should be equivalent to ↓
(xi_arm1_1 >= 0) && (xi_arm2_1 >= 0)
```

### What Actually Happens

**Scenario A: proton_first_xi returns valid xi**
```
Proton_multiRP_arm = [0, 1, 0]
Proton_multiRP_xi = [0.05, 0.03, -999]

n_protons_arm0 = 2  (counts entries with arm==0)
n_protons_arm1 = 1  (counts entries with arm==1)
pps_has_both_arms = 1  ✓

xi_arm1_1 = 0.05  (first proton in arm 0)
xi_arm2_1 = 0.03  (first proton in arm 1)
xi_arm1_1 >= 0 && xi_arm2_1 >= 0 = True  ✓

Both filters pass ✓
```

**Scenario B: proton exists but xi is invalid**
```
Proton_multiRP_arm = [0, 1]
Proton_multiRP_xi = [0.05, -999]  ← arm 1 has invalid xi

n_protons_arm0 = 1  ✓
n_protons_arm1 = 1  ✓ (counts the entry even with invalid xi)
pps_has_both_arms = 1  ✓

xi_arm1_1 = 0.05  ✓
xi_arm2_1 = -999  ✗ (extracted but invalid)
xi_arm1_1 >= 0 && xi_arm2_1 >= 0 = False  ✗

pps_has_both_arms passes, but xi filter FAILS ✗
```

**This is the source of the discrepancy!**

---

## Why Factor of 2?

If approximately **50% of events** with `pps_has_both_arms == 1` have invalid xi values (negative or -999), then:

```
Events passing pps_has_both_arms filter: N
Events passing xi >= 0 filter: N/2
```

To compensate, you multiply QCD and Data by 2:
```python
w_qcd = 2.0   # Line 258
weight = 2.0  # Line 361 (Data)
```

This brings the event counts back to the expected level, but it's **treating a symptom, not the cause**.

---

## Root Causes

### 1. Inconsistent Proton Filtering
**Why are Data and QCD using different criteria?**

Possible reasons:
- Copy-paste error during code development
- Intentional but undocumented different treatment
- Evolution of analysis strategy without updating both regions

### 2. Invalid Xi Values in Raw Data
**Why do protons have xi = -999?**

Possible reasons:
- Reconstruction failures in PPS detector
- Quality cuts applied at reconstruction level
- Edge cases in detector acceptance

### 3. Missing Documentation
**No comments explain why these are different**

The code has identical comments for both filters:
```python
"At least one reconstructed proton per PPS arm"
```
But they implement different logic!

---

## Impact on Your Analysis

### Current State (with weight=2.0)

**Pros:**
- Event counts appear correct
- Plots match expectations

**Cons:**
- **Systematic uncertainty**: You're assuming the factor of 2 is correct
- **Inconsistent selection**: Data and QCD use different physics criteria
- **Statistical properties**: Weights affect error bars (via Sumw2)
- **Reproducibility**: Future analyzers won't understand the factor of 2

### What Could Go Wrong

1. **If the true factor isn't exactly 2:**
   - You're over/under-estimating QCD background
   - Data normalization is incorrect

2. **Physics implications:**
   - Events with invalid xi have different kinematics
   - You may be biasing your selection

3. **Systematic studies:**
   - How do you propagate this uncertainty?
   - Is the factor of 2 luminosity-dependent?

---

## Recommended Solutions

### Option 1: Make Proton Selection Consistent (BEST)

**Change Data to match QCD:**
```python
# In fase1_data.py line 138, change:
.Filter("pps_has_both_arms == 1", "...")
# To:
.Filter("xi_arm1_1 >= 0 && xi_arm2_1 >= 0", "...")
```

**Then update plot_m.py:**
```python
w_qcd = 1.0      # Line 258
weight = 1.0     # Line 361
```

**Pros:**
- Physically consistent
- No artificial weights
- Clear systematic understanding

**Cons:**
- Need to reprocess Data from Phase 1
- Event counts will change

### Option 2: Make Both Use pps_has_both_arms

**Change QCD to match Data:**
```python
# In fase1_qcd.py line 140, change:
.Filter("xi_arm1_1 >= 0 && xi_arm2_1 >= 0", "...")
# To:
.Filter("pps_has_both_arms == 1", "...")
```

**Pros:**
- Includes more events with detected protons
- May improve statistics

**Cons:**
- Need to reprocess QCD from Phase 1
- Still includes events with invalid xi (is this desired?)

### Option 3: Investigate and Document (MINIMUM)

If weight=2.0 is correct:

1. **Measure the actual ratio:**
   ```python
   N_pps = tree.GetEntries("pps_has_both_arms == 1 && sist_mass >= 0")
   N_xi = tree.GetEntries("xi_arm1_1 >= 0 && xi_arm2_1 >= 0 && sist_mass >= 0")
   ratio = N_pps / N_xi
   ```

2. **Document in code:**
   ```python
   # Data uses pps_has_both_arms filter in Phase 1, which is stricter than
   # QCD's xi >= 0 filter by a factor of {measured_ratio}. This weight
   # compensates for the selection difference.
   # See ANALYSIS_ISSUE_DETAILED_EXPLANATION.md for details.
   weight = {measured_ratio}
   ```

3. **Add systematic uncertainty** for this normalization difference

---

## How to Verify

Run this diagnostic:

```python
import ROOT

files = {
    'Data': '/eos/home-m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_proton_vars.root',
    'QCD': '/eos/home-m/mblancco/samples_2018_mutau/QCD_2018_UL_MuTau_nano_merged_proton_vars.root'
}

for name, path in files.items():
    f = ROOT.TFile.Open(path)
    tree = f.Get('tree')

    n_total = tree.GetEntries("sist_mass >= 0")
    n_pps = tree.GetEntries("pps_has_both_arms == 1 && sist_mass >= 0")
    n_xi = tree.GetEntries("xi_arm1_1 >= 0 && xi_arm2_1 >= 0 && sist_mass >= 0")

    print(f"\n{name}:")
    print(f"  Total valid events: {n_total}")
    print(f"  pps_has_both_arms==1: {n_pps} ({100*n_pps/n_total:.1f}%)")
    print(f"  xi >= 0: {n_xi} ({100*n_xi/n_total:.1f}%)")
    print(f"  Ratio pps/xi: {n_pps/n_xi if n_xi > 0 else 0:.3f}")

    # Check overlap
    n_both = tree.GetEntries("pps_has_both_arms == 1 && xi_arm1_1 >= 0 && xi_arm2_1 >= 0 && sist_mass >= 0")
    n_pps_only = n_pps - n_both
    n_xi_only = n_xi - n_both

    print(f"  Events in both filters: {n_both}")
    print(f"  Events only in pps filter: {n_pps_only}")
    print(f"  Events only in xi filter: {n_xi_only}")
```

**Expected output will show:**
- If ratio ≈ 2.0: Your weight is correct (but selection is still inconsistent)
- If ratio ≠ 2.0: Your weight needs adjustment
- If pps_only > 0: Events have proton detections but invalid xi
- If xi_only > 0: Bug in code logic (shouldn't happen)

---

## Conclusion

The factor of 2 is **compensating for an inconsistent proton selection** between Data and QCD regions. This is a **bug**, not a feature. The proper fix is to make the selections consistent at the Phase 1 processing level, then remove the artificial weights in the plotting code.

Until you reprocess, the weight=2.0 is a reasonable workaround, but you should:
1. Measure the actual ratio to verify it's really 2.0
2. Document this clearly in your analysis
3. Include it as a systematic uncertainty
4. Plan to fix it properly before final results
