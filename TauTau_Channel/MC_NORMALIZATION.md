# TauTau MC normalization

The RDF pipeline uses one complete pre-proton event weight for DY and TTJets:

```text
mc_normalization = luminosity_pb * cross_section_pb / sum_gen_weights

event_weight = Generator_weight
             * mc_normalization
             * tau0_id_sf * tau1_id_sf
             * tau0_trigger_sf * tau1_trigger_sf
```

`weight` and `event_weight` contain the same value in phase 1.

## Physics inputs

| Sample | Cross section | Source files |
| --- | ---: | ---: |
| DYJetsToLL M-50 | 6077.22 pb | 204 |
| TTJets | 831.76 pb | 354 |

The analysis luminosity is 54,900 pb^-1. The exact `sum_gen_weights` is not
hardcoded. It is summed from `Runs.genEventSumw` in every original NanoAOD file.
The calculation fails if any source file is missing, so a partial dataset cannot
silently produce an inflated normalization.

Run the normalization prepass explicitly with:

```bash
cd TauTau_Channel
python3 calculate_mc_normalization.py all
```

Per-file results are cached in `.tautau_mc_normalization.json`. Re-running the
command retries missing files. Use `--refresh` only when the source dataset or
its metadata must be read again. Phase-1 DY and TTJets run this check
automatically before processing events.

## Sample behavior

- Data and data-driven QCD use `weight = event_weight = 1`.
- DY and TTJets use the same formula above, including signed per-event generator
  weights. The old constants `1.81` and `0.15` are not used.
- Tau ID and di-tau trigger scale factors are applied per genuine/fake origin as
  encoded by `Tau_genPartFlav`. Trigger scale factors apply to genuine hadronic
  tau legs.
- Proton acceptance is not included in phase 1. The later proton-mixing step
  must multiply `event_weight` by its TauTau proton factor, never replace it.
- Plotting must use the final weight branch directly. It must not apply an
  additional DY normalization scale or the legacy `1.81`/`0.15` constants.

The phase-1 MC trees also store `mc_luminosity_pb`, `mc_cross_section_pb`,
`mc_sum_gen_weights`, `mc_normalization`, the individual tau ID factors, and the
nominal/up/down trigger factors for auditing.
