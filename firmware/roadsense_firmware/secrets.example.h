// Copy this file to "secrets.h" (same folder) and fill in your values.
// secrets.h is listed in .gitignore - never commit it.
#pragma once

// ESP32 supports 2.4 GHz WiFi only (not 5 GHz).
const char* WIFI_SSID     = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// Full regional URL from the Firebase console, ending in /potholes.json
// Example:
// https://my-project-default-rtdb.asia-southeast1.firebasedatabase.app/potholes.json
const char* FIREBASE_URL  = "https://YOUR_PROJECT_ID-default-rtdb.YOUR_REGION.firebasedatabase.app/potholes.json";
