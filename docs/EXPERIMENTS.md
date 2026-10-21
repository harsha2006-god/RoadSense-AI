# Experiments and measurements

What was actually measured while building the prototype, and - just as important - what was **not**.

## 1. Sensor sanity check
MPU6050 lying still, raw register read, scaled at 16384 LSB/g:

| Ax | Ay | Az | Note |
|---|---|---|---|
| 0.326 g | 0.055 g | -1.018 g | ~1 g on Z = gravity; values drift slightly between reads |

I2C scan found the (replacement) module at `0x68`.

## 2. First deltaG characterisation (deviation from 1 g, 10 Hz)

| Condition | Observed deltaG |
|---|---|
| Still / normal handling | ~0.00 - 0.09 g |
| Firmer hand taps | ~0.14 - 0.21 g (max seen 0.205) |

Hand taps are not road impacts; this only showed the signal responds and gave a noise floor.

## 3. Final firmware (frame-to-frame deltaG, ~2 Hz, threshold 0.30)

| Situation | deltaG | Result |
|---|---|---|
| Idle on the desk | 0.0000 - 0.0090 | no event |
| Light handling | 0.0890, 0.1692 | no event (below 0.30) |
| Hand jolt | 0.6062 | event, severity 3, uploaded, Firebase HTTP 200 |
| Harder hand jolt | 1.0845 | event, severity 5 |
| Sample right after a jolt | 0.5201 / 1.0783 | suppressed by the 3 s cooldown |

GPS: 5-8 satellites after an outdoor lock; reported position wandered by roughly 3-6 m while the
unit sat still (one reason the dashboard groups reports into ~11 m cells rather than matching exact coordinates).

## 4. How the 0.30 threshold was chosen
It sits in the gap between the highest idle/handling value seen (0.17) and the lowest jolt (0.52),
i.e. it separates two observed clusters. That is a reasonable first cut, **not** a statistically
derived threshold.

## 5. What has NOT been done
* No on-vehicle or on-road testing (the chassis was never driven over real bumps with the logger).
* No labelled dataset; no precision / recall; no false-positive rate against speed breakers, braking, manhole covers.
* No study of sampling rate, accelerometer range, filter settings, or mounting location.
* No test with more than one physical unit; multi-vehicle behaviour is simulated.

## 6. Protocol to close those gaps
1. Log raw `ax, ay, az, speed, lat, lon, t` to CSV at >= 100 Hz while driving a fixed route.
2. Label events by hand (video or notes): pothole / speed breaker / braking / smooth road.
3. Plot deltaG (and peak, duration, energy) per class; compute ROC for the threshold.
4. Only then pick severity bands and decide whether a trained classifier beats the threshold baseline.
