#include "config.h"
#include "init.h"
#include "naming.h"
#include "src/controls/buttonControl.h"
#include "src/controls/encoderControl.h"
#include "src/controls/lcdControl.h"
#include "src/controls/motorControl.h"
#include "src/controls/reedControl.h"
#include "src/controls/relayControl.h"
#include "src/controls/servoControl.h"
#include "src/utils/eeprom.h"
#include "src/utils/utils.h"
#include "state.h"

#include <Adafruit_PWMServoDriver.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>

Adafruit_PWMServoDriver servo = Adafruit_PWMServoDriver();
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD1_D4, LCD1_D5, LCD1_D6, LCD1_D7);

bool hbfStop = false;
bool hbfStart = false;
int hbfMin = 60;
int hbfMax = 80;
int hbfBrake = 110;
int dist_rd1_rd2 = 31.5; // Distance between reed sensors 1 and 2 in cm
int dist_rd2_rd3 = 33.0; // Distance between reed sensors 2 and 3 in cm

void setup() {
    init(servo, lcd);
}

void loop() {

    // LCD RESET ##################################################################################
    LCDControl::resetLCD(LCD_RST, lcd);

    // TIMER ######################################################################################
    static unsigned long startTimer;

    // GET DIRECTION ##############################################################################
    static int dir = EncoderControl::getDirection(EncoderControl::encoderZoneA);

    // SYNC DIRECTION
    EncoderControl::syncDirections(EncoderControl::encoderZoneA, EncoderControl::encoderZoneB);

    // TRACK REED #################################################################################
    ReedControl::updateStates();

    ReedControl::push(3, []() {
        // HBF2 LEFT
        Utils::speedStart = millis();
        if (!HBF2.powered) {
            hbfBrake = HBF2.brake;
        }
    });

    ReedControl::push(4, []() {
        // HBF2 CENTER
        Utils::speedEnd = millis();
        Utils::currentSpeed = Utils::speedMeasure(Utils::speedStart, Utils::speedEnd, 31.0);
        if (!HBF2.powered) {
            hbfStop = true;
            hbfMin = HBF2.min;
        }
    });

    ReedControl::push(5, []() {
        // HBF2 RIGHT
        hbfStop = false;
        if (!HBF2.powered) {
            RelayControl::setRelay(5, true);
            RelayControl::setRelay(6, false);
        }
    });

    ReedControl::push(2, []() {
        // BBF1 RIGHT
    });

    ReedControl::push(1, []() {
        // BBF1 CENTER
    });

    ReedControl::push(6, []() {
        Utils::speedStart = millis();
        if (!HBF1.powered) {
            hbfBrake = HBF1.brake;
        }
    });

    ReedControl::push(7, []() {
        Utils::speedEnd = millis();
        Utils::currentSpeed = Utils::speedMeasure(Utils::speedStart, Utils::speedEnd, dist_rd1_rd2);
        if (!HBF1.powered) {
            hbfStop = true;
            hbfMin = HBF1.min;
        }
    });

    ReedControl::push(8, []() {
        hbfStop = false;
        if (!HBF1.powered) {
            // SWITCH FROM ZONE C TO A @ HBF1
            RelayControl::setRelay(7, true);
            // TURN OFF HBF1
            RelayControl::setRelay(8, false);
        }
    });

    ReedControl::push(14, []() {
        // SWITCH FROM ZONE C TO A @ HBF1
        RelayControl::setRelay(5, true);
        RelayControl::setRelay(7, true);
    });

    // TRACK CONTROL ##############################################################################
    ButtonControl::updateStates();

    ButtonControl::pushButton(BTN_HBF1, []() {
        if (!HBF1.selected) {
            return;
        }

        // Umschalten des Zustands (toggle)
        HBF1.powered = !HBF1.powered;

        if (HBF1.powered) {
            // === ABFAHRENDER ZUG ===
            // Zone C aktivieren, HBF1 einschalten
            RelayControl::setRelay(7, false); // Trenne Zone A
            RelayControl::setRelay(8, true); // Schalte HBF1 ein

            hbfStart = true;
            hbfMax = abs(HBF1.max) * (-1);
        } else {
            // === ANKOMMENDER ZUG ===
            // Zone A wieder verbinden
            RelayControl::setRelay(7, true); // Schalte Zone A an
        }

        EEPROM.put(EEPROM_ROUTE, ROUTE);
    });

    ButtonControl::pushButton(BTN_HBF2, []() {
        if (!HBF2.selected) {
            return;
        }
        // Umschalten des Zustands (toggle)
        HBF2.powered = !HBF2.powered;

        if (HBF2.powered) {
            // === ABFAHRENDER ZUG ===
            // Zone C aktivieren, HBF2 einschalten
            RelayControl::setRelay(5, false); // Trenne Zone A
            RelayControl::setRelay(6, true); // Schalte HBF2 ein

            hbfStart = true;
            hbfMax = abs(HBF2.max) * (-1);
        } else {
            // === ANKOMMENDER ZUG ===
            // Zone A wieder verbinden
            RelayControl::setRelay(5, true); // Schalte Zone A an
        }

        EEPROM.put(EEPROM_ROUTE, ROUTE);
    });

    ButtonControl::pushButton(BTN_BBF1, []() {
        if (!BBF1.selected) {
            return;
        }
        BBF1.powered = !BBF1.powered;
        RelayControl::setRelay(2, BBF1.powered);
        EEPROM.put(EEPROM_ROUTE, ROUTE);
    });

    ButtonControl::pushButton(BTN_BBF2, []() {
        if (!BBF2.selected) {
            return;
        }
        BBF2.powered = !BBF2.powered;
        RelayControl::setRelay(3, BBF2.powered);
        EEPROM.put(EEPROM_ROUTE, ROUTE);
    });

    ButtonControl::pushButton(BTN_BBF3, []() {
        if (!BBF3.selected) {
            return;
        }
        BBF3.powered = !BBF3.powered;
        RelayControl::setRelay(4, BBF3.powered);
        EEPROM.put(EEPROM_ROUTE, ROUTE);
    });

    ButtonControl::pushButton(SW_HBF1, []() {
        ServoControl::switchTurnout(servo, W1, false);
        ServoControl::switchTurnout(servo, W2, true);
        HBF1.selected = true;
        HBF2.selected = false;
        EEPROM.put(EEPROM_ROUTE, ROUTE);
    });

    ButtonControl::pushButton(SW_HBF2, []() {
        ServoControl::switchTurnout(servo, W1, true);
        ServoControl::switchTurnout(servo, W2, false);
        HBF1.selected = false;
        HBF2.selected = true;
        EEPROM.put(EEPROM_ROUTE, ROUTE);
    });

    ButtonControl::pushButton(SW_BBF1, []() {
        ServoControl::switchTurnout(servo, W3, true);
        ServoControl::switchTurnout(servo, W4, false);
        ServoControl::switchTurnout(servo, W5, false);
        ServoControl::switchTurnout(servo, W7, true);
        BBF1.selected = true;
        BBF2.selected = false;
        BBF3.selected = false;
        EEPROM.put(EEPROM_ROUTE, ROUTE);
    });

    ButtonControl::pushButton(SW_BBF2, []() {
        ServoControl::switchTurnout(servo, W3, true);
        ServoControl::switchTurnout(servo, W4, true);
        ServoControl::switchTurnout(servo, W5, true);
        ServoControl::switchTurnout(servo, W7, true);
        BBF1.selected = false;
        BBF2.selected = true;
        BBF3.selected = false;
        EEPROM.put(EEPROM_ROUTE, ROUTE);
    });

    ButtonControl::pushButton(SW_BBF3, []() {
        ServoControl::switchTurnout(servo, W3, false);
        ServoControl::switchTurnout(servo, W6, true);
        ServoControl::switchTurnout(servo, W7, false);
        BBF1.selected = false;
        BBF2.selected = false;
        BBF3.selected = true;
        EEPROM.put(EEPROM_ROUTE, ROUTE);
    });

    // DISPLAY COMMON STATES ######################################################################
    // Utils::currentSpeed != 0.0
    //     ? LCDControl::print(lcd, 9, 19, 3, "v:" + String(Utils::currentSpeed, 2) + "cm/s")
    //     : LCDControl::print(lcd, 9, 19, 3, "v:--.--cm/s");

    // LCDControl::print(lcd, 9, 19, 2, "v:" + String(Utils::scaleSpeed(Utils::currentSpeed)) + "km/h");

    // DISPLAY HBF STATE ##########################################################################
    for (int i = 0; i < 2; ++i) {
        String label = (HBF1.selected && i == 0)
                || (HBF2.selected && i == 1)
            ? ">"
            : " ";
        LCDControl::print(lcd, 0, 5, i, label + "HBF " + String(i + 1));

        bool power = (i == 0 && HBF1.powered) || (i == 1 && HBF2.powered);
        LCDControl::print(lcd, 6, 7, i, power ? "*" : " ");
    }

    for (int i = 0; i < 3; ++i) {
        String label = (BBF1.selected && i == 0)
                || (BBF2.selected && i == 1)
                || (BBF3.selected && i == 2)
            ? ">"
            : " ";
        LCDControl::print(lcd, 13, 18, i, label + "BBF " + String(i + 1));

        bool power = (i == 0 && BBF1.powered) || (i == 1 && BBF2.powered) || (i == 2 && BBF3.powered);
        LCDControl::print(lcd, 19, 19, i, power ? "*" : " ");
    }

    int percent = map(abs(ENC_ZONE_A), 0, 255, 0, 100);

    LCDControl::print(lcd, 0, 3, 3, String((int)ENC_ZONE_A), "RTL");
    LCDControl::print(lcd, 15, 18, 3, String((int)ENC_ZONE_B), "RTL");

    // LCDControl::print(lcd, 18, 19, 3, "0%");
    // LCDControl::print(lcd, 16, 18, 3, String((int)percent), "RTL");

    // # PRIMARY ENCODER BUTTON ###################################################################
    ButtonControl::pushButton(4, []() {
        // DO SOMETHING ON PRIMARY ENCODER BUTTON
    });

    // # SECONDARY ENCODER BUTTON ###################################################################
    ButtonControl::pushButton(5, []() {
        // DO SOMETHING ON SECONDARY ENCODER BUTTON
    });

    // MAIN SPEED CONTROL #########################################################################
    // if (hbfStart) {
    //     MotorControl::rampValue(ENC_ZONE_B, hbfMax, 2, 150);
    //     if (ENC_ZONE_B == hbfMax)
    //         hbfStart = false;
    // }

    // if (!HBF1.powered && hbfStop) {
    //     if (startTimer == 0)
    //         startTimer = millis();
    //     ENC_ZONE_B = (-1) * MotorControl::rampDynamicValue(millis() - startTimer, 0, dist_rd2_rd3, abs(hbfBrake), abs(hbfMin), Utils::currentSpeed);
    // } else {
    //     startTimer = 0;
    // }

    // MOTOR CONTROL ##############################################################################
    // CIRCUIT HBF - ZONE A
    MotorControl::setValue(ZONE_A, abs(ENC_ZONE_A));
    // CIRCUIT BBF - ZONE B
    MotorControl::setValue(ZONE_B, abs(ENC_ZONE_B));

    ButtonControl::setStates();
    ReedControl::setStates();

    delay(25);
}
