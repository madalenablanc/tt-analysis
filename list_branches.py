#!/usr/bin/env python3
import sys
import glob
import argparse
import ROOT

# Avoid ROOT eating argv meant for argparse
ROOT.PyConfig.IgnoreCommandLineOptions = True


def find_ttrees(root_dir, prefix=""):
    """Recursively collect (path, TTree) from a TFile or TDirectory."""
    trees = []
    keys = root_dir.GetListOfKeys()
    if not keys:
        return trees
    for key in keys:
        obj = key.ReadObj()
        name = obj.GetName()
        path = f"{prefix}/{name}" if prefix else name
        # Recurse into directories (TDirectory/TDirectoryFile)
        if hasattr(obj, "GetListOfKeys") and not isinstance(obj, ROOT.TTree):
            trees.extend(find_ttrees(obj, path))
        # Collect TTrees
        if isinstance(obj, ROOT.TTree):
            trees.append((path, obj))
    return trees


def list_branches(ttree):
    branches = []
    blist = ttree.GetListOfBranches()
    if not blist:
        return branches
    for b in blist:
        # Try to infer type from leaves
        leaves = b.GetListOfLeaves()
        types = set()
        if leaves:
            for lf in leaves:
                try:
                    types.add(lf.GetTypeName())
                except Exception:
                    pass
        branches.append((b.GetName(), ", ".join(sorted(types)) if types else ""))
    return branches


def main():
    ap = argparse.ArgumentParser(description="List branches in ROOT files (all TTrees).")
    ap.add_argument("files", nargs="+", help="ROOT files or glob patterns")
    ap.add_argument("--tree", help="Filter by tree name or path suffix")
    args = ap.parse_args()

    # Expand glob patterns
    file_list = []
    for pat in args.files:
        matched = glob.glob(pat)
        if matched:
            file_list.extend(matched)
        else:
            file_list.append(pat)

    if not file_list:
        print("No input files found.")
        sys.exit(1)

    exit_code = 0
    for fname in file_list:
        f = ROOT.TFile.Open(fname, "READ")
        if not f or f.IsZombie():
            print(f"[!] Could not open: {fname}")
            exit_code = 2
            continue

        print(f"\nFile: {fname}")
        trees = find_ttrees(f)
        if args.tree:
            trees = [(p, t) for (p, t) in trees if (p.endswith("/" + args.tree) or p == args.tree or t.GetName() == args.tree)]

        if not trees:
            print("  (no TTrees found)")
        for path, t in trees:
            try:
                n = t.GetEntries()
            except Exception:
                n = -1
            print(f"  Tree: {path} (entries: {n})")
            brs = list_branches(t)
            if not brs:
                print("    (no branches)")
            else:
                for name, typ in brs:
                    if typ:
                        print(f"    - {name} [{typ}]")
                    else:
                        print(f"    - {name}")

        f.Close()

    sys.exit(exit_code)


if __name__ == "__main__":
    main()

