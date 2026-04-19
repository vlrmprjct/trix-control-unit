#pragma once

#include "config.h"
#include "state.h"
#include "naming.h"
#include "../controls/lcdControl.h"
#include "../controls/relayControl.h"
#include "../controls/encoderControl.h"
#include "../controls/motorControl.h"
#include "../utils/eeprom.h"
#include "../utils/utils.h"
#include <Adafruit_PWMServoDriver.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <Wire.h>

inline void init(Adafruit_PWMServoDriver& servo, LiquidCrystal& lcd) {
    Serial.begin(9600);

    // INIT I2C BUS ###############################################################################
    Wire.begin();

    // INIT SPI BUS ###############################################################################
    SPI.begin();

    // INIT LCD DOT MATRIX ########################################################################
    lcd.begin(20, 4);
    LCDControl::print(lcd, 0, 10, 0, "BOOTING ...");
    LCDControl::print(lcd, 0, 19, 1, FIRMWARE_VERSION);

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

    // INIT MOTOR MODULE (Hardware SPI) ##########################################################
    pinMode(DIGIPOT1_CS, OUTPUT);
    pinMode(DIGIPOT2_CS, OUTPUT);
    digitalWrite(DIGIPOT1_CS, HIGH);
    digitalWrite(DIGIPOT2_CS, HIGH);
    MotorControl::setValue(ZONE_A, 0);
    MotorControl::setValue(ZONE_B, 0);
    MotorControl::setValue(ZONE_C, 0);
    MotorControl::setValue(ZONE_D, 0);

    // INIT TURNOUTS SERVO MODULE #################################################################
    servo.begin();
    servo.setPWMFreq(60);

    // INIT RELAYS ################################################################################
    pinMode(RELAY_LATCH, OUTPUT);
    pinMode(RELAY_CLOCK, OUTPUT);
    pinMode(RELAY_DATA, OUTPUT);
    RelayControl::initRelays();

    // INIT BUTTON CTRL ###########################################################################
    pinMode(BTN_DATA, INPUT);
    pinMode(BTN_CLOCK, OUTPUT);
    pinMode(BTN_LATCH, OUTPUT);

    // INIT REED CTRL #############################################################################
    pinMode(REED_DATA, INPUT);
    pinMode(REED2_DATA, INPUT);
    pinMode(REED_CLOCK, OUTPUT);
    pinMode(REED_LATCH, OUTPUT);

    // SET PWM FREQUENCY ##########################################################################
    Utils::setPrescalerTimers(0x01);

    // LOAD STATE / READ FROM EEPROM ##############################################################
    Eeprom::load();

    delay(2000);

    // CLEAR EEPROM ###############################################################################
    // Eeprom::clear();
    lcd.clear();
}
