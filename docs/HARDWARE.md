# Hardware

## Wiring (ESP32 DevKit, 30/38-pin)

| MPU6050 | ESP32 |   | NEO-6M GPS | ESP32 |
|---|---|---|---|---|
| VCC | 3V3 |   | VCC | 3V3 |
| GND | GND |   | GND | GND |
| SDA | GPIO 21 |   | TX | GPIO 16 (RX2) |
| SCL | GPIO 22 |   | RX | GPIO 17 (TX2) |

GPS TX/RX are **crossed** (module TX -> ESP32 RX). Avoid GPIO 0, 1, 2, 3, 12, 15 for anything
you wire up: they are used for upload and boot mode, and wiring peripherals to them can break flashing.

## Bill of materials (approximate, India, September 2026)

| Part | Approx. price | Notes |
|---|---|---|
| ESP32 dev board (WROOM, WiFi + BT) | Rs 300-500 | listings seen ranged roughly Rs 285-490 |
| MPU6050 module (GY-521) | Rs 170 | price paid |
| NEO-6M GPS module | Rs 320 | price paid |
| Breadboard + jumper wires + USB cable | ~Rs 150 | |
| Power bank (portable demo) | ~Rs 300 | usually already owned |
| *Optional:* 4WD acrylic chassis | Rs 767 | only used as a rolling platform for demos; not part of the sensing kit |

**Prototype sensing kit: roughly Rs 1,100 (about Rs 1,400 with a power bank).**
This is a hobby-prototype figure. A deployable unit would also need an enclosure, vehicle-power
conditioning, mounting hardware and probably cellular connectivity, and has **not** been costed.

## Mounting and enclosure (design intent - not built or tested)

* The accelerometer must be rigidly coupled to the vehicle body; a loose mount filters out the signal.
* The GPS module needs sky view (dashboard / windshield area), so it would be mounted apart from the accelerometer.
* A production unit would use a sealed, splash-resistant enclosure with cable glands. The current prototype is bare modules on a breadboard.
* Where exactly to mount for the best signal, and how to normalise across vehicle types, is an open question (see ARCHITECTURE.md limitation 9).
