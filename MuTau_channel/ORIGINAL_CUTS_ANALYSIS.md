# Original Analysis Cuts - Critical Discovery

## The Table Shows

From the image provided, here are the cuts applied in the **original analysis**:

### FASE0 Cuts (Applied to All Samples)

| Sample | ttjets | dy | qcd | data |
|--------|--------|----|----|------|
| **Selection** | 1 tau, 1 muon | 1 tau, 1 muon | 1 tau, 1 muon | 1 tau, 1 muon |
| **Iso_Mu24** | ✓ | ✓ | ✓ | ✓ |
| **Ele32_WPTight** | - | - | - | - |
| **Electron_** | - | - | - | - |
| **lumi filtering** | - | - | ✓ | ✓ |

### FASE1 Cuts (Applied to All Samples)

| Cut | ttjets | dy | qcd | data |
|-----|--------|----|----|------|
| **tau_id1 (VSjet)** | >63 | >63 | >63 | >63 |
| **tau_id2 (VSe)** | >7 | >7 | >7 | >7 |
| **tau_id3 (VSmu)** | >1 | >1 | >1 | >1 |
| **tau_pt** | >100 | >100 | >100 | >100 |
| **mu_id** | >=3 | >=3 | >=3 | >=3 |
| **mu_pt** | >35 | >35 | >35 | >35 |
| **ele_id** | - | - | - | - |
| **ele_pt** | - | - | - | - |
| **delta_R** | >0.4 | >0.4 | >0.4 | >0.4 |
| **eta** | <2.4 | <2.4 | <2.4 | <2.4 |
| **charge_signs** | opposite | opposite | same | opposite |
| **scaling_factors** | ✓ | ✓ | - | - |
| **valid_jet_and_MET_info** | ✓ | ✓ | - | - |

---

## Critical Finding: FASE0 Should NOT Have Tau ID Cuts!

Looking at this table, the **original analysis**:

### In FASE0:
- ✓ Applied trigger (Iso_Mu24)
- ✓ Applied lumi filtering (for data/qcd only)
- ✓ Required 1 tau, 1 muon
- ❌ **Did NOT apply tau ID cuts**
- ❌ **Did NOT apply pT cuts**
- ❌ **Did NOT apply muon ID cuts**

### In FASE1:
- ✓ Applied ALL quality cuts (tau_id1, tau_id2, tau_id3)
- ✓ Applied pT cuts
- ✓ Applied muon ID
- ✓ Applied delta_R, eta, charge

---

## What This Means for Your Current Issue

### Your Current Setup

**Fase0 ([fase0_qcd_data.py](fase0_qcd_data.py)):**
```python
.Filter("lumi_filter(run, luminosityBlock)", "Certified lumi")  ✓ Correct
.Filter("HLT_IsoMu24 == 1", "HLT single-muon 2018")            ✓ Correct
.Filter("Muon_pt.size() > 0 && Tau_pt.size() > 0", ...)        ✓ Correct
# All quality cuts commented out                                ✓ CORRECT!
```

**Fase1 ([fase1_data.py](fase1_data.py)):**
```python
.Filter("mu_id >= 3", "Muon ID (>= Medium)")                    ✓ Correct
.Filter("tau_id1 > 63", "Tau VSjet")                            ✓ Correct
.Filter("tau_id2 > 7", "Tau VSe")                               ✓ Correct
.Filter("tau_id3 > 1", "Tau VSmu")                              ✓ Correct
.Filter("mu_pt > 35. && tau_pt > 100.", "pT thresholds")        ✓ Correct
.Filter("delta_r>0.4", "Delta R acceptance")                    ✓ Correct
.Filter("fabs(tau_eta)<2.4 && fabs(mu_eta)<2.4", ...)          ✓ Correct
```

### YOUR CURRENT SETUP IS CORRECT!

The commented-out cuts in fase0 are **INTENTIONAL**, not a bug!

---

## So Why Are You Getting So Few Events?

If your cuts match the original analysis, then the issue must be elsewhere:

### Hypothesis 1: The VSmu Branch Issue is Real

From `check_tau_id_distribution.py` output:
```
tau_id3 (VSmu):
   > 0:   31,672 ( 3.09%)
   > 1:   22,479 ( 2.20%)
```

Only 2.2% of taus pass `tau_id3 > 1`. This is **extremely low** and suggests:

1. **The VSmu branch is not properly filled in your NanoAOD**
2. **The NanoAOD version changed** and VSmu is calculated differently
3. **There's a bug in how tau_id3 is being read**

**Action:** Compare with the original analysis - what NanoAOD version did they use?

---

### Hypothesis 2: The Original Analysis Had Different Input

The original analysis might have:
- Used a different NanoAOD version/campaign
- Had different tau collections
- Used different years of data
- Applied pre-selection we don't know about

---

### Hypothesis 3: Proton Requirement in Original Analysis

The table doesn't show **when proton cuts were applied** in the original analysis!

**Critical questions:**
1. Did the original analysis apply proton cuts in FASE0 or FASE1?
2. Or were proton cuts only applied during plotting (like TauTau)?
3. Did data/qcd use real or artificial protons in the original analysis?

---

## The Real Issue: VSmu Acceptance is Too Low

Your diagnostic showed:
```
Starting with protons: 172,895 events
+ Muon ID: 138,903 (80% pass - normal ✓)
+ Tau ID: 65       (99.95% REJECTED! ✗)
```

If the original analysis used the **same cuts**, they should have seen the same rejection!

This suggests one of:

1. **Different NanoAOD:** Original used NanoAOD where VSmu was properly filled
2. **Different tau collection:** Original used different tau objects
3. **Different working point:** The table might show simplified cuts, actual code used looser VSmu
4. **Proton cuts earlier:** Original applied proton cuts in FASE0, keeping only events with protons BEFORE tau ID cuts

---

## What to Check Next

### 1. Compare NanoAOD Versions

**Original analysis:**
- What NanoAOD campaign? (e.g., "2018 UL", "2018 Legacy", "2018 prompt")
- What NanoAOD version? (v9, v10, v11?)

**Your analysis:**
```python
# In fase0_qcd_data.py, what are your input files?
input_list = "Data_MuTau_phase0_2018.txt"
```

Check the first line of that file - what's the path?

---

### 2. Check Original Analysis Code

If you have access to the original analysis code, check:

```python
# What did they use for tau_id3?
# Was it:
.Filter("tau_id3 > 1")    # Loose
# Or:
.Filter("tau_id3 > 0")    # VLoose
# Or:
.Filter("tau_id3 >= 1")   # VLoose or higher
```

Small differences matter!

---

### 3. Verify VSmu in Your NanoAOD

On lxplus, check one of your raw NanoAOD files:

```bash
# Get the first input file path
head -1 Data_MuTau_phase0_2018.txt

# Check VSmu branch
root -l root://cms-xrd-global.cern.ch//[that_path]
Events->Scan("Tau_idDeepTau2017v2p1VSmu:Tau_pt", "", "", 100)
```

**Expected:** Mix of 0, 1, 2, 4, 8, 15
**If you see:** Mostly 0 → VSmu branch is broken in your NanoAOD

---

### 4. Compare Event Counts with Original

From the original analysis paper/note:
- How many Data events after all cuts?
- How many QCD events after all cuts?

**Your current counts:**
- Data: 1,708 events
- QCD: 234 events

**If original had similar counts:**
→ Your processing is correct, just need to use real counts

**If original had 10x-100x more:**
→ Something is different (NanoAOD, cuts, or dataset)

---

## Revised Understanding

### The "Factor of 2" Discrepancy

You said both Data and QCD are at "half their expected values."

**Questions:**
1. What is the "expected" value you're comparing to?
2. Is it from:
   - Previous run of your code with old method (artificial protons)?
   - The original analysis results?
   - A calculation based on cross-sections?
   - Your advisor's expectations?

**If from old method with artificial protons:**
- Old method: 100% of events got protons → N events
- New method: 16.9% of events have real protons → 0.169 × N events
- Ratio: N / (0.169 × N) ≈ 6x difference expected

But you see 2x difference, which suggests:
- Old method also had some proton requirement (maybe ~33% acceptance)
- Or old method applied stricter cuts elsewhere

---

## Action Items

### Immediate (Critical)

1. **Check NanoAOD VSmu values:**
   ```bash
   root -l root://cms-xrd-global.cern.ch//[your_input_file]
   Events->Scan("Tau_idDeepTau2017v2p1VSmu", "", "", 1000)
   ```

   If mostly 0 → **VSmu branch is the problem**

2. **Get original analysis event counts:**
   - How many Data events did they report?
   - How many QCD events?
   - What was their signal-to-background ratio?

3. **Clarify "expected" values:**
   - Document exactly where your "expected" counts come from
   - Show the calculation or reference

### Short-term

4. **Test with looser VSmu cut:**

   In fase1_data.py and fase1_qcd.py:
   ```python
   # Change from:
   .Filter("tau_id3 > 1", "Tau VSmu")
   # To:
   .Filter("tau_id3 > 0", "Tau VSmu (VLoose)")
   ```

   See if this gives you more events.

5. **Compare with TauTau channel:**
   - Do they have the same VSmu issue?
   - What NanoAOD do they use?
   - How many events do they get?

### Medium-term

6. **Contact original analysis authors:**
   - Ask about NanoAOD version
   - Ask if they saw low VSmu acceptance
   - Ask if there were any special handling for VSmu

7. **Check CMS Tau POG recommendations:**
   - For 2018 UL MuTau channel
   - Recommended VSmu working point
   - Known issues with VSmu in certain NanoAOD versions

---

## Summary

**Good news:** Your fase0/fase1 split matches the original analysis table!
- Fase0: No quality cuts ✓
- Fase1: All quality cuts ✓

**Bad news:** The VSmu acceptance is extremely low (2.2%)
- This is causing 99.95% event rejection
- Either the NanoAOD branch is broken, or working point is wrong

**Next step:** Verify VSmu values in your raw NanoAOD files. If they're mostly 0, that's the root cause.

**The weight=2.0 question:** Still need to understand where your "expected" values come from to know if this is the right correction.