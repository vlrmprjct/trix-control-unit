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
    static const unsigned long MIN_GAP = 100;
    static unsigned long _lastSend = 0;

    // SNAPSHOTS FOR CHANGE DETECTION
    static int _encA = -9999;
    static int _encB = -9999;
    static int _rawA = -9999;
    static int _rawB = -9999;
    static float _speed = -1.0;

    struct TrackSnap { bool selected; bool powered; bool occupied; int min; int max; int brake; };
    static TrackSnap _hbf[2];   // INDEX 1-2
    static TrackSnap _bbf[5];   // INDEX 3-7
    static TrackSnap _block[3]; // INDEX 8-10
    static bool _hbfInit = false;
    static bool _bbfInit = false;
    static bool _blockInit = false;

    static uint32_t _relay = 0xFFFFFFFF;

    static bool trackChanged(const TrackSnap* snap, int base, int count) {
        for (int i = 0; i < count; i++) {
            const Tracks& t = ROUTE.track[base + i];
            if (t.selected != snap[i].selected) return true;
            if (t.powered  != snap[i].powered)  return true;
            if (t.occupied != snap[i].occupied) return true;
            if (t.min      != snap[i].min)      return true;
            if (t.max      != snap[i].max)      return true;
            if (t.brake    != snap[i].brake)     return true;
        }
        return false;
    }

    static void snapTracks(TrackSnap* snap, int base, int count) {
        for (int i = 0; i < count; i++) {
            const Tracks& t = ROUTE.track[base + i];
            snap[i] = { t.selected, t.powered, t.occupied, t.min, t.max, t.brake };
        }
    }

    static void buildTrack(JSONVar& d, const char* key, const TrackSnap& snap) {
        JSONVar t;
        t["s"] = snap.selected;
        t["p"] = snap.powered;
        t["o"] = snap.occupied;
        t["min"] = snap.min;
        t["max"] = snap.max;
        t["brk"] = snap.brake;
        d[key] = t;
    }

    static bool trySend(unsigned long now) {
        return (now - _lastSend >= MIN_GAP);
    }

    void send(SimpleWebSerial& ws) {
        unsigned long now = millis();

        // COMMON: ENCODER + SPEED + SYSTEM
        if (trySend(now)) {
            if (ENC_ZONE_A != _encA || ENC_ZONE_B != _encB ||
                EncoderControl::encoderZoneA.raw != _rawA ||
                EncoderControl::encoderZoneB.raw != _rawB ||
                Utils::currentSpeed != _speed) {

                _encA = ENC_ZONE_A;
                _encB = ENC_ZONE_B;
                _rawA = EncoderControl::encoderZoneA.raw;
                _rawB = EncoderControl::encoderZoneB.raw;
                _speed = Utils::currentSpeed;

                JSONVar d;
                d["firmware"] = FIRMWARE_VERSION;
                d["uptime"] = (unsigned long)(millis() / 1000);
                d["encA"] = _encA;
                d["encB"] = _encB;
                d["rawA"] = _rawA;
                d["rawB"] = _rawB;
                d["dirA"] = (int)EncoderControl::getDirection(EncoderControl::encoderZoneA);
                d["dirB"] = (int)EncoderControl::getDirection(EncoderControl::encoderZoneB);
                d["spdRaw"] = (double)_speed;
                d["spdKmh"] = (int)Utils::scaleSpeed(_speed);
                ws.send("common", d);
                _lastSend = millis();
                return;
            }
        }

        // HBF: TRACKS 1-2
        if (trySend(now)) {
            if (!_hbfInit || trackChanged(_hbf, 1, 2)) {
                snapTracks(_hbf, 1, 2);
                _hbfInit = true;
                JSONVar d;
                buildTrack(d, "hbf1", _hbf[0]);
                buildTrack(d, "hbf2", _hbf[1]);
                ws.send("hbf", d);
                _lastSend = millis();
                return;
            }
        }

        // BBF: TRACKS 3-7
        if (trySend(now)) {
            if (!_bbfInit || trackChanged(_bbf, 3, 5)) {
                snapTracks(_bbf, 3, 5);
                _bbfInit = true;
                JSONVar d;
                buildTrack(d, "bbf1", _bbf[0]);
                buildTrack(d, "bbf2", _bbf[1]);
                buildTrack(d, "bbf3", _bbf[2]);
                buildTrack(d, "bbf4", _bbf[3]);
                buildTrack(d, "bbf5", _bbf[4]);
                ws.send("bbf", d);
                _lastSend = millis();
                return;
            }
        }

        // BLOCK: TRACKS 8-10
        if (trySend(now)) {
            if (!_blockInit || trackChanged(_block, 8, 3)) {
                snapTracks(_block, 8, 3);
                _blockInit = true;
                JSONVar d;
                buildTrack(d, "blocka", _block[0]);
                buildTrack(d, "blockb", _block[1]);
                buildTrack(d, "blockc", _block[2]);
                ws.send("block", d);
                _lastSend = millis();
                return;
            }
        }

        // RELAYS
        if (trySend(now)) {
            if (relayState != _relay) {
                _relay = relayState;
                JSONVar d;
                for (int i = 0; i < RELAY_COUNT; i++) {
                    d[i] = !((_relay >> i) & 1);
                }
                ws.send("relay", d);
                _lastSend = millis();
                return;
            }
        }
    }

}
