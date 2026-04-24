#include "src/core/config.h"
#include "src/core/init.h"
#include "src/core/naming.h"
#include "src/core/state.h"
#include "src/controls/buttonControl.h"
#include "src/operation/trackControl.h"
#include "src/operation/webState.h"
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

#include <SimpleWebSerial.h>

SimpleWebSerial WebSerial;

Adafruit_PWMServoDriver servo = Adafruit_PWMServoDriver();
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD1_D4, LCD1_D5, LCD1_D6, LCD1_D7);

void setup() {
    init(servo, lcd);
    Debug::enabled = false;
    Debug::eepromEnabled = false;

    // WEB SERIAL COMMANDS
    WebSerial.on("setSpeed", [](JSONVar data) {
        int zone  = (int)data["zone"];
        int value = constrain((int)data["value"], 0, 255);
        if (zone == 0) ENC_ZONE_A = value;
        if (zone == 1) ENC_ZONE_B = value;
    });
}

void loop() {

    WebSerial.check();
    WebState::send(WebSerial);

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

    ReedControl::push(RD_HBF1_L, []() {
        HBF1.occupied = true;
        Eeprom::save();
    });

    ReedControl::push(RD_HBF1_C, []() {
    });

    ReedControl::push(RD_HBF1_R, []() {
        TrackControl::stopHBF(HBF1, 7, 8);
    });

    ReedControl::push(RD_HBF2_L, []() {
        HBF2.occupied = true;
        Eeprom::save();
    });

    ReedControl::push(RD_HBF2_C, []() {
    });

    ReedControl::push(RD_HBF2_R, []() {
        TrackControl::stopHBF(HBF2, 5, 6);
    });

    ReedControl::push(RD_BBF1_R, []() {
        BBF1.occupied = true;
        Eeprom::save();
    });

    ReedControl::push(RD_BBF1_L, []() {
        TrackControl::stopBBF(BBF1, 2);
    });

    ReedControl::push(RD_BBF2_R, []() {
        BBF2.occupied = true;
        Eeprom::save();
    });

    ReedControl::push(RD_BBF2_L, []() {
        TrackControl::stopBBF(BBF2, 3);
    });

    ReedControl::push(RD_BBF3_R, []() {
        BBF3.occupied = true;
        Eeprom::save();
    });

    ReedControl::push(RD_BBF3_L, []() {
        TrackControl::stopBBF(BBF3, 4);
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
        TrackControl::releasePendingBBF();
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
        TrackControl::toggleHBF(HBF1, 8);
    });

    ButtonControl::pushButton(BTN_HBF2, []() {
        TrackControl::toggleHBF(HBF2, 6);
    });

    ButtonControl::pushButton(BTN_BBF1, []() {
        TrackControl::toggleBBF(BBF1, 2);
    });

    ButtonControl::pushButton(BTN_BBF2, []() {
        TrackControl::toggleBBF(BBF2, 3);
    });

    ButtonControl::pushButton(BTN_BBF3, []() {
        TrackControl::toggleBBF(BBF3, 4);
    });

    ButtonControl::pushButton(BTN_BLOCKA_OVERRIDE, []() {
        RelayControl::toggleRelay(10);
        Eeprom::save();
    });

    // TURNOUT CONTROL ############################################################################
    ButtonControl::pushButton(SW_HBF1, []() {
        ServoControl::switchTurnout(servo, W1, false);
        ServoControl::switchTurnout(servo, W2, true);
        TrackControl::cancelPending(HBF2, 6);
        HBF1.selected = true;
        HBF2.selected = false;
        Eeprom::save();
    });

    ButtonControl::pushButton(SW_HBF2, []() {
        ServoControl::switchTurnout(servo, W1, true);
        ServoControl::switchTurnout(servo, W2, false);
        TrackControl::cancelPending(HBF1, 8);
        HBF1.selected = false;
        HBF2.selected = true;
        Eeprom::save();
    });


    ButtonControl::pushButton(SW_BBF1, []() {
        ServoControl::switchTurnout(servo, W3, true);
        ServoControl::switchTurnout(servo, W4, false);
        ServoControl::switchTurnout(servo, W5, false, -5);
        ServoControl::switchTurnout(servo, W7, true, -10);
        TrackControl::cancelPending(BBF2, 3);
        TrackControl::cancelPending(BBF3, 4);
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
        TrackControl::cancelPending(BBF1, 2);
        TrackControl::cancelPending(BBF3, 4);
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
        TrackControl::cancelPending(BBF1, 2);
        TrackControl::cancelPending(BBF2, 3);
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
    static bool blinkOn = false;
    static unsigned long lastBlinkTime = 0;
    if (millis() - lastBlinkTime >= 400) {
        blinkOn = !blinkOn;
        lastBlinkTime = millis();
    }
    for (const auto& slot : lcdSlots) {
        LCDControl::print(lcd, slot.colStart, slot.colEnd, slot.row, String(slot.track->selected ? (char)CHAR_ARROW_RIGHT : ' ') + slot.name);
        lcd.setCursor(slot.powerCol, slot.row);
        if (slot.track->pending && slot.track->powered) {
            lcd.write(blinkOn ? CHAR_ARROW_RIGHT : CHAR_CIRCLE_FILLED); // BLINK: WAITING TO DEPART
        } else if (slot.track->pending && !slot.track->powered) {
            lcd.write(blinkOn ? CHAR_CIRCLE_FILLED : CHAR_CIRCLE_EMPTY); // BLINK: COASTING TO STOP
        } else if (slot.track->occupied) {
            lcd.write(CHAR_CIRCLE_FILLED);  // FILLED CIRCLE: TRAIN STOPPED OR RUNNING
        } else if (slot.track->powered) {
            lcd.write(CHAR_CIRCLE_EMPTY);   // EMPTY CIRCLE: FREE + POWERED
        } else {
            lcd.print(' ');                 // FREE + NOT POWERED
        }
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
