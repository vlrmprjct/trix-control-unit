#pragma once

#include "config.h"
#include "state.h"
#include "naming.h"
#include "src/controls/lcdControl.h"
#include "src/controls/relayControl.h"
#include "src/controls/encoderControl.h"
#include "src/controls/motorControl.h"
#include "src/utils/eeprom.h"
#include "src/utils/utils.h"
#include <Adafruit_PWMServoDriver.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <Wire.h>

inline void init(Adafruit_PWMServoDriver& servo, LiquidCrystal& lcd) {
    Serial.begin(9600);

    // INIT I2C BUS ###############################################################################
    SPI.begin();

    // INIT LCD DOT MATRIX ########################################################################
    lcd.begin(20, 4);
    LCDControl::print(lcd, 0, 10, 0, "BOOTING ...");
    delay(2000);

    // RESET LCD DOT MATRIX IN CASE OF WEIRD CHARACTERS ###########################################
    pinMode(LCD_RST, INPUT);

    // INIT ENCODER (ZONE A) ######################################################################
    pinMode(ENC_ZONE_A_CLK, INPUT_PULLUP);
    pinMode(ENC_ZONE_A_DT, INPUT_PULLUP);
    ENC_ZONE_A_CLK_STATE = digitalRead(ENC_ZONE_A_CLK);
    attachInterrupt(digitalPinToInterrupt(ENC_ZONE_A_CLK), EncoderControl::processZoneA, CHANGE);

    // INIT ENCODER (ZONE B) ######################################################################
    pinMode(ENC_ZONE_B_CLK, INPUT_PULLUP);
    pinMode(ENC_ZONE_B_DT, INPUT_PULLUP);
    ENC_ZONE_B_CLK_STATE = digitalRead(ENC_ZONE_B_CLK);
    attachInterrupt(digitalPinToInterrupt(ENC_ZONE_B_CLK), EncoderControl::processZoneB, CHANGE);

    // INIT MOTOR MODULE ##########################################################################
    pinMode(DIGIPOT_MOSI, OUTPUT);
    pinMode(DIGIPOT_SCK, OUTPUT);
    pinMode(DIGIPOT_CS, OUTPUT);
    digitalWrite(DIGIPOT_CS, HIGH);
    digitalWrite(DIGIPOT_SCK, LOW);
    MotorControl::setValue(ZONE_A, 0);
    MotorControl::setValue(ZONE_B, 0);

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
    // Eeprom::initState();
    EEPROM.get(EEPROM_ROUTE, HBF_ROUTE);

    // CLEAR EEPROM ###############################################################################
    // Eeprom::clear();

    lcd.clear();
}
