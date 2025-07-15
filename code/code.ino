#include "config.h"
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
#include <SPI.h>
#include <Wire.h>

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

    Serial.begin(9600);

    // INIT I2C BUS ###############################################################################
    SPI.begin();

    // INIT NFC RC522 RFID MODULE #################################################################
    rfid.PCD_Init();

    // INIT LCD DOT MATRIX ########################################################################
    lcd.begin(20, 4);
    LCDControl::print(lcd, 0, 10, 0, "BOOTING ...");
    delay(2000);

    // INIT ENCODER (MAIN) ########################################################################
    pinMode(ENC_MAIN_1_CLOCK, INPUT_PULLUP);
    pinMode(ENC_MAIN_1_DT, INPUT_PULLUP);
    ENC_MAIN_1_CLOCK_STATE = digitalRead(ENC_MAIN_1_CLOCK);
    attachInterrupt(digitalPinToInterrupt(ENC_MAIN_1_CLOCK), EncoderControl::process, CHANGE);

    // INIT MOTOR MODULE (MAIN) ###################################################################
    pinMode(MOTOR_MAIN_1, OUTPUT);
    pinMode(MOTOR_MAIN_2, OUTPUT);
    pinMode(MOTOR_HBF1_1, OUTPUT);
    pinMode(MOTOR_HBF1_2, OUTPUT);
    pinMode(MOTOR_HBF2_1, OUTPUT);
    pinMode(MOTOR_HBF2_2, OUTPUT);

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

void loop() {

    // TIMER ######################################################################################
    static unsigned long startTimer;

    // GET DIRECTION ##############################################################################
    static int dir = EncoderControl::getDirection();

    // NFC PROFILE READER #########################################################################
    currentProfile = getTag(rfid);

    if (currentProfile) {

        for (int i = 0; i < 3; ++i) {
            HBF_SLOT* slot = (i == 0) ? &HBF_ACTIVE.HBF1 : (i == 1) ? &HBF_ACTIVE.HBF2
                                                                    : &HBF_ACTIVE.HBF3;

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
        else if (HBF_ROUTE.HBF3.active)
            activeSlot = &HBF_ACTIVE.HBF3;

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
        RelayControl::toggleRelay(3);
        Utils::speedStart = millis();
        if (!HBF_ACTIVE.HBF2.active) {
            hbfBrake = HBF_ACTIVE.HBF2.brake;
        }
    });

    ReedControl::push(4, []() {
        // HBF2 CENTER
        RelayControl::toggleRelay(4);
        Utils::speedEnd = millis();
        Utils::currentSpeed = Utils::speedMeasure(Utils::speedStart, Utils::speedEnd, 31.0);
        if (!HBF_ACTIVE.HBF2.active) {
            hbfStop = true;
            hbfMin = HBF_ACTIVE.HBF2.min;
        }
    });

    ReedControl::push(5, []() {
        // HBF2 RIGHT
        RelayControl::toggleRelay(5);
        hbfStop = false;
        if (!HBF_ACTIVE.HBF2.active) {
            ENC_MAIN_1_VALUE = -1;
            RelayControl::setRelay(7, false);
        }
    });

    ReedControl::push(2, []() {
        // HBF3 RIGHT
        RelayControl::toggleRelay(4);
    });
    ReedControl::push(1, []() {
        // HBF3 CENTER
        RelayControl::toggleRelay(3);
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
            ENC_MAIN_1_VALUE = -1;
            RelayControl::setRelay(8, false);
        }
    });

    // TRACK CONTROL ##############################################################################
    ButtonControl::updateStates();

    ButtonControl::pushButton(BTN_HBF1, []() {
        if (!HBF_ROUTE.HBF1.active) {
            return;
        }
        if (!HBF_ACTIVE.HBF1.active) {
            RelayControl::setRelay(8, true);
        }
        bool wasActive = HBF_ACTIVE.HBF1.active;
        HBF_ACTIVE.HBF1.active = !HBF_ACTIVE.HBF1.active;
        HBF_ACTIVE.HBF2.active = false;
        HBF_ACTIVE.HBF3.active = false;
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
        if (!HBF_ACTIVE.HBF2.active) {
            RelayControl::setRelay(7, true);
        }
        bool wasActive = HBF_ACTIVE.HBF1.active;
        HBF_ACTIVE.HBF1.active = false;
        HBF_ACTIVE.HBF2.active = !HBF_ACTIVE.HBF2.active;
        HBF_ACTIVE.HBF3.active = false;
        EEPROM.put(EEPROM_ACTIVE, HBF_ACTIVE);

        if (!wasActive && HBF_ACTIVE.HBF2.active) {
            hbfStart = true;
            hbfMax = abs(HBF_ACTIVE.HBF2.max) * (-1);
        }
    });

    ButtonControl::pushButton(BTN_HBF3, []() {
        HBF_ACTIVE.HBF1.active = false;
        HBF_ACTIVE.HBF2.active = false;
        HBF_ACTIVE.HBF3.active = !HBF_ACTIVE.HBF3.active;
        EEPROM.put(EEPROM_ACTIVE, HBF_ACTIVE);
    });

    // TURNOUT MANUAL CONTROL #####################################################################
    ButtonControl::pushButton(SW_HBF3, []() {
        ServoControl::switchTurnout(servo, W1, true);
        ServoControl::switchTurnout(servo, W2, false);
        ServoControl::switchTurnout(servo, W3, true);
        HBF_ROUTE.HBF1.active = false;
        HBF_ROUTE.HBF2.active = false;
        HBF_ROUTE.HBF3.active = true;
        EEPROM.put(EEPROM_ROUTE, HBF_ROUTE);
    });

    ButtonControl::pushButton(SW_HBF2, []() {
        ServoControl::switchTurnout(servo, W1, true);
        ServoControl::switchTurnout(servo, W2, false);
        ServoControl::switchTurnout(servo, W3, false);
        HBF_ROUTE.HBF1.active = false;
        HBF_ROUTE.HBF2.active = true;
        HBF_ROUTE.HBF3.active = false;
        EEPROM.put(EEPROM_ROUTE, HBF_ROUTE);
    });

    ButtonControl::pushButton(SW_HBF1, []() {
        ServoControl::switchTurnout(servo, W1, false);
        ServoControl::switchTurnout(servo, W2, true);
        ServoControl::switchTurnout(servo, W3, false);
        HBF_ROUTE.HBF1.active = true;
        HBF_ROUTE.HBF2.active = false;
        HBF_ROUTE.HBF3.active = false;
        EEPROM.put(EEPROM_ROUTE, HBF_ROUTE);
    });

    // DISPLAY COMMON STATES ######################################################################
    int percent = map(abs(ENC_MAIN_1_VALUE), 0, 255, 0, 100);
    String dirAnim = Utils::getDirectionAnimation(dir, 500);

    LCDControl::print(lcd, 0, 3, 0, "VEL:");
    LCDControl::print(lcd, 6, 8, 0, dirAnim);
    LCDControl::print(lcd, 11, 14, 0, String((int)ENC_MAIN_1_VALUE));
    LCDControl::print(lcd, 18, 19, 0, "0%");
    LCDControl::print(lcd, 16, 18, 0, String((int)percent), "RTL");

    // Utils::currentSpeed != 0.0
    //     ? LCDControl::print(lcd, 9, 19, 3, "v:" + String(Utils::currentSpeed, 2) + "cm/s")
    //     : LCDControl::print(lcd, 9, 19, 3, "v:--.--cm/s");

    // LCDControl::print(lcd, 9, 19, 2, "v:" + String(Utils::scaleSpeed(Utils::currentSpeed)) + "km/h");

    // DISPLAY HBF STATE ##########################################################################
    for (int i = 0; i < 3; ++i) {
        String label = (HBF_ROUTE.HBF1.active && i == 0) || (HBF_ROUTE.HBF2.active && i == 1) || (HBF_ROUTE.HBF3.active && i == 2) ? ">" : " ";
        LCDControl::print(lcd, 0, 5, i + 1, label + "HBF " + String(i + 1));

        bool active = (i == 0 && HBF_ACTIVE.HBF1.active) || (i == 1 && HBF_ACTIVE.HBF2.active) || (i == 2 && HBF_ACTIVE.HBF3.active);
        LCDControl::print(lcd, 6, 7, i + 1, active ? "*" : " ");
    }

    LCDControl::print(lcd, 9, 19, 1, String(HBF_ACTIVE.HBF1.name));
    LCDControl::print(lcd, 9, 19, 2, String(HBF_ACTIVE.HBF2.name));

    // MAIN SPEED CONTROL #########################################################################
    if (hbfStart) {
        MotorControl::rampValue(ENC_MAIN_1_VALUE, hbfMax, 2, 150);
        if (ENC_MAIN_1_VALUE == hbfMax)
            hbfStart = false;
    }

    if (!HBF_ACTIVE.HBF1.active && hbfStop) {
        if (startTimer == 0)
            startTimer = millis();
        ENC_MAIN_1_VALUE = (-1) * MotorControl::rampDynamicValue(millis() - startTimer, 0, dist_rd2_rd3, abs(hbfBrake), abs(hbfMin), Utils::currentSpeed);
    } else {
        startTimer = 0;
    }

    MotorControl::setValue(ENC_MAIN_1_VALUE, MOTOR_MAIN_1, MOTOR_MAIN_2);

    // HBF MOTOR CONTROL ##########################################################################
    MotorControl::setValue(ENC_MAIN_1_VALUE, MOTOR_HBF1_1, MOTOR_HBF1_2);
    MotorControl::setValue(ENC_MAIN_1_VALUE, MOTOR_HBF2_1, MOTOR_HBF2_2);

    ButtonControl::setStates();
    ReedControl::setStates();

    delay(25);
}
