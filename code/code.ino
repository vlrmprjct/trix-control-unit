#include "config.h"
#include "init.h"
#include "naming.h"
#include "profiles.h"
#include "src/controls/buttonControl.h"
#include "src/controls/encoderControl.h"
#include "src/controls/lcdControl.h"
#include "src/controls/motorControl.h"
#include "src/controls/reedControl.h"
#include "src/controls/relayControl.h"
#include "src/controls/servoControl.h"
#include "src/controls/tagreader.h"
#include "src/utils/eeprom.h"
#include "src/utils/utils.h"
#include "state.h"

#include <Adafruit_PWMServoDriver.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <MFRC522.h>

Adafruit_PWMServoDriver servo = Adafruit_PWMServoDriver();
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD1_D4, LCD1_D5, LCD1_D6, LCD1_D7);
MFRC522 rfid(NFC_SDA, NFC_RST);

UIDProfile* currentProfile = nullptr;

bool hbfStop = false;
bool hbfStart = false;
int hbfMin = 60;
int hbfMax = 80;
int hbfBrake = 110;
int dist_rd1_rd2 = 31.5; // Distance between reed sensors 1 and 2 in cm
int dist_rd2_rd3 = 33.0; // Distance between reed sensors 2 and 3 in cm

void setup() {
    init(servo, lcd, rfid);
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

    // NFC PROFILE READER #########################################################################
    currentProfile = getTag(rfid);

    if (currentProfile) {

        for (int i = 0; i < 3; ++i) {
            HBF_SLOT* slot = (i == 0) ? &HBF_ACTIVE.HBF1 : &HBF_ACTIVE.HBF2;

            if (strncmp(slot->name, currentProfile->name, sizeof(slot->name)) == 0 || strncmp(slot->uid, currentProfile->uid, sizeof(slot->uid)) == 0) {
                slot->name[0] = '\0';
                slot->uid[0] = '\0';
                slot->min = 0;
                slot->max = 0;
                slot->brake = 0;
            }
        }

        HBF_SLOT* activeSlot = nullptr;
        if (HBF_ROUTE.HBF1.active)
            activeSlot = &HBF_ACTIVE.HBF1;
        else if (HBF_ROUTE.HBF2.active)
            activeSlot = &HBF_ACTIVE.HBF2;

        if (activeSlot) {
            strncpy(activeSlot->name, currentProfile->name, sizeof(activeSlot->name));
            strncpy(activeSlot->uid, currentProfile->uid, sizeof(activeSlot->uid));
            activeSlot->min = currentProfile->min;
            activeSlot->max = currentProfile->max;
            activeSlot->brake = currentProfile->brake;
        }

        EEPROM.put(EEPROM_ACTIVE, HBF_ACTIVE);
    }

    // TRACK REED #################################################################################
    ReedControl::updateStates();

    ReedControl::push(3, []() {
        // HBF2 LEFT
        Utils::speedStart = millis();
        if (!HBF_ACTIVE.HBF2.active) {
            hbfBrake = HBF_ACTIVE.HBF2.brake;
        }
    });

    ReedControl::push(4, []() {
        // HBF2 CENTER
        Utils::speedEnd = millis();
        Utils::currentSpeed = Utils::speedMeasure(Utils::speedStart, Utils::speedEnd, 31.0);
        if (!HBF_ACTIVE.HBF2.active) {
            hbfStop = true;
            hbfMin = HBF_ACTIVE.HBF2.min;
        }
    });

    ReedControl::push(5, []() {
        // HBF2 RIGHT
        hbfStop = false;
        if (!HBF_ACTIVE.HBF2.active) {
            RelayControl::setRelay(5, true);
            RelayControl::setRelay(6, false);
        }
    });

    ReedControl::push(2, []() {
        // HBF3 RIGHT
    });

    ReedControl::push(1, []() {
        // HBF3 CENTER
    });

    ReedControl::push(6, []() {
        Utils::speedStart = millis();
        if (!HBF_ACTIVE.HBF1.active) {
            hbfBrake = HBF_ACTIVE.HBF1.brake;
        }
    });

    ReedControl::push(7, []() {
        Utils::speedEnd = millis();
        Utils::currentSpeed = Utils::speedMeasure(Utils::speedStart, Utils::speedEnd, dist_rd1_rd2);
        if (!HBF_ACTIVE.HBF1.active) {
            hbfStop = true;
            hbfMin = HBF_ACTIVE.HBF1.min;
        }
    });

    ReedControl::push(8, []() {
        hbfStop = false;
        if (!HBF_ACTIVE.HBF1.active) {
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
        if (!HBF_ROUTE.HBF1.active) {
            return;
        }

        // DEPARTING TRAIN
        if (!HBF_ACTIVE.HBF1.active) {
            // SWITCH FROM ZONE A TO C
            RelayControl::setRelay(7, false);
            // TURN ON HBF1
            RelayControl::setRelay(8, true);
        }

        // ARRIVING TRAIN
        if (HBF_ACTIVE.HBF1.active) {
            // SWITCH FROM ZONE C TO A
            RelayControl::setRelay(7, true);
        }

        bool wasActive = HBF_ACTIVE.HBF1.active;
        HBF_ACTIVE.HBF1.active = !HBF_ACTIVE.HBF1.active;
        HBF_ACTIVE.HBF2.active = false;
        EEPROM.put(EEPROM_ACTIVE, HBF_ACTIVE);

        if (!wasActive && HBF_ACTIVE.HBF1.active) {
            hbfStart = true;
            hbfMax = abs(HBF_ACTIVE.HBF1.max) * (-1);
        }
    });

    ButtonControl::pushButton(BTN_HBF2, []() {
        if (!HBF_ROUTE.HBF2.active) {
            return;
        }

        // DEPARTING TRAIN
        if (!HBF_ACTIVE.HBF2.active) {
            // SWITCH FROM ZONE A TO C
            RelayControl::setRelay(5, false);
            // TURN OFF HBF2
            RelayControl::setRelay(6, true);
        }

        // ARRIVING TRAIN
        if (HBF_ACTIVE.HBF2.active) {
            // SWITCH FROM ZONE C TO A
            RelayControl::setRelay(5, true);
        }

        bool wasActive = HBF_ACTIVE.HBF1.active;
        HBF_ACTIVE.HBF1.active = false;
        HBF_ACTIVE.HBF2.active = !HBF_ACTIVE.HBF2.active;
        EEPROM.put(EEPROM_ACTIVE, HBF_ACTIVE);

        if (!wasActive && HBF_ACTIVE.HBF2.active) {
            hbfStart = true;
            hbfMax = abs(HBF_ACTIVE.HBF2.max) * (-1);
        }
    });


    ButtonControl::pushButton(SW_HBF2, []() {
        ServoControl::switchTurnout(servo, W1, true);
        ServoControl::switchTurnout(servo, W2, false);
        HBF_ROUTE.HBF1.active = false;
        HBF_ROUTE.HBF2.active = true;
        EEPROM.put(EEPROM_ROUTE, HBF_ROUTE);
    });

    ButtonControl::pushButton(SW_HBF1, []() {
        ServoControl::switchTurnout(servo, W1, false);
        ServoControl::switchTurnout(servo, W2, true);
        HBF_ROUTE.HBF1.active = true;
        HBF_ROUTE.HBF2.active = false;
        EEPROM.put(EEPROM_ROUTE, HBF_ROUTE);
    });

    // DISPLAY COMMON STATES ######################################################################
    // Utils::currentSpeed != 0.0
    //     ? LCDControl::print(lcd, 9, 19, 3, "v:" + String(Utils::currentSpeed, 2) + "cm/s")
    //     : LCDControl::print(lcd, 9, 19, 3, "v:--.--cm/s");

    // LCDControl::print(lcd, 9, 19, 2, "v:" + String(Utils::scaleSpeed(Utils::currentSpeed)) + "km/h");

    // DISPLAY HBF STATE ##########################################################################
    for (int i = 0; i < 3; ++i) {
        String label = (HBF_ROUTE.HBF1.active && i == 0) || (HBF_ROUTE.HBF2.active && i == 1) ? ">" : " ";
        LCDControl::print(lcd, 0, 5, i, label + "HBF " + String(i + 1));

        bool active = (i == 0 && HBF_ACTIVE.HBF1.active) || (i == 1 && HBF_ACTIVE.HBF2.active);
        LCDControl::print(lcd, 6, 7, i, active ? "*" : " ");
    }

    LCDControl::print(lcd, 9, 19, 0, String(HBF_ACTIVE.HBF1.name));
    LCDControl::print(lcd, 9, 19, 1, String(HBF_ACTIVE.HBF2.name));

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

    // if (!HBF_ACTIVE.HBF1.active && hbfStop) {
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
