#include "tagreader.h"
#include "../../profiles.h"
#include <Arduino.h>

UIDProfile* getTag(MFRC522& rfid) {
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
        return nullptr;

    String uidStr = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] < 0x10)
            uidStr += "0";
        uidStr += String(rfid.uid.uidByte[i], HEX);
        if (i < rfid.uid.size - 1)
            uidStr += " ";
    }
    uidStr.toUpperCase();

    Serial.print("Gelesene UID: ");
    Serial.println(uidStr);

    for (int i = 0; i < profileCount; i++) {
        if (uidStr.equals(profiles[i].uid)) {
            rfid.PICC_HaltA();
            rfid.PCD_StopCrypto1();
            return &profiles[i];
        }
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return nullptr;
}
