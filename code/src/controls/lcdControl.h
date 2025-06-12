#ifndef LCDCONTROL_H
#define LCDCONTROL_H

#include <Arduino.h>
#include <LiquidCrystal.h>

namespace LCDControl {
    void print(LiquidCrystal& lcd, int startChar, int endChar, int row, String value, const char* direction = "LTR");
}
#endif
