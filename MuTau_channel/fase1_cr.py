#!/usr/bin/env python3
"""Prepare simple MuTau trees for control-region plots.

The phase-0 files already contain the flat MuTau variables used by plot_cr.cpp.
This script only standardizes their names, applies the common selection, and
keeps the requested charge category.  It intentionally does not apply the
control-region cuts; those remain visible in the plotting code.

Examples:
  python3 fase1_cr.py --sample data --sign all
  python3 fase1_cr.py --sample data --sign ss
  python3 fase1_cr.py --sample dy --sign all
  python3 fase1_cr.py --sample ttjets --sign all
  python3 fase1_cr.py --sample qcd --sign all
"""

import argparse
import json
import os
import signal
from pathlib import Path

import ROOT

ROOT.EnableImplicitMT()

BASE = Path(__file__).resolve().parent
EOS_BASE = "/eos/user/m/mblancco/samples_2018_mutau"
OUTPUT_BASE = Path(EOS_BASE) / "fase1_cr_mutau_v2"

SAMPLES = {
    "data": {
        "input_list": BASE / "Data_QCD_MuTau_skimmed_2018.txt",
        "input_prefix": "/eos/home-m/mblancco/samples_2018_mutau/fase0_mutau_proton_vars/",
    },
    "qcd": {
        "input_list": BASE / "Data_QCD_MuTau_skimmed_2018.txt",
        "input_prefix": "/eos/home-m/mblancco/samples_2018_mutau/fase0_mutau_proton_vars/",
    },
    "dy": {
        "input_list": BASE / "DY_MuTau_skimmed_2018.txt",
        "input_prefix": f"{EOS_BASE}/fase0_DY/",
    },
    "ttjets": {
        "input_list": BASE / "TTJets_MuTau_skimmed_2018.txt",
        "input_prefix": f"{EOS_BASE}/fase0_ttjets/",
    },
}

BASE_COLUMNS = [
    "mu_id", "tau_id1", "tau_id2", "tau_id3",
    "mu_pt", "tau_pt", "mu_charge", "tau_charge",
    "mu_eta", "tau_eta", "mu_n", "tau_n",
    "mu_phi", "tau_phi", "mu_mass", "tau_mass",
    "sist_mass", "acop", "sist_pt", "sist_rap",
    "met_pt", "met_phi", "jet_pt", "jet_eta", "jet_phi",
    "jet_mass", "jet_btag", "n_b_jet", "weight",
    "generator_weight", "event_weight",
]

PROTON_COLUMNS = [
    "nproton_multi", "nproton_single", "proton_multi_xi",
    "proton_multi_arm", "proton_multi_t", "proton_multi_thetaX",
    "proton_multi_thetaY", "proton_multi_time", "proton_multi_timeUnc",
    "proton_single_xi",
]


def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--sample", choices=sorted(SAMPLES), required=True)
    parser.add_argument("--sign", choices=("os", "ss", "all"), default="all")
    parser.add_argument("--line", type=int, default=-1,
                        help="Process one input-list index; -1 means all")
    parser.add_argument("--input-list", type=Path)
    parser.add_argument("--input-prefix")
    parser.add_argument("--output-dir", type=Path,
                        help="Override the default phase-1 output directory")
    return parser.parse_args()


def load_resume(path):
    try:
        with path.open() as handle:
            return json.load(handle)
    except Exception:
        return {"done": []}


def save_resume(path, state):
    temporary = path.with_suffix(path.suffix + ".tmp")
    with temporary.open("w") as handle:
        json.dump(state, handle, indent=2, sort_keys=True)
    temporary.replace(path)


def install_signal_handlers(path, state):
    def handler(signum, frame):
        print("\nCaught signal; writing resume file...")
        save_resume(path, state)
        raise SystemExit(1)

    signal.signal(signal.SIGINT, handler)
    signal.signal(signal.SIGTERM, handler)


def add_alias(df, available, target, *sources):
    """Create a canonical column from the first matching source column."""
    if target in available:
        return df
    for source in sources:
        if source in available:
            df = df.Define(target, source)
            available.add(target)
            return df
    raise RuntimeError(f"Missing input column for {target}: {sources}")


def add_column_if_missing(df, available, name, expression):
    if name in available:
        return df
    df = df.Define(name, expression)
    available.add(name)
    return df


def valid_root_output(path):
    """Check that a completed Snapshot contains the expected tree."""
    root_file = ROOT.TFile.Open(str(path), "READ")
    if not root_file or root_file.IsZombie():
        if root_file:
            root_file.Close()
        return False
    valid = bool(root_file.Get("tree"))
    root_file.Close()
    return valid


def prepare_tree(input_file, sample, sign):
    df = ROOT.RDataFrame("tree", input_file)
    available = {str(column) for column in df.GetColumnNames()}

    aliases = {
        "mu_id": ("mu_id", "muon_id"),
        "mu_pt": ("mu_pt", "muon_pt"),
        "mu_eta": ("mu_eta", "muon_eta"),
        "mu_phi": ("mu_phi", "muon_phi"),
        "mu_mass": ("mu_mass", "muon_mass"),
        "mu_charge": ("mu_charge", "muon_charge"),
        "mu_n": ("mu_n", "muon_n"),
        "acop": ("acop", "sist_acop"),
    }
    for target, sources in aliases.items():
        df = add_alias(df, available, target, *sources)

    # Phase-0 data and MC files normally already contain these columns.  The
    # definitions make failures explicit instead of silently producing zeros.
    for column in BASE_COLUMNS:
        if column in ("generator_weight", "event_weight", "weight"):
            continue
        if column not in available:
            raise RuntimeError(f"Missing required input column: {column}")

    df = add_column_if_missing(df, available, "weight", "1.0")
    df = add_column_if_missing(
        df,
        available,
        "generator_weight",
        "1.0" if "generator_weight" not in available else "generator_weight",
    )

    if sample == "dy":
        weight_expression = "generator_weight"
    elif sample == "ttjets":
        # Match fase1_ttjets.py: generator_weight is not used for ttbar.
        # plot_cr.cpp applies the proton factor separately.
        weight_expression = "0.15"
    else:
        weight_expression = "1.0"
    df = add_column_if_missing(df, available, "event_weight", weight_expression)

    df = df.Define(
        "delta_r_cr",
        """
        TLorentzVector mu, tau;
        mu.SetPtEtaPhiM(mu_pt, mu_eta, mu_phi, mu_mass);
        tau.SetPtEtaPhiM(tau_pt, tau_eta, tau_phi, tau_mass);
        return mu.DeltaR(tau);
        """,
    )

    df = (
        df.Filter("mu_id >= 3", "Muon ID")
          .Filter("tau_id1 > 63 && tau_id2 > 7 && tau_id3 > 1", "Tau IDs")
          .Filter("mu_pt > 35. && tau_pt > 100.", "pT thresholds")
          .Filter("delta_r_cr > 0.4", "Delta R")
          .Filter("fabs(mu_eta) < 2.4 && fabs(tau_eta) < 2.4", "Acceptance")
    )

    if sign == "os":
        df = df.Filter("mu_charge * tau_charge < 0", "Opposite sign")
    elif sign == "ss":
        df = df.Filter("mu_charge * tau_charge > 0", "Same sign")

    return df, available


def main():
    args = parse_args()
    config = SAMPLES[args.sample]
    input_list = args.input_list or config["input_list"]
    input_prefix = args.input_prefix or config["input_prefix"]
    output_dir = args.output_dir or OUTPUT_BASE / args.sample / args.sign
    output_dir.mkdir(parents=True, exist_ok=True)

    with input_list.open() as handle:
        paths = [line.strip() for line in handle if line.strip()]
    if args.line >= 0 and args.line >= len(paths):
        raise IndexError(f"--line {args.line} is outside {input_list}")
    indices = range(len(paths)) if args.line < 0 else [args.line]

    resume_path = BASE / f".fase1_cr_{args.sample}_{args.sign}.json"
    resume = load_resume(resume_path)
    install_signal_handlers(resume_path, resume)

    for index in indices:
        source = paths[index]
        input_file = source if source.startswith(("root://", "/")) else input_prefix + source
        output_file = output_dir / f"{args.sample}_{args.sign}_MuTau_cr_{index}.root"

        if output_file.exists():
            if valid_root_output(output_file):
                print(f"Skipping existing output {index}: {output_file}")
                continue
            print(f"Removing incomplete CR output {index}: {output_file}")
            output_file.unlink()
        elif index in resume.get("done", []):
            resume["done"].remove(index)

        print(f"[{args.sample}, {args.sign}] {index}: {input_file}")
        try:
            df, available = prepare_tree(input_file, args.sample, args.sign)
            columns = [column for column in BASE_COLUMNS if column in available]
            columns += [column for column in PROTON_COLUMNS if column in available]
            snapshot = df.Snapshot("tree", str(output_file), columns)
            snapshot.GetValue()  # Force completion before opening the next file.
            del snapshot
            del df
            if not valid_root_output(output_file):
                raise RuntimeError(f"Snapshot did not produce a valid ROOT file: {output_file}")
            resume.setdefault("done", []).append(index)
            save_resume(resume_path, resume)
            print(f"  wrote {output_file}")
        except Exception as error:
            print(f"  failed: {error}")


if __name__ == "__main__":
    main()
