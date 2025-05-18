#include "lcdControl.h"

namespace LCDControl {

    void print(LiquidCrystal& lcd, int startChar, int endChar, int row, String value, const char* direction) {
        static String lastValue = "";
        if (value == lastValue)
            return;
        lastValue = value;

        int length = endChar - startChar + 1;

        lcd.setCursor(startChar, row);
        for (int i = 0; i < length; i++) {
            lcd.print(" ");
        }

        int col = startChar;

        if (direction == "RTL") {
            int textLength = value.length();
            col = 19 - textLength;

            if (col < startChar) {
                col = startChar;
            }
        }

        lcd.setCursor(col, row);
        lcd.print(value);
    }

}
