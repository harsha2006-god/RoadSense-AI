---
name: Prevent Firebase uploads when GPS has no valid fix
about: Decide and implement safe behavior for detections without GPS coordinates
title: "[Firmware] Handle pothole detections without a GPS fix"
labels: ["firmware", "bug"]
assignees: []
---

## Problem

The firmware can detect an acceleration event while the GPS receiver has no valid fix. Uploading a report with latitude and longitude set to `0.0` can create a misleading marker near the Gulf of Guinea.

## Decision required

Choose and document one behavior before implementing it:

- **Drop the event:** do not upload a location-based report when no valid GPS fix exists; or
- **Queue the event:** temporarily retain it and upload it only after a valid GPS fix is obtained.

Discuss trade-offs with the maintainer first. If queuing is selected, define how long an event remains queued and what happens if no fix is obtained.

## Acceptance criteria

- [ ] The firmware checks GPS fix validity before uploading a location-based report.
- [ ] A no-fix event cannot be uploaded with placeholder coordinates such as `0.0, 0.0`.
- [ ] The chosen drop-or-queue behavior is documented in code comments or project docs.
- [ ] Serial output clearly explains what happened to an event when GPS has no fix.
- [ ] A valid GPS fix still allows normal reports to upload.
- [ ] The change is tested on the ESP32; testing conditions and serial output are described in the PR.
- [ ] No Wi-Fi passwords, Firebase credentials, or API keys are added to the commit.

## Test plan

Reproduce a no-fix condition indoors, then test again outdoors with a valid GPS fix. If a queued design is chosen, also verify behavior when a fix later becomes available.
