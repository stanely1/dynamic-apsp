"""
Benchmark Results Plotter
Reads benchmark_results.txt and generates min/max/avg time plots
per testcase into the assets/ directory.

(Generated with Claude Sonnet 4.6)
"""

import re
import os
import sys
from collections import defaultdict
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

# ── Configuration ─────────────────────────────────────────────────────────────

INPUT_FILE = "benchmark_results.txt"
OUTPUT_DIR = "assets"

ALGO_COLORS = {
    "Floyd-Warshall":      "#2196F3",   # blue
    "Dijkstra":            "#4CAF50",   # green
    "Demetrescu-Italiano": "#FF5722",   # deep-orange
}
ALGO_MARKERS = {
    "Floyd-Warshall":      "o",
    "Dijkstra":            "s",
    "Demetrescu-Italiano": "^",
}

METRIC_STYLES = {
    "min": dict(linestyle="--",  alpha=0.70, linewidth=1.4),
    "avg": dict(linestyle="-",   alpha=1.00, linewidth=2.2),
    "max": dict(linestyle=":",   alpha=0.70, linewidth=1.4),
}

# ── Parser ────────────────────────────────────────────────────────────────────

def parse_file(path: str):
    """
    Returns a list of testcases:
        [
          {
            "name": str,
            "x_label": str,        # e.g. "n" or "m"
            "x_values": [int, ...],
            "x_tick_labels": [str, ...],   # raw label like "45 (1% full)"
            "algorithms": {
              "AlgoName": {"min": [...], "max": [...], "avg": [...]},
              ...
            }
          },
          ...
        ]
    """
    with open(path, "r") as f:
        lines = f.readlines()

    # Strip header lines (BASELINE / Measuring time …)
    header_re = re.compile(r"^(BASELINE:|Measuring time)")
    data_lines = [l.rstrip("\n") for l in lines
                  if not header_re.match(l.strip())]

    # Split into blocks on blank lines
    blocks: list[list[str]] = []
    current: list[str] = []
    for line in data_lines:
        if line.strip() == "":
            if current:
                blocks.append(current)
                current = []
        else:
            current.append(line)
    if current:
        blocks.append(current)

    # Regexes
    param_re  = re.compile(
        r"^(?P<key>[a-zA-Z]+)\s*=\s*(?P<val>\d+)"
        r"(?:\s*\((?P<pct>[^)]+)\))?\s*:"
    )
    metric_re = re.compile(
        r"^(?P<algo>.+?):\s+min:\s*(?P<mn>[\d.]+),\s*max:\s*(?P<mx>[\d.]+),\s*avg:\s*(?P<av>[\d.]+)"
    )

    testcases = []

    for block in blocks:
        if not block:
            continue

        # First non-empty line is the testcase name
        name = block[0].strip()
        if not name:
            continue

        x_label       = "n"
        x_values      = []
        x_tick_labels = []
        # algo -> list-of-dicts in order of x_values
        algo_rows: dict[str, list[dict]] = defaultdict(list)
        current_x        = None
        current_x_label  = None
        current_metrics: dict[str, dict] = {}

        def flush():
            nonlocal current_x, current_x_label, current_metrics
            if current_x is None:
                return
            x_values.append(current_x)
            x_tick_labels.append(current_x_label)
            for algo, m in current_metrics.items():
                algo_rows[algo].append(m)
            current_metrics = {}

        for line in block[1:]:
            stripped = line.strip()
            if not stripped:
                continue

            m = param_re.match(stripped)
            if m:
                flush()
                key = m.group("key")   # e.g. "n" or "m" or "layer size"
                val = int(m.group("val"))
                pct = m.group("pct")   # e.g. "1% full" or None
                x_label = key
                current_x = val
                current_x_label = str(val) if not pct else f"{val}\n({pct})"
                continue

            # Handle "layer size=NN:" pattern
            ls_m = re.match(r"^layer size\s*=\s*(\d+)\s*:", stripped)
            if ls_m:
                flush()
                val = int(ls_m.group(1))
                x_label = "layer size"
                current_x = val
                current_x_label = str(val)
                continue

            m2 = metric_re.match(stripped)
            if m2:
                algo = m2.group("algo").strip()
                current_metrics[algo] = {
                    "min": float(m2.group("mn")),
                    "max": float(m2.group("mx")),
                    "avg": float(m2.group("av")),
                }

        flush()   # last group

        # Build per-algo arrays aligned with x_values
        algorithms: dict[str, dict[str, list[float]]] = {}
        all_algos = set()
        for rows in algo_rows.values():
            pass
        all_algos = set(algo_rows.keys())

        for algo in all_algos:
            rows = algo_rows[algo]
            if len(rows) != len(x_values):
                # Pad missing entries with None so lengths match
                pass  # we'll handle below
            algorithms[algo] = {
                "min": [r["min"] for r in rows],
                "max": [r["max"] for r in rows],
                "avg": [r["avg"] for r in rows],
            }

        testcases.append({
            "name":          name,
            "x_label":       x_label,
            "x_values":      x_values,
            "x_tick_labels": x_tick_labels,
            "algorithms":    algorithms,
        })

    return testcases


# ── Plotter ───────────────────────────────────────────────────────────────────

def safe_filename(name: str) -> str:
    return re.sub(r"[^\w\-]", "_", name).strip("_")


def plot_testcase(tc: dict, out_dir: str):
    name         = tc["name"]
    x_label      = tc["x_label"]
    x_values     = tc["x_values"]
    x_tick_lbls  = tc["x_tick_labels"]
    algorithms   = tc["algorithms"]

    if not x_values:
        print(f"  [skip] No data for '{name}'")
        return

    fig, axes = plt.subplots(1, 3, figsize=(18, 5.5), sharey=False)
    fig.suptitle(name, fontsize=14, fontweight="bold", y=1.01)

    metrics = ["min", "avg", "max"]
    titles  = ["Minimum Time", "Average Time", "Maximum Time"]

    for ax, metric, title in zip(axes, metrics, titles):
        for algo, data in sorted(algorithms.items()):
            if metric not in data:
                continue
            y = data[metric]
            xs = x_values[:len(y)]

            color  = ALGO_COLORS.get(algo, "#888888")
            marker = ALGO_MARKERS.get(algo, "D")
            style  = METRIC_STYLES[metric]

            ax.plot(
                xs, y,
                label=algo,
                color=color,
                marker=marker,
                markersize=5,
                **style,
            )

        ax.set_title(title, fontsize=12, pad=8)
        ax.set_xlabel(x_label, fontsize=10)
        ax.set_ylabel("Time [ms]", fontsize=10)
        ax.grid(True, linestyle="--", alpha=0.4)
        ax.set_xticks(x_values)
        ax.set_xticklabels(
            x_tick_lbls,
            fontsize=7.5,
            rotation=45,
            ha="right",
        )
        ax.yaxis.set_major_formatter(ticker.FormatStrFormatter("%.1f"))
        ax.legend(fontsize=9, loc="upper left")

    fig.tight_layout()
    fname = os.path.join(out_dir, safe_filename(name) + ".png")
    fig.savefig(fname, dpi=130, bbox_inches="tight")
    plt.close(fig)
    print(f"  Saved: {fname}")


# ── Main ──────────────────────────────────────────────────────────────────────

def main():
    input_file = INPUT_FILE
    if len(sys.argv) > 1:
        input_file = sys.argv[1]

    if not os.path.exists(input_file):
        print(f"Error: '{input_file}' not found.", file=sys.stderr)
        sys.exit(1)

    os.makedirs(OUTPUT_DIR, exist_ok=True)

    print(f"Parsing '{input_file}' …")
    testcases = parse_file(input_file)
    print(f"Found {len(testcases)} testcase(s).\n")

    for tc in testcases:
        print(f"Plotting: {tc['name']}")
        plot_testcase(tc, OUTPUT_DIR)

    print(f"\nDone. All plots saved to '{OUTPUT_DIR}/'.")


if __name__ == "__main__":
    main()
