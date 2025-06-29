#include "../../config.h"
#include <Arduino.h>
#include <EEPROM.h>

uint32_t relayState = 0;

namespace RelayControl {

    void setRelays(uint32_t value) {
        digitalWrite(RELAY_LATCH, LOW);

        shiftOut(RELAY_DATA, RELAY_CLOCK, MSBFIRST, (value >> 16) & 0xFF);
        shiftOut(RELAY_DATA, RELAY_CLOCK, MSBFIRST, (value >> 8) & 0xFF);
        shiftOut(RELAY_DATA, RELAY_CLOCK, MSBFIRST, value & 0xFF);

        digitalWrite(RELAY_LATCH, HIGH);
    }

    void setRelay(int relayNumber, bool state) {
        if (relayNumber < 1 || relayNumber > RELAY_COUNT)
            return;
        uint32_t mask = (1UL << (relayNumber - 1));

        if (!state) {
            relayState |= mask;
        } else {
            relayState &= ~mask;
        }
        setRelays(relayState);
        EEPROM.put(EEPROM_RELAY, relayState);
    }

    void toggleRelay(int relayNumber) {
        if (relayNumber < 1 || relayNumber > RELAY_COUNT)
            return;

        relayState ^= (1UL << (relayNumber - 1));
        setRelays(relayState);
        EEPROM.put(EEPROM_RELAY, relayState);
    }

    void initRelays() {
        EEPROM.get(EEPROM_RELAY, relayState);
        setRelays(relayState);
    }
}
