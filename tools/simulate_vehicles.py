#!/usr/bin/env python3
"""
RoadSense AI - simulated vehicle reports
=========================================
Posts SYNTHETIC reports for one location to the same /potholes node the real
ESP32 writes to, so you can watch the dashboard's confidence score climb
(30 % -> 65 % -> 100 %) without needing several instrumented vehicles.

These are NOT real detections. Every record is tagged  "source": "simulator"
so it can be told apart from (and deleted separately from) real data.
The project has one physical sensor unit; multi-vehicle verification is
demonstrated with this script.

Usage:
    pip install -r requirements.txt
    export ROADSENSE_DB_URL=https://<project>-default-rtdb.<region>.firebasedatabase.app
    python simulate_vehicles.py --lat 13.0827 --lon 80.2707 --severity 3

    # or pass the URL explicitly
    python simulate_vehicles.py --db-url https://... --lat ... --lon ...
"""
import argparse
import os
import random
import sys
import time

import requests


def build_report(lat, lon, severity, jitter_deg):
    """Same fields (and units) as the firmware's upload, plus a source tag.

    The dashboard groups reports by lat/lon rounded to 4 decimals (~11 m).
    To make sure every simulated report lands in the SAME cell as the real
    one, jitter is applied around the centre of that cell, not around the raw
    coordinate (which could sit next to a cell edge and spill over).
    """
    cell_lat, cell_lon = round(lat, 4), round(lon, 4)
    return {
        "latitude": round(cell_lat + random.uniform(-jitter_deg, jitter_deg), 6),
        "longitude": round(cell_lon + random.uniform(-jitter_deg, jitter_deg), 6),
        "deltaG": round(random.uniform(0.35, 0.95), 4),
        "severity": severity,
        "satellites": random.randint(5, 8),
        "timestamp": int(time.time() * 1000),
        "source": "simulator",
    }


def main():
    ap = argparse.ArgumentParser(description="Post simulated vehicle reports to RoadSense AI.")
    ap.add_argument("--db-url", default=os.environ.get("ROADSENSE_DB_URL", ""),
                    help="Firebase Realtime Database URL (or set ROADSENSE_DB_URL)")
    ap.add_argument("--lat", type=float, required=True, help="latitude of the pothole your real unit reported")
    ap.add_argument("--lon", type=float, required=True, help="longitude of the pothole your real unit reported")
    ap.add_argument("--severity", type=int, default=3, choices=range(1, 6), help="severity 1-5 (default 3)")
    ap.add_argument("--count", type=int, default=3, help="number of simulated reports (default 3)")
    ap.add_argument("--interval", type=float, default=3.0, help="seconds between reports (default 3)")
    ap.add_argument("--jitter", type=float, default=0.00004,
                    help="jitter in degrees around the grid-cell centre (must stay < 0.00005 to remain in the cell)")
    args = ap.parse_args()

    if not args.db_url:
        sys.exit("error: pass --db-url or set ROADSENSE_DB_URL")
    url = args.db_url.rstrip("/") + "/potholes.json"

    print(f"Posting {args.count} SIMULATED report(s) near ({args.lat}, {args.lon})")
    print("Watch the dashboard: confidence should climb with each report.\n")
    for i in range(args.count):
        r = requests.post(url, json=build_report(args.lat, args.lon, args.severity, args.jitter), timeout=10)
        print(f"  report {i + 1}/{args.count} -> HTTP {r.status_code}")
        if r.status_code in (401, 403):
            sys.exit("Permission denied - your Firebase rules probably expired. See firebase/README.md")
        r.raise_for_status()
        if i < args.count - 1:
            time.sleep(args.interval)
    print("\nDone.")


if __name__ == "__main__":
    main()
