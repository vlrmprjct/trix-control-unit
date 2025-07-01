#include "../../config.h"
#include "../../state.h"
#include <Arduino.h>
#include <EEPROM.h>

namespace Eeprom {

    void initState() {
        EEPROM.get(EEPROM_ROUTE, HBF_ROUTE);
        EEPROM.get(EEPROM_ACTIVE, HBF_ACTIVE);

        bool needsInit = false;
        if (HBF_ROUTE.HBF1.name[0] == '\0' && HBF_ROUTE.HBF2.name[0] == '\0' && HBF_ROUTE.HBF3.name[0] == '\0')
            needsInit = true;
        if (HBF_ACTIVE.HBF1.name[0] == '\0' && HBF_ACTIVE.HBF2.name[0] == '\0' && HBF_ACTIVE.HBF3.name[0] == '\0')
            needsInit = true;

        if (needsInit) {
            HBF_ROUTE.HBF1.active = false;
            HBF_ROUTE.HBF2.active = false;
            HBF_ROUTE.HBF3.active = false;
            HBF_ROUTE.HBF1.name[0] = '\0';
            HBF_ROUTE.HBF2.name[0] = '\0';
            HBF_ROUTE.HBF3.name[0] = '\0';

            HBF_ACTIVE.HBF1.active = false;
            HBF_ACTIVE.HBF2.active = false;
            HBF_ACTIVE.HBF3.active = false;
            HBF_ACTIVE.HBF1.name[0] = '\0';
            HBF_ACTIVE.HBF2.name[0] = '\0';
            HBF_ACTIVE.HBF3.name[0] = '\0';

            EEPROM.put(EEPROM_ROUTE, HBF_ROUTE);
            EEPROM.put(EEPROM_ROUTE, HBF_ACTIVE);
        }
    }

    void debug(int from = 0, int to = EEPROM.length()) {
        Serial.println("EEPROM dump:");
        for (int i = from; i < to; ++i) {
            byte value = EEPROM.read(i);
            Serial.print("Addr ");
            Serial.print(i);
            Serial.print(": ");
            Serial.println(value, DEC);
        }
    }

    void clear(uint8_t value) {
        for (int i = 0; i < EEPROM.length(); i++) {
            EEPROM.write(i, value);
        }
        while (1)
            ;
    }
}

