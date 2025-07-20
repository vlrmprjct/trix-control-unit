#pragma once

#include "config.h"
#include "state.h"
#include "src/controls/lcdControl.h"
#include "src/controls/relayControl.h"
#include "src/controls/encoderControl.h"
#include "src/utils/eeprom.h"
#include "src/utils/utils.h"
#include <Adafruit_PWMServoDriver.h>
#include <LiquidCrystal.h>
#include <MFRC522.h>
#include <SPI.h>
#include <Wire.h>

inline void init(Adafruit_PWMServoDriver& servo, LiquidCrystal& lcd, MFRC522& rfid) {
    Serial.begin(9600);

    // INIT I2C BUS ###############################################################################
    SPI.begin();

    // INIT NFC RC522 RFID MODULE #################################################################
    rfid.PCD_Init();

    // INIT LCD DOT MATRIX ########################################################################
    lcd.begin(20, 4);
    LCDControl::print(lcd, 0, 10, 0, "BOOTING ...");
    delay(2000);

    // RESET LCD DOT MATRIX IN CASE OF WEIRD CHARACTERS ###########################################
    pinMode(LCD_RST, INPUT);

    // INIT ENCODER (PRIMARY) #####################################################################
    pinMode(ENC_PRIMARY_CLOCK, INPUT_PULLUP);
    pinMode(ENC_PRIMARY_DT, INPUT_PULLUP);
    ENC_PRIMARY_CLOCK_STATE = digitalRead(ENC_PRIMARY_CLOCK);
    attachInterrupt(digitalPinToInterrupt(ENC_PRIMARY_CLOCK), EncoderControl::processPrimary, CHANGE);

    // INIT ENCODER (SECONDARY) ###################################################################
    pinMode(ENC_SECONDARY_CLOCK, INPUT_PULLUP);
    pinMode(ENC_SECONDARY_DT, INPUT_PULLUP);
    ENC_SECONDARY_CLOCK_STATE = digitalRead(ENC_SECONDARY_CLOCK);
    attachInterrupt(digitalPinToInterrupt(ENC_SECONDARY_CLOCK), EncoderControl::processSecondary, CHANGE);

    // INIT MOTOR MODULE ##########################################################################
    pinMode(MOTOR_ZONE_A_1, OUTPUT);
    pinMode(MOTOR_ZONE_A_2, OUTPUT);
    pinMode(MOTOR_ZONE_B_1, OUTPUT);
    pinMode(MOTOR_ZONE_B_2, OUTPUT);
    pinMode(MOTOR_ZONE_C_1, OUTPUT);
    pinMode(MOTOR_ZONE_C_2, OUTPUT);

    // INIT TURNOUTS SERVO MODULE #################################################################
    servo.begin();
    servo.setPWMFreq(60);
    delay(500);

    // INIT RELAYS ################################################################################
    pinMode(RELAY_LATCH, OUTPUT);
    pinMode(RELAY_CLOCK, OUTPUT);
    pinMode(RELAY_DATA, OUTPUT);
    RelayControl::initRelays();
    delay(500);

    // INIT BUTTON CTRL ###########################################################################
    pinMode(BTN_DATA, INPUT);
    pinMode(BTN_CLOCK, OUTPUT);
    pinMode(BTN_LATCH, OUTPUT);

    // INIT REED CTRL #############################################################################
    pinMode(REED_DATA, INPUT);
    pinMode(REED_CLOCK, OUTPUT);
    pinMode(REED_LATCH, OUTPUT);

    // SET PWM FREQUENCY ##########################################################################
    Utils::setPrescalerTimers(0x01);

    // INIT STATE / READ FROM EEPROM ##############################################################
    Eeprom::initState();
    EEPROM.get(EEPROM_ROUTE, HBF_ROUTE);
    EEPROM.get(EEPROM_ACTIVE, HBF_ACTIVE);

    // CLEAR EEPROM ###############################################################################
    // Eeprom::clear();

    lcd.clear();
}
