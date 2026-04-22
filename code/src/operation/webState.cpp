#include "webState.h"
#include "../core/config.h"
#include "../core/state.h"
#include "../controls/encoderControl.h"
#include "../utils/utils.h"
#include <Arduino.h>
#include <Arduino_JSON.h>
#include <string.h>

extern uint32_t relayState;

namespace WebState {

    // MINIMUM GAP BETWEEN SENDS TO AVOID SERIAL OVERLAP
    static const unsigned long SEND_GAP = 100;
    static unsigned long lastSendTime = 0;

    // SNAPSHOTS FOR CHANGE DETECTION
    static int prevEncA = -9999;
    static int prevEncB = -9999;
    static int prevRawA = -1;
    static int prevRawB = -1;
    static float prevSpeed = -1.0;

    struct TrackSnapshot { bool selected; bool powered; bool occupied; };
    static TrackSnapshot hbfSnap[2];
    static TrackSnapshot bbfSnap[5];
    static TrackSnapshot blockSnap[3];
    static bool hbfInitialized = false;
    static bool bbfInitialized = false;
    static bool blockInitialized = false;

    static uint32_t prevRelayState = 0xFFFFFFFF;

    static bool hasTrackChanged(const TrackSnapshot* snapshot, int baseIndex, int count) {
        for (int idx = 0; idx < count; idx++) {
            const Tracks& track = ROUTE.track[baseIndex + idx];
            if (track.selected != snapshot[idx].selected) return true;
            if (track.powered  != snapshot[idx].powered)  return true;
            if (track.occupied != snapshot[idx].occupied) return true;
        }
        return false;
    }

    static void takeSnapshot(TrackSnapshot* snapshot, int baseIndex, int count) {
        for (int idx = 0; idx < count; idx++) {
            const Tracks& track = ROUTE.track[baseIndex + idx];
            snapshot[idx] = { track.selected, track.powered, track.occupied };
        }
    }

    static void buildTrackEntry(JSONVar& payload, const char* key, const TrackSnapshot& snapshot) {
        JSONVar entry;
        entry["s"] = snapshot.selected;
        entry["p"] = snapshot.powered;
        entry["o"] = snapshot.occupied;
        payload[key] = entry;
    }

    static bool canSend(unsigned long now) {
        return (now - lastSendTime >= SEND_GAP);
    }

    void send(SimpleWebSerial& ws) {
        unsigned long now = millis();

        // COMMON: ENCODER + SPEED + SYSTEM
        if (canSend(now)) {
            if (ENC_ZONE_A != prevEncA || ENC_ZONE_B != prevEncB ||
                Utils::currentSpeed != prevSpeed) {

                prevEncA = ENC_ZONE_A;
                prevEncB = ENC_ZONE_B;
                prevRawA = EncoderControl::encoderZoneA.raw;
                prevRawB = EncoderControl::encoderZoneB.raw;
                prevSpeed = Utils::currentSpeed;

                JSONVar payload;
                payload["firmware"] = FIRMWARE_VERSION;
                payload["uptime"] = (unsigned long)(millis() / 1000);
                payload["encA"] = prevEncA;
                payload["encB"] = prevEncB;
                payload["rawA"] = prevRawA;
                payload["rawB"] = prevRawB;
                payload["dirA"] = (int)EncoderControl::getDirection(EncoderControl::encoderZoneA);
                payload["dirB"] = (int)EncoderControl::getDirection(EncoderControl::encoderZoneB);
                payload["spdRaw"] = (double)prevSpeed;
                payload["spdKmh"] = (int)Utils::scaleSpeed(prevSpeed);
                ws.send("common", payload);
                lastSendTime = millis();
                return;
            }
        }

        // HBF: TRACKS 1-2
        if (canSend(now)) {
            if (!hbfInitialized || hasTrackChanged(hbfSnap, 1, 2)) {
                takeSnapshot(hbfSnap, 1, 2);
                hbfInitialized = true;
                JSONVar payload;
                buildTrackEntry(payload, "hbf1", hbfSnap[0]);
                buildTrackEntry(payload, "hbf2", hbfSnap[1]);
                ws.send("hbf", payload);
                lastSendTime = millis();
                return;
            }
        }

        // BBF: TRACKS 3-7
        if (canSend(now)) {
            if (!bbfInitialized || hasTrackChanged(bbfSnap, 3, 5)) {
                takeSnapshot(bbfSnap, 3, 5);
                bbfInitialized = true;
                JSONVar payload;
                buildTrackEntry(payload, "bbf1", bbfSnap[0]);
                buildTrackEntry(payload, "bbf2", bbfSnap[1]);
                buildTrackEntry(payload, "bbf3", bbfSnap[2]);
                buildTrackEntry(payload, "bbf4", bbfSnap[3]);
                buildTrackEntry(payload, "bbf5", bbfSnap[4]);
                ws.send("bbf", payload);
                lastSendTime = millis();
                return;
            }
        }

        // BLOCK: TRACKS 8-10
        if (canSend(now)) {
            if (!blockInitialized || hasTrackChanged(blockSnap, 8, 3)) {
                takeSnapshot(blockSnap, 8, 3);
                blockInitialized = true;
                JSONVar payload;
                buildTrackEntry(payload, "blocka", blockSnap[0]);
                buildTrackEntry(payload, "blockb", blockSnap[1]);
                buildTrackEntry(payload, "blockc", blockSnap[2]);
                ws.send("block", payload);
                lastSendTime = millis();
                return;
            }
        }

        // RELAYS
        if (canSend(now)) {
            if (relayState != prevRelayState) {
                prevRelayState = relayState;
                JSONVar payload;
                for (int idx = 0; idx < RELAY_COUNT; idx++) {
                    payload[idx] = !((prevRelayState >> idx) & 1);
                }
                ws.send("relay", payload);
                lastSendTime = millis();
                return;
            }
        }
    }

}
