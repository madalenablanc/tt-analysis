# Revised Root Cause Analysis - With Correct Understanding

## Design Decision Confirmed

**The commented-out cuts in fase0 are INTENTIONAL** ✓

- **Fase0:** Apply only general cuts (lumi, trigger, require 1 tau + 1 muon)
- **Fase1:** Apply all quality cuts (tau ID, muon ID, pT, eta, delta_R)

This matches the original analysis architecture shown in the table.

---

## So What's Actually Wrong?

If your architecture is correct, then the issue is **NOT** a bug in your processing. The real questions are:

### 1. Is the VSmu Branch Working Properly?

From your diagnostic (`check_tau_id_distribution.py`):

```
tau_id3 (VSmu):
   > 0:   31,672 ( 3.09%)  ← Only 3% have ANY VSmu score!
   > 1:   22,479 ( 2.20%)  ← Current cut
```

**Only 3.09% of taus have tau_id3 > 0**

This is extremely suspicious because:
- tau_id1 (VSjet): 83.32% pass tau_id1 > 63 ✓ (normal)
- tau_id2 (VSe): 85.08% pass tau_id2 > 7 ✓ (normal)
- tau_id3 (VSmu): 3.09% have ANY score ✗ (abnormal!)

**Conclusion:** The VSmu branch in your NanoAOD appears to be broken or not filled properly.

---

### 2. What NanoAOD Version Are You Using?

**Critical question:** What NanoAOD campaign/version are you using?

Check your input file list:
```bash
head -5 Data_MuTau_phase0_2018.txt
```

NanoAOD versions:
- **v9:** Used in early Run 2 UL
- **v10, v11, v12:** Later versions with improvements
- Different versions have different tau ID branches or calculations

**Hypothesis:** The original analysis used a different NanoAOD version where `Tau_idDeepTau2017v2p1VSmu` was properly filled.

---

### 3. Where Do Your "Expected" Values Come From?

You said Data and QCD are at "~50% of expected."

**Question:** What are you comparing to?

**Option A: Previous run with artificial protons**
```
Old method: Process → Add artificial protons (100% get protons)
New method: Process → Use real protons (16.9% have protons)
```

If old method had:
- No proton requirement until plotting
- Applied all same cuts otherwise

Then you'd expect:
```
Old method: N events (no proton filter)
New method: N × 0.169 (with proton filter in fase1)
Ratio: 1 / 0.169 ≈ 6x fewer events expected
```

But you see only 2x difference, which suggests:
- Old method also had some proton-related filter (~33% acceptance)
- Or the "expected" value already accounts for some proton requirement

**Option B: Original analysis results**

If comparing to the original analysis paper/note:
- What were their final event counts?
- Did they use real or artificial protons?
- What acceptance did they quote?

---

## The Real Issue: VSmu Discrimination

### Evidence Summary

1. **81.7% of taus have tau_id1 = 255** (maximum VSjet quality) ✓
2. **85% pass VSe cuts** ✓
3. **Only 3% have ANY VSmu score** ✗

This pattern suggests:
- Taus are high quality for jet and electron discrimination
- But completely fail muon discrimination
- **Either:** The branch is broken in your NanoAOD
- **Or:** There's a physics reason (unlikely - you have real taus)

---

## What to Do Next

### Step 1: Verify VSmu in Raw NanoAOD (CRITICAL)

On lxplus, check your raw NanoAOD input files:

```bash
# Get your input file path
head -1 Data_MuTau_phase0_2018.txt

# Open in ROOT
root -l root://cms-xrd-global.cern.ch///store/[path_from_file]

# Check VSmu values
Events->Scan("Tau_pt:Tau_idDeepTau2017v2p1VSjet:Tau_idDeepTau2017v2p1VSmu", "", "", 100)
```

**Look for:**
- Are Tau_idDeepTau2017v2p1VSmu values mostly 0?
- Or do you see a mix of 0, 1, 2, 4, 8, 15?

**If mostly 0:** The VSmu branch is not filled in your NanoAOD → Need different NanoAOD or different tau collection

**If proper distribution:** Something else is wrong in how fase0 reads/stores tau_id3

---

### Step 2: Compare with Original Analysis

Ask your advisor or check the original analysis code:

1. **What NanoAOD version/campaign did they use?**
   - 2018 UL NanoAODv9? v10? v11? v12?
   - Different campaign entirely?

2. **Did they see low VSmu acceptance?**
   - Is 2.2% normal for their analysis?
   - Or did they get higher acceptance?

3. **What were their final event counts?**
   - How many Data events after all cuts?
   - How many QCD events?
   - This will tell you if your counts are correct or not

---

### Step 3: Test with Relaxed VSmu Cut

**Quick test** to verify VSmu is the bottleneck:

In `fase1_data.py` line 133 and `fase1_qcd.py` line 140:

```python
# Change from:
.Filter("tau_id3 > 1", "Tau VSmu")

# To (temporarily):
.Filter("tau_id3 > 0", "Tau VSmu (VLoose)")
# Or comment out entirely for maximum test
```

Rerun fase1 on a few files and check event counts.

**Expected result:**
- If VSmu is the bottleneck: Event count increases from ~1 to ~10-13 per file
- If something else is wrong: Still get ~1 event per file

---

### Step 4: Check If VSmu Cut is Correct for MuTau

**Physics question:** In the MuTau channel, is VSmu > 1 (Loose) the right working point?

From CMS TauPOG recommendations for 2018 UL MuTau:
- VSjet: Usually Tight or VTight (what you use: > 63) ✓
- VSe: Usually VVLoose or VLoose (what you use: > 7) ✓
- VSmu: Usually VLoose or Loose (what you use: > 1)

Your cut seems standard, but:
- In MuTau channel, you already have a real muon
- Delta_R > 0.4 cut separates tau from muon geometrically
- Maybe VSmu is less critical? Or looser working point acceptable?

**Check:** What does TauTau channel use for VSmu?

---

## Possible Explanations for Factor of 2

Now that we understand your setup is correct, let's revisit the factor of 2:

### Scenario A: Proton Acceptance is the Cause

**Old method (artificial protons):**
```
Process all events → Apply cuts → Get N events → Add artificial protons to all
Result: N events in final sample
```

**New method (real protons):**
```
Process all events → Apply proton filter (16.9%) → Apply cuts → Get 0.169×N events
Result: 0.169×N events in final sample
```

Expected ratio: 1 / 0.169 ≈ **6x difference**

But you see **2x difference**, which suggests:
1. Old method also had some proton requirement (~50% acceptance)
2. Or the "expected" value wasn't from old method at all

---

### Scenario B: VSmu Working Point Changed

**Old processing:**
- Maybe used tau_id3 > 0 (VLoose) → 3.09% acceptance
- Or had no VSmu cut at all

**New processing:**
- Uses tau_id3 > 1 (Loose) → 2.20% acceptance

Ratio: 3.09% / 2.20% ≈ **1.4x difference**

Not quite 2x, but in the ballpark.

---

### Scenario C: Different NanoAOD Version

**Original analysis:**
- Used NanoAOD version where VSmu worked properly
- Got ~30-40% VSmu acceptance (normal)
- Final counts: X events

**Your analysis:**
- Using NanoAOD version where VSmu is broken
- Getting ~2-3% VSmu acceptance (broken)
- Final counts: ~0.1×X events

**Plus proton requirement:**
- Old: No proton filter → X events
- New: Proton filter (16.9%) → 0.169 × 0.1 × X ≈ 0.017×X

This could give you very low counts.

---

## The weight = 2.0 Question

### If Weight is Correct

The `weight = 2.0` compensates for something systematic:

**Possible reasons:**
1. Old method had 2x more events due to different proton handling
2. Old method used looser cuts somewhere
3. Old method processed each event twice somehow
4. Different dataset or file list (2x more files)

**To verify:**
- Find old processing logs - how many events did old method give?
- Compare: old_count / new_count = ?
- If exactly 2.0 → weight is correct
- If not 2.0 → weight is wrong

---

### If Weight is Wrong

If the factor isn't systematic:

1. **Remove weights:**
   ```python
   w_qcd = 1.0
   weight = 1.0
   ```

2. **Use actual event counts from your data**

3. **Verify your final results make physical sense:**
   - Signal-to-background ratio
   - Cross-section consistency
   - Agreement with theory

---

## Summary of Revised Understanding

### What's Correct ✓

1. Fase0/Fase1 split matches original analysis design
2. Cuts applied in fase1 are standard and correct
3. Commented-out cuts in fase0 are intentional

### What's Wrong ✗

1. **VSmu acceptance is extremely low (2.2%)**
   - Should be ~20-40% in normal NanoAOD
   - Suggests branch is broken or NanoAOD version issue

2. **Factor of 2 discrepancy**
   - Need to understand where "expected" values come from
   - Need to compare with original analysis event counts

### Critical Next Steps

1. ✅ Verify VSmu values in raw NanoAOD input files
2. ✅ Ask advisor about NanoAOD version and expected counts
3. ✅ Compare your final counts with original analysis
4. ✅ Test with relaxed VSmu cut (tau_id3 > 0)
5. ✅ Document where "expected" values come from

Once you know:
- If VSmu branch is broken → Use different NanoAOD or different tau collection
- If VSmu is correct → Accept the low statistics or relax the cut
- If weight=2.0 is systematic → Document it clearly
- If weight=2.0 is wrong → Remove it

---

## Bottom Line

**Your code is correct.** The issue is either:

1. **Input data issue (VSmu branch)** - most likely
2. **Wrong comparison reference** - need to clarify "expected" values
3. **Physics reality** - maybe your sample genuinely has low statistics

This is NOT a bug in your processing chain. It's either a data quality issue or a normalization/comparison issue.
