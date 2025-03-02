#include "config.h"
#include <Arduino.h>

uint32_t relaisState = 0; // Aktueller Status aller Relais

void setRelays(uint32_t value)
{
    digitalWrite(LATCH, LOW);
    shiftOut(DATA, CLOCK, MSBFIRST, (value >> 16) & 0xFF);
    shiftOut(DATA, CLOCK, MSBFIRST, (value >> 8) & 0xFF);
    shiftOut(DATA, CLOCK, MSBFIRST, value & 0xFF);
    digitalWrite(LATCH, HIGH);
}

void setRelay(int relayNumber, bool state)
{
    if (relayNumber < 1 || relayNumber > RELAY_COUNT)
        return;
    uint32_t mask = (1UL << (relayNumber - 1)); // Maske für das Relais

    if (state) {
        relaisState |= mask; // Relais einschalten (Bit setzen)
    } else {
        relaisState &= ~mask; // Relais ausschalten (Bit löschen)
    }
    setRelays(relaisState); // Neuen Zustand anwenden
}

void toggleRelay(int relayNumber)
{
    if (relayNumber < 1 || relayNumber > RELAY_COUNT)
        return;

    relaisState ^= (1UL << (relayNumber - 1)); // Relais-Zustand umkehren
    setRelays(relaisState);
}
