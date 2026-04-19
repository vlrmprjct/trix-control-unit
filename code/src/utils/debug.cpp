#include "debug.h"
#include "../core/config.h"
#include "../core/state.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <string.h>

namespace Debug {

    bool enabled          = true;
    bool eepromEnabled    = true;
    unsigned long interval = 1000;

    static Routes _prev;
    static bool   _initialized = false;
    static unsigned long _lastCheck = 0;

    // EEPROM snapshot
    struct EepromSnapshot {
        Routes      route;
        uint32_t    relay;
        RouteMeta   meta;
    };
    static EepromSnapshot _prevEeprom;
    static bool           _eepromInitialized = false;
    static unsigned long  _lastEepromCheck   = 0;

    // -----------------------------------------------------------------------
    // Internal helpers
    // -----------------------------------------------------------------------

    static void printSeparator() {
        Serial.println(F("+------+----------+---------+----------+-----+-----+-------+"));
    }

    static void printCell(const char* value, int width) {
        Serial.print(F("| "));
        int len = strlen(value);
        Serial.print(value);
        for (int i = len; i < width; i++) Serial.print(' ');
        Serial.print(' ');
    }

    static void printIntCell(int value, int width) {
        char buf[8];
        itoa(value, buf, 10);
        int len = strlen(buf);
        Serial.print(F("| "));
        for (int i = len; i < width; i++) Serial.print(' ');
        Serial.print(buf);
        Serial.print(' ');
    }

    static void printRow(const char* label, const Tracks& t) {
        printCell(label,            4);
        printCell(t.selected  ? "YES" : "no",  8);
        printCell(t.powered   ? "YES" : "no",  7);
        printCell(t.occupied  ? "YES" : "no",  8);
        printIntCell(t.min,   3);
        printIntCell(t.max,   3);
        printIntCell(t.brake, 5);
        Serial.println(F("|"));
    }

    // -----------------------------------------------------------------------
    // Public API
    // -----------------------------------------------------------------------

    void printState() {
        if (!enabled) return;

        unsigned long now = millis();
        if (now - _lastCheck < interval) return;
        _lastCheck = now;

        if (!_initialized) {
            _prev        = ROUTE;
            _initialized = true;
            Serial.println(F("\n=== DEBUG: initial state ==="));
        } else {
            if (memcmp(&ROUTE, &_prev, sizeof(Routes)) == 0) return;
            _prev = ROUTE;
            Serial.println(F("\n=== DEBUG: state changed ==="));
        }

        printSeparator();
        Serial.println(F("| Track| selected | powered | occupied | min | max | brake |"));
        printSeparator();
        printRow("HBF1", ROUTE.track[1]);
        printRow("HBF2", ROUTE.track[2]);
        printRow("BBF1", ROUTE.track[3]);
        printRow("BBF2", ROUTE.track[4]);
        printRow("BBF3", ROUTE.track[5]);
        printRow("BBF4", ROUTE.track[6]);
        printRow("BBF5", ROUTE.track[7]);
        printRow("BLKA", ROUTE.track[8]);
        printRow("BLKB", ROUTE.track[9]);
        printRow("BLKC", ROUTE.track[10]);
        printSeparator();
    }

    // -----------------------------------------------------------------------
    // EEPROM debug
    // -----------------------------------------------------------------------

    static void printEepromSeparator() {
        Serial.println(F("+------+----------+---------+----------+-----+-----+-------+"));
    }

    static void printEepromRow(const char* label, const Tracks& ram, const Tracks& ee) {
        printCell(label, 4);
        // selected
        if (ram.selected != ee.selected) {
            char buf[10];
            strcpy(buf, ee.selected ? "YES!" : "no!");
            printCell(buf, 8);
        } else {
            printCell(ee.selected ? "YES" : "no", 8);
        }
        // powered
        if (ram.powered != ee.powered) {
            char buf[10];
            strcpy(buf, ee.powered ? "YES!" : "no!");
            printCell(buf, 7);
        } else {
            printCell(ee.powered ? "YES" : "no", 7);
        }
        printCell(ee.occupied ? "YES" : "no", 8);
        printIntCell(ee.min,   3);
        printIntCell(ee.max,   3);
        printIntCell(ee.brake, 5);
        Serial.println(F("|"));
    }

    void printEeprom() {
        if (!eepromEnabled) return;

        unsigned long now = millis();
        if (now - _lastEepromCheck < interval) return;
        _lastEepromCheck = now;

        // Read current EEPROM content
        EepromSnapshot current;
        EEPROM.get(EEPROM_ROUTE, current.route);
        EEPROM.get(EEPROM_RELAY, current.relay);
        EEPROM.get(EEPROM_META,  current.meta);

        if (!_eepromInitialized) {
            _prevEeprom       = current;
            _eepromInitialized = true;
            Serial.println(F("\n=== EEPROM DEBUG: initial ==="));
        } else {
            if (memcmp(&current, &_prevEeprom, sizeof(EepromSnapshot)) == 0) return;
            _prevEeprom = current;
            Serial.println(F("\n=== EEPROM DEBUG: changed ==="));
        }

        // META
        Serial.print(F("  META  magic=0x"));
        Serial.print(current.meta.magic, HEX);
        Serial.print(F("  version="));
        Serial.print(current.meta.version);
        bool valid = (current.meta.magic == ROUTE_MAGIC && current.meta.version == ROUTE_VERSION);
        Serial.println(valid ? F("  [OK]") : F("  [INVALID!]"));

        // RELAY bitmask
        Serial.print(F("  RELAY bitmask=0b"));
        for (int i = 23; i >= 0; i--) {
            Serial.print((current.relay >> i) & 1);
            if (i > 0 && i % 8 == 0) Serial.print('_');
        }
        Serial.println();

        // ROUTE table (EEPROM vs RAM, highlight mismatches with !)
        printEepromSeparator();
        Serial.println(F("| Track| selected | powered | occupied | min | max | brake |"));
        printEepromSeparator();
        const char* labels[] = {"HBF1","HBF2","BBF1","BBF2","BBF3","BBF4","BBF5","BLKA","BLKB","BLKC"};
        for (int i = 1; i <= 10; i++) {
            printEepromRow(labels[i-1], ROUTE.track[i], current.route.track[i]);
        }
        printEepromSeparator();
        Serial.println(F("  (! = differs from RAM)"));
    }

}
