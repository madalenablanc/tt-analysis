# Root Cause Analysis: Factor of 2 Discrepancy

## ⚠️ CRITICAL BUG FOUND!

**Diagnostic output reveals the real problem: Tau ID cuts are rejecting 99.95% of events!**

```
Starting with protons: 172,895 events
+ Muon ID: 138,903 (80% pass - normal ✓)
+ Tau ID: 65       (99.95% REJECTED! ✗)
```

**Root cause:** [fase0_qcd_data.py](fase0_qcd_data.py) has all tau quality cuts **commented out** (lines 177-182), so it saves taus with ANY quality (including tau_id1=0). Then [fase1_data.py:131-133](fase1_data.py#L131) applies strict cuts (`tau_id1 > 63`), rejecting nearly everything!

---

## Summary

Both Data and QCD samples appear at approximately **half their expected values**. This issue appeared when the processing workflow changed from **artificial pileup proton addition** (like DY/ttjets) to **using real proton variables from the detector**.

**UPDATE:** The diagnostic reveals the issue is NOT just the proton selection change - it's that **fase0 has no tau quality cuts**, causing massive rejection in fase1.

---

## The Processing Change

### Old Method (like DY/ttjets - [merge_pp_mutau.py](merge_pp_mutau.py))

**Workflow:**
```
Raw NanoAOD → Fase1 (physics cuts, NO proton filter) → Merge → Add pileup protons
```

**Key characteristics:**
1. **No fase0 step** - works directly from NanoAOD
2. **No proton requirements in fase1** - all events passing physics cuts are kept
3. **Artificial proton addition** via `merge_pp_mutau.py`:
   - Reads from proton pool: `/eos/cms/store/group/phys_smp/Exclusive_DiTau/proton_pool_2018/proton_pool_2018.root`
   - **Guarantees every event gets protons** in both arms (lines 180-192)
   - Uses probability-based selection (P11=0.08, P12=0.02, P21=0.02, P22=0.005)
   - Applies fixed weight = 0.13 (PPS acceptance correction, line 16)

**Result:** 100% of events passing physics cuts get artificial protons added

---

### New Method (current for Data/QCD)

**Workflow:**
```
Raw NanoAOD → Fase0 (save real proton vars) → Fase1 (physics cuts + proton filter) → Merge
```

**Key characteristics:**
1. **Fase0 ([fase0_qcd_data.py](fase0_qcd_data.py))** extracts real proton information:
   - Defines `pps_has_both_arms` (line 245): checks if `n_protons_arm0 > 0 && n_protons_arm1 > 0`
   - Defines `xi_arm1_1`, `xi_arm2_1` (lines 237-240): actual xi values from detector
   - Applies lumi filter and HLT trigger (lines 174-175)

2. **Fase1** applies proton requirements:
   - Data ([fase1_data.py:137](fase1_data.py#L137)): `.Filter("pps_has_both_arms == 1", ...)`
   - QCD ([fase1_qcd.py:140](fase1_qcd.py#L140)): `.Filter("xi_arm1_1 >= 0 && xi_arm2_1 >= 0", ...)`

3. **NO artificial proton addition** - uses real detector data

**Result:** Only ~16.9% of events have real protons in both PPS arms

---

## Why This Causes a Problem

### Expected Impact of the Change

If you switched from **100% artificial protons** to **16.9% real protons**, you'd expect:

```
Old method events: N
New method events: N × 0.169 ≈ N/6
```

This predicts a **factor of 6 reduction**, NOT a factor of 2!

---

## Possible Explanations for Factor of 2 (Not 6)

### Hypothesis 1: The Old Method Also Had Proton Requirements

If the old fase1 processing **already required some proton-related cuts** (even before `merge_pp_mutau.py`), then:
- Old method: Filter on proton variables → ~33% pass → add pileup protons to those
- New method: Filter on real protons → ~16.9% pass
- Ratio: 33% / 16.9% ≈ 2.0 ✓

**How to verify:** Check if there's a version of fase1 from before the change that shows proton-related cuts.

---

### Hypothesis 2: The "Expected" Value is From a Different Reference

The "expected" value you're comparing against might be:
- From a previous analysis with different cuts
- From theory predictions
- From a partial dataset (not all 393 files)
- Calculated incorrectly

**How to verify:** Document exactly where the "expected" value comes from.

---

### Hypothesis 3: Luminosity or HLT Filtering Changed

Looking at [fase0_qcd_data.py:174-175](fase0_qcd_data.py#L174):
```python
df.Filter("lumi_filter(run, luminosityBlock)", "Certified lumi")
  .Filter("HLT_IsoMu24 == 1", "HLT single-muon 2018")
```

If these filters **were NOT applied** in the old method but **ARE applied** in fase0 now:
- Lumi filter efficiency: ~X%
- HLT trigger efficiency: ~Y%
- Combined: X × Y × 16.9% (proton acceptance)

If X × Y ≈ 0.5, then total efficiency ≈ 0.5 × 0.169 ≈ 8.5%, giving:
- Expected from old: N events
- Expected from new: N × 0.5 (lumi+HLT) ≈ N/2 ✓

**How to verify:** Check if old processing had lumi/HLT filters.

---

### Hypothesis 4: Event Duplication in Old Method

If `merge_pp_mutau.py` somehow processed events **twice** (e.g., same file merged multiple times), the old method would have inflated counts by 2x.

**How to verify:** Check merge logs and file lists from old processing.

---

### Hypothesis 5: Different Input File Lists

If the old processing used **more input files** (e.g., 786 files = 2 × 393), that would explain it:
- Old: 786 files → N events
- New: 393 files → N/2 events ✓

**How to verify:** Check input file list from old processing.

---

## The Weight = 2.0 Compensation

Current code ([plot_m.py:258, 361](plot_m.py)):
```python
w_qcd = 2.0   # Line 258
weight = 2.0  # Line 361 (Data)
```

This compensates for the factor of 2, but:
- ❌ **Doesn't fix the underlying issue**
- ❌ **Affects statistical uncertainties** (weights propagate to error bars)
- ❌ **No clear documentation** of what it corrects for
- ❌ **May be wrong** if the factor isn't exactly 2.0

---

## Recommended Actions

### 1. Determine the Source of "Expected" Value

**Critical question:** What are you comparing your current event counts to?

- [ ] Previous analysis results with old processing?
- [ ] Theory predictions?
- [ ] Calculations based on cross-sections?
- [ ] A reference file from your advisor?

**Action:** Document this clearly.

---

### 2. Run Diagnostic on Lxplus

Run [investigate_factor_of_2.py](investigate_factor_of_2.py) on lxplus to see:
- How many events pass each cut sequentially
- What the charge split is before/after proton requirements
- Whether there's a systematic step that removes 50%

```bash
# On lxplus:
python3 investigate_factor_of_2.py
```

---

### 3. Check Old Processing Workflow

Find the old processing scripts/logs to understand:
- Did old fase1 have ANY proton requirements?
- Were lumi/HLT filters applied in the old method?
- How many input files were used?

Look for:
- Old fase1 scripts (before your advisor's recommendation)
- Merge logs showing which files were combined
- Event count reports from the old analysis

---

### 4. Decision Tree

```
Is the factor EXACTLY 2.0?
│
├─ YES → Likely a systematic cut difference
│         → Investigate Hypotheses 1, 3, or 5
│
└─ NO → Likely a reference/calculation error
          → Investigate Hypothesis 2
```

---

## If Weight = 2.0 is Correct

If investigation confirms the factor of 2 is real and systematic:

1. **Document it clearly** in the code:
   ```python
   # Data/QCD weights compensate for the change from artificial pileup protons
   # (old method) to real proton data (new method). The old processing had
   # [EXPLAIN THE SPECIFIC DIFFERENCE] which resulted in 2x more events.
   # Measured ratio: [ACTUAL VALUE]
   w_qcd = 2.0
   weight = 2.0
   ```

2. **Add systematic uncertainty** for this normalization factor

3. **Verify with your advisor** that this is the intended correction

---

## If Weight = 2.0 is Wrong

If the factor is NOT systematic:

1. **Remove the weights:**
   ```python
   w_qcd = 1.0
   weight = 1.0
   ```

2. **Use the actual event counts** from your data

3. **Re-evaluate your "expected" values**

---

## Comparison to TauTau Channel

TauTau channel does NOT have this issue because:
- Uses C++ processing throughout (no Python fase0/fase1 split)
- Proton filtering happens **during plotting**, not during processing
- Data and QCD go through identical selection (except charge)
- Uses `w_data = 1.0` and `w_qcd = 1.0` ([plot_m.cpp:238, 258](../TauTau_channel/plot_m.cpp))

**Recommendation:** Consider adopting TauTau's architecture (apply proton cuts in plotting, not in fase1).

---

## THE ACTUAL FIX NEEDED

Based on the diagnostic output, the **real problem** is that fase0 has no tau quality cuts, causing 99.95% rejection in fase1!

### Option 1: Apply Tau ID Cuts in Fase0 (Recommended)

Uncomment the tau quality filters in [fase0_qcd_data.py:177-182](fase0_qcd_data.py#L177):

```python
# In fase0_qcd_data.py, uncomment these lines:
.Filter("Muon_mvaId[0] >= 3", "Muon ID (>= Medium)")
.Filter("Tau_idDeepTau2017v2p1VSjet[0] >= 63", "Tau VSjet")  # or > 63
.Filter("Tau_idDeepTau2017v2p1VSe[0] >= 7", "Tau VSe")      # or > 7
.Filter("Tau_idDeepTau2017v2p1VSmu[0] >= 1", "Tau VSmu")    # or > 1
.Filter("Muon_pt[0] > 35. && Tau_pt[0] > 100.", "pT thresholds")
```

**Then reprocess from fase0!** The current fase1 output is based on bad fase0 data.

### Option 2: Loosen Fase1 Cuts (Quick Fix)

If you can't reprocess, temporarily loosen the cuts in [fase1_data.py:131-133](fase1_data.py#L131):

```python
# Temporary fix - use looser cuts:
.Filter("tau_id1 > 1", "Tau VSjet (loose)")   # Instead of > 63
.Filter("tau_id2 > 1", "Tau VSe (loose)")     # Instead of > 7
.Filter("tau_id3 > 0", "Tau VSmu (loose)")    # Instead of > 1
```

But this will include lower-quality taus and may affect physics results!

---

## Next Steps

1. ✅ Created diagnostic script: [investigate_factor_of_2.py](investigate_factor_of_2.py)
2. ✅ Created tau ID checker: [check_tau_id_distribution.py](check_tau_id_distribution.py)
3. ✅ **IDENTIFIED ROOT CAUSE:** Fase0 has no tau ID cuts → fase1 rejects 99.95%
4. ⚠️ **ACTION REQUIRED:** Uncomment tau quality cuts in fase0_qcd_data.py and reprocess
5. ⏳ **After reprocessing:** Remove `weight = 2.0` from plot_m.py (should no longer be needed)
6. ⏳ **Discuss with your advisor** about the proper tau ID working points

Once you fix the fase0 cuts and reprocess, the event counts should be correct without needing artificial weight=2.0 compensation.
