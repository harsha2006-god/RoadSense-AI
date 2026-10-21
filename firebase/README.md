# Firebase setup

RoadSense AI uses the **Firebase Realtime Database** purely as a free cloud
store. The ESP32 and the simulator `POST` JSON to `/potholes.json`; the
dashboard `GET`s it every 2 seconds. No Firebase SDK is used anywhere.

## 1. Create the database
1. <https://console.firebase.google.com> -> **Add project** (Analytics not needed).
2. **Build -> Realtime Database -> Create Database**; pick a region.
3. Copy the URL shown at the top of the Data tab. Regional databases look like
   `https://<project>-default-rtdb.<region>.firebasedatabase.app`
   (not the older `...firebaseio.com` form - that only works for the default region).

## 2. Replace the test-mode rules (important)
**Test-mode rules expire after 30 days.** After that every read and write is
denied: the ESP32 gets an HTTP 401 and the dashboard shows *"Permission denied"*.
Nothing is wrong with your code when this happens.

Open **Realtime Database -> Rules**, paste the contents of
[`database.rules.json`](database.rules.json) and **Publish**. What they do:

| Rule | Effect |
|---|---|
| `.read: true` on `/potholes` | Dashboard can read without auth (demo only) |
| `.write: "!data.exists()"` | Records can be **created** but not edited or deleted |
| `.validate` on each field | Rejects wrong types, out-of-range lat/lon, severity outside 1-5, unknown fields |

Please use the **Rules Playground** (Rules tab -> Simulator) to try a write and
a read before relying on this - these rules were written against the documented
rule syntax but not run against a live database by the author.

## 3. Be honest about what these rules do *not* do
Anyone who knows your database URL can still **read all records** (they contain
GPS positions) and **create new validly-shaped records** (spam). That is fine
for a prototype on a throwaway project. For anything real you would add
Firebase Authentication (or a small backend that signs device requests),
per-device credentials, rate limiting and a data-retention policy for location
data.
