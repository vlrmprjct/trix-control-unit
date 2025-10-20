#pragma once

#include <EEPROM.h>

namespace Eeprom {
    void debug();
    void reset();
    void load();
    void save();
    void clear();
}
