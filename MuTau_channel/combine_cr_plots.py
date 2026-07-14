#!/usr/bin/env python3
"""
Combine the 12 individual μτh CR plots into a single 3×4 figure.
Layout: DY (top), TT (middle), QCD (bottom) × aco, mass, pt, rapidity

Run after plot_cr.cpp has produced the PNGs in output_plots_cr/.
"""

import os
import matplotlib.pyplot as plt
import matplotlib.image as mpimg

PLOT_DIR = "output_plots_cr"

plots = {
    "DY":  [
        f"{PLOT_DIR}/DY_CR_acoplanarity.png",
        f"{PLOT_DIR}/DY_CR_mass.png",
        f"{PLOT_DIR}/DY_CR_pt.png",
        f"{PLOT_DIR}/DY_CR_rapidity.png",
    ],
    "TT":  [
        f"{PLOT_DIR}/TT_CR_acoplanarity.png",
        f"{PLOT_DIR}/TT_CR_mass.png",
        f"{PLOT_DIR}/TT_CR_pt.png",
        f"{PLOT_DIR}/TT_CR_rapidity.png",
    ],
    "QCD": [
        f"{PLOT_DIR}/QCD_CR_acoplanarity.png",
        f"{PLOT_DIR}/QCD_CR_mass.png",
        f"{PLOT_DIR}/QCD_CR_pt.png",
        f"{PLOT_DIR}/QCD_CR_rapidity.png",
    ],
}

col_titles = [
    "Acoplanarity of the central system",
    "Invariant mass of the central system [GeV]",
    "Transverse momentum of the central system [GeV]",
    "Rapidity of the central system",
]

row_labels = ["DY CR", "t$\\bar{t}$ CR", "QCD CR"]

# Check for missing files
missing = [f for files in plots.values() for f in files if not os.path.exists(f)]
if missing:
    print("Missing files:")
    for f in missing:
        print(f"  {f}")
    raise SystemExit("Run plot_cr first.")

fig, axes = plt.subplots(3, 4, figsize=(22, 16))
fig.suptitle(
    r"$\mu\tau_h$ final state: DY (top row), $t\bar{t}$ (middle row), QCD (bottom row) CRs."
    "\nAcoplanarity, invariant mass, transverse momentum and rapidity of the central system.",
    fontsize=13, y=0.995,
)

for row, (cr, files) in enumerate(plots.items()):
    for col, png in enumerate(files):
        ax = axes[row, col]
        ax.imshow(mpimg.imread(png))
        ax.axis("off")
        if row == 0:
            ax.set_title(col_titles[col], fontsize=11, pad=6)

for row, label in enumerate(row_labels):
    fig.text(0.005, 0.83 - row * 0.285, label,
             fontsize=12, fontweight="bold",
             rotation=90, va="center", ha="center")

plt.tight_layout(rect=[0.02, 0.01, 1.0, 0.97])
plt.subplots_adjust(hspace=0.03, wspace=0.03)

out_png = f"{PLOT_DIR}/combined_control_regions.png"
out_pdf = f"{PLOT_DIR}/combined_control_regions.pdf"
plt.savefig(out_png, dpi=200, bbox_inches="tight", facecolor="white")
plt.savefig(out_pdf,          bbox_inches="tight", facecolor="white")
print(f"Saved {out_png}")
print(f"Saved {out_pdf}")
