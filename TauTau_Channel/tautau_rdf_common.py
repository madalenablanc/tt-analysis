#!/usr/bin/env python3
from __future__ import annotations

import hashlib
import json
import math
import os
import signal
import subprocess
import sys
import time
from pathlib import Path

import ROOT


ROOT.EnableImplicitMT()

BASE_DIR = Path(__file__).resolve().parent
PROXY_MIN_LEFT_SEC = 30 * 60
MC_LUMINOSITY_PB = 54_900.0
MC_NORMALIZATION_CACHE = BASE_DIR / ".tautau_mc_normalization.json"
REDIRECTORS = [
    "root://cms-xrd-global.cern.ch",
    "root://xrootd-cms.infn.it",
]
CACHE_DIR = Path(os.path.expanduser("~/.tautau_cache"))
CACHE_DIR.mkdir(parents=True, exist_ok=True)
_TAU_CORRECTIONS_DECLARED = False

MC_NORMALIZATION_CONFIGS = {
    "dy": {
        "input_list": "DY_2018_UL.txt",
        "cross_section_pb": 6077.22,
    },
    "ttjets": {
        "input_list": "ttJets_2018_UL.txt",
        "cross_section_pb": 831.76,
    },
}

PHASE0_COLUMNS = [
    "tau0_id1",
    "tau0_id2",
    "tau0_id3",
    "tau1_id1",
    "tau1_id2",
    "tau1_id3",
    "tau0_pt",
    "tau1_pt",
    "tau0_charge",
    "tau1_charge",
    "tau0_eta",
    "tau1_eta",
    "tau_n",
    "tau0_phi",
    "tau1_phi",
    "tau0_mass",
    "tau1_mass",
    "sist_mass",
    "sist_acop",
    "sist_pt",
    "sist_rap",
    "met_pt",
    "met_phi",
    "jet_pt",
    "jet_eta",
    "jet_phi",
    "jet_mass",
    "jet_btag",
    "weight",
    "n_b_jet",
    "generator_weight",
    "tau0_decay",
    "tau1_decay",
    "tau0_genmatch",
    "tau1_genmatch",
    "nproton_multi",
    "nproton_single",
    "proton_multi_xi",
    "proton_multi_arm",
    "proton_multi_t",
    "proton_multi_thetaX",
    "proton_multi_thetaY",
    "proton_multi_time",
    "proton_multi_timeUnc",
    "proton_single_xi",
]

PHASE1_COLUMNS = [
    "tau0_id1",
    "tau0_id2",
    "tau0_id3",
    "tau1_id1",
    "tau1_id2",
    "tau1_id3",
    "tau0_pt",
    "tau1_pt",
    "tau0_charge",
    "tau1_charge",
    "tau0_eta",
    "tau1_eta",
    "tau_n",
    "tau0_phi",
    "tau1_phi",
    "tau0_mass",
    "tau1_mass",
    "sist_mass",
    "sist_acop",
    "sist_pt",
    "sist_rap",
    "met_pt",
    "met_phi",
    "jet_pt",
    "jet_eta",
    "jet_phi",
    "jet_mass",
    "jet_btag",
    "weight",
    "n_b_jet",
    "generator_weight",
    "tau0_decay",
    "tau1_decay",
    "tau0_genmatch",
    "tau1_genmatch",
    "nproton_multi",
    "nproton_single",
    "proton_multi_xi",
    "proton_multi_arm",
    "proton_multi_t",
    "proton_multi_thetaX",
    "proton_multi_thetaY",
    "proton_multi_time",
    "proton_multi_timeUnc",
    "proton_single_xi",
    "event_weight",
]

PHASE1_DY_EXTRA_COLUMNS = [
    "syst_tau_id_VSjet",
    "syst_tau_id_VSe",
    "syst_tau_id_VSmu",
    "tau0_pt_up",
    "tau0_pt_dw",
    "tau1_pt_up",
    "tau1_pt_dw",
    "sist_mass_up",
    "sist_mass_dw",
    "sist_pt_up",
    "sist_pt_dw",
]

PHASE1_MC_WEIGHT_COLUMNS = [
    "tau0_id_sf",
    "tau1_id_sf",
    "tau_id_sf",
    "tau0_trigger_sf",
    "tau1_trigger_sf",
    "tau_trigger_sf",
    "tau_trigger_sf_up",
    "tau_trigger_sf_down",
    "mc_luminosity_pb",
    "mc_cross_section_pb",
    "mc_sum_gen_weights",
    "mc_normalization",
]

PHASE0_CONFIGS = {
    "data": {
        "input_list": "QCD_2018_UL.txt",
        "input_prefix": "root://cms-xrd-global.cern.ch//",
        "output_prefix": "/eos/user/m/mblancco/samples_2018_tautau/fase0_rdf/QCD_2018_UL_skimmed_TauTau_nano_",
        "resume_path": ".tautau_phase0_qcd_resume.json",
        "is_data": True,
        "apply_lumi": True,
        "trigger": "HLT_DoubleMediumChargedIsoPFTauHPS35_Trk1_eta2p1_Reg == 1",
    },
    "qcd": {
        "input_list": "QCD_2018_UL.txt",
        "input_prefix": "root://cms-xrd-global.cern.ch//",
        "output_prefix": "/eos/user/m/mblancco/samples_2018_tautau/fase0_rdf/QCD_2018_UL_skimmed_TauTau_nano_",
        "resume_path": ".tautau_phase0_qcd_resume.json",
        "is_data": True,
        "apply_lumi": True,
        "trigger": "HLT_DoubleMediumChargedIsoPFTauHPS35_Trk1_eta2p1_Reg == 1",
    },
    "dy": {
        "input_list": "DY_2018_UL.txt",
        "input_prefix": "root://cms-xrd-global.cern.ch//",
        "output_prefix": "/eos/user/m/mblancco/samples_2018_tautau/fase0_rdf/DY_2018_UL_skimmed_TauTau_nano_",
        "resume_path": ".tautau_phase0_dy_resume.json",
        "is_data": False,
        "apply_lumi": False,
        "trigger": "HLT_DoubleMediumChargedIsoPFTauHPS40_Trk1_eta2p1_Reg == 1",
    },
    "ttjets": {
        "input_list": "ttJets_2018_UL.txt",
        "input_prefix": "root://cms-xrd-global.cern.ch//",
        "output_prefix": "/eos/user/m/mblancco/samples_2018_tautau/fase0_rdf/ttJets_2018_UL_skimmed_TauTau_nano_",
        "resume_path": ".tautau_phase0_ttjets_resume.json",
        "is_data": False,
        "apply_lumi": False,
        "trigger": "HLT_DoubleMediumChargedIsoPFTauHPS40_Trk1_eta2p1_Reg == 1",
    },
}

PHASE1_CONFIGS = {
    "data": {
        "phase0_sample": "qcd",
        "output_prefix": "/eos/user/m/mblancco/samples_2018_tautau/fase1_rdf/Dados_2018_UL_skimmed_TauTau_cuts_",
        "resume_path": ".tautau_phase1_data_resume.json",
    },
    "qcd": {
        "phase0_sample": "qcd",
        "output_prefix": "/eos/user/m/mblancco/samples_2018_tautau/fase1_rdf/QCD_2018_UL_skimmed_TauTau_cuts_",
        "resume_path": ".tautau_phase1_qcd_resume.json",
    },
    "dy": {
        "phase0_sample": "dy",
        "output_prefix": "/eos/user/m/mblancco/samples_2018_tautau/fase1_rdf/DY_2018_UL_skimmed_TauTau_cuts_",
        "resume_path": ".tautau_phase1_dy_resume.json",
    },
    "ttjets": {
        "phase0_sample": "ttjets",
        "output_prefix": "/eos/user/m/mblancco/samples_2018_tautau/fase1_rdf/ttJets_2018_UL_skimmed_TauTau_cuts_",
        "resume_path": ".tautau_phase1_ttjets_resume.json",
    },
}


def init_environment(with_proxy: bool = False) -> None:
    os.environ.setdefault("X509_USER_PROXY", f"/tmp/x509up_u{os.getuid()}")
    os.environ.setdefault("XrdSecGSISRVNAMES", "*")
    os.environ.setdefault("XRD_REQUESTTIMEOUT", "10")
    os.environ.setdefault("XRD_CONNECTIONWINDOW", "5")
    os.environ.setdefault("XRD_STREAMTIMEOUT", "10")
    if with_proxy:
        subprocess.run(
            "voms-proxy-init --rfc --voms cms --valid 172:00",
            shell=True,
            executable="/bin/bash",
            check=False,
        )


def load_resume(path: Path) -> dict:
    try:
        with path.open() as handle:
            return json.load(handle)
    except Exception:
        return {"done": []}


def save_resume(path: Path, state: dict) -> None:
    tmp = path.with_suffix(path.suffix + ".tmp")
    with tmp.open("w") as handle:
        json.dump(state, handle, indent=2, sort_keys=True)
    os.replace(tmp, path)


def install_signal_handlers(path: Path, state: dict) -> None:
    def _handler(signum, frame):
        print("\nCaught signal, writing resume file...")
        save_resume(path, state)
        sys.exit(1)

    signal.signal(signal.SIGINT, _handler)
    signal.signal(signal.SIGTERM, _handler)


def proxy_timeleft_sec() -> int:
    try:
        output = subprocess.check_output(["voms-proxy-info", "-timeleft"], text=True).strip()
        return int(output)
    except Exception:
        return 0


def ensure_proxy(min_left: int = PROXY_MIN_LEFT_SEC) -> bool:
    left = proxy_timeleft_sec()
    if left >= min_left:
        return True
    try:
        subprocess.run(["voms-proxy-init", "-voms", "cms", "-valid", "24:00"], check=True)
        return proxy_timeleft_sec() > 0
    except Exception:
        return False


def read_lines(path: Path) -> list[str]:
    if not path.exists():
        raise FileNotFoundError(f"Input list not found: {path}")
    with path.open() as handle:
        return [
            line.strip()
            for line in handle
            if line.strip() and not line.lstrip().startswith("#")
        ]


def phase1_input_files(sample: str) -> list[str]:
    cfg = PHASE1_CONFIGS[sample]
    phase0_sample = cfg.get("phase0_sample")
    if phase0_sample is None:
        return read_lines(BASE_DIR / cfg["input_list"])

    phase0_cfg = PHASE0_CONFIGS[phase0_sample]
    source_paths = read_lines(BASE_DIR / phase0_cfg["input_list"])
    return [f'{phase0_cfg["output_prefix"]}{idx}.root' for idx in range(len(source_paths))]


def build_xrd_url(redirector: str, path: str) -> str:
    return f"{redirector}//{path.lstrip('/')}"


def candidate_urls(path: str) -> list[str]:
    urls = []
    if path.startswith("root://"):
        urls.append(path)
        store_idx = path.find("/store/")
        if store_idx != -1:
            store_path = path[store_idx:]
            for redirector in REDIRECTORS:
                candidate = build_xrd_url(redirector, store_path)
                if candidate not in urls:
                    urls.append(candidate)
    else:
        for redirector in REDIRECTORS:
            urls.append(build_xrd_url(redirector, path))
    return urls


def open_with_fallbacks(path: str, retries: int = 1, sleep_s: float = 1.0, renew_proxy: bool = True) -> str | None:
    for url in candidate_urls(path):
        for _ in range(retries + 1):
            try:
                root_file = ROOT.TFile.Open(url)
                if not root_file or root_file.IsZombie():
                    raise OSError("Zombie/null TFile")
                root_file.Close()
                return url
            except Exception:
                time.sleep(sleep_s)

    if renew_proxy and ensure_proxy():
        for url in candidate_urls(path):
            try:
                root_file = ROOT.TFile.Open(url)
                if not root_file or root_file.IsZombie():
                    raise OSError("Zombie/null TFile")
                root_file.Close()
                return url
            except Exception:
                continue
    return None


def _load_mc_normalization_cache() -> dict:
    try:
        with MC_NORMALIZATION_CACHE.open() as handle:
            cache = json.load(handle)
        if cache.get("version") != 1 or not isinstance(cache.get("samples"), dict):
            raise ValueError("unsupported cache schema")
        return cache
    except Exception:
        return {"version": 1, "samples": {}}


def _source_list_hash(paths: list[str]) -> str:
    payload = "\n".join(paths).encode()
    return hashlib.sha256(payload).hexdigest()


def _read_runs_metadata(path: str, retries: int = 2) -> dict:
    last_error = "no redirector was attempted"
    for url in candidate_urls(path):
        for attempt in range(1, retries + 1):
            root_file = None
            try:
                root_file = ROOT.TFile.Open(url, "READ")
                if not root_file or root_file.IsZombie():
                    raise OSError("zombie/null TFile")

                runs = root_file.Get("Runs")
                if not runs:
                    raise KeyError("missing Runs tree")

                branch_names = {branch.GetName() for branch in runs.GetListOfBranches()}
                sumw_branch = next(
                    (name for name in ("genEventSumw", "genEventSumw_") if name in branch_names),
                    None,
                )
                if sumw_branch is None:
                    raise KeyError("Runs tree has no genEventSumw branch")

                count_branch = next(
                    (name for name in ("genEventCount", "genEventCount_") if name in branch_names),
                    None,
                )
                sum_gen_weights = 0.0
                event_count = 0
                for run_entry in runs:
                    sum_gen_weights += float(getattr(run_entry, sumw_branch))
                    if count_branch is not None:
                        event_count += int(getattr(run_entry, count_branch))

                if not math.isfinite(sum_gen_weights) or sum_gen_weights == 0.0:
                    raise ValueError(f"invalid genEventSumw total: {sum_gen_weights}")

                return {
                    "sum_gen_weights": sum_gen_weights,
                    "event_count": event_count,
                    "resolved_url": url,
                }
            except Exception as exc:
                last_error = f"{url} (attempt {attempt}/{retries}): {exc}"
                time.sleep(1.0)
            finally:
                if root_file:
                    root_file.Close()

    raise RuntimeError(f"could not read Runs metadata for {path}: {last_error}")


def calculate_mc_normalization(sample: str, refresh: bool = False) -> dict:
    """Calculate L*xsec/sum(genEventSumw) from the complete original dataset."""
    if sample not in MC_NORMALIZATION_CONFIGS:
        raise ValueError(f"No MC normalization configuration for sample '{sample}'")

    cfg = MC_NORMALIZATION_CONFIGS[sample]
    source_list = BASE_DIR / cfg["input_list"]
    paths = read_lines(source_list)
    if not paths:
        raise ValueError(f"MC source list is empty: {source_list}")
    if len(paths) != len(set(paths)):
        raise ValueError(f"MC source list contains duplicate files: {source_list}")
    source_hash = _source_list_hash(paths)
    cache = _load_mc_normalization_cache()
    cached = cache["samples"].get(sample, {})

    if refresh or cached.get("source_hash") != source_hash:
        cached = {"files": {}}

    file_metadata = cached.setdefault("files", {})
    file_metadata = {
        path: metadata
        for path, metadata in file_metadata.items()
        if path in paths
        and math.isfinite(float(metadata.get("sum_gen_weights", 0.0)))
        and float(metadata.get("sum_gen_weights", 0.0)) != 0.0
    }
    cached.update(
        {
            "source_list": cfg["input_list"],
            "source_hash": source_hash,
            "luminosity_pb": MC_LUMINOSITY_PB,
            "cross_section_pb": cfg["cross_section_pb"],
            "files": file_metadata,
        }
    )
    cache["samples"][sample] = cached
    save_resume(MC_NORMALIZATION_CACHE, cache)

    missing = [path for path in paths if path not in file_metadata]
    if missing:
        ensure_proxy()
        print(
            f"Calculating {sample} normalization from Runs.genEventSumw "
            f"({len(missing)} missing of {len(paths)} files)"
        )

    failures = []
    for position, path in enumerate(missing, start=1):
        print(f"  Runs metadata {position}/{len(missing)}: {path}")
        try:
            file_metadata[path] = _read_runs_metadata(path)
            cached.pop("failures", None)
            save_resume(MC_NORMALIZATION_CACHE, cache)
        except Exception as exc:
            failures.append({"path": path, "error": str(exc)})
            print(f"  ERROR: {exc}")

    if failures:
        cached["failures"] = failures
        cached["complete"] = False
        save_resume(MC_NORMALIZATION_CACHE, cache)
        raise RuntimeError(
            f"{sample} normalization is incomplete: {len(failures)} of {len(paths)} "
            f"Runs trees failed. Re-run to retry only those files."
        )

    still_missing = [path for path in paths if path not in file_metadata]
    if still_missing:
        raise RuntimeError(
            f"{sample} normalization cache is incomplete: {len(still_missing)} files are missing"
        )

    sum_gen_weights = math.fsum(
        float(file_metadata[path]["sum_gen_weights"]) for path in paths
    )
    event_count = sum(int(file_metadata[path].get("event_count", 0)) for path in paths)
    normalization = MC_LUMINOSITY_PB * float(cfg["cross_section_pb"]) / sum_gen_weights
    if not math.isfinite(normalization) or normalization <= 0.0:
        raise ValueError(f"Invalid {sample} MC normalization: {normalization}")

    cached.update(
        {
            "complete": True,
            "n_files": len(paths),
            "event_count": event_count,
            "sum_gen_weights": sum_gen_weights,
            "normalization": normalization,
        }
    )
    cached.pop("failures", None)
    save_resume(MC_NORMALIZATION_CACHE, cache)

    print(f"{sample} MC normalization:")
    print(f"  luminosity       = {MC_LUMINOSITY_PB:.1f} pb^-1")
    print(f"  cross section    = {float(cfg['cross_section_pb']):.8g} pb")
    print(f"  generator events = {event_count:,}")
    print(f"  sum gen weights  = {sum_gen_weights:.17g}")
    print(f"  L*xsec/sumw      = {normalization:.17g}")
    return cached


def snapshot_atomically(df, output_file: str, columns: list[str]) -> None:
    out_path = Path(output_file)
    part_path = out_path.with_suffix(out_path.suffix + ".part")
    if part_path.exists():
        part_path.unlink()
    df.Snapshot("tree", str(part_path), columns)
    os.replace(part_path, out_path)


def cache_key_from_path(path: str) -> str:
    base = os.path.basename(path)
    digest = hashlib.md5(path.encode()).hexdigest()[:10]
    return f"{digest}_{base}"


def xrdcp_with_retries(src: str, dst: Path, tries: int = 3, sleep_s: float = 5.0) -> bool:
    for attempt in range(tries):
        try:
            subprocess.run(
                ["xrdcp", "-f", "-s", "--cksum", "adler32", src, str(dst)],
                check=True,
                text=True,
                capture_output=True,
            )
            return True
        except subprocess.CalledProcessError as exc:
            stderr = (exc.stderr or "").strip()
            print(f"xrdcp try {attempt + 1}/{tries} failed: {stderr}")
            time.sleep(sleep_s)
    return False


def local_cache_or_none(pfn: str) -> str | None:
    local = CACHE_DIR / cache_key_from_path(pfn)
    if local.exists() and local.stat().st_size > 0:
        return str(local)
    return str(local) if xrdcp_with_retries(pfn, local, tries=3) else None


def declare_lumi_filter(lumi_file: Path) -> None:
    certified = {}
    current_run = None
    with lumi_file.open() as handle:
        for raw_line in handle:
            parts = raw_line.strip().split()
            if not parts:
                continue
            numbers = list(map(int, parts))
            if numbers[0] > 50000:
                current_run = numbers[0]
                certified[current_run] = []
                pairs = numbers[1:]
            else:
                pairs = numbers
            for index in range(0, len(pairs), 2):
                if index + 1 < len(pairs):
                    certified[current_run].append((pairs[index], pairs[index + 1]))

    cpp_lines = [
        "#include <map>",
        "#include <utility>",
        "#include <vector>",
        "bool tautau_lumi_filter(ULong64_t run, ULong64_t lumi) {",
        "  static std::map<ULong64_t, std::vector<std::pair<ULong64_t, ULong64_t>>> good = {",
    ]
    items = []
    for run, ranges in certified.items():
        payload = ", ".join(f"{{{start}, {stop}}}" for start, stop in ranges)
        items.append(f"    {{{run}, {{{payload}}}}}")
    cpp_lines.append(",\n".join(items))
    cpp_lines.extend(
        [
            "  };",
            "  auto it = good.find(run);",
            "  if (it == good.end()) return false;",
            "  for (const auto &rg : it->second) {",
            "    if (lumi >= rg.first && lumi <= rg.second) return true;",
            "  }",
            "  return false;",
            "}",
        ]
    )
    ROOT.gInterpreter.Declare("\n".join(cpp_lines))


def declare_tau_corrections() -> None:
    global _TAU_CORRECTIONS_DECLARED
    if _TAU_CORRECTIONS_DECLARED:
        return

    jet_file = str((BASE_DIR / "TauID_SF_pt_DeepTau2017v2p1VSjet_UL2018.root").resolve())
    vse_file = str((BASE_DIR / "TauID_SF_eta_DeepTau2017v2p1VSe_UL2018.root").resolve())
    vsmu_file = str((BASE_DIR / "TauID_SF_eta_DeepTau2017v2p1VSmu_UL2018.root").resolve())
    tes_file = str((BASE_DIR / "TauES_dm_DeepTau2017v2p1VSjet_UL2018.root").resolve())
    trigger_file = str(
        (BASE_DIR / "POGCorrections/2018UL_tauTriggerEff_DeepTau2017v2p1.root").resolve()
    )

    declared = ROOT.gInterpreter.Declare(
        f"""
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include "TFile.h"
#include "TF1.h"
#include "TH1.h"

namespace tautau_corr {{
  static TFile *jetFile = nullptr;
  static TFile *vseFile = nullptr;
  static TFile *vsmuFile = nullptr;
  static TFile *tesFile = nullptr;
  static TFile *triggerFile = nullptr;
  static TF1 *jetCent = nullptr;
  static TF1 *jetUp = nullptr;
  static TH1F *vseLoose = nullptr;
  static TH1F *vsmuLoose = nullptr;
  static TH1F *tesHist = nullptr;
  static TH1F *triggerHist = nullptr;

  void load() {{
    if (jetFile) return;
    jetFile = TFile::Open("{jet_file}");
    vseFile = TFile::Open("{vse_file}");
    vsmuFile = TFile::Open("{vsmu_file}");
    tesFile = TFile::Open("{tes_file}");
    triggerFile = TFile::Open("{trigger_file}");
    if (!jetFile || !vseFile || !vsmuFile || !tesFile || !triggerFile) {{
      throw std::runtime_error("Failed to open TauTau correction files");
    }}
    jetCent = dynamic_cast<TF1*>(jetFile->Get("VTight_cent"));
    jetUp = dynamic_cast<TF1*>(jetFile->Get("VTight_up"));
    vseLoose = dynamic_cast<TH1F*>(vseFile->Get("Loose"));
    vsmuLoose = dynamic_cast<TH1F*>(vsmuFile->Get("Loose"));
    tesHist = dynamic_cast<TH1F*>(tesFile->Get("tes"));
    triggerHist = dynamic_cast<TH1F*>(triggerFile->Get("sf_ditau_VTight_dmall_fitted"));
    if (!jetCent || !jetUp || !vseLoose || !vsmuLoose || !tesHist || !triggerHist) {{
      throw std::runtime_error("Failed to load TauTau correction objects");
    }}
  }}

  double clampAxisValue(const TH1F *hist, double value) {{
    const auto xmin = hist->GetXaxis()->GetXmin() + 1e-6;
    const auto xmax = hist->GetXaxis()->GetXmax() - 1e-6;
    return std::max(xmin, std::min(xmax, value));
  }}

  double tauVSjetSF(double pt) {{
    load();
    return jetCent->Eval(pt);
  }}

  double tauVSjetErr(double pt) {{
    load();
    return jetUp->Eval(pt) - jetCent->Eval(pt);
  }}

  double tauVSeSF(double eta) {{
    load();
    const auto x = clampAxisValue(vseLoose, eta);
    return vseLoose->GetBinContent(vseLoose->FindBin(x));
  }}

  double tauVSeErr(double eta) {{
    load();
    const auto x = clampAxisValue(vseLoose, eta);
    return vseLoose->GetBinError(vseLoose->FindBin(x));
  }}

  double tauVSmuSF(double eta) {{
    load();
    const auto x = clampAxisValue(vsmuLoose, eta);
    return vsmuLoose->GetBinContent(vsmuLoose->FindBin(x));
  }}

  double tauVSmuErr(double eta) {{
    load();
    const auto x = clampAxisValue(vsmuLoose, eta);
    return vsmuLoose->GetBinError(vsmuLoose->FindBin(x));
  }}

  double tauTES(double decay, int shift) {{
    load();
    const auto bin = tesHist->FindBin(decay);
    const auto cent = tesHist->GetBinContent(bin);
    const auto err = tesHist->GetBinError(bin);
    if (shift > 0) return cent + err;
    if (shift < 0) return cent - err;
    return cent;
  }}

  double tauTriggerSF(double pt, int shift) {{
    load();
    const auto x = clampAxisValue(triggerHist, pt);
    const auto bin = triggerHist->FindBin(x);
    const auto cent = triggerHist->GetBinContent(bin);
    if (shift > 0) return cent + triggerHist->GetBinErrorUp(bin);
    if (shift < 0) return cent - triggerHist->GetBinErrorLow(bin);
    return cent;
  }}
}}
"""
    )
    if not declared:
        raise RuntimeError("Failed to declare TauTau correction helpers")
    _TAU_CORRECTIONS_DECLARED = True


def apply_tau_id_sf_defines(df):
    return (
        df.Define(
            "tau0_id_sf",
            """
            if (tau0_genmatch == 5) return tautau_corr::tauVSjetSF(tau0_pt);
            if (tau0_genmatch == 1 || tau0_genmatch == 3) return tautau_corr::tauVSeSF(tau0_eta);
            if (tau0_genmatch == 2 || tau0_genmatch == 4) return tautau_corr::tauVSmuSF(tau0_eta);
            return 1.0;
            """,
        )
        .Define(
            "tau1_id_sf",
            """
            if (tau1_genmatch == 5) return tautau_corr::tauVSjetSF(tau1_pt);
            if (tau1_genmatch == 1 || tau1_genmatch == 3) return tautau_corr::tauVSeSF(tau1_eta);
            if (tau1_genmatch == 2 || tau1_genmatch == 4) return tautau_corr::tauVSmuSF(tau1_eta);
            return 1.0;
            """,
        )
    )


def apply_tau_trigger_sf_defines(df):
    return (
        df.Define(
            "tau0_trigger_sf",
            "tau0_genmatch == 5 ? tautau_corr::tauTriggerSF(tau0_pt, 0) : 1.0",
        )
        .Define(
            "tau1_trigger_sf",
            "tau1_genmatch == 5 ? tautau_corr::tauTriggerSF(tau1_pt, 0) : 1.0",
        )
        .Define(
            "tau0_trigger_sf_up",
            "tau0_genmatch == 5 ? tautau_corr::tauTriggerSF(tau0_pt, +1) : 1.0",
        )
        .Define(
            "tau1_trigger_sf_up",
            "tau1_genmatch == 5 ? tautau_corr::tauTriggerSF(tau1_pt, +1) : 1.0",
        )
        .Define(
            "tau0_trigger_sf_down",
            "tau0_genmatch == 5 ? tautau_corr::tauTriggerSF(tau0_pt, -1) : 1.0",
        )
        .Define(
            "tau1_trigger_sf_down",
            "tau1_genmatch == 5 ? tautau_corr::tauTriggerSF(tau1_pt, -1) : 1.0",
        )
        .Define("tau_trigger_sf", "tau0_trigger_sf * tau1_trigger_sf")
        .Define("tau_trigger_sf_up", "tau0_trigger_sf_up * tau1_trigger_sf_up")
        .Define(
            "tau_trigger_sf_down",
            "tau0_trigger_sf_down * tau1_trigger_sf_down",
        )
    )


def apply_mc_weight_defines(df, normalization: dict):
    luminosity_pb = float(normalization["luminosity_pb"])
    cross_section_pb = float(normalization["cross_section_pb"])
    sum_gen_weights = float(normalization["sum_gen_weights"])
    mc_normalization = float(normalization["normalization"])

    return (
        df.Define("tau_id_sf", "tau0_id_sf * tau1_id_sf")
        .Define("mc_luminosity_pb", f"double({luminosity_pb:.17g})")
        .Define("mc_cross_section_pb", f"double({cross_section_pb:.17g})")
        .Define("mc_sum_gen_weights", f"double({sum_gen_weights:.17g})")
        .Define("mc_normalization", f"double({mc_normalization:.17g})")
        .Redefine(
            "weight",
            "generator_weight * mc_normalization * tau_id_sf * tau_trigger_sf",
        )
        .Define("event_weight", "weight")
    )


def apply_common_phase0_defines(df, is_data: bool):
    generator_weight = "1.0" if is_data else "Generator_weight"
    columns = {str(name) for name in df.GetColumnNames()}

    df = (
        df.Filter("Tau_pt.size() >= 2", "At least 2 taus")
        .Define("tau0_pt", "Tau_pt[0]")
        .Define("tau1_pt", "Tau_pt[1]")
        .Define("tau0_eta", "Tau_eta[0]")
        .Define("tau1_eta", "Tau_eta[1]")
        .Define("tau0_phi", "Tau_phi[0]")
        .Define("tau1_phi", "Tau_phi[1]")
        .Define("tau0_mass", "Tau_mass[0]")
        .Define("tau1_mass", "Tau_mass[1]")
        .Define("tau0_charge", "Tau_charge[0]")
        .Define("tau1_charge", "Tau_charge[1]")
        .Define("tau0_id1", "Tau_idDeepTau2017v2p1VSjet[0]")
        .Define("tau0_id2", "Tau_idDeepTau2017v2p1VSe[0]")
        .Define("tau0_id3", "Tau_idDeepTau2017v2p1VSmu[0]")
        .Define("tau1_id1", "Tau_idDeepTau2017v2p1VSjet[1]")
        .Define("tau1_id2", "Tau_idDeepTau2017v2p1VSe[1]")
        .Define("tau1_id3", "Tau_idDeepTau2017v2p1VSmu[1]")
        .Define("tau_n", "int(Tau_pt.size())")
        .Define(
            "tau0_vec",
            "TLorentzVector tau; tau.SetPtEtaPhiM(tau0_pt, tau0_eta, tau0_phi, tau0_mass); return tau;",
        )
        .Define(
            "tau1_vec",
            "TLorentzVector tau; tau.SetPtEtaPhiM(tau1_pt, tau1_eta, tau1_phi, tau1_mass); return tau;",
        )
        .Define("sistema", "return tau0_vec + tau1_vec;")
        .Define("sist_mass", "sistema.M()")
        .Define("sist_pt", "sistema.Pt()")
        .Define("sist_rap", "sistema.Rapidity()")
        .Define(
            "sist_acop",
            """
            double dphi = fabs(tau1_phi - tau0_phi);
            if (dphi > M_PI) dphi = 2 * M_PI - dphi;
            return fabs(dphi) / M_PI;
            """,
        )
        .Define("met_pt", "MET_pt")
        .Define("met_phi", "MET_phi")
        .Define("jet_pt", "Jet_pt.size() > 0 ? Jet_pt[0] : -999.0f")
        .Define("jet_eta", "Jet_eta.size() > 0 ? Jet_eta[0] : -999.0f")
        .Define("jet_phi", "Jet_phi.size() > 0 ? Jet_phi[0] : -999.0f")
        .Define("jet_mass", "Jet_mass.size() > 0 ? Jet_mass[0] : -999.0f")
        .Define("jet_btag", "Jet_btagDeepB.size() > 0 ? Jet_btagDeepB[0] : -999.0f")
        .Define(
            "n_b_jet",
            "int nb = 0; for (auto score : Jet_btagDeepB) if (score > 0.4506) ++nb; return nb;",
        )
        .Define("weight", "1.0")
        .Define("generator_weight", generator_weight)
        .Define("tau0_decay", is_data and "0.0" or "Tau_decayMode[0]")
        .Define("tau1_decay", is_data and "0.0" or "Tau_decayMode[1]")
        .Define("tau0_genmatch", is_data and "0.0" or "Tau_genPartFlav[0]")
        .Define("tau1_genmatch", is_data and "0.0" or "Tau_genPartFlav[1]")
    )

    scalar_defaults = {
        "nproton_multi": ("nProton_multiRP", "0"),
        "nproton_single": ("nProton_singleRP", "0"),
    }
    vector_defaults = {
        "proton_multi_xi": ("Proton_multiRP_xi", "ROOT::RVecF{}"),
        "proton_multi_arm": ("Proton_multiRP_arm", "ROOT::RVecI{}"),
        "proton_multi_t": ("Proton_multiRP_t", "ROOT::RVecF{}"),
        "proton_multi_thetaX": ("Proton_multiRP_thetaX", "ROOT::RVecF{}"),
        "proton_multi_thetaY": ("Proton_multiRP_thetaY", "ROOT::RVecF{}"),
        "proton_multi_time": ("Proton_multiRP_time", "ROOT::RVecF{}"),
        "proton_multi_timeUnc": ("Proton_multiRP_timeUnc", "ROOT::RVecF{}"),
        "proton_single_xi": ("Proton_singleRP_xi", "ROOT::RVecF{}"),
    }

    for output_name, (source_name, default_expr) in scalar_defaults.items():
        df = df.Define(output_name, source_name if source_name in columns else default_expr)
    for output_name, (source_name, default_expr) in vector_defaults.items():
        df = df.Define(output_name, source_name if source_name in columns else default_expr)

    return df


def run_phase0(sample: str, line_number: int = -1, overwrite: bool = True) -> None:
    cfg = PHASE0_CONFIGS[sample]
    resume_path = BASE_DIR / cfg["resume_path"]
    input_list = BASE_DIR / cfg["input_list"]
    state = load_resume(resume_path)
    install_signal_handlers(resume_path, state)

    if cfg["apply_lumi"]:
        declare_lumi_filter(BASE_DIR / "dadosluminosidade.txt")

    lines = read_lines(input_list)
    if line_number >= 0 and line_number >= len(lines):
        raise IndexError(f"line_number {line_number} >= number of files {len(lines)}")

    indices = range(len(lines)) if line_number < 0 else [line_number]
    for file_idx, idx in enumerate(indices, start=1):
        rel_path = lines[idx]
        output_file = f'{cfg["output_prefix"]}{idx}.root'
        if not overwrite and (os.path.exists(output_file) or idx in state.get("done", [])):
            print(f"Skipping index {idx} (already done).")
            continue

        ensure_proxy()
        requested_input = rel_path if rel_path.startswith("root://") else cfg["input_prefix"] + rel_path
        input_file = open_with_fallbacks(requested_input, retries=1, sleep_s=1.0, renew_proxy=True)
        if not input_file:
            print(f"Unreachable across all redirectors: {requested_input}")
            continue
        print(f"\nProcessing file {file_idx}/{len(indices)}")
        print(f"Input : {input_file}")
        print(f"Output: {output_file}")

        out_dir = os.path.dirname(output_file)
        if out_dir:
            os.makedirs(out_dir, exist_ok=True)

        def try_rdf(input_spec: str, use_mt: bool) -> bool:
            try:
                if use_mt:
                    ROOT.EnableImplicitMT()
                else:
                    ROOT.DisableImplicitMT()
                df = ROOT.RDataFrame("Events", input_spec)
                if cfg["apply_lumi"]:
                    df = df.Filter("tautau_lumi_filter(run, luminosityBlock)", "Certified lumi")
                df = df.Filter(cfg["trigger"], "Tau trigger")
                df = apply_common_phase0_defines(df, cfg["is_data"])
                snapshot_atomically(df, output_file, PHASE0_COLUMNS)
                return True
            except Exception as exc:
                mode = "MT" if use_mt else "ST"
                print(f"RDF ({mode}) failed on {input_spec}: {exc}")
                return False

        if try_rdf(input_file, use_mt=True) or try_rdf(input_file, use_mt=False):
            if idx not in state["done"]:
                state["done"].append(idx)
            save_resume(resume_path, state)
            continue

        print("Falling back to local cache via xrdcp ...")
        local = local_cache_or_none(input_file)
        if not local:
            print(f"xrdcp fallback failed for {input_file}")
            continue

        if not (try_rdf(local, use_mt=True) or try_rdf(local, use_mt=False)):
            print(f"Failed remote and local processing for {input_file}")
            continue

        if idx not in state["done"]:
            state["done"].append(idx)
        save_resume(resume_path, state)


def apply_common_phase1_defines(df):
    return (
        df.Define(
            "tau0_vec",
            "TLorentzVector tau; tau.SetPtEtaPhiM(tau0_pt, tau0_eta, tau0_phi, tau0_mass); return tau;",
        )
        .Define(
            "tau1_vec",
            "TLorentzVector tau; tau.SetPtEtaPhiM(tau1_pt, tau1_eta, tau1_phi, tau1_mass); return tau;",
        )
        .Define("delta_r", "return tau0_vec.DeltaR(tau1_vec);")
    )


def run_phase1_data(line_number: int = -1, overwrite: bool = True) -> None:
    run_phase1_simple(
        sample="data",
        line_number=line_number,
        overwrite=overwrite,
        weight_expr="1.0",
        sign_expr="tau0_charge * tau1_charge < 0",
        cuts="""
            tau0_id1 > 64 && tau1_id1 > 64 &&
            tau0_id2 > 8  && tau1_id2 > 8  &&
            tau0_id3 > 2  && tau1_id3 > 2  &&
            tau0_pt > 100. && tau1_pt > 100. &&
            delta_r > 0.4 &&
            fabs(tau0_eta) < 2.4 && fabs(tau1_eta) < 2.4
        """,
    )


def run_phase1_qcd(line_number: int = -1, overwrite: bool = True) -> None:
    run_phase1_simple(
        sample="qcd",
        line_number=line_number,
        overwrite=overwrite,
        weight_expr="1.0",
        sign_expr="tau0_charge * tau1_charge > 0",
        cuts="""
            tau0_id1 > 64 && tau1_id1 > 64 &&
            tau0_id2 > 8  && tau1_id2 > 8  &&
            tau0_id3 > 2  && tau1_id3 > 2  &&
            tau0_pt > 100. && tau1_pt > 100. &&
            delta_r > 0.4 &&
            fabs(tau0_eta) < 2.4 && fabs(tau1_eta) < 2.4
        """,
    )


def run_phase1_ttjets(line_number: int = -1, overwrite: bool = True) -> None:
    cfg = PHASE1_CONFIGS["ttjets"]
    resume_path = BASE_DIR / cfg["resume_path"]
    state = load_resume(resume_path)
    install_signal_handlers(resume_path, state)
    declare_tau_corrections()
    lines = phase1_input_files("ttjets")
    normalization = calculate_mc_normalization("ttjets")

    if line_number >= 0 and line_number >= len(lines):
        raise IndexError(f"line_number {line_number} >= number of files {len(lines)}")

    indices = range(len(lines)) if line_number < 0 else [line_number]
    for file_idx, idx in enumerate(indices, start=1):
        input_file = lines[idx]
        output_file = f'{cfg["output_prefix"]}{idx}.root'
        if not overwrite and (os.path.exists(output_file) or idx in state.get("done", [])):
            print(f"Skipping index {idx} (already done).")
            continue

        print(f"\nProcessing file {file_idx}/{len(indices)}")
        print(f"Input : {input_file}")
        print(f"Output: {output_file}")

        try:
            df = ROOT.RDataFrame("tree", input_file)
        except Exception as exc:
            print(f"Failed to create RDataFrame for {input_file}: {exc}")
            continue
        df = apply_common_phase1_defines(df)
        df = (
            df.Filter(
                """
                tau0_id1 > 64 && tau1_id1 > 64 &&
                tau0_id2 > 8  && tau1_id2 > 8  &&
                tau0_id3 > 2  && tau1_id3 > 2  &&
                tau0_pt > 100. && tau1_pt > 100. &&
                delta_r > 0.4 &&
                fabs(tau0_eta) < 2.4 && fabs(tau1_eta) < 2.4
                """,
                "TauTau ttjets phase-1 cuts",
            )
            .Filter("tau0_charge * tau1_charge < 0", "Opposite-sign")
        )
        df = apply_tau_id_sf_defines(df)
        df = apply_tau_trigger_sf_defines(df)
        df = apply_mc_weight_defines(df, normalization)

        out_dir = os.path.dirname(output_file)
        if out_dir:
            os.makedirs(out_dir, exist_ok=True)
        try:
            snapshot_atomically(
                df,
                output_file,
                PHASE1_COLUMNS + PHASE1_MC_WEIGHT_COLUMNS,
            )
        except Exception as exc:
            print(f"Snapshot failed for {output_file}: {exc}")
            continue

        if idx not in state["done"]:
            state["done"].append(idx)
        save_resume(resume_path, state)


def run_phase1_simple(
    *,
    sample: str,
    line_number: int,
    overwrite: bool,
    weight_expr: str,
    sign_expr: str,
    cuts: str,
) -> None:
    cfg = PHASE1_CONFIGS[sample]
    resume_path = BASE_DIR / cfg["resume_path"]
    state = load_resume(resume_path)
    install_signal_handlers(resume_path, state)
    lines = phase1_input_files(sample)

    if line_number >= 0 and line_number >= len(lines):
        raise IndexError(f"line_number {line_number} >= number of files {len(lines)}")

    indices = range(len(lines)) if line_number < 0 else [line_number]
    for file_idx, idx in enumerate(indices, start=1):
        input_file = lines[idx]
        output_file = f'{cfg["output_prefix"]}{idx}.root'
        if not overwrite and (os.path.exists(output_file) or idx in state.get("done", [])):
            print(f"Skipping index {idx} (already done).")
            continue

        print(f"\nProcessing file {file_idx}/{len(indices)}")
        print(f"Input : {input_file}")
        print(f"Output: {output_file}")

        try:
            df = ROOT.RDataFrame("tree", input_file)
        except Exception as exc:
            print(f"Failed to create RDataFrame for {input_file}: {exc}")
            continue
        df = apply_common_phase1_defines(df)
        df = (
            df.Filter(cuts, "TauTau phase-1 cuts")
            .Filter(sign_expr, "Charge selection")
            .Redefine("weight", weight_expr)
            .Define("event_weight", "weight")
        )

        out_dir = os.path.dirname(output_file)
        if out_dir:
            os.makedirs(out_dir, exist_ok=True)
        try:
            snapshot_atomically(df, output_file, PHASE1_COLUMNS)
        except Exception as exc:
            print(f"Snapshot failed for {output_file}: {exc}")
            continue

        if idx not in state["done"]:
            state["done"].append(idx)
        save_resume(resume_path, state)


def run_phase1_dy(line_number: int = -1, overwrite: bool = True) -> None:
    cfg = PHASE1_CONFIGS["dy"]
    resume_path = BASE_DIR / cfg["resume_path"]
    state = load_resume(resume_path)
    install_signal_handlers(resume_path, state)
    declare_tau_corrections()
    lines = phase1_input_files("dy")
    normalization = calculate_mc_normalization("dy")

    if line_number >= 0 and line_number >= len(lines):
        raise IndexError(f"line_number {line_number} >= number of files {len(lines)}")

    indices = range(len(lines)) if line_number < 0 else [line_number]
    for file_idx, idx in enumerate(indices, start=1):
        input_file = lines[idx]
        output_file = f'{cfg["output_prefix"]}{idx}.root'
        if not overwrite and (os.path.exists(output_file) or idx in state.get("done", [])):
            print(f"Skipping index {idx} (already done).")
            continue

        print(f"\nProcessing file {file_idx}/{len(indices)}")
        print(f"Input : {input_file}")
        print(f"Output: {output_file}")

        try:
            df = ROOT.RDataFrame("tree", input_file)
        except Exception as exc:
            print(f"Failed to create RDataFrame for {input_file}: {exc}")
            continue
        df = apply_common_phase1_defines(df)
        df = (
            df.Filter(
                """
                tau0_id1 > 64 && tau1_id1 > 64 &&
                tau0_id2 > 8  && tau1_id2 > 8  &&
                tau0_id3 > 2  && tau1_id3 > 2  &&
                delta_r > 0.5 &&
                fabs(tau0_eta) < 2.3 && fabs(tau1_eta) < 2.3
                """,
                "TauTau DY phase-1 cuts",
            )
            .Filter("tau0_charge * tau1_charge < 0", "Opposite-sign")
            .Define(
                "syst_tau_id_VSjet",
                """
                double err = 0.0;
                if (tau0_genmatch == 5) err += tautau_corr::tauVSjetErr(tau0_pt);
                if (tau1_genmatch == 5) err += tautau_corr::tauVSjetErr(tau1_pt);
                return err;
                """,
            )
            .Define(
                "syst_tau_id_VSe",
                """
                double err = 0.0;
                if (tau0_genmatch == 1 || tau0_genmatch == 3) err += tautau_corr::tauVSeErr(tau0_eta);
                if (tau1_genmatch == 1 || tau1_genmatch == 3) err += tautau_corr::tauVSeErr(tau1_eta);
                return err;
                """,
            )
            .Define(
                "syst_tau_id_VSmu",
                """
                double err = 0.0;
                if (tau0_genmatch == 2 || tau0_genmatch == 4) err += tautau_corr::tauVSmuErr(tau0_eta);
                if (tau1_genmatch == 2 || tau1_genmatch == 4) err += tautau_corr::tauVSmuErr(tau1_eta);
                return err;
                """,
            )
            .Define("tau0_tes", "tau0_genmatch == 5 ? tautau_corr::tauTES(tau0_decay, 0) : 1.0")
            .Define("tau1_tes", "tau1_genmatch == 5 ? tautau_corr::tauTES(tau1_decay, 0) : 1.0")
            .Define("tau0_tes_up", "tau0_genmatch == 5 ? tautau_corr::tauTES(tau0_decay, +1) : 1.0")
            .Define("tau1_tes_up", "tau1_genmatch == 5 ? tautau_corr::tauTES(tau1_decay, +1) : 1.0")
            .Define("tau0_tes_dw", "tau0_genmatch == 5 ? tautau_corr::tauTES(tau0_decay, -1) : 1.0")
            .Define("tau1_tes_dw", "tau1_genmatch == 5 ? tautau_corr::tauTES(tau1_decay, -1) : 1.0")
            .Redefine("tau0_pt", "tau0_pt * tau0_tes")
            .Redefine("tau1_pt", "tau1_pt * tau1_tes")
            .Define("tau0_pt_up", "tau0_pt * (tau0_tes_up / tau0_tes)")
            .Define("tau1_pt_up", "tau1_pt * (tau1_tes_up / tau1_tes)")
            .Define("tau0_pt_dw", "tau0_pt * (tau0_tes_dw / tau0_tes)")
            .Define("tau1_pt_dw", "tau1_pt * (tau1_tes_dw / tau1_tes)")
            .Filter("tau0_pt > 100. && tau1_pt > 100.", "pT thresholds")
            .Define(
                "syst_vec_up",
                "TLorentzVector t0, t1; t0.SetPtEtaPhiM(tau0_pt_up, tau0_eta, tau0_phi, tau0_mass); t1.SetPtEtaPhiM(tau1_pt_up, tau1_eta, tau1_phi, tau1_mass); return t0 + t1;",
            )
            .Define(
                "syst_vec_dw",
                "TLorentzVector t0, t1; t0.SetPtEtaPhiM(tau0_pt_dw, tau0_eta, tau0_phi, tau0_mass); t1.SetPtEtaPhiM(tau1_pt_dw, tau1_eta, tau1_phi, tau1_mass); return t0 + t1;",
            )
            .Define(
                "nominal_vec",
                "TLorentzVector t0, t1; t0.SetPtEtaPhiM(tau0_pt, tau0_eta, tau0_phi, tau0_mass); t1.SetPtEtaPhiM(tau1_pt, tau1_eta, tau1_phi, tau1_mass); return t0 + t1;",
            )
            .Redefine("sist_mass", "nominal_vec.M()")
            .Redefine("sist_pt", "nominal_vec.Pt()")
            .Redefine("sist_rap", "nominal_vec.Rapidity()")
            .Define("sist_mass_up", "syst_vec_up.M()")
            .Define("sist_mass_dw", "syst_vec_dw.M()")
            .Define("sist_pt_up", "syst_vec_up.Pt()")
            .Define("sist_pt_dw", "syst_vec_dw.Pt()")
        )
        df = apply_tau_id_sf_defines(df)
        df = apply_tau_trigger_sf_defines(df)
        df = apply_mc_weight_defines(df, normalization)

        out_dir = os.path.dirname(output_file)
        if out_dir:
            os.makedirs(out_dir, exist_ok=True)
        try:
            snapshot_atomically(
                df,
                output_file,
                PHASE1_COLUMNS + PHASE1_MC_WEIGHT_COLUMNS + PHASE1_DY_EXTRA_COLUMNS,
            )
        except Exception as exc:
            print(f"Snapshot failed for {output_file}: {exc}")
            continue

        if idx not in state["done"]:
            state["done"].append(idx)
        save_resume(resume_path, state)
