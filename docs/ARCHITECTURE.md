# Architecture

![data flow](images/architecture.svg)

## 1. Sensing (ESP32 firmware)

| Item | Value |
|---|---|
| Accelerometer | MPU6050 at I2C address `0x68`, read through raw registers (`0x3B/0x3D/0x3F`), no library |
| Range / scale | Default +/-2 g, 16384 LSB per g |
| GPS | NEO-6M, NMEA over UART2 at 9600 baud, parsed with TinyGPSPlus |
| Loop | read GPS buffer -> read accelerometer -> compute deltaG -> maybe trigger -> `delay(500)` |

### Detection rule

```
magnitude = sqrt(ax^2 + ay^2 + az^2)        # in g
deltaG    = | magnitude - previous_magnitude |   # frame-to-frame change
event     = deltaG > 0.30  AND  >3 s since last event
```

* **Frame-to-frame (not "distance from 1 g").** The first prototype compared against a fixed 1 g
  baseline. The final firmware compares consecutive samples instead, so a sustained tilt
  (hill, ramp) or steady acceleration does not keep the detector firing - only *sudden* changes do.
* **Cooldown.** One physical jolt produces two large deltas (up, then back down). The 3 s cooldown
  suppresses the second one; in the logs the sample right after a trigger (deltaG about 1.08)
  was correctly ignored.

### Severity (rule-based)

| deltaG | Severity |
|---|---|
| > 1.0 | 5 (Critical) |
| > 0.7 | 4 (High) |
| > 0.5 | 3 (Moderate) |
| > 0.30 | 2 (Low) |
| otherwise | 1 (Minor) |

Severity 1 is currently unreachable from real data: events only fire above 0.30, which is
already severity 2. The boundaries were chosen by hand from the hand-jolt measurements in
[EXPERIMENTS.md](EXPERIMENTS.md); they have not been calibrated against real potholes.

## 2. Upload

On an event the ESP32 `POST`s JSON to `<database>/potholes.json` (HTTPS, Firebase REST):

| Field | Type | Meaning |
|---|---|---|
| `deltaG` | number | deltaG that triggered the event |
| `severity` | integer 1-5 | rule-based, see above |
| `latitude`, `longitude` | number | GPS position, or `0.0` if there is no fix |
| `satellites` | integer | satellites in view |
| `timestamp` | integer | `millis()` since boot - **not** a wall clock |
| `source` | string, optional | `"simulator"` on synthetic records (firmware does not send it) |

Because `timestamp` is uptime, the dashboard ignores it and instead decodes the real creation time
from the Firebase push-ID (the first 8 characters of every push key encode milliseconds since epoch).

## 3. Dashboard (`dashboard/index.html`)

* Polls `GET <database>/potholes.json` every 2 s. No SDK, no build step, one HTML file.
* Every record is validated (finite numbers, lat/lon in range, severity clamped to 1-5). Anything
  else is dropped, so a malformed or hostile record in an open database cannot break the page or inject HTML.
* Records with `lat = lon = 0` (no GPS fix) are counted but not mapped.
* **Grouping:** latitude/longitude rounded to 4 decimals give a cell of about 11 m.
* **Confidence:** `min(100, 30 + 35 * (reports_in_cell - 1))` -> 1 report 30 %, 2 reports 65 %, 3+ reports 100 %.
  A cell is **verified** at >= 65 %. Marker colour is the maximum severity seen in the cell.

## 4. Multi-vehicle verification - what is and is not real

* The **logic** (grouping + confidence) is real and runs in the dashboard.
* There is **one** physical sensor unit. Additional "vehicles" come from
  [`tools/simulate_vehicles.py`](../tools/simulate_vehicles.py), which posts synthetic records tagged `source: "simulator"`.
* The firmware sends no device id, so confidence counts **reports**, not distinct vehicles. One noisy
  unit driving over the same spot repeatedly would raise confidence by itself.

## 5. Design decisions and trade-offs

| Decision | Why | Cost |
|---|---|---|
| Raw I2C register reads instead of the Adafruit MPU6050 library | The library's `begin()` failed on our module while raw reads worked (possibly a clone reporting a different chip id - not verified) | Less portable; no built-in filtering/range helpers |
| Firebase REST polling instead of the SDK | No API key/config in the page, trivial to host and test | Up to 2 s latency, repeated full reads |
| Rounded-coordinate grid for grouping | Simple, explainable, O(n) | Two reports either side of a cell edge land in different cells (see below) |
| Open demo rules + validation | Fast to set up | Anyone can read all data and add valid-looking records - see [firebase/README.md](../firebase/README.md) |

## 6. Known limitations

1. **Sampling rate ~2 Hz.** At 30 km/h a vehicle moves ~4 m between samples, while a wheel crosses a
   pothole in tens of milliseconds, so real driving would miss most impacts. Needs >= 100 Hz sampling
   (interrupt/FIFO based) with a low-pass filter.
2. **+/-2 g range.** Hard impacts can clip. Set +/-8 g or higher for road use.
3. **No classification.** Nothing distinguishes a pothole from a speed breaker, braking, or a door slam.
   Detection is a threshold; the "AI" in the name is the roadmap, not the current code.
4. **Not evaluated.** No labelled dataset, no precision/recall, no on-vehicle road tests yet.
5. **Grid-edge splitting.** Grouping by rounded coordinates can split one pothole across two cells.
   Planned fix: DBSCAN / geohash clustering with a distance threshold.
6. **No device identity or authentication** on uploads.
7. **No-fix events** are uploaded with coordinates `0,0` (and ignored by the map).
8. **`connectWiFi()` blocks** until connected; detection does not run while offline.
9. **Mounting and vehicle dependence** are untested: different vehicles/suspensions respond differently
   to the same defect, so absolute deltaG thresholds will not transfer between vehicles without calibration.

## 7. Roadmap

- [ ] Log raw accelerometer data (>= 100 Hz) from a real vehicle with labels (pothole / speed breaker / braking / smooth)
- [ ] Evaluate the threshold (precision, recall, ROC) and replace hand-picked severity bands
- [ ] Train and evaluate a classifier (features: peak, duration, energy, axis ratios) against the threshold baseline
- [ ] Per-device id + authenticated uploads; count distinct vehicles for confidence
- [ ] Distance-based clustering instead of the rounded grid
- [ ] Enclosure, vehicle power, mounting study
- [ ] Deterioration trend per segment from repeated detections over time
