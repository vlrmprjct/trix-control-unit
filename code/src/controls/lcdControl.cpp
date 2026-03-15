#include "lcdControl.h"
#include <LiquidCrystal.h>

namespace LCDControl {

    void print(LiquidCrystal& lcd, int startChar, int endChar, int row, String value, const char* direction) {
        // Cache mit fester Größe (8 Slots) - generisch für beliebige Positionen
        const int CACHE_SIZE = 8;
        static String cacheValue[CACHE_SIZE];
        static int cacheKey[CACHE_SIZE];
        static bool cacheInit = false;
        
        if (!cacheInit) {
            for (int i = 0; i < CACHE_SIZE; i++) {
                cacheKey[i] = -1;
                cacheValue[i] = "";
            }
            cacheInit = true;
        }

        // Eindeutiger Key aus Position: row * 100 + startChar
        int posKey = row * 100 + startChar;
        int cacheSlot = posKey % CACHE_SIZE;
        
        // Cache-Check
        if (cacheKey[cacheSlot] == posKey && cacheValue[cacheSlot] == value) {
            return;
        }
        
        cacheKey[cacheSlot] = posKey;
        cacheValue[cacheSlot] = value;

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
