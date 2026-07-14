"""
ROC curve comparison for BDT scans A, B, C.
Reads MVA_BDT_rejBvsS histograms from per-scan TMVA files in tmva_outputs/,
extracts bin arrays with PyROOT, then plots with matplotlib.

Files required (produced by run_all_scans.sh):
  tmva_outputs/TMVA_scanA.root
  tmva_outputs/TMVA_scanB.root
  tmva_outputs/TMVA_scanC.root

AUC values parsed from train_scanX.log files.

Run on lxplus with:
  source /cvmfs/sft.cern.ch/lcg/views/LCG_106/x86_64-el9-gcc13-opt/setup.sh
  python3 plot_roc.py
"""

import os
import re
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
from mpl_toolkits.axes_grid1.inset_locator import inset_axes

TMVA_DIR = "tmva_outputs"
OUT_DIR  = "plots_shapes"
os.makedirs(OUT_DIR, exist_ok=True)

ROC_PATH = "dataset/Method_BDT/BDT/MVA_BDT_rejBvsS"

SCANS = [
    ("A", "TMVA_scanA.root", "train_scanA.log", "#1f77b4", "-"),    # blue solid
    ("B", "TMVA_scanB.root", "train_scanB.log", "#d62728", "--"),   # red dashed
    ("C", "TMVA_scanC.root", "train_scanC.log", "#2ca02c", ":"),    # green dotted
]


def parse_auc(log_path):
    if not os.path.exists(log_path):
        return None
    pattern = re.compile(r"dataset\s+BDT\s+:\s+([0-9.]+)")
    with open(log_path) as fh:
        for line in fh:
            m = pattern.search(line)
            if m:
                return float(m.group(1))
    return None


def load_roc_arrays(root_path):
    """Return (sig_eff, bkg_rej) numpy arrays from TMVA ROC histogram."""
    import ROOT
    ROOT.gROOT.SetBatch(True)
    f = ROOT.TFile.Open(root_path)
    if not f or f.IsZombie():
        return None, None
    h = f.Get(ROC_PATH)
    if not h:
        f.Close()
        return None, None
    n = h.GetNbinsX()
    x = np.array([h.GetXaxis().GetBinCenter(i) for i in range(1, n + 1)])
    y = np.array([h.GetBinContent(i)           for i in range(1, n + 1)])
    f.Close()
    return x, y


def main():
    fig, ax = plt.subplots(figsize=(6.5, 6.5))
    inset_ax = inset_axes(ax, width="42%", height="42%", loc="lower right",
                          bbox_to_anchor=(0.0, 0.18, 1.0, 1.0),
                          bbox_transform=ax.transAxes, borderpad=1.2)

    found_any = False
    for label, rfile, lfile, color, ls in SCANS:
        rpath = os.path.join(TMVA_DIR, rfile)
        lpath = lfile if os.path.exists(lfile) else os.path.join(TMVA_DIR, lfile)

        auc = parse_auc(lpath)
        auc_str = f"  (AUC = {auc:.3f})" if auc is not None else ""

        sig_eff, bkg_rej = load_roc_arrays(rpath)
        if sig_eff is None:
            print(f"  WARNING: cannot load ROC for scan {label} from {rpath}")
            print(f"           Re-run run_all_scans.sh to generate per-scan TMVA files.")
            continue

        bkg_eff = 1.0 - bkg_rej

        ax.plot(bkg_eff, sig_eff, color=color, linestyle=ls, linewidth=2.5,
                label=f"Scan {label}{auc_str}")
        inset_ax.plot(bkg_eff, sig_eff, color=color, linestyle=ls, linewidth=2.2)
        found_any = True

    if not found_any:
        print("No ROC histograms found. Run run_all_scans.sh first.")
        return

    # Random-classifier reference for the standard ROC convention
    ref_x = np.linspace(0, 1, 400)
    ref_y = ref_x
    ax.plot(ref_x, ref_y, color="gray", linestyle="--", linewidth=1.0, alpha=0.7,
            label="Random classifier")
    inset_ax.plot(ref_x, ref_y, color="gray", linestyle="--", linewidth=0.9, alpha=0.7)

    ax.set_xlim(0, 1)
    ax.set_ylim(0, 1)
    ax.set_xlabel("Background efficiency", fontsize=14, labelpad=6)
    ax.set_ylabel("Signal efficiency", fontsize=14, labelpad=6)
    ax.tick_params(axis="both", labelsize=12)
    ax.xaxis.set_major_locator(ticker.MultipleLocator(0.2))
    ax.yaxis.set_major_locator(ticker.MultipleLocator(0.2))
    ax.grid(True, which="major", linestyle=":", linewidth=0.8, alpha=0.35)

    inset_ax.set_xlim(0.0, 0.10)
    inset_ax.set_ylim(0.85, 1.00)
    inset_ax.xaxis.set_major_locator(ticker.MultipleLocator(0.05))
    inset_ax.yaxis.set_major_locator(ticker.MultipleLocator(0.05))
    inset_ax.tick_params(axis="both", labelsize=9)
    inset_ax.grid(True, which="major", linestyle=":", linewidth=0.7, alpha=0.3)

    ax.legend(fontsize=12, frameon=False, loc="lower left",
              handlelength=2.2, handletextpad=0.6)

    # CMS-TOTEM style header
    ax.text(0.01, 1.04, "CMS-TOTEM", transform=ax.transAxes,
            fontsize=16, fontweight="bold", va="bottom")
    ax.text(0.01, 1.005, "Preliminary", transform=ax.transAxes,
            fontsize=13, fontstyle="italic", va="bottom")
    ax.text(1.0, 1.04, r"54.9 fb$^{-1}$ (13 TeV)", transform=ax.transAxes,
            fontsize=12, ha="right", va="bottom")

    fig.tight_layout()
    out = os.path.join(OUT_DIR, "roc_comparison")
    fig.savefig(f"{out}.png", dpi=150, bbox_inches="tight")
    fig.savefig(f"{out}.pdf", bbox_inches="tight")
    print(f"Saved {out}.png and {out}.pdf")


if __name__ == "__main__":
    main()
