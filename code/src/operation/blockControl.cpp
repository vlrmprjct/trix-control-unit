#include "blockControl.h"
#include "../controls/relayControl.h"
#include "../utils/eeprom.h"

const BBFConfig BlockControl::bbf[] = {
    { &BBF1, 2 },
    { &BBF2, 3 },
    { &BBF3, 4 },
    // { &BBF4, ?? },  // RELAY TBD
    // { &BBF5, ?? },  // RELAY TBD
};
const int BlockControl::BBF_COUNT = sizeof(bbf) / sizeof(bbf[0]);

// STOP BBF TRAIN IF NOT POWERED OR ZONE C IS BLOCKED
void BlockControl::stopBBF(Tracks& track, int relay) {
    if (!track.powered || BLOCKC.occupied) {
        RelayControl::setRelay(relay, false);
        track.occupied = true;
        Eeprom::save();
    }
}

// TOGGLE BBF POWER, ONLY ENABLE RELAY IF ZONE C IS FREE
void BlockControl::toggleBBF(Tracks& track, int relay) {
    if (!track.selected) return;
    track.powered = !track.powered;
    if (track.powered && !BLOCKC.occupied) {
        RelayControl::setRelay(relay, true);
    }
    Eeprom::save();
}

// RELEASE FIRST SELECTED+POWERED+OCCUPIED BBF (CALLED WHEN ZONE C CLEARS)
void BlockControl::releasePendingBBF() {
    for (int i = 0; i < BBF_COUNT; i++) {
        Tracks& t = *bbf[i].track;
        if (t.selected && t.powered && t.occupied) {
            RelayControl::setRelay(bbf[i].relay, true);
            t.occupied = false;
            return;
        }
    }
}
