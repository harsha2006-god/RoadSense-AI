---
name: Add Wi-Fi timeout and clearer upload errors
about: Make connection failures bounded and Firebase upload failures easier to diagnose
title: "[Firmware] Add Wi-Fi timeout and useful upload error messages"
labels: ["firmware", "reliability"]
assignees: []
---

## Problem

A Wi-Fi connection attempt should not block indefinitely, and failed Firebase uploads should produce diagnostic output that helps distinguish authorization errors from network or server failures.

## Proposed scope

Add a bounded Wi-Fi connection timeout and improve diagnostic output for failed HTTP uploads. Confirm the current connection and upload flow with the maintainer before editing.

## Acceptance criteria

- [ ] Wi-Fi connection attempts stop after a documented timeout.
- [ ] A timeout produces a clear Serial message and the firmware remains in a defined, recoverable state.
- [ ] Successful connections and uploads continue to work.
- [ ] Failed HTTP requests print the HTTP status code when available and a short, accurate explanation.
- [ ] HTTP `401` is described as an authentication/authorization failure, not automatically as expired Firebase rules. Firebase Realtime Database rules failures can be returned as `401` or `403`, depending on configuration and request.
- [ ] Network failures where no HTTP response is received are distinguished from HTTP error responses.
- [ ] The PR includes serial logs for at least one successful path and one failure path, with secrets removed.
- [ ] Hardware testing is reported honestly.

## Out of scope

- Committing credentials or changing Firebase security rules as part of this issue.
