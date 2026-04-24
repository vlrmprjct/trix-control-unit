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

// BBF _L REED: STOP OR SET PENDING IF ZONE C BLOCKED
void TrackControl::stopBBF(Tracks& track, int relay) {
    if (!track.powered || BLOCKC.occupied) {
        RelayControl::setRelay(relay, false);
        if (track.powered && BLOCKC.occupied) {
            track.pending = true;  // WAITING TO DEPART
        } else {
            track.pending = false; // TRAIN ARRIVED AT STOP POSITION
        }
        Eeprom::save();
    } else {
        // PASS-THROUGH: POWERED + ZONE C FREE
        track.occupied = false;
        track.pending = false;
        Eeprom::save();
    }
}

// BBF BUTTON: TOGGLE POWER OR MANUAL RELEASE IF PENDING
void TrackControl::toggleBBF(Tracks& track, int relay) {
    if (!track.selected) return;

    // PENDING + POWERED: WAITING TO DEPART - BUTTON ABORTS OR RELEASES
    if (track.powered && track.pending) {
        if (!BLOCKC.occupied) {
            // ZONE C FREE: RELEASE NOW
            RelayControl::setRelay(relay, true);
            track.pending = false;
            track.occupied = false;
        } else {
            // ZONE C BUSY: ABORT WAITING
            RelayControl::setRelay(relay, false);
            track.powered = false;
            track.pending = false;
        }
        Eeprom::save();
        return;
    }

    track.powered = !track.powered;
    if (track.powered) {
        if (!BLOCKC.occupied) {
            RelayControl::setRelay(relay, true);
            track.pending = false;
            track.occupied = false;
        } else {
            track.pending = true; // ZONE C BUSY: WAIT FOR releasePendingBBF
        }
    } else {
        // POWERED OFF: SET PENDING SO DISPLAY SHOWS TRAIN IS COASTING TO _L REED
        track.pending = true;
    }
    Eeprom::save();
}

// CANCEL PENDING WHEN TURNOUT IS SWITCHED AWAY FROM THIS TRACK
void TrackControl::cancelPending(Tracks& track, int relay) {
    if (!track.pending) return;
    track.pending = false;
    if (track.powered) {
        // WAITING TO DEPART: TURN OFF RELAY + POWER
        RelayControl::setRelay(relay, false);
        track.powered = false;
    } else {
        // STOP-REQUEST: RESTORE POWER, TRAIN KEEPS RUNNING
        track.powered = true;
    }
    Eeprom::save();
}

// AUTO-RELEASE FIRST PENDING BBF WHEN ZONE C CLEARS
void TrackControl::releasePendingBBF() {
    for (int idx = 0; idx < BBF_COUNT; idx++) {
        Tracks& track = *bbf[idx].track;
        if (track.selected && track.powered && track.pending) {
            RelayControl::setRelay(bbf[idx].relay, true);
            track.pending = false;
            track.occupied = false;
            return;
        }
    }
}

// HBF _R REED: PARK IF NOT POWERED, FREE TRACK IF DEPARTING
void TrackControl::stopHBF(Tracks& track, int zoneRelay, int trackRelay) {
    if (!track.powered) {
        // ARRIVING - PARK
        RelayControl::setRelay(zoneRelay, true);
        RelayControl::setRelay(trackRelay, false);
        track.pending = false; // TRAIN ARRIVED AT STOP POSITION
    } else {
        // DEPARTING
        track.occupied = false;
        track.pending = false;
    }
    // RELEASE BLOCKB IF AT LEAST ONE HBF IS FREE
    if (!(HBF1.occupied && HBF2.occupied)) {
        RelayControl::setRelay(10, true);
        BLOCKB.occupied = false;
    }
    Eeprom::save();
}

// HBF BUTTON: TOGGLE POWER
void TrackControl::toggleHBF(Tracks& track, int trackRelay) {
    if (!track.selected) return;
    track.powered = !track.powered;
    if (track.powered) {
        if (track.occupied) MotorControl::setValue(ZONE_A, 0);
        track.occupied = false;
        track.pending = false;
        RelayControl::setRelay(trackRelay, true);
        // RELEASE BLOCK IF AT LEAST ONE HBF TRACK IS FREE
        if (!(HBF1.occupied && HBF2.occupied)) {
            RelayControl::setRelay(10, true);
            BLOCKB.occupied = false;
        }
    } else {
        // POWERED OFF: SET PENDING SO DISPLAY SHOWS TRAIN IS COASTING TO _R REED
        track.pending = true;
    }
    Eeprom::save();
}
