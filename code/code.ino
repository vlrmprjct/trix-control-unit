#include "buttonControl.h"
#include "reedControl.h"
#include "encoderControl.h"
#include "config.h"
#include "lcdControl.h"
#include "motorControl.h"
#include "relayControl.h"
#include "servoControl.h"

#include <Adafruit_PWMServoDriver.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <Wire.h>

Adafruit_PWMServoDriver servo = Adafruit_PWMServoDriver();
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD1_D4, LCD1_D5, LCD1_D6, LCD1_D7);

void setup() {

    Serial.begin(9600);

    // INIT LCD DOT MATRIX ########################################################################
    lcd.begin(20, 4);
    lcd.print("BOOTING ...");
    delay(2000);
    lcd.clear();

    // INIT ENCODER (MAIN) ########################################################################
    pinMode(ENC_MAIN_1_CLOCK, INPUT_PULLUP);
    pinMode(ENC_MAIN_1_DT, INPUT_PULLUP);
    ENC_MAIN_1_CLOCK_STATE = digitalRead(ENC_MAIN_1_CLOCK);
    attachInterrupt(digitalPinToInterrupt(ENC_MAIN_1_CLOCK), EncoderControl::process, CHANGE);

    // INIT MOTOR MODULE (MAIN) ####################################################################
    pinMode(MOTOR_MAIN_1, OUTPUT);
    pinMode(MOTOR_MAIN_2, OUTPUT);
    pinMode(MOTOR_HBF1_1, OUTPUT);
    pinMode(MOTOR_HBF1_2, OUTPUT);
    pinMode(MOTOR_HBF2_1, OUTPUT);
    pinMode(MOTOR_HBF2_2, OUTPUT);

    // INIT TURNOUTS SERVO MODULE ##################################################################
    servo.begin();
    servo.setPWMFreq(60);

    // INIT RELAYS ################################################################################
    pinMode(RELAY_LATCH, OUTPUT);
    pinMode(RELAY_CLOCK, OUTPUT);
    pinMode(RELAY_DATA, OUTPUT);
    // INITIALIZE ALL RELAYS "OFF"
    for (int i = 0; i < RELAY_COUNT; i++) {
        setRelay(i, false);
    }

    // INIT BUTTON CTRL ##########################################################################
    pinMode(BTN_DATA, INPUT);
    pinMode(BTN_CLOCK, OUTPUT);
    pinMode(BTN_LATCH, OUTPUT);

    // INIT REED CTRL #############################################################################
    pinMode(REED_DATA, INPUT);
    pinMode(REED_CLOCK, OUTPUT);
    pinMode(REED_LATCH, OUTPUT);

    // SET PWM FREQUENCY ##########################################################################
    // 0x02    / 0x03  / 0x04  / 0x05
    // 3.92kHz / 490Hz / 122Hz / 30.5Hz
    TCCR4B = (TCCR4B & 0b11111000) | 0x03;
    analogWrite(MOTOR_MAIN_1, 128); // 50% Duty Cycle
    analogWrite(MOTOR_HBF1_1, 128); // 50% Duty Cycle
    analogWrite(MOTOR_HBF2_1, 128); // 50% Duty Cycle

    // READ FROM EEPROM ###########################################################################
    // INIT TRACK STATION #########################################################################
    EEPROM.get(0, HBF_STATE);
}

void loop() {

    // TRACK REED #################################################################################
    ReedControl::updateStates();

    ReedControl::push(8, []() {
        ServoControl::switchTurnout(servo, 0, false);
        ServoControl::switchTurnout(servo, 1, true);
        ServoControl::switchTurnout(servo, 2, false);
        HBF_STATE = { true, false, false };
        EEPROM.put(0, HBF_STATE);
    });

    // TURNOUT MANUAL CONTROL ######################################################################
    updateButtonStates();

    pushButton(1, []() {
        ServoControl::switchTurnout(servo, 0, false);
        ServoControl::switchTurnout(servo, 1, true);
        ServoControl::switchTurnout(servo, 2, false);
        HBF_STATE = { true, false, false };
        EEPROM.put(0, HBF_STATE);
    });

    pushButton(2, []() {
        ServoControl::switchTurnout(servo, 0, true);
        ServoControl::switchTurnout(servo, 1, false);
        ServoControl::switchTurnout(servo, 2, false);
        HBF_STATE = { false, true, false };
        EEPROM.put(0, HBF_STATE);
    });

    pushButton(3, []() {
        ServoControl::switchTurnout(servo, 0, true);
        ServoControl::switchTurnout(servo, 1, false);
        ServoControl::switchTurnout(servo, 2, true);
        HBF_STATE = { false, false, true };
        EEPROM.put(0, HBF_STATE);
    });

    // DISPLAY COMMON STATES ######################################################################
    int percent = map(abs(ENC_MAIN_1_VALUE), 0, 255, 0, 100);

    LCDControl::print(lcd, 0, 3, 0, "VEL:");
    LCDControl::print(lcd, 6, 8, 0, ENC_MAIN_1_VALUE > 0 ? ">" : "<");
    LCDControl::print(lcd, 11, 14, 0, String((int)ENC_MAIN_1_VALUE));
    LCDControl::print(lcd, 18, 19, 0, "0%");
    LCDControl::print(lcd, 16, 18, 0, String((int)percent), "RTL");

    // DIPLAY HBF STATE ###########################################################################
    for (int i = 0; i < 3; ++i) {
        String label = (HBF_STATE.HBF1 && i == 0) || (HBF_STATE.HBF2 && i == 1) || (HBF_STATE.HBF3 && i == 2) ? ">" : " ";
        LCDControl::print(lcd, 0, 5, i + 1, label + "HBF " + String(i + 1));
    }

    // MAIN SPEED CONTROL #########################################################################
    motorEncoderControl(ENC_MAIN_1_VALUE, MOTOR_MAIN_1, MOTOR_MAIN_2);

    // HBF MOTOR CONTROL ##########################################################################
    motorEncoderControl(HBF_STATE.HBF1 ? ENC_MAIN_1_VALUE : 0, MOTOR_HBF1_1, MOTOR_HBF1_2);
    motorEncoderControl(HBF_STATE.HBF2 ? ENC_MAIN_1_VALUE : 0, MOTOR_HBF2_1, MOTOR_HBF2_2);

    setButtonStates();
    ReedControl::setStates();

    delay(25);
}
