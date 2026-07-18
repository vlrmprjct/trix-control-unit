#include "src/controls/buttonControl.h"
#include "src/controls/encoderControl.h"
#include "src/controls/lcdControl.h"
#include "src/controls/motorControl.h"
#include "src/controls/reedControl.h"
#include "src/controls/relayControl.h"
#include "src/controls/servoControl.h"
#include "src/core/config.h"
#include "src/core/init.h"
#include "src/core/naming.h"
#include "src/core/state.h"
#include "src/display/trackDisplay.h"
#include "src/operation/trackControl.h"
#include "src/operation/webState.h"
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

void setup()
{
    init(servo, lcd);
    Debug::enabled = false;
    Debug::eepromEnabled = false;

    // WEB SERIAL COMMANDS
    WebSerial.on("setSpeed", [](JSONVar data) {
        int zone = (int)data["zone"];
        int value = constrain((int)data["value"], 0, 255);
        if (zone == 0)
            ENC_ZONE_A = value;
        if (zone == 1)
            ENC_ZONE_B = value;
    });
}

void loop()
{

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
        // ARRIVAL ONLY: START BRAKE RAMP (PASS-THROUGH + DEPARTING = POWERED)
        if (!HBF1.powered) MotorControl::startRampDown();
    });

    ReedControl::push(RD_HBF1_R, []() {
        // ALWAYS STOP BRAKE RAMP ON _R (CLEANUP FOR ALL CASES)
        MotorControl::stopRampDown();
        if (BLOCKA.occupied && TrackControl::getDepartingHBF() != 1) {
            // ZONE A BLOCKED BY ANOTHER TRAIN: HOLD ARRIVING TRAIN AT STATION
            RelayControl::setRelay(RELAY_HBF1_ZONE, true);
            RelayControl::setRelay(RELAY_HBF1_TRACK, false);
            HBF1.powered = true;
            HBF1.pending = true;
            // BLOCK B: RELEASE (TRAIN IS NOW IN STATION)
            if (!(HBF1.occupied && HBF2.occupied)) {
                RelayControl::setRelay(RELAY_BLOCKB, true);
                BLOCKB.occupied = false;
            }
            Eeprom::save();
            return;
        }
        TrackControl::stopHBF(HBF1, RELAY_HBF1_ZONE, RELAY_HBF1_TRACK);
        // BLOCK B: RELEASE IF AT LEAST ONE HBF IS FREE
        if (!(HBF1.occupied && HBF2.occupied)) {
            RelayControl::setRelay(RELAY_BLOCKB, true);
            BLOCKB.occupied = false;
        }
        if (HBF1.occupied) {
            // TRAIN STOPPED: SWITCH W1 TO FREE HBF (W2 HANDLED BY BUTTON)
            if (!HBF2.occupied) {
                ServoControl::switchTurnout(servo, W1, true);
                TrackControl::cancelPending(HBF1, RELAY_HBF1_TRACK);
                HBF1.selected = false;
                HBF2.selected = true;
                Eeprom::save();
            }
        } else {
            // PASS-THROUGH: CORRECT W2 FOR HBF1 ONLY IF ZONE A IS FREE
            if (!BLOCKA.occupied) ServoControl::switchTurnout(servo, W2, true);
        }
    });

    ReedControl::push(RD_HBF2_L, []() {
        HBF2.occupied = true;
        Eeprom::save();
    });

    ReedControl::push(RD_HBF2_C, []() {
        // ARRIVAL ONLY: START BRAKE RAMP (PASS-THROUGH + DEPARTING = POWERED)
        if (!HBF2.powered) MotorControl::startRampDown();
    });

    ReedControl::push(RD_HBF2_R, []() {
        // ALWAYS STOP BRAKE RAMP ON _R (CLEANUP FOR ALL CASES)
        MotorControl::stopRampDown();
        if (BLOCKA.occupied && TrackControl::getDepartingHBF() != 2) {
            // ZONE A BLOCKED BY ANOTHER TRAIN: HOLD ARRIVING TRAIN AT STATION
            RelayControl::setRelay(RELAY_HBF2_ZONE, true);
            RelayControl::setRelay(RELAY_HBF2_TRACK, false);
            HBF2.powered = true;
            HBF2.pending = true;
            // BLOCK B: RELEASE (TRAIN IS NOW IN STATION)
            if (!(HBF1.occupied && HBF2.occupied)) {
                RelayControl::setRelay(RELAY_BLOCKB, true);
                BLOCKB.occupied = false;
            }
            Eeprom::save();
            return;
        }
        TrackControl::stopHBF(HBF2, RELAY_HBF2_ZONE, RELAY_HBF2_TRACK);
        // BLOCK B: RELEASE IF AT LEAST ONE HBF IS FREE
        if (!(HBF1.occupied && HBF2.occupied)) {
            RelayControl::setRelay(RELAY_BLOCKB, true);
            BLOCKB.occupied = false;
        }
        if (HBF2.occupied) {
            // TRAIN STOPPED: SWITCH W1 TO FREE HBF (W2 HANDLED BY BUTTON)
            if (!HBF1.occupied) {
                ServoControl::switchTurnout(servo, W1, false);
                TrackControl::cancelPending(HBF2, RELAY_HBF2_TRACK);
                HBF1.selected = true;
                HBF2.selected = false;
                Eeprom::save();
            }
        } else {
            // PASS-THROUGH: CORRECT W2 FOR HBF2 ONLY IF ZONE A IS FREE
            if (!BLOCKA.occupied) ServoControl::switchTurnout(servo, W2, false);
        }
    });

    ReedControl::push(RD_BBF1_R, []() {
        BBF1.occupied = true;
        Eeprom::save();
    });

    ReedControl::push(RD_BBF1_L, []() {
        TrackControl::onBBFReedL(servo, 1);
    });

    ReedControl::push(RD_BBF2_R, []() {
        BBF2.occupied = true;
        Eeprom::save();
    });

    ReedControl::push(RD_BBF2_L, []() {
        TrackControl::onBBFReedL(servo, 2);
    });

    ReedControl::push(RD_BBF3_R, []() {
        BBF3.occupied = true;
        Eeprom::save();
    });

    ReedControl::push(RD_BBF3_L, []() {
        TrackControl::onBBFReedL(servo, 3);
    });

    ReedControl::push(RD_BBF4_R, []() {
        BBF4.occupied = true;
        Eeprom::save();
    });

    ReedControl::push(RD_BBF4_L, []() {
        TrackControl::onBBFReedL(servo, 4);
    });

    ReedControl::push(RD_BBF5_R, []() {
        BBF5.occupied = true;
        Eeprom::save();
    });

    ReedControl::push(RD_BBF5_L, []() {
        TrackControl::onBBFReedL(servo, 5);
    });

    ReedControl::push(RD_05, []() {
        // PICK A FREE BBF: PRIORITY BBF1-3, FALLBACK BBF4-5
        int freeBBF = TrackControl::findFreeBBF();
        if (freeBBF == 0) return;
        TrackControl::setBBFEntryRoute(servo, freeBBF);
    });

    ReedControl::push(RD_10, []() {
        // LEAVING BLOCK ZONE A (MAIN ROUTE)
        TrackControl::releaseZoneA(servo, BLOCKA);
        // BLOCK ZONE C ENTRY
        BLOCKC.occupied = true;
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
        RelayControl::setRelay(RELAY_BLOCKB, false);
        // RELEASE WAITING BBF (powered + pending)
        TrackControl::releasePendingBBF();
        Eeprom::save();
    });

    ReedControl::push(RD_50, []() {
        // SELECT TARGET HBF VIA W1 (W2 CORRECTED BY _R):
        // BOTH FREE → RANDOM, ONE OCCUPIED → TAKE THE FREE ONE
        int which = 0;
        if      (!HBF1.occupied && !HBF2.occupied && !BLOCKA.occupied ) which = random(1, 3);
        else if (!HBF1.occupied)                   which = 1;
        else if (!HBF2.occupied)                   which = 2;
        if (which == 1) {
            // FORCE SWITCH FROM ZONE A TO B @ HBF1
            // ARRIVING TRAINS ALWAYS USING ZONE B
            RelayControl::setRelay(RELAY_HBF1_ZONE, false);
            ServoControl::switchTurnout(servo, W1, false);
            HBF1.selected = true;
            HBF2.selected = false;
            // IF HBF2 WAS COASTING: TRANSFER PENDING TO HBF1
            if (HBF2.pending && !HBF2.powered) {
                TrackControl::cancelPending(HBF2, RELAY_HBF2_TRACK);
                HBF1.powered = false;
                HBF1.pending = true;
            }
        } else if (which == 2) {
            // FORCE SWITCH FROM ZONE A TO B @ HBF2
            // ARRIVING TRAINS ALWAYS USING ZONE B
            RelayControl::setRelay(RELAY_HBF2_ZONE, false);
            ServoControl::switchTurnout(servo, W1, true);
            HBF1.selected = false;
            HBF2.selected = true;
            // IF HBF1 WAS COASTING: TRANSFER PENDING TO HBF2
            if (HBF1.pending && !HBF1.powered) {
                TrackControl::cancelPending(HBF1, RELAY_HBF1_TRACK);
                HBF2.powered = false;
                HBF2.pending = true;
            }
        }
        if (which != 0) Eeprom::save();
    });

    // TRACK CONTROL ##############################################################################
    ButtonControl::updateStates();

    ButtonControl::pushButton(BTN_HBF1, []() {
        // CANCEL WAITING STATE IF ZONE A STILL BLOCKED
        if (HBF1.powered && HBF1.pending && BLOCKA.occupied) {
            HBF1.powered = false;
            HBF1.pending = false;
            Eeprom::save();
            return;
        }
        // BLOCK NEW DEPARTURE WHILE ZONE A IS OCCUPIED: QUEUE AS WAITING TO DEPART
        if (BLOCKA.occupied && !HBF1.powered) {
            HBF1.powered = true;
            HBF1.pending = true;
            Eeprom::save();
            return;
        }
        TrackControl::toggleHBF(HBF1, RELAY_HBF1_TRACK);
        if (HBF1.powered) {
            BLOCKA.occupied = true;
            if (!TrackControl::isHBFDeparting()) ServoControl::switchTurnout(servo, W2, true);
            TrackControl::setDepartingHBF(1);
            MotorControl::startRamp();
        }
        // BLOCK B: RELEASE IF TRAIN JUST POWERED ON AND AT LEAST ONE HBF IS FREE
        if (HBF1.powered && !(HBF1.occupied && HBF2.occupied)) {
            RelayControl::setRelay(RELAY_BLOCKB, true);
            BLOCKB.occupied = false;
        }
    });

    ButtonControl::pushButton(BTN_HBF2, []() {
        // CANCEL WAITING STATE IF ZONE A STILL BLOCKED
        if (HBF2.powered && HBF2.pending && BLOCKA.occupied) {
            HBF2.powered = false;
            HBF2.pending = false;
            Eeprom::save();
            return;
        }
        // BLOCK NEW DEPARTURE WHILE ZONE A IS OCCUPIED: QUEUE AS WAITING TO DEPART
        if (BLOCKA.occupied && !HBF2.powered) {
            HBF2.powered = true;
            HBF2.pending = true;
            Eeprom::save();
            return;
        }
        TrackControl::toggleHBF(HBF2, RELAY_HBF2_TRACK);
        if (HBF2.powered) {
            BLOCKA.occupied = true;
            if (!TrackControl::isHBFDeparting()) ServoControl::switchTurnout(servo, W2, false);
            TrackControl::setDepartingHBF(2);
            MotorControl::startRamp();
        }
        // BLOCK B: RELEASE IF TRAIN JUST POWERED ON AND AT LEAST ONE HBF IS FREE
        if (HBF2.powered && !(HBF1.occupied && HBF2.occupied)) {
            RelayControl::setRelay(RELAY_BLOCKB, true);
            BLOCKB.occupied = false;
        }
    });

    ButtonControl::pushButton(BTN_BBF1, []() {
        TrackControl::toggleBBF(BBF1, RELAY_BBF1, !BLOCKC.occupied);
        // SAME PATTERN AS HBF/W2: ON ACTUAL DEPARTURE START, SET ROUTE IMMEDIATELY.
        if (BBF1.powered && !BBF1.pending) {
            TrackControl::setBBFExitRoute(servo, 1);
        }
    });

    ButtonControl::pushButton(BTN_BBF2, []() {
        TrackControl::toggleBBF(BBF2, RELAY_BBF2, !BLOCKC.occupied);
        if (BBF2.powered && !BBF2.pending) {
            TrackControl::setBBFExitRoute(servo, 2);
        }
    });

    ButtonControl::pushButton(BTN_BBF3, []() {
        TrackControl::toggleBBF(BBF3, RELAY_BBF3, !BLOCKC.occupied);
        if (BBF3.powered && !BBF3.pending) {
            TrackControl::setBBFExitRoute(servo, 3);
        }
    });

    ButtonControl::pushButton(BTN_BBF4, []() {
        TrackControl::toggleBBF(BBF4, RELAY_BBF4, !BLOCKC.occupied);
        if (BBF4.powered && !BBF4.pending) {
            TrackControl::setBBFExitRoute(servo, 4);
        }
    });

    ButtonControl::pushButton(BTN_BBF5, []() {
        TrackControl::toggleBBF(BBF5, RELAY_BBF5, !BLOCKC.occupied);
        if (BBF5.powered && !BBF5.pending) {
            TrackControl::setBBFExitRoute(servo, 5);
        }
    });

    ButtonControl::pushButton(BTN_BLOCKA_OVERRIDE, []() {
        RelayControl::toggleRelay(RELAY_BLOCKB);
        Eeprom::save();
    });

    // TURNOUT CONTROL ############################################################################
    ButtonControl::pushButton(SW_HBF1, []() {
        ServoControl::switchTurnout(servo, W1, false);
        TrackControl::cancelPending(HBF2, RELAY_HBF2_TRACK);
        HBF1.selected = true;
        HBF2.selected = false;
        Eeprom::save();
    });

    ButtonControl::pushButton(SW_HBF2, []() {
        ServoControl::switchTurnout(servo, W1, true);
        TrackControl::cancelPending(HBF1, RELAY_HBF1_TRACK);
        HBF1.selected = false;
        HBF2.selected = true;
        Eeprom::save();
    });

    ButtonControl::pushButton(SW_BBF1, []() {
        ServoControl::switchTurnout(servo, W3, true);
        ServoControl::switchTurnout(servo, W4, false);
        ServoControl::switchTurnout(servo, W5, false);
        ServoControl::switchTurnout(servo, W7, true);
        TrackControl::cancelPending(BBF2, RELAY_BBF2);
        TrackControl::cancelPending(BBF3, RELAY_BBF3);
        TrackControl::cancelPending(BBF4, RELAY_BBF4);
        TrackControl::cancelPending(BBF5, RELAY_BBF5);
        BBF1.selected = true;
        BBF2.selected = false;
        BBF3.selected = false;
        BBF4.selected = false;
        BBF5.selected = false;
        Eeprom::save();
    });

    ButtonControl::pushButton(SW_BBF2, []() {
        ServoControl::switchTurnout(servo, W3, true);
        ServoControl::switchTurnout(servo, W4, true);
        ServoControl::switchTurnout(servo, W5, true);
        ServoControl::switchTurnout(servo, W7, true);
        TrackControl::cancelPending(BBF1, RELAY_BBF1);
        TrackControl::cancelPending(BBF3, RELAY_BBF3);
        TrackControl::cancelPending(BBF4, RELAY_BBF4);
        TrackControl::cancelPending(BBF5, RELAY_BBF5);
        BBF1.selected = false;
        BBF2.selected = true;
        BBF3.selected = false;
        BBF4.selected = false;
        BBF5.selected = false;
        Eeprom::save();
    });

    ButtonControl::pushButton(SW_BBF3, []() {
        ServoControl::switchTurnout(servo, W3, false);
        ServoControl::switchTurnout(servo, W6, true);
        ServoControl::switchTurnout(servo, W7, false);
        ServoControl::switchTurnout(servo, W9, true);
        TrackControl::cancelPending(BBF1, RELAY_BBF1);
        TrackControl::cancelPending(BBF2, RELAY_BBF2);
        TrackControl::cancelPending(BBF4, RELAY_BBF4);
        TrackControl::cancelPending(BBF5, RELAY_BBF5);
        BBF1.selected = false;
        BBF2.selected = false;
        BBF3.selected = true;
        BBF4.selected = false;
        BBF5.selected = false;
        Eeprom::save();
    });

    ButtonControl::pushButton(SW_BBF4, []() {
        ServoControl::switchTurnout(servo, W3, false);
        ServoControl::switchTurnout(servo, W6, false);
        ServoControl::switchTurnout(servo, W7, false);
        ServoControl::switchTurnout(servo, W8, true);
        ServoControl::switchTurnout(servo, W9, false);
        ServoControl::switchTurnout(servo, W10, false);
        TrackControl::cancelPending(BBF1, RELAY_BBF1);
        TrackControl::cancelPending(BBF2, RELAY_BBF2);
        TrackControl::cancelPending(BBF3, RELAY_BBF3);
        TrackControl::cancelPending(BBF5, RELAY_BBF5);
        BBF1.selected = false;
        BBF2.selected = false;
        BBF3.selected = false;
        BBF4.selected = true;
        BBF5.selected = false;
        Eeprom::save();
    });

    ButtonControl::pushButton(SW_BBF5, []() {
        ServoControl::switchTurnout(servo, W3, false);
        ServoControl::switchTurnout(servo, W6, false);
        ServoControl::switchTurnout(servo, W7, false);
        ServoControl::switchTurnout(servo, W8, false);
        ServoControl::switchTurnout(servo, W9, false);
        ServoControl::switchTurnout(servo, W10, true);
        ServoControl::switchTurnout(servo, W11, false);
        TrackControl::cancelPending(BBF1, RELAY_BBF1);
        TrackControl::cancelPending(BBF2, RELAY_BBF2);
        TrackControl::cancelPending(BBF3, RELAY_BBF3);
        TrackControl::cancelPending(BBF4, RELAY_BBF4);
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
            // BLINK: WAITING TO DEPART
            lcd.write(blinkOn ? CHAR_ARROW_RIGHT : CHAR_CIRCLE_FILLED);
        } else if (slot.track->pending && !slot.track->powered) {
            // BLINK: COASTING TO STOP
            lcd.write(blinkOn ? CHAR_CIRCLE_FILLED : CHAR_CIRCLE_EMPTY);
        } else if (slot.track->occupied) {
            // FILLED CIRCLE: TRAIN STOPPED OR RUNNING
            lcd.write(CHAR_CIRCLE_FILLED);
        } else if (slot.track->powered) {
            // EMPTY CIRCLE: FREE + POWERED
            lcd.write(CHAR_CIRCLE_EMPTY);
        } else {
            // FREE + NOT POWERED
            lcd.print(' ');
        }
    }

    // DEBUG: HBFSTART + RAMP VALUE ON ROW 2
    LCDControl::print(lcd, 0, 1, 2, String(MotorControl::isRampActive()));
    LCDControl::print(lcd, 2, 5, 2, String(MotorControl::getRampValue()));

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
    // ZONE A: BBFx + DEPARTING HBF TRAINS // HARDWARE LEFT ENCODER
    MotorControl::setValue(ZONE_A, MotorControl::isRampActive() ? MotorControl::rampUp(ENC_ZONE_A) : abs(ENC_ZONE_A));
    // ZONE B: HBF ARRIVING TRAINS — BRAKE RAMP IF ACTIVE, ELSE ENCODER
    MotorControl::setValue(ZONE_B, MotorControl::isRampDownActive() ? MotorControl::rampDown(ENC_ZONE_B) : abs(ENC_ZONE_B));

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
