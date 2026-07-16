#!/usr/bin/env python3
from tautau_rdf_common import init_environment, run_phase1_data


if __name__ == "__main__":
    init_environment(with_proxy=True)
    run_phase1_data()
