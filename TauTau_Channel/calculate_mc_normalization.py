#!/usr/bin/env python3
import argparse

from tautau_rdf_common import calculate_mc_normalization, init_environment


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Calculate TauTau MC normalization from NanoAOD Runs trees."
    )
    parser.add_argument(
        "sample",
        choices=("dy", "ttjets", "all"),
        nargs="?",
        default="all",
    )
    parser.add_argument(
        "--refresh",
        action="store_true",
        help="Discard cached per-file Runs metadata and read every source file again.",
    )
    args = parser.parse_args()

    init_environment(with_proxy=True)
    samples = ("dy", "ttjets") if args.sample == "all" else (args.sample,)
    for sample in samples:
        calculate_mc_normalization(sample, refresh=args.refresh)


if __name__ == "__main__":
    main()
