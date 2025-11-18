# Debug: Both Data and QCD at Half Expected Value

## The Real Problem

If **both Data AND QCD** are at ~50% of expected values (not just different from each other), this indicates a **systematic issue** affecting both samples equally, NOT the proton filter inconsistency.

## Possible Causes (In Order of Likelihood)

### 1. **Double-Counting in "Expected" Value**
**Most likely cause:** Your "expected" count might be wrong.

**Check:**
- Are you comparing to a count that includes **both** opposite-sign AND same-sign events?
- Data uses opposite-sign, QCD uses same-sign
- If reference includes both → you'd expect ~50% in each

**How to verify:**
```bash
# Count events in fase0 output (before charge cut)
# vs fase1 output (after charge cut)
```

---

### 2. **Charge Sign Split**
**Very likely:** If your reference doesn't account for charge selection:

```python
# fase1_data.py applies opposite-sign cut:
.Filter("mu_charge * tau_charge < 0", "Opposite sign")

# fase1_qcd.py applies same-sign cut:
.Filter("mu_charge * tau_charge > 0", "Same sign")
```

If charges are **roughly evenly distributed** (~50% OS, ~50% SS), then:
- Data gets ~50% of total events
- QCD gets ~50% of total events
- Each appears at "half" of the total

**This is NOT a bug - it's expected physics!**

---

### 3. **Input File Issues**

**Possibility:** Only half your input files were processed or merged.

**Check:**
```bash
# How many fase0 files went into the merge?
ls /eos/user/m/mblancco/samples_2018_mutau/fase0_with_proton_vars/Data*.root | wc -l

# How many were supposed to be processed?
wc -l Data_QCD_MuTau_skimmed_2018.txt

# Check the merged file size
ls -lh /eos/home-m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_proton_vars.root
```

---

### 4. **Proton Filter Removing 50%**

**Possibility:** The proton requirement itself removes ~50% of events.

**In Data ([fase1_data.py:138](fase1_data.py#L138)):**
```python
.Filter("pps_has_both_arms == 1", "At least one reconstructed proton per PPS arm")
```

**In QCD ([fase1_qcd.py:140](fase1_qcd.py#L140)):**
```python
.Filter("xi_arm1_1 >= 0 && xi_arm2_1 >= 0", "At least one reconstructed proton per PPS arm")
```

If only ~50% of events have protons in both arms, this is **expected behavior** for PPS analysis!

---

### 5. **Luminosity Filtering (fase0 only)**

In [fase0_qcd_data.py:174-175](fase0_qcd_data.py#L174):
```python
df.Filter("lumi_filter(run, luminosityBlock)", "Certified lumi")
  .Filter("HLT_IsoMu24 == 1", "HLT single-muon 2018")
```

**Note:** These are **commented out** in fase1! So if fase0 was run with these active, they could remove events.

---

## Diagnostic Steps

### Step 1: Count events at each stage

Create this script (`count_events.py`):

```python
import ROOT

stages = {
    "Fase 0 Data": "/eos/user/m/mblancco/samples_2018_mutau/fase0_with_proton_vars/Data_2018_UL_skimmed_MuTau_nano_0.root",
    "Fase 1 Data (merged)": "/eos/home-m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_proton_vars.root",
    "Fase 0 QCD": "/eos/user/m/mblancco/samples_2018_mutau/fase0_with_proton_vars/Data_2018_UL_skimmed_MuTau_nano_0.root",  # Same file, different cuts in fase1
    "Fase 1 QCD (merged)": "/eos/home-m/mblancco/samples_2018_mutau/QCD_2018_UL_MuTau_nano_merged_proton_vars.root",
}

for name, path in stages.items():
    try:
        f = ROOT.TFile.Open(path)
        if not f or f.IsZombie():
            print(f"{name}: CANNOT OPEN")
            continue

        tree = f.Get("tree")
        if not tree:
            tree = f.Get("tree_out")

        if not tree:
            print(f"{name}: NO TREE")
            continue

        total = tree.GetEntries()
        valid_mass = tree.GetEntries("sist_mass >= 0")

        # Count by charge if branches exist
        if tree.GetBranch("mu_charge") and tree.GetBranch("tau_charge"):
            os = tree.GetEntries("mu_charge * tau_charge < 0")
            ss = tree.GetEntries("mu_charge * tau_charge > 0")
            print(f"\n{name}:")
            print(f"  Total: {total}")
            print(f"  Valid mass: {valid_mass}")
            print(f"  Opposite sign: {os} ({100*os/total:.1f}%)")
            print(f"  Same sign: {ss} ({100*ss/total:.1f}%)")
        else:
            print(f"\n{name}:")
            print(f"  Total: {total}")
            print(f"  Valid mass: {valid_mass}")

        f.Close()
    except Exception as e:
        print(f"{name}: ERROR - {e}")
```

---

### Step 2: Check what "expected" means

**Critical question:** What are you comparing against?

1. **Theory prediction?** (unlikely for Data)
2. **Previous analysis results?**
3. **Total events before charge cuts?** ← **Most likely!**
4. **Sum of all trigger paths?** (could have overlap)

**If comparing to pre-charge-cut total:** Finding 50% in OS and 50% in SS is **CORRECT**!

---

### Step 3: Check the weight=2.0 origin

Look at your code history:
```bash
git log -p plot_m.py | grep -A 5 -B 5 "weight.*2.0"
```

**Question:** Who added `weight=2.0` and why? The commit message should explain the reasoning.

---

## Most Likely Answer

Based on standard CMS PPS analysis:

1. **Charge split:** OS and SS each get ~40-60% of events (not exactly 50%)
2. **Proton acceptance:** Only ~10-20% of events have reconstructed protons in both PPS arms
3. **Combined:** The "factor of 2" might be compensating for the **combination** of:
   - Charge selection removing ~50%
   - Different proton filters removing different amounts
   - Making the final plots comparable to a reference that doesn't account for these cuts

**The weight=2.0 is likely CORRECT if:**
- Your reference expectation includes both OS+SS events
- Your reference doesn't require protons in both arms
- You're trying to extrapolate to a larger phase space

---

## Action Items

1. **Define "expected value"** - what exactly are you comparing to?
2. **Run diagnostic script** to count events at each stage
3. **Check if OS:SS ratio is ~50:50** (if so, this explains the factor)
4. **Verify proton acceptance** - what fraction of events have both arms?
5. **Document the weight=2.0** - add a comment explaining what it corrects for

---

## Questions to Answer

1. What is the "expected" event count you're comparing against?
2. Is it before or after charge selection?
3. Is it before or after proton requirements?
4. Are you sure both Data and QCD files are fully merged (all chunks included)?
5. What does TauTau channel use - do they also have a factor?

Once you answer these, we can identify the real issue.