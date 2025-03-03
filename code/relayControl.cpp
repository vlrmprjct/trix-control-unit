#include "config.h"
#include <Arduino.h>

uint32_t relaisState = 0;

void setRelays(uint32_t value) {
    digitalWrite(RELAY_LATCH, LOW);

    shiftOut(RELAY_DATA, RELAY_CLOCK, MSBFIRST, value & 0xFF);
    shiftOut(RELAY_DATA, RELAY_CLOCK, MSBFIRST, (value >> 8) & 0xFF);
    shiftOut(RELAY_DATA, RELAY_CLOCK, MSBFIRST, (value >> 16) & 0xFF);

    digitalWrite(RELAY_LATCH, HIGH);
}

void setRelay(int relayNumber, bool state) {
    if (relayNumber < 1 || relayNumber > RELAY_COUNT)
        return;
    uint32_t mask = (1UL << (relayNumber - 1));

    if (!state) {
        relaisState |= mask;
    } else {
        relaisState &= ~mask;
    }
    setRelays(relaisState);
}

void toggleRelay(int relayNumber) {
    if (relayNumber < 1 || relayNumber > RELAY_COUNT)
        return;

    relaisState ^= (1UL << (relayNumber - 1));
    setRelays(relaisState);
}
