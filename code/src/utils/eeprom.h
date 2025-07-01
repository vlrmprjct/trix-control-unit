#pragma once

#include <EEPROM.h>

namespace Eeprom {
    void initState();
    void debug(int from = 0, int to = EEPROM.length());
    void clear(uint8_t value = 0xFF);
}
