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
bool hbf1ShouldStop = false;

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
    lcd.clear();

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

    // INIT RELAYS ################################################################################
    pinMode(RELAY_LATCH, OUTPUT);
    pinMode(RELAY_CLOCK, OUTPUT);
    pinMode(RELAY_DATA, OUTPUT);
    RelayControl::initRelays(); // Relais auf gespeicherten Zustand setzen

    // INIT BUTTON CTRL ###########################################################################
    pinMode(BTN_DATA, INPUT);
    pinMode(BTN_CLOCK, OUTPUT);
    pinMode(BTN_LATCH, OUTPUT);

    // INIT REED CTRL #############################################################################
    pinMode(REED_DATA, INPUT);
    pinMode(REED_CLOCK, OUTPUT);
    pinMode(REED_LATCH, OUTPUT);

    // SET PWM FREQUENCY ##########################################################################
    Utils::setPrescalerTimers(0x03);

    // INIT STATE / READ FROM EEPROM ##############################################################
    initState();
    EEPROM.get(EEPROM_ROUTE, HBF_ROUTE);
    EEPROM.get(EEPROM_ACTIVE, HBF_ACTIVE);

    // CLEAR EEPROM ###############################################################################
    // Utils::clearEEPROM();
}

void loop() {

    // NFC READER #################################################################################
    currentProfile = getTag(rfid);

    if (currentProfile) {
        // Array mit Zeigern auf die Namen der drei HBF-Slots
        char* hbfNames[3] = {
            HBF_ACTIVE.HBF1.name,
            HBF_ACTIVE.HBF2.name,
            HBF_ACTIVE.HBF3.name
        };

        // Namen aus allen Feldern entfernen, falls vorhanden (nur exakt passenden Namen)
        for (int i = 0; i < 3; ++i) {
            if (strncmp(hbfNames[i], currentProfile->name, sizeof(HBF_ACTIVE.HBF1.name)) == 0) {
                hbfNames[i][0] = '\0';
            }
        }

        // Aktives HBF-Feld finden und Namen setzen
        bool* hbfActive[3] = {
            &HBF_ROUTE.HBF1.active,
            &HBF_ROUTE.HBF2.active,
            &HBF_ROUTE.HBF3.active
        };

        for (int i = 0; i < 3; ++i) {
            if (*hbfActive[i]) {
                strncpy(hbfNames[i], currentProfile->name, sizeof(HBF_ACTIVE.HBF1.name));
                break; // Nur in das erste aktive Feld schreiben
            }
        }

        EEPROM.put(EEPROM_ACTIVE, HBF_ACTIVE);
    }

    // TRACK REED #################################################################################
    ReedControl::updateStates();

    ReedControl::push(6, []() {
        Utils::speedStart = millis();
    });

    ReedControl::push(7, []() {
        Utils::speedEnd = millis();
        Utils::currentSpeed = Utils::speedMeasure(Utils::speedStart, Utils::speedEnd, 31.0);
        if (!HBF_ACTIVE.HBF1.active)
            hbf1ShouldStop = true;
    });

    ReedControl::push(8, []() {
        hbf1ShouldStop = false;
        if (!HBF_ACTIVE.HBF1.active) {
            ENC_MAIN_1_VALUE = 0;
            RelayControl::setRelay(8, false);
        }
    });

    if (!HBF_ACTIVE.HBF1.active && hbf1ShouldStop) {
        MotorControl::rampDown(ENC_MAIN_1_VALUE, 2, 60);
    }

    // TRACK CONTROL ##############################################################################
    ButtonControl::updateStates();

    ButtonControl::pushButton(BTN_HBF1, []() {
        if (!HBF_ACTIVE.HBF1.active) {
            RelayControl::setRelay(8, true);
        }
        HBF_ACTIVE.HBF1.active = !HBF_ACTIVE.HBF1.active;
        HBF_ACTIVE.HBF2.active = false;
        HBF_ACTIVE.HBF3.active = false;
        EEPROM.put(EEPROM_ACTIVE, HBF_ACTIVE);
    });

    ButtonControl::pushButton(BTN_HBF2, []() {
        HBF_ACTIVE.HBF1.active = false;
        HBF_ACTIVE.HBF2.active = !HBF_ACTIVE.HBF2.active;
        HBF_ACTIVE.HBF3.active = false;
        EEPROM.put(EEPROM_ACTIVE, HBF_ACTIVE);
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

    LCDControl::print(lcd, 0, 3, 0, "VEL:");
    LCDControl::print(lcd, 6, 8, 0, ENC_MAIN_1_VALUE > 0 ? "<" : ">");
    LCDControl::print(lcd, 11, 14, 0, String((int)ENC_MAIN_1_VALUE));
    LCDControl::print(lcd, 18, 19, 0, "0%");
    LCDControl::print(lcd, 16, 18, 0, String((int)percent), "RTL");

    Utils::currentSpeed != 0.0
        ? LCDControl::print(lcd, 9, 19, 3, "v:" + String(Utils::currentSpeed, 2) + "cm/s")
        : LCDControl::print(lcd, 9, 19, 3, "v:--.--cm/s");

    LCDControl::print(lcd, 9, 19, 2, "v:" + String(Utils::scaleSpeed(Utils::currentSpeed)) + "km/h");

    // DISPLAY HBF STATE ##########################################################################
    for (int i = 0; i < 3; ++i) {
        String label = (HBF_ROUTE.HBF1.active && i == 0) || (HBF_ROUTE.HBF2.active && i == 1) || (HBF_ROUTE.HBF3.active && i == 2) ? ">" : " ";
        LCDControl::print(lcd, 0, 5, i + 1, label + "HBF " + String(i + 1));

        bool active = (i == 0 && HBF_ACTIVE.HBF1.active) || (i == 1 && HBF_ACTIVE.HBF2.active) || (i == 2 && HBF_ACTIVE.HBF3.active);
        LCDControl::print(lcd, 6, 7, i + 1, active ? "*" : " ");
    }

    LCDControl::print(lcd, 9, 19, 1, String(HBF_ACTIVE.HBF1.name));

    // MAIN SPEED CONTROL #########################################################################
    MotorControl::setValue(ENC_MAIN_1_VALUE, MOTOR_MAIN_1, MOTOR_MAIN_2);

    // HBF MOTOR CONTROL ##########################################################################
    MotorControl::setValue(ENC_MAIN_1_VALUE, MOTOR_HBF1_1, MOTOR_HBF1_2);
    MotorControl::setValue(ENC_MAIN_1_VALUE, MOTOR_HBF2_1, MOTOR_HBF2_2);

    ButtonControl::setStates();
    ReedControl::setStates();

    delay(25);
}
