# Contributing to RoadSense AI

Thanks for helping improve RoadSense AI! This project combines ESP32 firmware, GPS-based pothole reports, Firebase Realtime Database, and a live dashboard.

## Before you start

1. Check the existing issues and open a new issue before beginning substantial work.
2. Explain the problem you intend to solve and agree on expected behavior with the maintainer.
3. Work on a separate branch. Do not commit directly to `main`.
4. Keep changes focused: one pull request should solve one issue.
5. Never commit Wi-Fi passwords, Firebase secrets, API keys, or personal data. Keep local secrets in the ignored `secrets.h` file and use `secrets.example.h` as a guide.

## Suggested workflow

1. Open or select an issue and discuss the proposed solution.
2. Fork the repository (or create a branch if you have collaborator access).
3. Create a descriptive branch, such as `test/simulator-report-builder`.
4. Make the smallest complete change.
5. Run relevant tests and describe exactly what you tested.
6. Open a pull request linking the issue, such as `Closes #12`.
7. Respond to review feedback and make follow-up commits on the same branch.

## Python changes

Install pytest with:

```bash
python -m pip install pytest
```

Run tests from the repository root:

```bash
python -m pytest -q
```

Tests should be deterministic and should not contact Firebase, require Wi-Fi, or require ESP32 hardware unless an issue explicitly says otherwise. Prefer testing pure functions with fixed inputs.

For tests involving `tools/simulate_vehicles.py`, first inspect the current `build_report` function and preserve its actual interface. Do not change production behavior just to make a test pass unless the issue calls for that change.

## Firmware changes

Firmware changes should be tested on the actual ESP32 setup when possible. In a pull request, state the board, GPS conditions, Wi-Fi conditions, and serial output used during testing. If hardware testing was not possible, say so clearly and do not claim it was tested.

## Pull request expectations

- Link the issue being addressed.
- Explain the problem and the chosen solution.
- List tests run and their results.
- Include screenshots or serial logs when useful, with secrets removed.
- Mention limitations or hardware tests that remain.
- Keep unrelated formatting changes out of the PR.

A pull request is not automatically accepted because it exists. The maintainer will review the implementation, test the behavior, and may request changes before merging.
