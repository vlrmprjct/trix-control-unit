#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include "trackControl.h"
#include "../core/config.h"
#include "../core/naming.h"
#include "../core/servocal.h"
#include "../controls/relayControl.h"
#include "../controls/motorControl.h"
#include "../controls/servoControl.h"
#include "../utils/eeprom.h"

const BBFConfig TrackControl::bbf[] = {
    { &BBF1, RELAY_BBF1 },
    { &BBF2, RELAY_BBF2 },
    { &BBF3, RELAY_BBF3 },
    { &BBF4, RELAY_BBF4 },
    { &BBF5, RELAY_BBF5 },
};

const int TrackControl::BBF_COUNT = sizeof(bbf) / sizeof(bbf[0]);

const HBFConfig TrackControl::hbf[] = {
    { &HBF1, RELAY_HBF1_ZONE, RELAY_HBF1_TRACK, true  },
    { &HBF2, RELAY_HBF2_ZONE, RELAY_HBF2_TRACK, false },
};

const int TrackControl::HBF_COUNT = sizeof(hbf) / sizeof(hbf[0]);

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
    // RELAY STAYS ON - TRAIN COASTS TO _L REED
    track.powered = false;
    track.pending = true;
}

static void setWaitingToDepart(Tracks& track) {
    // RELAY OFF - WAITING FOR ZONE C TO CLEAR
    track.powered = true;
    track.pending = true;
}

// BBF _L REED
void TrackControl::stopBBF(Tracks& track, int relay, bool canDepart) {
    if (track.powered && canDepart) {
        // PASS-THROUGH: TRAIN RUNS WITHOUT STOPPING
        track.occupied = false;
        track.pending = false;
    } else {
        RelayControl::setRelay(relay, false);
        if (track.powered && !canDepart) {
            // WANTS TO GO BUT ZONE C IS BUSY
            setWaitingToDepart(track);
        } else {
            // TRAIN ARRIVED AT STOP POSITION
            track.pending = false;
        }
    }
    Eeprom::save();
}

// BBF BUTTON
void TrackControl::toggleBBF(Tracks& track, int relay, bool canDepart) {
    // if (!track.selected) return;

    if (track.powered && track.pending) {
        // STATE: WAITING TO DEPART
        if (canDepart) setRunning(track, relay);
        // ABORT WAIT
        else setStopped(track, relay);
        Eeprom::save();
        return;
    }

    if (track.powered) {
        // STATE: RUNNING - TRAIN COASTS TO _L REED
        setStopRequested(track);
        Eeprom::save();
        return;
    }

    // STATE: STOPPED OR STOP-REQUESTED
    if (canDepart) setRunning(track, relay);
    else setWaitingToDepart(track);
    Eeprom::save();
}

// CANCEL PENDING WHEN TURNOUT IS SWITCHED AWAY FROM THIS TRACK
void TrackControl::cancelPending(Tracks& track, int relay) {
    if (!track.pending) return;
    if (track.powered) {
        // WAS WAITING TO DEPART: ABORT
        setStopped(track, relay);
    } else {
        // WAS STOP-REQUESTED: CANCEL, TRAIN KEEPS RUNNING
        track.pending = false;
        track.powered = true;
    }
    Eeprom::save();
}

// AUTO-RELEASE WAITING HBF TRAIN WHEN ZONE A CLEARS
// RETURNS TRUE IF A PENDING HBF WAS RELEASED
bool TrackControl::releasePendingHBF(Adafruit_PWMServoDriver& driver) {
    for (int idx = 0; idx < HBF_COUNT; idx++) {
        Tracks& track = *hbf[idx].track;
        if (track.powered && track.pending) {
            MotorControl::setDepartingHBF(idx + 1); // 1=HBF1, 2=HBF2
            ServoControl::switchTurnout(driver, W2, hbf[idx].w2Diverging);
            toggleHBF(track, hbf[idx].trackRelay);
            return true;
        }
    }
    return false;
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
        // ARRIVING AND PARK
        RelayControl::setRelay(zoneRelay, true);
        RelayControl::setRelay(trackRelay, false);
        track.pending = false;
    } else {
        // DEPARTING
        track.occupied = false;
        track.pending = false;
    }
    Eeprom::save();
}

// HBF BUTTON: TOGGLE POWER
void TrackControl::toggleHBF(Tracks& track, int trackRelay) {
    // WAITING TO DEPART (HELD AT _R BY BLOCKA): RELEASE INTO ZONE A
    if (track.powered && track.pending) {
        track.occupied = false;
        track.pending = false;
        // PRE-ZERO BEFORE RELAY ON
        MotorControl::setValue(ZONE_A, 0);
        RelayControl::setRelay(trackRelay, true);
        Eeprom::save();
        return;
    }
    track.powered = !track.powered;
    if (track.powered) {
        track.occupied = false;
        track.pending = false;
        // PRE-ZERO BEFORE RELAY ON
        MotorControl::setValue(ZONE_A, 0);
        RelayControl::setRelay(trackRelay, true);
    } else {
        // POWERED OFF: TRAIN COASTS TO _R REED
        track.pending = true;
    }
    Eeprom::save();
}
