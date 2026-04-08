#pragma once

#include <EEPROM.h>

namespace Eeprom {
    void reset();
    void load();
    void save();
    void clear();
}
