/*
  RoadSense AI - ESP32 firmware
  -----------------------------------------------------------------
  Reads an MPU6050 accelerometer (raw I2C registers, no library) and a
  NEO-6M GPS module (TinyGPSPlus). When the frame-to-frame change in
  acceleration magnitude (deltaG) crosses BUMP_THRESHOLD_G, the event is
  tagged with the current GPS position and POSTed to Firebase Realtime
  Database as JSON.

  Wiring (ESP32 DevKit):
    MPU6050  VCC->3V3  GND->GND  SDA->GPIO21  SCL->GPIO22
    NEO-6M   VCC->3V3  GND->GND  TX->GPIO16   RX->GPIO17   (TX/RX crossed)

  Libraries: TinyGPSPlus (Library Manager). Board: "ESP32 Dev Module".
  Serial Monitor: 115200 baud.

  Known limitations (see docs/ARCHITECTURE.md):
    - Samples at ~2 Hz (delay(500)); real vehicle speeds need >= 100 Hz.
    - MPU6050 left at its default +/-2 g range, so hard impacts can clip.
    - If GPS has no fix, the event is uploaded with lat/lon = 0.0.
    - connectWiFi() blocks until connected (no timeout).
*/
#include <Wire.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define MPU_ADDR 0x68

// WiFi credentials and Firebase URL live in secrets.h (git-ignored).
// Copy secrets.example.h to secrets.h and fill in your own values.
#include "secrets.h"

// ---------------- GPS ----------------
TinyGPSPlus gps;
HardwareSerial GPSSerial(2);   // RX=16 TX=17

// ---------------- MPU ----------------
float prevG = 1.0;
float BUMP_THRESHOLD_G = 0.30;

unsigned long lastEventTime = 0;
const unsigned long EVENT_COOLDOWN_MS = 3000;

// --------------------------------------------------

void connectWiFi() {

  Serial.print("Connecting WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// --------------------------------------------------

void uploadToFirebase(float deltaG,
                      int severity,
                      double lat,
                      double lon,
                      int satellites) {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected.");
    return;
  }

  HTTPClient http;

  http.begin(FIREBASE_URL);

  http.addHeader("Content-Type", "application/json");

  String json = "{";
  json += "\"deltaG\":" + String(deltaG, 4) + ",";
  json += "\"severity\":" + String(severity) + ",";
  json += "\"latitude\":" + String(lat, 6) + ",";
  json += "\"longitude\":" + String(lon, 6) + ",";
  json += "\"satellites\":" + String(satellites) + ",";
  json += "\"timestamp\":" + String(millis());
  json += "}";

  int code = http.POST(json);

  Serial.print("Firebase Response: ");
  Serial.println(code);

  http.end();
}

// --------------------------------------------------

void writeRegister(byte reg, byte value) {

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

// --------------------------------------------------

int16_t read16(byte reg) {

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU_ADDR, (byte)2);

  int16_t value = 0;

  if (Wire.available() >= 2) {

    byte highByte = Wire.read();
    byte lowByte = Wire.read();

    value = ((int16_t)highByte << 8) | lowByte;
  }

  return value;
}

// --------------------------------------------------

int severityFromDeltaG(float deltaG) {

  if (deltaG > 1.0) return 5;
  if (deltaG > 0.7) return 4;
  if (deltaG > 0.5) return 3;
  if (deltaG > BUMP_THRESHOLD_G) return 2;

  return 1;
}

// --------------------------------------------------

void setup() {

  Serial.begin(115200);

  Wire.begin(21, 22);

  GPSSerial.begin(9600, SERIAL_8N1, 16, 17);

  writeRegister(0x6B, 0x00);

  delay(100);

  connectWiFi();

  Serial.println();
  Serial.println("RoadSense AI Started");
}

// --------------------------------------------------

void loop() {

  while (GPSSerial.available() > 0) {
    gps.encode(GPSSerial.read());
  }

  int16_t rawAx = read16(0x3B);
  int16_t rawAy = read16(0x3D);
  int16_t rawAz = read16(0x3F);

  float ax = rawAx / 16384.0;
  float ay = rawAy / 16384.0;
  float az = rawAz / 16384.0;

  float currentG = sqrt(ax * ax + ay * ay + az * az);

  float deltaG = fabs(currentG - prevG);

  prevG = currentG;

  // Live Data

  Serial.print("DeltaG: ");
  Serial.print(deltaG, 4);

  if (gps.location.isValid()) {

    Serial.print(" | Lat: ");
    Serial.print(gps.location.lat(), 6);

    Serial.print(" | Lon: ");
    Serial.print(gps.location.lng(), 6);

    Serial.print(" | Sats: ");
    Serial.print(gps.satellites.value());

  } else {

    Serial.print(" | Waiting for GPS lock...");
  }

  Serial.println();

  // Pothole Detection

  unsigned long now = millis();

  if (deltaG > BUMP_THRESHOLD_G &&
      (now - lastEventTime) > EVENT_COOLDOWN_MS) {

    lastEventTime = now;

    int severity = severityFromDeltaG(deltaG);

    double lat = gps.location.isValid()
                     ? gps.location.lat()
                     : 0.0;

    double lon = gps.location.isValid()
                     ? gps.location.lng()
                     : 0.0;

    int sats = gps.satellites.value();

    Serial.println();
    Serial.println("************************");
    Serial.println("POTHOLE DETECTED!");
    Serial.print("Severity: ");
    Serial.println(severity);

    Serial.print("DeltaG: ");
    Serial.println(deltaG, 4);

    Serial.print("Latitude: ");
    Serial.println(lat, 6);

    Serial.print("Longitude: ");
    Serial.println(lon, 6);

    Serial.print("Satellites: ");
    Serial.println(sats);

    Serial.println("Uploading to Firebase...");

    uploadToFirebase(
      deltaG,
      severity,
      lat,
      lon,
      sats
    );

    Serial.println("************************");
    Serial.println();
  }

  delay(500);
}