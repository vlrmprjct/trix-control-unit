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

// HBF DEPARTURE OWNER OF W2 / ZONE A (0=NONE, 1=HBF1, 2=HBF2)
static int departingHBF = 0;

// A BBF IS AVAILABLE AS A TARGET WHEN NO TRAIN OCCUPIES IT
static bool isBBFFree(const Tracks& track) {
    return !track.occupied;
}

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

namespace TrackControl {

    const BBFConfig bbf[] = {
        { &BBF1, RELAY_BBF1 },
        { &BBF2, RELAY_BBF2 },
        { &BBF3, RELAY_BBF3 },
        { &BBF4, RELAY_BBF4 },
        { &BBF5, RELAY_BBF5 },
    };

    const HBFConfig hbf[] = {
        { &HBF1, RELAY_HBF1_ZONE, RELAY_HBF1_TRACK, true  },
        { &HBF2, RELAY_HBF2_ZONE, RELAY_HBF2_TRACK, false },
    };

    const int BBF_COUNT = sizeof(bbf) / sizeof(bbf[0]);
    const int HBF_COUNT = sizeof(hbf) / sizeof(hbf[0]);

    // BBF OPERATIONS ############################################################################

    // BBF _L REED
    void stopBBF(Tracks& track, int relay, bool canDepart) {
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
    void toggleBBF(Tracks& track, int relay, bool canDepart) {
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

    // AUTO-RELEASE FIRST WAITING BBF WHEN ZONE C CLEARS
    void releasePendingBBF() {
        for (int idx = 0; idx < BBF_COUNT; idx++) {
            Tracks& track = *bbf[idx].track;
            if (track.powered && track.pending) {
                setRunning(track, bbf[idx].relay);
                return;
            }
        }
    }

    // BBF ROUTING (RD_05 ENTRY / RD_BBFx_L EXIT) ################################################

    // FIND A FREE BBF: PRIORITY BBF1-3, FALLBACK BBF4-5
    // RETURNS 1-5, OR 0 IF ALL BBF ARE OCCUPIED
    int findFreeBBF() {
        int candidates[3];
        int count = 0;

        if (isBBFFree(BBF1)) candidates[count++] = 1;
        if (isBBFFree(BBF2)) candidates[count++] = 2;
        if (isBBFFree(BBF3)) candidates[count++] = 3;

        if (count == 0) {
            if (isBBFFree(BBF4)) candidates[count++] = 4;
            if (isBBFFree(BBF5)) candidates[count++] = 5;
        }

        if (count == 0) return 0;
        return candidates[random(0, count)];
    }

    // RD_05 ENTRY ROUTING: ONLY ENTRY TURNOUTS (W3, W4, W9, W10, W11)
    void setBBFEntryRoute(Adafruit_PWMServoDriver& driver, int slot) {
        if (slot == 1) {
            ServoControl::switchTurnout(driver, W3, true);
            ServoControl::switchTurnout(driver, W4, false);
        } else if (slot == 2) {
            ServoControl::switchTurnout(driver, W3, true);
            ServoControl::switchTurnout(driver, W4, true);
        } else if (slot == 3) {
            ServoControl::switchTurnout(driver, W3, false);
            ServoControl::switchTurnout(driver, W9, true);
        } else if (slot == 4) {
            ServoControl::switchTurnout(driver, W3, false);
            ServoControl::switchTurnout(driver, W9, false);
            ServoControl::switchTurnout(driver, W10, false);
        } else if (slot == 5) {
            ServoControl::switchTurnout(driver, W3, false);
            ServoControl::switchTurnout(driver, W9, false);
            ServoControl::switchTurnout(driver, W10, true);
            ServoControl::switchTurnout(driver, W11, false);
        }

        BBF1.selected = (slot == 1);
        BBF2.selected = (slot == 2);
        BBF3.selected = (slot == 3);
        BBF4.selected = (slot == 4);
        BBF5.selected = (slot == 5);

        // FREE THROUGH-BBF IS DRIVABLE: SYNC THE powered FLAG TO REALITY (SECTION IS LIVE,
        // OR THE TRAIN COULDN'T ROLL IN). FLAG ONLY — NO RELAY, NO ZONE SWITCH (BBF HAS NONE).
        // → onBBFReedL SETS THE EXIT TURNOUTS (if wasPowered), AND THE STOP BUTTON REGISTERS.
        // GUARD: DO NOT OVERWRITE A STOP-REQUEST (powered=false && pending) — RD_05 WOULD OTHERWISE
        // FLIP A REQUESTED STOP INTO "WAITING TO DEPART" AND THE TRAIN WOULD PASS THROUGH.
        Tracks& target = *bbf[slot - 1].track;
        if (!(target.pending && !target.powered)) {
            target.powered = true;
        }

        // PERSIST SELECTED + powered FLAGS
        Eeprom::save();
    }

    // RD_BBFx_L EXIT ROUTING: ONLY EXIT TURNOUTS (W5, W6, W7, W8)
    // NO EEPROM SAVE: TURNOUT POSITIONS ARE NOT PERSISTED, NO ROUTE STATE CHANGES
    void setBBFExitRoute(Adafruit_PWMServoDriver& driver, int slot) {
        if (slot == 1) {
            ServoControl::switchTurnout(driver, W5, false);
            ServoControl::switchTurnout(driver, W7, true);
        } else if (slot == 2) {
            ServoControl::switchTurnout(driver, W5, true);
            ServoControl::switchTurnout(driver, W7, true);
        } else if (slot == 3) {
            ServoControl::switchTurnout(driver, W6, true);
            ServoControl::switchTurnout(driver, W7, false);
        } else if (slot == 4) {
            ServoControl::switchTurnout(driver, W6, false);
            ServoControl::switchTurnout(driver, W7, false);
            ServoControl::switchTurnout(driver, W8, true);
        } else if (slot == 5) {
            ServoControl::switchTurnout(driver, W6, false);
            ServoControl::switchTurnout(driver, W7, false);
            ServoControl::switchTurnout(driver, W8, false);
        }
    }

    // RD_BBFx_L REED (SLOT 1-5): TRAIN REACHES THE BBF EXIT END
    // - IF RUNNING AND ZONE C FREE: PASS THROUGH, SET EXIT TURNOUTS.
    // - ELSE STOP/HOLD: RELEASE ZONE A AND PREPARE THE NEXT FREE BBF ENTRY.
    void onBBFReedL(Adafruit_PWMServoDriver& driver, int slot) {
        if (slot < 1 || slot > BBF_COUNT) return;
        Tracks& track = *bbf[slot - 1].track;
        int relay = bbf[slot - 1].relay;

        bool wasPowered = track.powered;
        bool stopped = !(track.powered && !BLOCKC.occupied);

        stopBBF(track, relay, !BLOCKC.occupied);

        if (wasPowered) {
            setBBFExitRoute(driver, slot);
        }

        if (stopped) {
            releaseZoneA(driver, BLOCKA);
            int freeBBF = findFreeBBF();
            if (freeBBF != 0) {
                setBBFEntryRoute(driver, freeBBF);
            }
        }
    }

    // HBF OPERATIONS ############################################################################

    // HBF DEPARTURE OWNER OF W2 / ZONE A (0=NONE, 1=HBF1, 2=HBF2)
    void setDepartingHBF(int n) {
        departingHBF = n;
    }

    int  getDepartingHBF() {
        return departingHBF;
    }

    bool isHBFDeparting() {
        return departingHBF != 0;
    }

    // HBF _R REED: PARK IF NOT POWERED, FREE TRACK IF DEPARTING
    void stopHBF(Tracks& track, int zoneRelay, int trackRelay) {
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
    void toggleHBF(Tracks& track, int trackRelay) {
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

    // AUTO-RELEASE WAITING HBF TRAIN WHEN ZONE A CLEARS
    // RETURNS TRUE IF A PENDING HBF WAS RELEASED
    bool releasePendingHBF(Adafruit_PWMServoDriver& driver) {
        for (int idx = 0; idx < HBF_COUNT; idx++) {
            Tracks& track = *hbf[idx].track;
            if (track.powered && track.pending) {
                departingHBF = idx + 1; // 1=HBF1, 2=HBF2
                ServoControl::switchTurnout(driver, W2, hbf[idx].w2Diverging);
                toggleHBF(track, hbf[idx].trackRelay);
                return true;
            }
        }
        return false;
    }

    // ZONE A RELEASE ###########################################################################

    // ZONE A RELEASE: CALLED FROM RD_10 OR RD_BBFx_L WHEN HBF DEPARTURE CLEARED ZONE A
    // GUARD: ONLY FIRES WHEN BLOCKA IS OCCUPIED AND A DEPARTURE IS ACTIVE
    void releaseZoneA(Adafruit_PWMServoDriver& driver, Tracks& blockA) {
        if (!blockA.occupied || !isHBFDeparting()) return;
        blockA.occupied = false;
        MotorControl::stopRamp();
        // W2 IS NOW FREE
        setDepartingHBF(0);
        if (releasePendingHBF(driver)) {
            // DEPARTINGHBF SET INSIDE releasePendingHBF
            MotorControl::startRamp();
            blockA.occupied = true;
        }
        Eeprom::save();
    }

    // SHARED (BBF + HBF) #######################################################################

    // CANCEL PENDING WHEN TURNOUT IS SWITCHED AWAY FROM THIS TRACK
    void cancelPending(Tracks& track, int relay) {
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

}
