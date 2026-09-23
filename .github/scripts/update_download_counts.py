#!/usr/bin/env python3
"""Build cumulative GitHub Release Asset download counters for NUSA firmware."""

import json
import os
import re
import urllib.request
from pathlib import Path

REPO = os.environ.get(
    "GITHUB_REPOSITORY",
    "stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE",
)
TOKEN = os.environ.get("GITHUB_TOKEN", "")

CATEGORIES = {
    "digi-normal": ("DIGI Normal", "blue"),
    "digi-standalone": ("DIGI Standalone", "blue"),
    "igate-normal": ("iGATE Normal", "blue"),
    "igate-standalone": ("iGATE Standalone", "blue"),
    "esp32-igate": ("ESP32 iGATE", "blue"),
    "esp32c3-igate": ("ESP32-C3 iGATE", "blue"),
}


def github_get(url):
    headers = {
        "Accept": "application/vnd.github+json",
        "User-Agent": "nusa-download-counter",
        "X-GitHub-Api-Version": "2022-11-28",
    }
    if TOKEN:
        headers["Authorization"] = f"Bearer {TOKEN}"
    request = urllib.request.Request(url, headers=headers)
    try:
        with urllib.request.urlopen(request, timeout=30) as response:
            return json.load(response)
    except urllib.error.HTTPError as exc:
        print(f"Warning: GitHub API error ({exc.code}) for {url}: {exc.reason}")
        return []
    except Exception as exc:
        print(f"Warning: Failed to fetch {url}: {exc}")
        return []


def get_all_releases():
    releases = []
    page = 1
    while True:
        url = f"https://api.github.com/repos/{REPO}/releases?per_page=100&page={page}"
        batch = github_get(url)
        if not isinstance(batch, list) or len(batch) == 0:
            break
        releases.extend(batch)
        if len(batch) < 100:
            break
        page += 1
    return releases


def classify_asset(filename):
    name = filename.upper()

    if name.endswith(".ZIP"):
        if "ESP32" in name and "IGATE" in name:
            if re.search(r"ESP32[-_]?C3", name):
                return "esp32c3-igate"
            return "esp32-igate"
        return None

    # .packed.bin and ordinary .bin are both covered here, including UV-K5 V3.
    if not name.endswith(".BIN"):
        return None

    if "IGATE" in name:
        return "igate-standalone" if "STANDALONE" in name else "igate-normal"

    if "DIGI" in name:
        return "digi-standalone" if "STANDALONE" in name else "digi-normal"

    return None


def badge_payload(label, count, color):
    return {
        "schemaVersion": 1,
        "label": label,
        "message": f"{count} downloads",
        "color": color,
        "cacheSeconds": 300,
    }


def write_json(path, data):
    p = Path(path)
    p.parent.mkdir(parents=True, exist_ok=True)
    p.write_text(json.dumps(data, indent=2) + "\n", encoding="utf-8")


def main():
    counts = {key: 0 for key in CATEGORIES}

    for release in get_all_releases():
        if release.get("draft"):
            continue
        for asset in release.get("assets", []):
            category = classify_asset(asset.get("name", ""))
            if category:
                counts[category] += int(asset.get("download_count", 0))

    total = sum(counts.values())

    for key, (label, color) in CATEGORIES.items():
        write_json(
            f"badges/{key}.json",
            badge_payload(label, counts[key], color),
        )

    write_json(
        "badges/total.json",
        badge_payload("TOTAL Firmware", total, "brightgreen"),
    )

    write_json(
        "badges/download-counts.json",
        {
            "repository": REPO,
            "description": (
                "Cumulative GitHub Release Asset download counts "
                "grouped by firmware family."
            ),
            "counts": {
                "digi_normal": counts["digi-normal"],
                "digi_standalone": counts["digi-standalone"],
                "igate_normal": counts["igate-normal"],
                "igate_standalone": counts["igate-standalone"],
                "esp32_igate": counts["esp32-igate"],
                "esp32c3_igate": counts["esp32c3-igate"],
                "total": total,
            },
        },
    )

    print("Cumulative firmware download counts:")
    for key, (label, _) in CATEGORIES.items():
        print(f"  {label}: {counts[key]}")
    print(f"  TOTAL Firmware: {total}")


if __name__ == "__main__":
    main()
