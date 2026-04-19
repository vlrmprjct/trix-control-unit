#include "src/core/config.h"
#include "src/core/init.h"
#include "src/core/naming.h"
#include "src/core/state.h"
#include "src/controls/buttonControl.h"
#include "src/operation/blockControl.h"
#include "src/display/trackDisplay.h"
#include "src/controls/encoderControl.h"
#include "src/controls/lcdControl.h"
#include "src/controls/motorControl.h"
#include "src/controls/reedControl.h"
#include "src/controls/relayControl.h"
#include "src/controls/servoControl.h"
#include "src/utils/debug.h"
#include "src/utils/eeprom.h"
#include "src/utils/utils.h"

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
        BlockControl::stopBBF(BBF1, 2);
    });

    ReedControl::push(RD_BBF2_L, []() {
        BlockControl::stopBBF(BBF2, 3);
    });

    ReedControl::push(RD_BBF3_L, []() {
        BlockControl::stopBBF(BBF3, 4);
    });

    ReedControl::push(RD_10, []() {
        // BLOCK ZONE C ENTRY
        BLOCKC.occupied = true;
        RelayControl::setRelay(9, false);
        Eeprom::save();
        // START SPEED MEASURE
        Utils::speedStart = millis();
    });

    ReedControl::push(RD_20, []() {
        // END SPEED MEASURE
        // DISTANCE BETWEEN RD_10 AND RD_20 = 112CM
        Utils::speedEnd = millis();
        Utils::currentSpeed = Utils::speedMeasure(Utils::speedStart, Utils::speedEnd, 112.0);
    });

    ReedControl::push(RD_30, []() {
        // BLOCK ZONE B ENTRY + RELEASE ZONE C
        BLOCKB.occupied = true;
        BLOCKC.occupied = false;
        RelayControl::setRelay(9, true);
        RelayControl::setRelay(10, false);
        // RELEASE WAITING BBF (selected + powered + occupied)
        BlockControl::releasePendingBBF();
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
        BlockControl::toggleBBF(BBF1, 2);
    });

    ButtonControl::pushButton(BTN_BBF2, []() {
        BlockControl::toggleBBF(BBF2, 3);
    });

    ButtonControl::pushButton(BTN_BBF3, []() {
        BlockControl::toggleBBF(BBF3, 4);
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

    // DISPLAY HBF/BBF STATE #######################################################################
    for (const auto& s : lcdSlots) {
        LCDControl::print(lcd, s.colStart, s.colEnd, s.row, String(s.track->selected ? ">" : " ") + s.name);
        LCDControl::print(lcd, s.powerCol, s.powerCol, s.row, s.track->powered ? "*" : " ");
    }

    // DISPLAY ENCODER VALUES (ZONE A/C LEFT, ZONE B RIGHT)
    LCDControl::print(lcd, 0, 3, 3, String((int)ENC_ZONE_A), "RTL");
    LCDControl::print(lcd, 15, 18, 3, String((int)ENC_ZONE_B), "RTL");

    // DISPLAY SPEED KM/H SCALED SPEED
    int speedKmh = (int)Utils::scaleSpeed(Utils::currentSpeed);
    LCDControl::print(lcd, 7, 13, 3, Utils::currentSpeed > 0.0 ? String(speedKmh) + "km/h" : "---km/h");

    // # PRIMARY ENCODER BUTTON ###################################################################
    ButtonControl::pushButton(4, []() {
        // DO SOMETHING ON PRIMARY ENCODER BUTTON
    });

    // # SECONDARY ENCODER BUTTON ###################################################################
    ButtonControl::pushButton(5, []() {
        // DO SOMETHING ON SECONDARY ENCODER BUTTON
    });

    // MOTOR CONTROL ##############################################################################
    // ZONE A: BBFx // HARDWARE LEFT ENCODER
    // ZONE A: DEPARTING HBF TRAINS + BBF // ALWAYS SOFT-START FROM 0
    MotorControl::setValue(ZONE_A, abs(ENC_ZONE_A));
    // ZONE B: HBFx // HARDWARE RIGHT ENCODER
    // ZONE B: HBF ARRIVING TRAINS // NORMAL PASSTROUGH OR BRAKING RAMP AFTER REED xBF_C
    MotorControl::setValue(ZONE_B, abs(ENC_ZONE_B));

    // ZONE C: OUTER ROUTE // HARDWARE LEFT ENCODER
    // ZONE C: DEPARTING BBF TRAINS, CURRENTLY CONNECTED TO ZONE A ENCODER
    MotorControl::setValue(ZONE_C, abs(ENC_ZONE_A));

    // ZONE D: NOT CONNECTED YET
    // MotorControl::setValue(ZONE_D, abs(ENC_ZONE_D));

    ButtonControl::setStates();
    ReedControl::setStates();

    // DEBUG ####################################################################################
    Debug::printState();
    Debug::printEeprom();

    delay(25);
}
