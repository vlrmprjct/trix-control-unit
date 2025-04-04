#ifndef LCDPRINT_H
#define LCDPRINT_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

void lcdPrint(LiquidCrystal_I2C& lcd, int startChar, int endChar, int row, String value, const char* direction = "LTR");

#endif // LCDPRINT_H
