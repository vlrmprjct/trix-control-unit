#pragma once

namespace Debug {

    extern bool enabled;        // state debug on/off
    extern bool eepromEnabled;  // eeprom debug on/off
    extern unsigned long interval; // check interval in ms (default 500)

    void printState();
    void printEeprom();

}
