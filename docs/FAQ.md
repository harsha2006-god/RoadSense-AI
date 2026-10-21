# FAQ - questions the TECHNOVA 2026 jury asked (and honest answers)

Each answer is tagged: **[built]** exists and was demonstrated, **[design]** a design intention that is
not implemented, **[open]** an unsolved question, **[not done]** something we have not done.

### 1. Why only potholes? There are other road problems too.
Potholes were a deliberate first scope: they are a costly, safety-relevant defect with a clear
physical signature (a sharp vertical shock), so they make a measurable MVP. **[design]** The same
pipeline - vibration sensing, GPS tagging, multi-report confirmation, a priority map - could extend to
cracks, damaged speed breakers, or rough surfaces, but each class needs its own labelled data; we have
only built and tested the pothole-style shock detector. **[not done]** other defect types.

### 2. Where do the electronics sit? Any container?
**[built]** The prototype is bare modules on a breadboard. **[design]** A deployable unit would use a sealed,
splash-resistant enclosure with cable glands, with the accelerometer rigidly fixed to the body.

### 3. Where in the car would it be fitted?
**[design / open]** The accelerometer has to be rigidly attached to the vehicle structure; the GPS needs sky view
(dashboard/windshield area), so the two would be placed separately. The best mounting point has **not** been
measured. Near a wheel/suspension mount gives a stronger, less-damped signal, the cabin floor a weaker one;
we would test this with logged drives (see `docs/EXPERIMENTS.md`, section 6).

### 4. Cars differ - how do you fit and calibrate it for each manufacturer?
**[design / open]** A universal aftermarket bracket (not an OEM integration) avoids per-model hardware. The harder
problem is signal, not fitting: suspensions, tyres and speed change the response to the same pothole, so an absolute
threshold will not transfer across vehicles. Options we would evaluate: per-vehicle calibration, speed-normalised
features, and relying on agreement between multiple vehicles (which is what the confidence score is for).
None of this is implemented.

### 5. What happens in rain or flood?
**[not done]** The prototype has no weather protection. A sealed enclosure would address splash and road spray;
submersion is out of scope. We have not tested any of it.

### 6. Do you have permission from the government and car companies?
**[not done]** No - this is a student prototype. We have not approached authorities or manufacturers. A real
deployment would need agreements with the vehicle owners (e.g. fleet operators) and road authorities, and
would handle location data, which is personal data (India's Digital Personal Data Protection Act, 2023, would
need to be reviewed with proper legal advice). We are not making legal claims here.

### 7. Is it compatible and cost-efficient?
**[built]** The sensing kit (ESP32 + MPU6050 + NEO-6M + wiring) cost roughly Rs 1,100 in parts
(see `docs/HARDWARE.md`). **[not done]** Compatibility across vehicles, an enclosure, vehicle power and connectivity
are not costed or tested, so we cannot claim a production unit cost.

### 8. Did you check existing solutions? What is different?
Related work includes academic systems that sensed road quality with accelerometers - Microsoft Research India's
*Nericell* (smartphones, SenSys 2008) and MIT's *Pothole Patrol* (taxi-mounted sensors, MobiSys 2008) - plus manual
and vehicle-based road surveys, and citizen complaint apps. So the basic idea (accelerometer + GPS) is **not new**.
Our contribution is a small, low-cost, end-to-end implementation with a **report-confirmation layer** (confidence from
agreeing reports in the same ~11 m cell) and a live priority dashboard. **[open]** We have not measured whether the
confirmation layer actually reduces false positives; with one physical unit it is demonstrated with simulated reports.

### 9. What does each device cost?
Prototype sensing kit about **Rs 1,100** (about Rs 1,400 with a power bank). A production-grade unit is not costed. See question 7.

### Is the "AI" real?
**[built]** Detection is rule-based signal processing (a threshold on frame-to-frame acceleration change plus a
cooldown) and severity is a fixed lookup. It is **not** a trained model. **[not done]** A trained classifier compared
against this baseline is the main planned next step (`docs/ARCHITECTURE.md`, roadmap).

### How was the 0.30 threshold chosen?
From hand-jolt and idle measurements: it separates the highest idle/handling reading (0.17) from the lowest jolt (0.52).
It is a first cut, not a validated operating point (`docs/EXPERIMENTS.md`).

### Why does the dashboard poll instead of using Firebase's live listener?
So the page needs no SDK, API key or build step - one HTML file that can be opened anywhere. The price is up to 2 s of latency.
