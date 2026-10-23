---
name: Add pytest coverage for simulator report builder
about: Add deterministic, hardware-free tests for build_report
title: "[Tests] Add pytest coverage for build_report"
labels: ["testing", "python", "good first issue"]
assignees: []
---

## Problem

`tools/simulate_vehicles.py` builds simulated pothole reports, but the report-building behavior should have automated regression tests. This task is Python-only and must not send data to Firebase.

## Proposed scope

Add `pytest` tests for the existing `build_report` function. Inspect its current signature and output before writing tests; do not assume or silently change its interface.

## Acceptance criteria

- [ ] Tests import and call `build_report` using its current interface.
- [ ] Tests verify that the returned report contains the required fields used by the simulator and Firebase payload.
- [ ] Tests verify expected field types and sensible values for fixed, deterministic input.
- [ ] Tests cover the coordinate/grid-cell behavior: reports intended to represent the same simulated location remain within the same approximately 11 m grid cell, using the project's existing calculation rather than inventing a new one.
- [ ] Tests use fixed inputs or controlled randomness and are repeatable.
- [ ] Tests do not make network requests, write to Firebase, or require ESP32 hardware.
- [ ] Tests run with `python -m pytest -q` from the repository root.
- [ ] Any assumptions about the grid-cell calculation are documented in the test or issue discussion.

## Out of scope

- Changing dashboard validation (already implemented and tested).
- Changing simulator production behavior unless a separate change is agreed in the issue.
- Posting mock reports to the live Firebase database.

## Test notes

Include the exact test command and its result in the pull request.
