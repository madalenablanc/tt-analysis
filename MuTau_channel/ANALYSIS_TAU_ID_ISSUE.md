# Tau ID Issue Analysis

## Problem Summary

The tau ID distribution reveals that **tau_id3 (VSmu)** is the bottleneck:
- 81.7% of events have tau_id1 = 255 (excellent quality) ✓
- 85.08% pass tau_id2 > 7 (VSe) ✓
- **Only 2.20% pass tau_id3 > 1 (VSmu)** ✗

This creates a combined efficiency of only **0.117%** for all three tau ID cuts together!

---

## Tau ID Bit Values Reference

DeepTau2017v2p1 working points (bit masks):

### VSjet (tau_id1):
- 1 = VVVLoose
- 2 = VVLoose
- 4 = VLoose
- 8 = Loose
- 16 = Medium
- 32 = Tight
- 64 = VTight
- 128 = VVTight

**Current cut: `> 63`** requires at least VTight (64 or higher)

### VSe (tau_id2):
- 1 = VVVLoose
- 2 = VVLoose
- 4 = VLoose
- 8 = Loose
- 16 = Medium
- 32 = Tight
- 64 = VTight
- 128 = VVTight

**Current cut: `> 7`** requires at least Loose (8 or higher)

### VSmu (tau_id3):
- 1 = VLoose
- 2 = Loose
- 4 = Medium
- 8 = Tight

**Current cut: `> 1`** requires at least Loose (2 or higher)

---

## Why VSmu is Problematic in MuTau Channel

In the **MuTau channel**, you have:
- One real muon (from W/Z decay)
- One tau (which you want to identify)

The tau VSmu discriminator is designed to **reject taus that look like muons**. However:

1. **Your real muon might be close to taus in the detector** → taus near muons get low VSmu scores
2. **The delta_r > 0.4 cut should already separate them geometrically**
3. **VSmu might be overly conservative** when there's a high-pT muon nearby

---

## Comparison with TauTau Channel

Let me check what TauTau uses for tau_id3...

Looking at TauTau C++ code (would need to see the actual cuts), but typically:
- TauTau channel: **Two taus**, no real muons → VSmu can be looser
- MuTau channel: **One muon + one tau** → VSmu should still work, but needs careful tuning

---

## Diagnostic Results Interpretation

From `check_tau_id_distribution.py` output:

```
tau_id3 (VSmu):
   > 0:   31,672 ( 3.09%)  ← Only 3% have ANY VSmu score!
   > 1:   22,479 ( 2.20%)  ← Current cut
   > 2:   22,479 ( 2.20%)
   > 4:   19,891 ( 1.94%)
   > 8:   18,320 ( 1.79%)
```

**Critical observation:** Only 3.09% of taus have **any** VSmu discrimination (> 0)!

This suggests either:
1. **Most taus in your sample ARE muon-like** (unlikely - you have real taus)
2. **The VSmu branch is not properly filled** in your NanoAOD or fase0 output
3. **The VSmu cut is being evaluated incorrectly**

---

## Possible Root Causes

### 1. VSmu Branch Not Properly Saved in Fase0

Check if `Tau_idDeepTau2017v2p1VSmu[0]` exists and is correctly filled in the input NanoAOD.

**Action:** Run this diagnostic:
```python
import ROOT
f = ROOT.TFile.Open("root://cms-xrd-global.cern.ch///store/...")  # Your NanoAOD input
tree = f.Get("Events")
tree.Scan("Tau_idDeepTau2017v2p1VSmu", "", "", 100)
```

---

### 2. Wrong Working Point for MuTau Channel

The TauPOG recommendations might differ for MuTau vs TauTau channels.

**Standard MuTau recommendations (2018 UL):**
- VSjet: **Tight** or **VTight** (safe choice)
- VSe: **VVLoose** or **VLoose** (less critical in MuTau)
- VSmu: **VLoose** or **Loose** (this is your bottleneck!)

**Current cuts:**
- VSjet > 63 → requires VTight (64) or higher ✓
- VSe > 7 → requires Loose (8) or higher ✓
- VSmu > 1 → requires Loose (2) or higher ← **Might be too tight!**

---

### 3. Comparing with Old Method

**Critical question:** What tau ID cuts did the **old processing** use (before switching to real proton data)?

If the old method used:
- VSjet: Medium (16)
- VSe: VVLoose (2)
- VSmu: **None or VLoose (1)** ← This would explain the factor difference!

Then switching to VSmu > 1 (Loose) could cut your events significantly.

---

## Recommended Solutions

### Solution 1: Relax VSmu Cut (Quick Test)

In [fase1_data.py:133](fase1_data.py#L133) and [fase1_qcd.py](fase1_qcd.py), change:

```python
# From:
.Filter("tau_id3 > 1", "Tau VSmu")

# To:
.Filter("tau_id3 > 0", "Tau VSmu (VLoose)")
```

This changes from **Loose** to **VLoose** working point.

**Impact:** Should increase acceptance from 2.2% → 3.09% (not huge, but helps)

---

### Solution 2: Check if VSmu Branch is Correct

Verify that `tau_id3` is being read correctly:

```python
import ROOT
f = ROOT.TFile.Open("/eos/user/m/mblancco/samples_2018_mutau/fase0_with_proton_vars/Data_2018_UL_skimmed_MuTau_nano_0.root")
tree = f.Get("tree")

# Print first 100 events
for i in range(100):
    tree.GetEntry(i)
    tau_id3 = tree.GetLeaf("tau_id3").GetValue()
    print(f"Event {i}: tau_id3 = {tau_id3}")
```

**Expected:** Values should be 0, 1, 2, 4, 8, or 15 (bit combinations)

**If you see:** All zeros or -999 → VSmu branch is not properly filled!

---

### Solution 3: Remove VSmu Cut Entirely (Debugging)

Temporarily comment out the VSmu cut to see how many events you get:

```python
# In fase1_data.py and fase1_qcd.py:
.Filter("mu_id >= 3", "Muon ID")
.Filter("tau_id1 > 63", "Tau VSjet")
.Filter("tau_id2 > 7", "Tau VSe")
# .Filter("tau_id3 > 1", "Tau VSmu")  ← Comment this out
```

**Expected result:** If this is the issue, you should get ~100x more events

---

### Solution 4: Apply Cuts in Fase0 (Long-term Fix)

Uncomment lines in [fase0_qcd_data.py:177-182](fase0_qcd_data.py#L177) BUT:
1. **First verify** that the VSmu branch is correctly filled in raw NanoAOD
2. **Adjust** the VSmu working point based on TauPOG recommendations
3. **Reprocess** from fase0

---

## Decision Matrix

| If tau_id3 values are: | Then: | Action: |
|------------------------|-------|---------|
| All 0 or -999 | VSmu branch not filled | Check NanoAOD input, may need different tau collection |
| Mostly 0 with few 1,2,4,8 | Real taus ARE muon-like | This is physics - relax cut or accept low statistics |
| Correct distribution | Cut is too tight | Use VSmu > 0 (VLoose) instead of > 1 (Loose) |

---

## Next Steps

1. **Run diagnostic** to check actual tau_id3 values in raw NanoAOD input
2. **Compare with TauTau** - what VSmu cut does that channel use?
3. **Check TauPOG recommendations** for 2018 UL MuTau analysis
4. **Test with relaxed VSmu cut** (> 0 instead of > 1)
5. **If that works:** Reprocess from fase0 with correct cuts
6. **If that doesn't work:** Investigate why VSmu has such low acceptance

---

## Where to Find More Info

- CMS TWiki TauID: https://twiki.cern.ch/twiki/bin/view/CMS/TauIDRecommendationForRun2
- DeepTau2017v2p1 working points: Check NanoAOD documentation
- Your advisor's previous analyses: What cuts did they use?

---

## Summary

**The factor of 2 is a red herring.** The real issue is:

1. **99.5% of events are rejected** by combined tau ID cuts
2. **tau_id3 (VSmu) is the killer** - only 2.2% pass
3. **Only 3.09% of taus have ANY VSmu score** - this suggests a potential issue with the branch or the physics

Before worrying about `weight = 2.0`, you need to:
1. Understand why VSmu acceptance is so low
2. Verify the branch is correctly filled
3. Check if the working point is appropriate for MuTau channel
4. Compare with what the old processing used

Once you fix the tau ID issue, the event counts should be much higher, and the need for weight=2.0 might disappear entirely.