#!/usr/bin/env python3
import os, sys, time, json, subprocess, signal, argparse, hashlib
import ROOT

# ----------------------- Environment / XRootD knobs -----------------------
os.environ.setdefault("X509_USER_PROXY", f"/tmp/x509up_u{os.getuid()}")
os.environ.setdefault("XrdSecGSISRVNAMES", "*")
# If your site has IPv6 quirks, uncomment:
# os.environ.setdefault("XRD_NETWORKSTACK", "IPv4")
# XRootD timeouts tuned for resilience
os.environ.setdefault("XRD_REQUESTTIMEOUT", "60")
os.environ.setdefault("XRD_CONNECTIONWINDOW", "15")
os.environ.setdefault("XRD_STREAMTIMEOUT", "120")

# Enable MT in the driver; worker will decide MT vs fallback single-thread
ROOT.EnableImplicitMT()

# ----------------------------- Configuration ------------------------------
line_number   = -1   # -1 => process all lines; >=0 => only that index
input_list    = "Data_MuTau_phase0_2018.txt"     # one /store/... path (or full PFN) per line
output_prefix = "/eos/user/m/mblancco/samples_2018_mutau/fase0_mutau/Data_2018_UL_skimmed_MuTau_nano_"
lumi_file     = "dadosluminosidade.txt"
resume_path   = ".mutau_phase0_resume.json"
overwrite     = False
proxy_min_left_sec = 30*60             # auto-renew proxy if < 30 min left

CACHE_DIR     = os.path.expanduser("~/.mutau_cache")
os.makedirs(CACHE_DIR, exist_ok=True)

# Redirectors to try (NO trailing slash)
REDIRS = [
    "root://cms-xrd-global.cern.ch",
    "root://xrootd-cms.infn.it",
]
extra = os.environ.get("CMS_XRD_FALLBACKS", "")
if extra.strip():
    REDIRS += [h.strip() for h in extra.split(",") if h.strip()]

# ------------------------------- Resume I/O --------------------------------
def load_resume(path):
    try:
        with open(path) as f: return json.load(f)
    except Exception:
        return {"done": []}

def save_resume(path, state):
    tmp = path + ".tmp"
    with open(tmp, "w") as f: json.dump(state, f, indent=2, sort_keys=True)
    os.replace(tmp, path)

resume = load_resume(resume_path)

def _sig_handler(signum, frame):
    print("\n📝 Caught signal, writing resume file…")
    save_resume(resume_path, resume)
    sys.exit(1)

signal.signal(signal.SIGINT, _sig_handler)
signal.signal(signal.SIGTERM, _sig_handler)

# ------------------------------- Proxy utils -------------------------------
def proxy_timeleft_sec():
    try:
        out = subprocess.check_output(["voms-proxy-info", "-timeleft"], text=True).strip()
        return int(out)
    except Exception:
        return 0

def ensure_proxy(min_left=proxy_min_left_sec):
    left = proxy_timeleft_sec()
    if left >= min_left:
        return True
    print(f"🔐 Proxy low/absent (timeleft={left}s). Attempting voms-proxy-init …")
    try:
        subprocess.run(["voms-proxy-init", "-voms", "cms", "-valid", "24:00"], check=True)
        new_left = proxy_timeleft_sec()
        print(f"🔐 New proxy timeleft: {new_left}s")
        return new_left > 0
    except Exception as e:
        print(f"⛔ voms-proxy-init failed: {e}")
        return False

# ------------------------ XRootD fallback + caching ------------------------
def build_url(redir, p):
    """Return root://host//store/... (always double slash before /store)."""
    return f"{redir}//{p.lstrip('/')}"

def open_with_fallbacks(store_or_pfn, retries=1, sleep=1.0, renew_proxy=True):
    """
    Try PFN directly (if provided), else iterate redirectors.
    On failure, optionally renew proxy and retry once.
    Return first working URL string, or None.
    """
    candidates = []
    if store_or_pfn.startswith("root://"):
        candidates.append(store_or_pfn)
    else:
        for r in REDIRS:
            candidates.append(build_url(r, store_or_pfn))

    for url in candidates:
        for attempt in range(retries + 1):
            try:
                f = ROOT.TFile.Open(url)
                if not f or f.IsZombie():
                    raise OSError("Zombie/null TFile")
                f.Close()
                return url
            except Exception as e:
                print(f"⚠️  Open failed [{attempt+1}/{retries+1}] on {url}: {e}")
                time.sleep(sleep)

    if renew_proxy and ensure_proxy():
        for url in candidates:
            try:
                f = ROOT.TFile.Open(url)
                if not f or f.IsZombie():
                    raise OSError("Zombie/null TFile")
                f.Close()
                print("✅ Succeeded after proxy renewal.")
                return url
            except Exception as e:
                print(f"⚠️  Retry after renew failed on {url}: {e}")

    return None

def cache_key_from_path(pfn):
    base = os.path.basename(pfn)
    h = hashlib.md5(pfn.encode()).hexdigest()[:10]
    return f"{h}_{base}"

def xrdcp_with_retries(pfn, dst, tries=3, sleep=5):
    for k in range(tries):
        try:
            subprocess.run(["xrdcp", "-f", "-s", "--cksum", "adler32", pfn, dst],
                           check=True, text=True, capture_output=True)
            return True
        except subprocess.CalledProcessError as e:
            print(f"⚠️ xrdcp try {k+1}/{tries} failed: {e.stderr.strip()}")
            time.sleep(sleep)
    return False

def local_cache_or_none(pfn):
    fname = cache_key_from_path(pfn)
    local = os.path.join(CACHE_DIR, fname)
    if os.path.exists(local) and os.path.getsize(local) > 0:
        return local
    ok = xrdcp_with_retries(pfn, local, tries=3)
    return local if ok else None

# ------------------------------ Lumi mask C++ ------------------------------
def load_lumi_list(filename):
    certified = {}
    with open(filename) as f:
        lines = f.readlines()
    current_run = None
    for line in lines:
        parts = line.strip().split()
        if not parts:
            continue
        nums = list(map(int, parts))
        if nums[0] > 50000:
            current_run = nums[0]
            certified[current_run] = []
            pairs = nums[1:]
        else:
            pairs = nums
        for i in range(0, len(pairs), 2):
            if i + 1 < len(pairs):
                certified[current_run].append((pairs[i], pairs[i+1]))
    return certified

certified_lumis = load_lumi_list(lumi_file)

cpp_lumi = [
    'bool lumi_filter(ULong64_t run, ULong64_t lumi) {',
    '  std::map<ULong64_t, std::vector<std::pair<ULong64_t, ULong64_t>>> good;'
]
for run, ranges in certified_lumis.items():
    pairs = ", ".join([f"{{{a}, {b}}}" for (a, b) in ranges])
    cpp_lumi.append(f"  good[{run}] = {{{pairs}}};")
cpp_lumi += [
    '  auto it = good.find(run);',
    '  if (it == good.end()) return false;',
    '  for (const auto &rg : it->second) {',
    '    if (lumi >= rg.first && lumi <= rg.second) return true;',
    '  }',
    '  return false;',
    '}'
]
ROOT.gInterpreter.Declare("\n".join(cpp_lumi))

# --------------------------- Input list & indices --------------------------
with open(input_list) as f:
    lines = [ln.strip() for ln in f if ln.strip()]

if line_number >= 0 and line_number >= len(lines):
    raise IndexError(f"line_number {line_number} >= number of files {len(lines)}")

indices = range(len(lines)) if line_number < 0 else [line_number]

# ----------------------------- Snapshot columns ---------------------------
COLUMNS = [
    "muon_id","tau_id1","tau_id2","tau_id3",
    "muon_pt","tau_pt",
    "muon_charge","tau_charge",
    "muon_eta","tau_eta",
    "muon_n","tau_n",
    "muon_phi","tau_phi",
    "muon_mass","tau_mass",
    "sist_mass","acop","sist_pt","sist_rap",
    "met_pt","met_phi",
    "jet_pt","jet_eta","jet_phi","jet_mass","jet_btag",
    "weight","n_b_jet","generator_weight"
]

# ------------------------------ Worker logic ------------------------------
def build_df_defs(df):
    """Apply MuTau selections and define output columns on a given RDF."""
    df_sel = (
        df.Filter("lumi_filter(run, luminosityBlock)", "Certified lumi")
          .Filter("HLT_IsoMu24 == 1", "HLT single-muon 2018")
          .Filter("Muon_pt.size() > 0 && Tau_pt.size() > 0", "Muon and tau present")
          .Filter("Muon_mvaId[0] >= 3", "Muon ID (>= Medium)")
          .Filter("Tau_idDeepTau2017v2p1VSjet[0] >= 63", "Tau VSjet (>= VeryTight bit)")
          .Filter("Tau_idDeepTau2017v2p1VSe[0]   >= 7",  "Tau VSe (>= Tight bit)")
          .Filter("Tau_idDeepTau2017v2p1VSmu[0]  >= 1",  "Tau VSmu (>= Loose bit)")
          .Filter("Muon_pt[0] > 35. && Tau_pt[0] > 100.", "pT thresholds")
          .Filter("Muon_charge[0] * Tau_charge[0] < 0", "Opposite sign")
    )
    df_defs = (
        df_sel
        .Define("muon_pt",    "Muon_pt[0]")
        .Define("tau_pt",     "Tau_pt[0]")
        .Define("muon_eta",   "Muon_eta[0]")
        .Define("tau_eta",    "Tau_eta[0]")
        .Define("muon_phi",   "Muon_phi[0]")
        .Define("tau_phi",    "Tau_phi[0]")
        .Define("muon_mass",  "Muon_mass[0]")
        .Define("tau_mass",   "Tau_mass[0]")
        .Define("muon_charge","Muon_charge[0]")
        .Define("tau_charge", "Tau_charge[0]")
        .Define("muon_id",    "Muon_mvaId[0]")
        .Define("tau_id1",    "Tau_idDeepTau2017v2p1VSjet[0]")
        .Define("tau_id2",    "Tau_idDeepTau2017v2p1VSe[0]")
        .Define("tau_id3",    "Tau_idDeepTau2017v2p1VSmu[0]")
        .Define("muon_n",     "int(Muon_pt.size())")
        .Define("tau_n",      "int(Tau_pt.size())")
        .Define("muon", "TLorentzVector m; m.SetPtEtaPhiM(muon_pt,muon_eta,muon_phi,muon_mass); return m;")
        .Define("tau",  "TLorentzVector t; t.SetPtEtaPhiM(tau_pt,tau_eta,tau_phi,tau_mass); return t;")
        .Define("sistema",  "return muon + tau;")
        .Define("sist_mass","sistema.M()")
        .Define("sist_pt",  "sistema.Pt()")
        .Define("sist_rap", "sistema.Rapidity()")
        .Define("acop",     "double dphi=fabs(muon_phi-tau_phi); if(dphi>M_PI) dphi=2*M_PI-dphi; return fabs(dphi)/M_PI;")
        .Define("met_pt",   "MET_pt")
        .Define("met_phi",  "MET_phi")
        .Define("jet_pt",   "Jet_pt.size()>0   ? Jet_pt[0]   : -1")
        .Define("jet_eta",  "Jet_eta.size()>0  ? Jet_eta[0]  : -999")
        .Define("jet_phi",  "Jet_phi.size()>0  ? Jet_phi[0]  : -999")
        .Define("jet_mass", "Jet_mass.size()>0 ? Jet_mass[0] : -999")
        .Define("jet_btag", "Jet_btagDeepB.size()>0 ? Jet_btagDeepB[0] : -999")
        .Define("n_b_jet",  "int nb=0; for (auto b: Jet_btagDeepB) if (b>0.4506) ++nb; return nb;")
        .Define("generator_weight", "1.0")
        .Define("weight",          "1.0")
    )
    return df_defs

def process_index(idx):
    """Worker: process exactly one file index; return POSIX exit code."""
    # Decide MT in worker dynamically; first attempt with MT enabled
    # (Driver already enabled MT globally.)

    path = lines[idx]  # "/store/...root" or full PFN
    out_file = f"{output_prefix}{idx}.root"
    part_file = out_file + ".part"

    # Skip if already completed (unless overwrite)
    if (not overwrite) and os.path.exists(out_file):
        print(f"⏭️  Index {idx}: output exists, skipping.")
        return 0

    # Ensure local dirs for outputs
    out_dir = os.path.dirname(out_file)
    if out_dir and not out_dir.startswith("root://"):
        os.makedirs(out_dir, exist_ok=True)
    part_dir = os.path.dirname(part_file)
    if part_dir and not part_dir.startswith("root://"):
        os.makedirs(part_dir, exist_ok=True)

    # Ensure proxy & resolve a working PFN
    ensure_proxy()
    pfn = open_with_fallbacks(path, retries=1, sleep=1.0, renew_proxy=True)
    if not pfn:
        print(f"⛔ Index {idx}: unreachable across all redirectors: {path}")
        return 2

    print(f"📦 Index {idx} input : {pfn}")
    print(f"📤 Index {idx} output: {out_file}")

    def try_rdf(input_spec, use_mt=True):
        """Build RDF, apply selection, snapshot. Return True/False."""
        try:
            if use_mt:
                ROOT.EnableImplicitMT()
            else:
                ROOT.DisableImplicitMT()
            df = ROOT.RDataFrame("Events", input_spec)
            df_defs = build_df_defs(df)
            if os.path.exists(part_file): os.remove(part_file)
            df_defs.Snapshot("tree", part_file, COLUMNS)
            os.replace(part_file, out_file)
            return True
        except Exception as e:
            mode = "MT" if use_mt else "ST"
            print(f"⚠️ RDF ({mode}) failed on {input_spec}: {e}")
            return False

    # A) Remote, with MT first → fallback ST
    if try_rdf(pfn, use_mt=True) or try_rdf(pfn, use_mt=False):
        print(f"✅ finished {'remote-MT' if os.path.exists(out_file) else 'remote-ST'} {out_file}")
        return 0

    # B) Local cache fallback (xrdcp), again MT→ST
    print("🔁 Falling back to local cache via xrdcp …")
    local = local_cache_or_none(pfn)
    if not local:
        print("⛔ xrdcp fallback failed")
        return 3

    if try_rdf(local, use_mt=True) or try_rdf(local, use_mt=False):
        print(f"✅ finished LOCAL {out_file}")
        return 0

    print("⛔ failed remote and local processing (both MT and ST)")
    return 4

# ------------------------------- Driver mode -------------------------------
def run_worker(single_idx):
    # Run worker in a subprocess (isolates potential segfaults)
    env = os.environ.copy()
    return subprocess.run(
        [sys.executable, sys.argv[0], "--single-index", str(single_idx)],
        env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True
    )

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--single-index", type=int, default=None,
                        help="Internal: run worker for a single index.")
    args = parser.parse_args()

    # Worker mode
    if args.single_index is not None:
        sys.exit(process_index(args.single_index))

    # Driver mode
    for idx in indices:
        out_file = f"{output_prefix}{idx}.root"
        if (not overwrite) and (os.path.exists(out_file) or idx in resume.get("done", [])):
            print(f"⏭️  Skipping index {idx} (already done).")
            continue

        res = run_worker(idx)
        print(res.stdout, end="")
        if res.returncode == 0:
            if idx not in resume["done"]:
                resume["done"].append(idx)
            save_resume(resume_path, resume)
        else:
            print(f"⛔ worker failed for idx={idx} (exit {res.returncode})")

    print("✅ MuTau phase-0 data skimming completed for requested files.")

if __name__ == "__main__":
    main()
