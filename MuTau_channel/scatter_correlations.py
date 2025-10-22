#!/usr/bin/env python3

"""
This mirrors the post–correlation-matrix cross-check done in other channels:
for each configured pair of observables we draw a ROOT TGraph populated with
events from the MuTau signal ntuple and save it to disk.  Update the list of
pairs or pass a text file with custom pairs if different variables are needed.
"""

import argparse
import math
from pathlib import Path
from typing import Iterable, List, Tuple

import numpy as np
import ROOT

# Friendly defaults – tweak or override with --pairs-file as needed.
DEFAULT_PAIRS: List[Tuple[str, str]] = [
    ("sist_mass", "mu_pt"),
    ("sist_mass", "tau_pt"),
    ("sist_pt", "acop"),
    ("sist_rap", "acop"),
    ("mu_pt", "tau_pt"),
    ("mu_eta", "tau_eta"),
    ("mu_pt", "met_pt"),
    ("tau_pt", "met_pt"),
    ("xi_arm1_1", "xi_arm2_1"),
    ("sist_mass", "sist_rap"),
]


def read_pairs_file(path: Path) -> List[Tuple[str, str]]:
    pairs: List[Tuple[str, str]] = []
    with path.open() as handle:
        for line in handle:
            stripped = line.strip()
            if not stripped or stripped.startswith("#"):
                continue
            parts = [token.strip() for token in stripped.replace(",", " ").split()]
            if len(parts) != 2:
                raise ValueError(f"Malformed pair line '{line.strip()}' in {path}")
            pairs.append((parts[0], parts[1]))
    if not pairs:
        raise ValueError(f"No variable pairs found in {path}")
    return pairs


def determine_axis_range(values: np.ndarray, padding: float = 0.08) -> Tuple[float, float]:
    finite_values = values[np.isfinite(values)]
    if finite_values.size == 0:
        return -1.0, 1.0
    v_min = float(finite_values.min())
    v_max = float(finite_values.max())
    if math.isclose(v_min, v_max):
        span = 1.0 if v_min == 0 else abs(v_min) * 0.2
        return v_min - span, v_max + span
    delta = (v_max - v_min) * padding
    return v_min - delta, v_max + delta


def create_scatter_plot(
    x_vals: np.ndarray,
    y_vals: np.ndarray,
    x_label: str,
    y_label: str,
    title: str,
    output_path: Path,
) -> None:
    n_points = len(x_vals)
    if n_points == 0:
        print(f"[warn] no entries for {title}, skipping plot")
        return

    x = np.asarray(x_vals, dtype=np.float64)
    y = np.asarray(y_vals, dtype=np.float64)

    x_min, x_max = determine_axis_range(x)
    y_min, y_max = determine_axis_range(y)

    canvas = ROOT.TCanvas("canvas", "", 800, 700)
    canvas.SetMargin(0.14, 0.05, 0.13, 0.05)

    frame = canvas.DrawFrame(x_min, y_min, x_max, y_max, "")
    frame.GetXaxis().SetTitle(x_label)
    frame.GetYaxis().SetTitle(y_label)
    frame.GetXaxis().SetTitleSize(0.045)
    frame.GetYaxis().SetTitleSize(0.045)
    frame.GetXaxis().SetLabelSize(0.04)
    frame.GetYaxis().SetLabelSize(0.04)

    graph = ROOT.TGraph(n_points, x, y)
    graph.SetMarkerStyle(20)
    graph.SetMarkerColor(ROOT.kAzure + 2)
    graph.SetMarkerSize(0.6)
    graph.Draw("P SAME")

    fit = ROOT.TF1("lin_fit", "pol1", x_min, x_max)
    graph.Fit(fit, "Q")
    fit.SetLineColor(ROOT.kRed)
    fit.SetLineWidth(2)
    fit.Draw("same")

    legend = ROOT.TLegend(0.16, 0.82, 0.55, 0.90)
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.SetTextFont(42)
    legend.SetTextSize(0.035)
    legend.AddEntry(graph, "Events", "p")
    legend.AddEntry(fit, "Linear fit", "l")
    legend.Draw()

    title_latex = ROOT.TLatex()
    title_latex.SetNDC(True)
    title_latex.SetTextFont(62)
    title_latex.SetTextSize(0.048)
    title_latex.DrawLatex(0.14, 0.94, title)

    canvas.SaveAs(str(output_path))
    canvas.Close()
    print(f"[info] saved {output_path}")


def load_columns(file_path: Path, columns: Iterable[str]) -> dict:
    unique_columns = sorted(set(columns))
    rdf = ROOT.RDataFrame("tree", str(file_path))
    numpy_arrays = rdf.AsNumpy(unique_columns)
    return numpy_arrays


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="MuTau correlation scatter plots")
    parser.add_argument(
        "--input",
        default="MuTau_sinal_SM_2018_july.root",
        help="Path to the MuTau ROOT file (default: %(default)s)",
    )
    parser.add_argument(
        "--output-dir",
        default="plots_py/scatter",
        help="Directory to store the scatter plots (default: %(default)s)",
    )
    parser.add_argument(
        "--pairs-file",
        type=Path,
        help="Optional text file with variable pairs (one pair per line, e.g. 'sist_mass mu_pt')",
    )
    return parser.parse_args()


def main() -> None:
    ROOT.gROOT.SetBatch(True)
    ROOT.EnableImplicitMT()

    args = parse_arguments()
    input_path = Path(args.input).expanduser().resolve()
    if not input_path.exists():
        raise FileNotFoundError(f"Input ROOT file '{input_path}' not found")

    pairs = DEFAULT_PAIRS if args.pairs_file is None else read_pairs_file(args.pairs_file)

    output_dir = Path(args.output_dir).expanduser().resolve()
    output_dir.mkdir(parents=True, exist_ok=True)

    flat_columns = [var for pair in pairs for var in pair]
    arrays = load_columns(input_path, flat_columns)

    for x_var, y_var in pairs:
        if x_var not in arrays or y_var not in arrays:
            print(f"[warn] Variables '{x_var}'/'{y_var}' not found in tree, skipping")
            continue
        x_vals = arrays[x_var]
        y_vals = arrays[y_var]
        plot_title = f"{x_var} vs {y_var}"
        out_file = output_dir / f"scatter_{x_var}_vs_{y_var}.png"
        create_scatter_plot(x_vals, y_vals, x_var, y_var, plot_title, out_file)


if __name__ == "__main__":
    main()
