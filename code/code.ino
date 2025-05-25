#include "buttonControl.h"
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
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD1_D4, LCD1_D5,LCD1_D6, LCD1_D7);

int REED_PIN1 = 51;
int REED_PIN2 = 53;
int lastSwitchState = HIGH;
int counter = 0;

const float trackDistance = 1.0;
const float scaleFactor = 160.0;
unsigned long startTime = 0;
unsigned long endTime = 0;
bool measurementStarted = false;
bool reed1Triggered = false;
bool reed2Triggered = false;

void setup() {

    Serial.begin(9600);

    // INIT LCD DOT MATRIX ########################################################################
    lcd.begin(20, 4);
    lcd.print("      Hello! :)     ");
    delay(2000);
    lcd.clear();

    pinMode(REED_PIN1, INPUT_PULLUP);
    pinMode(REED_PIN2, INPUT_PULLUP);

    // INIT ENCODER (MAIN) ########################################################################
    pinMode(ENC_MAIN_1_CLK, INPUT_PULLUP);
    pinMode(ENC_MAIN_1_DT, INPUT_PULLUP);
    ENC_MAIN_1_CLK_STATE = digitalRead(ENC_MAIN_1_CLK);
    attachInterrupt(digitalPinToInterrupt(ENC_MAIN_1_CLK), processEncoder, CHANGE);

    // INIT MOTOR MODULE (MAIN) ####################################################################
    pinMode(MOTOR_IN1, OUTPUT);
    pinMode(MOTOR_IN2, OUTPUT);
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

    // SET PWM FREQUENCY ##########################################################################
    // 0x02    / 0x03  / 0x04  / 0x05
    // 3.92kHz / 490Hz / 122Hz / 30.5Hz
    TCCR4B = (TCCR4B & 0b11111000) | 0x03;
    analogWrite(MOTOR_IN1, 128);    // 50% Duty Cycle
    analogWrite(MOTOR_HBF1_1, 128); // 50% Duty Cycle
    analogWrite(MOTOR_HBF2_1, 128); // 50% Duty Cycle

    // READ FROM EEPROM ###########################################################################
    // INIT TRACK STATION #########################################################################
    EEPROM.get(0, HBF_STATE);

}

void loop() {

    // BUTTON TEST ################################################################################
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

    // LCD PRINT TEST #############################################################################
    int percent = map(abs(ENC_MAIN_1_VALUE), 0, 255, 0, 100);

    LCDControl::print(lcd, 0, 3, 0, "VEL:");
    LCDControl::print(lcd, 6, 8, 0, ENC_MAIN_1_VALUE > 0 ? ">" : "<");
    LCDControl::print(lcd, 11, 14, 0, String((int)ENC_MAIN_1_VALUE));
    LCDControl::print(lcd, 18, 19, 0, "0%");
    LCDControl::print(lcd, 16, 18, 0, String((int)percent), "RTL");

    if (HBF_STATE.HBF1) {
        LCDControl::print(lcd, 0, 5, 1, "HBF 1");
    } else if (HBF_STATE.HBF2) {
        LCDControl::print(lcd, 0, 5, 1, "HBF 2");
    } else if (HBF_STATE.HBF3) {
        LCDControl::print(lcd, 0, 5, 1, "HBF 3");
    } else {
        LCDControl::print(lcd, 0, 5, 1, "     ");
    }

    // REED SWITCH TEST ###########################################################################
    int switchState = digitalRead(REED_PIN1);
    if (switchState == LOW && lastSwitchState == HIGH) {
        counter++;
        // LCDControl::print(lcd, 6, 19, 3, String((int)counter));
    }
    lastSwitchState = switchState;

    // SPEED MEASURE ##############################################################################
    if (digitalRead(REED_PIN1) == LOW && !reed1Triggered) {
        startMeasurement();
        reed1Triggered = true;
    }

    if (digitalRead(REED_PIN2) == LOW && reed1Triggered && !reed2Triggered) {
        stopMeasurement();
        reed2Triggered = true;
    }

    if (measurementStarted && endTime > startTime) {
        float timeTaken = (endTime - startTime) / 1000.0; // Zeit in Sekunden
        float speed = trackDistance / timeTaken; // Geschwindigkeit in m/s
        float realSpeed = speed * scaleFactor * 3.6; // Geschwindigkeit im Original in km/h

        // lcdPrint(lcd, 0, 3, 3, String((int)realSpeed));
        // lcdPrint(lcd, 7, 10, 3, "km/h");

        measurementStarted = false; // Messung zurücksetzen
        reed1Triggered = false;
        reed2Triggered = false;
    }

    // MAIN SPEED CONTROL #########################################################################
    motorEncoderControl(ENC_MAIN_1_VALUE, MOTOR_IN1, MOTOR_IN2);
    motorEncoderControl(HBF_STATE.HBF1 ? ENC_MAIN_1_VALUE : 0, MOTOR_HBF1_1, MOTOR_HBF1_2);
    motorEncoderControl(HBF_STATE.HBF2 ? ENC_MAIN_1_VALUE : 0, MOTOR_HBF2_1, MOTOR_HBF2_2);

    setButtonStates();

    delay(25);
}

void processEncoder() {
    int currentCLKState = digitalRead(ENC_MAIN_1_CLK);
    int currentDTState = digitalRead(ENC_MAIN_1_DT);

    if (currentCLKState != ENC_MAIN_1_CLK_STATE) {
        ENC_MAIN_1_VALUE += (currentDTState == currentCLKState) ? 3 : -3;
    }

    ENC_MAIN_1_VALUE = constrain(ENC_MAIN_1_VALUE, -255, 255);
    ENC_MAIN_1_CLK_STATE = currentCLKState;
}

void startMeasurement() {
    startTime = millis();
    measurementStarted = true;
}

void stopMeasurement() {
    endTime = millis();
}
