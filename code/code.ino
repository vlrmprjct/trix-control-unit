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
#include "src/utils/debug.h"
#include "src/utils/eeprom.h"
#include "src/utils/utils.h"
#include "state.h"

#include <Adafruit_PWMServoDriver.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>

Adafruit_PWMServoDriver servo = Adafruit_PWMServoDriver();
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD1_D4, LCD1_D5, LCD1_D6, LCD1_D7);

void setup() {
    init(servo, lcd);
    Debug::enabled = false;
    Debug::eepromEnabled = false;
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

    ReedControl::push(RD_HBF1_C, []() {
        RelayControl::toggleRelay(9);
    });

    ReedControl::push(RD_HBF1_R, []() {
        if (!HBF1.powered) {
            // ARRIVING TRAIN - PARK IN HBF1
            // SWITCH FROM ZONE A TO B @ HBF1
            RelayControl::setRelay(7, true);
            // TURN OFF HBF1
            RelayControl::setRelay(8, false);
            HBF1.occupied = true;
            Eeprom::save();
        } else {
            // PASSING THROUGH - RELEASE BLOCK IF AT LEAST ONE HBF TRACK IS FREE
            if (!(HBF1.occupied && HBF2.occupied)) {
                RelayControl::setRelay(10, true);
                BLOCKB.occupied = false;
                Eeprom::save();
            }
        }
    });

    ReedControl::push(RD_HBF2_C, []() {
    });

    ReedControl::push(RD_HBF2_R, []() {
        if (!HBF2.powered) {
            // ARRIVING TRAIN - PARK IN HBF2
            // SWITCH FROM ZONE A TO B @ HBF2
            RelayControl::setRelay(5, true);
            // TURN OFF HBF2
            RelayControl::setRelay(6, false);
            HBF2.occupied = true;
            Eeprom::save();
        } else {
            // PASSING THROUGH - RELEASE BLOCK IF AT LEAST ONE HBF TRACK IS FREE
            if (!(HBF1.occupied && HBF2.occupied)) {
                RelayControl::setRelay(10, true);
                BLOCKB.occupied = false;
                Eeprom::save();
            }
        }
    });

    ReedControl::push(RD_BBF1_L, []() {
        if (!BBF1.powered) {
            RelayControl::setRelay(2, false);
        }
    });

    ReedControl::push(RD_BBF2_L, []() {
        if (!BBF2.powered) {
            RelayControl::setRelay(3, false);
        }
    });

    ReedControl::push(RD_BBF3_L, []() {
        if (!BBF3.powered) {
            RelayControl::setRelay(4, false);
        }
    });

    ReedControl::push(RD_30, []() {
        // BLOCK ZONE B ENTRY
        BLOCKB.occupied = true;
        RelayControl::setRelay(10, false);
        Eeprom::save();
    });

    ReedControl::push(RD_50, []() {
        // FORCE SWITCH FROM ZONE A TO B @ HBFx
        RelayControl::setRelay(5, false);
        RelayControl::setRelay(7, false);
    });

    // TRACK CONTROL ##############################################################################
    ButtonControl::updateStates();

    ButtonControl::pushButton(BTN_HBF1, []() {
        if (!HBF1.selected) return;
        HBF1.powered = !HBF1.powered;
        if (HBF1.powered) {
            if (HBF1.occupied) MotorControl::setValue(ZONE_A, 0);
            HBF1.occupied = false;
            RelayControl::setRelay(8, true);
            // RELEASE BLOCK IF AT LEAST ONE HBF TRACK IS FREE
            if (!(HBF1.occupied && HBF2.occupied)) {
                RelayControl::setRelay(10, true);
                BLOCKB.occupied = false;
            }
        }
        Eeprom::save();
    });

    ButtonControl::pushButton(BTN_HBF2, []() {
        if (!HBF2.selected) return;
        HBF2.powered = !HBF2.powered;
        if (HBF2.powered) {
            if (HBF2.occupied) MotorControl::setValue(ZONE_A, 0);
            HBF2.occupied = false;
            RelayControl::setRelay(6, true);
            // RELEASE BLOCK IF AT LEAST ONE HBF TRACK IS FREE
            if (!(HBF1.occupied && HBF2.occupied)) {
                RelayControl::setRelay(10, true);
                BLOCKB.occupied = false;
            }
        }
        Eeprom::save();
    });

    ButtonControl::pushButton(BTN_BBF1, []() {
        if (!BBF1.selected) return;
        BBF1.powered = !BBF1.powered;
        if (BBF1.powered) {
            RelayControl::setRelay(2, true);
        }
        Eeprom::save();
    });

    ButtonControl::pushButton(BTN_BBF2, []() {
        if (!BBF2.selected) return;
        BBF2.powered = !BBF2.powered;
        if (BBF2.powered) {
            RelayControl::setRelay(3, true);
        } else {
            RelayControl::setRelay(3, false);
        }
        Eeprom::save();
    });

    ButtonControl::pushButton(BTN_BBF3, []() {
        if (!BBF3.selected) return;
        BBF3.powered = !BBF3.powered;
        if (BBF3.powered) {
            RelayControl::setRelay(4, true);
        }
        Eeprom::save();
    });

    ButtonControl::pushButton(BTN_BLOCKA_OVERRIDE, []() {
        RelayControl::toggleRelay(10);
        Eeprom::save();
    });

    // TURNOUT CONTROL ############################################################################
    ButtonControl::pushButton(SW_HBF1, []() {
        ServoControl::switchTurnout(servo, W1, false);
        ServoControl::switchTurnout(servo, W2, true);
        HBF1.selected = true;
        HBF2.selected = false;
        Eeprom::save();
    });

    ButtonControl::pushButton(SW_HBF2, []() {
        ServoControl::switchTurnout(servo, W1, true);
        ServoControl::switchTurnout(servo, W2, false);
        HBF1.selected = false;
        HBF2.selected = true;
        Eeprom::save();
    });


    ButtonControl::pushButton(SW_BBF1, []() {
        ServoControl::switchTurnout(servo, W3, true);
        ServoControl::switchTurnout(servo, W4, false);
        ServoControl::switchTurnout(servo, W5, false, -5);
        ServoControl::switchTurnout(servo, W7, true, -10);
        BBF1.selected = true;
        BBF2.selected = false;
        BBF3.selected = false;
        BBF4.selected = false;
        BBF5.selected = false;
        Eeprom::save();
    });

    ButtonControl::pushButton(SW_BBF2, []() {
        ServoControl::switchTurnout(servo, W3, true);
        ServoControl::switchTurnout(servo, W4, true, 15);
        ServoControl::switchTurnout(servo, W5, true, 0);
        ServoControl::switchTurnout(servo, W7, true);
        BBF1.selected = false;
        BBF2.selected = true;
        BBF3.selected = false;
        BBF4.selected = false;
        BBF5.selected = false;
        Eeprom::save();
    });

    ButtonControl::pushButton(SW_BBF3, []() {
        ServoControl::switchTurnout(servo, W3, false);
        ServoControl::switchTurnout(servo, W6, true, 15);
        ServoControl::switchTurnout(servo, W7, false, -15);
        ServoControl::switchTurnout(servo, W9, true);
        BBF1.selected = false;
        BBF2.selected = false;
        BBF3.selected = true;
        BBF4.selected = false;
        BBF5.selected = false;
        Eeprom::save();
    });

    ButtonControl::pushButton(SW_BBF4, []() {
        ServoControl::switchTurnout(servo, W3, false);
        ServoControl::switchTurnout(servo, W6, false, -10);
        ServoControl::switchTurnout(servo, W7, false);
        ServoControl::switchTurnout(servo, W8, true);
        ServoControl::switchTurnout(servo, W9, false, -20);
        ServoControl::switchTurnout(servo, W10, false);
        BBF1.selected = false;
        BBF2.selected = false;
        BBF3.selected = false;
        BBF4.selected = true;
        BBF5.selected = false;
        Eeprom::save();
    });

    ButtonControl::pushButton(SW_BBF5, []() {
        ServoControl::switchTurnout(servo, W3, false);
        ServoControl::switchTurnout(servo, W6, false, -10);
        ServoControl::switchTurnout(servo, W7, false);
        ServoControl::switchTurnout(servo, W8, false);
        ServoControl::switchTurnout(servo, W9, false, -20);
        ServoControl::switchTurnout(servo, W10, true);
        ServoControl::switchTurnout(servo, W11, false, -20);
        BBF1.selected = false;
        BBF2.selected = false;
        BBF3.selected = false;
        BBF4.selected = false;
        BBF5.selected = true;
        Eeprom::save();
    });

    // DISPLAY COMMON STATES ######################################################################
    // Utils::currentSpeed != 0.0
    //     ? LCDControl::print(lcd, 9, 19, 3, "v:" + String(Utils::currentSpeed, 2) + "cm/s")
    //     : LCDControl::print(lcd, 9, 19, 3, "v:--.--cm/s");

    // LCDControl::print(lcd, 9, 19, 2, "v:" + String(Utils::scaleSpeed(Utils::currentSpeed)) + "km/h");

    // DISPLAY HBF/BBF STATE #######################################################################
    for (int i = 0; i < 2; ++i) {
        String label = (HBF1.selected && i == 0)
                || (HBF2.selected && i == 1)
            ? ">"
            : " ";
        LCDControl::print(lcd, 0, 4, i, label + "HBF" + String(i + 1));

        bool power = (i == 0 && HBF1.powered) || (i == 1 && HBF2.powered);
        LCDControl::print(lcd, 5, 5, i, power ? "*" : " ");
    }

    for (int i = 0; i < 3; ++i) {
        String label = (BBF1.selected && i == 0)
                || (BBF2.selected && i == 1)
                || (BBF3.selected && i == 2)
            ? ">"
            : " ";
        LCDControl::print(lcd, 7, 11, i, label + "BBF" + String(i + 1));

        bool power = (i == 0 && BBF1.powered) || (i == 1 && BBF2.powered) || (i == 2 && BBF3.powered);
        LCDControl::print(lcd, 12, 12, i, power ? "*" : " ");
    }

    for (int i = 0; i < 2; ++i) {
        String label = (BBF4.selected && i == 0)
                || (BBF5.selected && i == 1)
            ? ">"
            : " ";
        LCDControl::print(lcd, 14, 18, i, label + "BBF" + String(i + 4));

        bool power = (i == 0 && BBF4.powered) || (i == 1 && BBF5.powered);
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

    // MOTOR CONTROL ##############################################################################
    // ZONE B: HBFx // HARDWARE RIGHT ENCODER
    // ZONE B: HBF ARRIVING TRAINS // NORMAL PASSTROUGH OR BRAKING RAMP AFTER REED xBF_C
    MotorControl::setValue(ZONE_B, abs(ENC_ZONE_B));
    // ZONE A: BBFx // HARDWARE LEFT ENCODER
    // ZONE A: DEPARTING HBF TRAINS + BBF // ALWAYS SOFT-START FROM 0
    MotorControl::setValue(ZONE_A, abs(ENC_ZONE_A));

    ButtonControl::setStates();
    ReedControl::setStates();

    // DEBUG ####################################################################################
    Debug::printState();
    Debug::printEeprom();

    delay(25);
}
