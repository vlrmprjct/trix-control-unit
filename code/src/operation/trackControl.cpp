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

// STATE TRANSITIONS
static void setRunning(Tracks& track, int relay) {
    RelayControl::setRelay(relay, true);
    track.powered = true;
    track.pending = false;
    track.occupied = false;
}

static void setStopped(Tracks& track, int relay) {
    RelayControl::setRelay(relay, false);
    track.powered = false;
    track.pending = false;
}

static void setStopRequested(Tracks& track) {
    track.powered = false; // RELAY STAYS ON - TRAIN COASTS TO _L REED
    track.pending = true;
}

static void setWaitingToDepart(Tracks& track) {
    track.powered = true; // RELAY OFF - WAITING FOR ZONE C TO CLEAR
    track.pending = true;
}

// BBF _L REED
void TrackControl::stopBBF(Tracks& track, int relay) {
    if (track.powered && !BLOCKC.occupied) {
        // PASS-THROUGH: TRAIN RUNS WITHOUT STOPPING
        track.occupied = false;
        track.pending = false;
    } else {
        RelayControl::setRelay(relay, false);
        if (track.powered && BLOCKC.occupied) {
            setWaitingToDepart(track); // WANTS TO GO BUT ZONE C IS BUSY
        } else {
            track.pending = false;     // TRAIN ARRIVED AT STOP POSITION
        }
    }
    Eeprom::save();
}

// BBF BUTTON
void TrackControl::toggleBBF(Tracks& track, int relay) {
    if (!track.selected) return;

    if (track.powered && track.pending) {
        // STATE: WAITING TO DEPART
        if (!BLOCKC.occupied) setRunning(track, relay); // RELEASE
        else setStopped(track, relay);                  // ABORT WAIT
        Eeprom::save();
        return;
    }

    if (track.powered) {
        // STATE: RUNNING
        setStopRequested(track); // TRAIN COASTS TO _L REED
        Eeprom::save();
        return;
    }

    // STATE: STOPPED OR STOP-REQUESTED
    if (!BLOCKC.occupied) setRunning(track, relay);
    else setWaitingToDepart(track);
    Eeprom::save();
}

// CANCEL PENDING WHEN TURNOUT IS SWITCHED AWAY FROM THIS TRACK
void TrackControl::cancelPending(Tracks& track, int relay) {
    if (!track.pending) return;
    if (track.powered) {
        setStopped(track, relay); // WAS WAITING TO DEPART: ABORT
    } else {
        track.pending = false;
        track.powered = true;     // WAS STOP-REQUESTED: CANCEL, TRAIN KEEPS RUNNING
    }
    Eeprom::save();
}

// AUTO-RELEASE FIRST WAITING BBF WHEN ZONE C CLEARS
void TrackControl::releasePendingBBF() {
    for (int idx = 0; idx < BBF_COUNT; idx++) {
        Tracks& track = *bbf[idx].track;
        if (track.selected && track.powered && track.pending) {
            setRunning(track, bbf[idx].relay);
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
    // RELEASE BLOCK B IF AT LEAST ONE HBF IS FREE
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
