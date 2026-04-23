#include <Arduino.h>
#include "trackControl.h"
#include "../core/config.h"
#include "../core/naming.h"
#include "../controls/relayControl.h"
#include "../controls/motorControl.h"
#include "../utils/eeprom.h"

const BBFConfig TrackControl::bbf[] = {
    { &BBF1, 2 },
    { &BBF2, 3 },
    { &BBF3, 4 },
    // { &BBF4, ?? },  // RELAY TBD
    // { &BBF5, ?? },  // RELAY TBD
};
const int TrackControl::BBF_COUNT = sizeof(bbf) / sizeof(bbf[0]);

// STOP BBF TRAIN IF NOT POWERED OR ZONE C IS BLOCKED
void TrackControl::stopBBF(Tracks& track, int relay) {
    if (!track.powered || BLOCKC.occupied) {
        RelayControl::setRelay(relay, false);
        track.occupied = true;
        Eeprom::save();
    }
}

// TOGGLE BBF POWER, ONLY ENABLE RELAY IF ZONE C IS FREE
void TrackControl::toggleBBF(Tracks& track, int relay) {
    if (!track.selected) return;
    track.powered = !track.powered;
    if (track.powered && !BLOCKC.occupied) {
        RelayControl::setRelay(relay, true);
    }
    Eeprom::save();
}

// RELEASE FIRST SELECTED+POWERED+OCCUPIED BBF (CALLED WHEN ZONE C CLEARS)
void TrackControl::releasePendingBBF() {
    for (int idx = 0; idx < BBF_COUNT; idx++) {
        Tracks& track = *bbf[idx].track;
        if (track.selected && track.powered && track.occupied) {
            RelayControl::setRelay(bbf[idx].relay, true);
            track.occupied = false;
            return;
        }
    }
}

// STOP HBF TRAIN: SWITCH ZONE + TURN OFF TRACK RELAY
void TrackControl::stopHBF(Tracks& track, int zoneRelay, int trackRelay) {
    if (!track.powered) {
        // ARRIVING TRAIN - PARK
        RelayControl::setRelay(zoneRelay, true);
        RelayControl::setRelay(trackRelay, false);
        track.occupied = true;
        Eeprom::save();
    } else {
        // PASSING THROUGH - RELEASE BLOCK IF AT LEAST ONE HBF TRACK IS FREE
        if (!(HBF1.occupied && HBF2.occupied)) {
            RelayControl::setRelay(10, true);
            BLOCKB.occupied = false;
            Eeprom::save();
        }
    }
}

// TOGGLE HBF POWER + RELEASE BLOCK
void TrackControl::toggleHBF(Tracks& track, int trackRelay) {
    if (!track.selected) return;
    track.powered = !track.powered;
    if (track.powered) {
        if (track.occupied) MotorControl::setValue(ZONE_A, 0);
        track.occupied = false;
        RelayControl::setRelay(trackRelay, true);
        // RELEASE BLOCK IF AT LEAST ONE HBF TRACK IS FREE
        if (!(HBF1.occupied && HBF2.occupied)) {
            RelayControl::setRelay(10, true);
            BLOCKB.occupied = false;
        }
    }
    Eeprom::save();
}
