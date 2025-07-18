#include "lcdControl.h"
#include <LiquidCrystal.h>

namespace LCDControl {

    void print(LiquidCrystal& lcd, int startChar, int endChar, int row, String value, const char* direction) {
        static String lastValue1 = "";
        static String lastValue2 = "";

        String* lastValue = nullptr;
        if (startChar <= 7) {
            lastValue = &lastValue1;
        } else {
            lastValue = &lastValue2;
        }

        if (value == *lastValue)
            return;
        *lastValue = value;

        int length = endChar - startChar + 1;

        lcd.setCursor(startChar, row);
        for (int i = 0; i < length; i++) {
            lcd.print(" ");
        }

        int col = startChar;

        if (direction == "RTL") {
            int textLength = value.length();
            col = endChar - textLength + 1;
            if (col < startChar) {
                col = startChar;
            }
        }

        lcd.setCursor(col, row);
        lcd.print(value);
    }



    void resetLCD(int buttonPin, LiquidCrystal& lcd) {
        if (digitalRead(buttonPin) == LOW) return;
        lcd.clear();
        lcd.print("RST");
        delay(1000);
    }

}
