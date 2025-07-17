#pragma once

#include <Arduino.h>
#include <LiquidCrystal.h>

namespace LCDControl {
    void print(LiquidCrystal& lcd, int startChar, int endChar, int row, String value, const char* direction = "LTR");
    void resetLCD(int buttonPin, LiquidCrystal& lcd);
}
