#include "../../config.h"
#include "../../state.h"
#include <Arduino.h>
#include <EEPROM.h>

namespace Eeprom {

    void debug() {
        Serial.println(F("=== EEPROM ROUTE DEBUG ==="));

        constexpr int trackCount = sizeof(ROUTE.track)/sizeof(ROUTE.track[0]);

        for (int i = 1; i < trackCount; ++i) {
            Tracks t;
            EEPROM.get(EEPROM_ROUTE + sizeof(Tracks) * i, t);

            Serial.print(F("Track "));
            Serial.print(i);
            Serial.print(F(" | "));

            Serial.print(F("occupied: "));
            Serial.print(t.occupied ? "true" : "false");
            Serial.print(F(" | selected: "));
            Serial.print(t.selected ? "true" : "false");
            Serial.print(F(" | powered: "));
            Serial.print(t.powered ? "true" : "false");
            Serial.print(F(" | name: "));
            Serial.print(t.name);
            Serial.print(F(" | uid: "));
            Serial.print(t.uid);
            Serial.print(F(" | min: "));
            Serial.print(t.min);
            Serial.print(F(" | max: "));
            Serial.print(t.max);
            Serial.print(F(" | brake: "));
            Serial.println(t.brake);
        }

        Serial.println(F("==========================="));
    }

    void reset() {
        for (int i = 1; i <= 7; ++i) {
            ROUTE.track[i].occupied = false;
            ROUTE.track[i].selected = false;
            ROUTE.track[i].powered  = false;
            ROUTE.track[i].name[0]  = '\0';
            ROUTE.track[i].uid[0]   = '\0';
            ROUTE.track[i].min      = 0;
            ROUTE.track[i].max      = 0;
            ROUTE.track[i].brake    = 0;
        }
    }

    void load() {
        RouteMeta META;
        EEPROM.get(EEPROM_META, META);

        if (META.magic != ROUTE_MAGIC || META.version != ROUTE_VERSION) {
            Serial.println(F("EEPROM: Invalid versioning - setting defaults"));

            reset();

            META.magic = ROUTE_MAGIC;
            META.version = ROUTE_VERSION;

            EEPROM.put(EEPROM_ROUTE, ROUTE);
            EEPROM.put(EEPROM_META, META);
        } else {
            EEPROM.get(EEPROM_ROUTE, ROUTE);
        }
    }

    void save() {
        EEPROM.put(EEPROM_ROUTE, ROUTE);
    }

    void clear() {
        for (int i = 0; i < EEPROM.length(); i++) {
            EEPROM.write(i, 0xFF);
        }
        Serial.println(F("EEPROM completely wiped"));
    }
}

