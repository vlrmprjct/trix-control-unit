#include "../../config.h"
#include "../../state.h"
#include <Arduino.h>
#include <EEPROM.h>

namespace Eeprom {

    void initState() {
        // EEPROM.get(EEPROM_ROUTE, ROUTE);

        // bool needsInit = false;
        // if (HBF1.name[0] == '\0' && HBF2.name[0] == '\0')
        //     needsInit = true;

        // if (needsInit) {
        //     HBF1.powered = true;
        //     HBF2.powered = true;
        //     BBF1.powered = true;
        //     BBF2.powered = true;
        //     BBF3.powered = true;
        //     HBF1.name[0] = '\0';
        //     HBF2.name[0] = '\0';

        //     EEPROM.put(EEPROM_ROUTE, ROUTE);
        // }
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

