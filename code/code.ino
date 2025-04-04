#include "config.h"
#include "lcdPrint.h"
#include "motorControl.h"
#include "relayControl.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

int REED_PIN1 = 51;
int REED_PIN2 = 53;
int lastSwitchState = HIGH;
int counter = 0;

const float trackDistance = 1.0; // Abstand der Reedkontakte in Metern
const float scaleFactor = 160.0; // Maßstab 1:160 für Spur N
unsigned long startTime = 0;
unsigned long endTime = 0;
bool measurementStarted = false;
bool reed1Triggered = false;
bool reed2Triggered = false;

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);

void setup() {
    Serial.begin(9600);

    lcd.init();
    lcd.backlight();

    pinMode(REED_PIN1, INPUT_PULLUP);
    pinMode(REED_PIN2, INPUT_PULLUP);

    pinMode(ENC_MAIN_1_CLK, INPUT_PULLUP);
    pinMode(ENC_MAIN_1_DT, INPUT_PULLUP);
    ENC_MAIN_1_CLK_STATE = digitalRead(ENC_MAIN_1_CLK);

    attachInterrupt(digitalPinToInterrupt(ENC_MAIN_1_CLK), processEncoder, CHANGE);

    pinMode(MTR_MDL_1_ENA, OUTPUT);
    pinMode(MTR_MDL_1_IN1, OUTPUT);
    pinMode(MTR_MDL_1_IN2, OUTPUT);

    pinMode(MTR_MDL_2_ENB, OUTPUT);
    pinMode(MTR_MDL_2_IN3, OUTPUT);
    pinMode(MTR_MDL_2_IN4, OUTPUT);

    pinMode(RELAY_LATCH, OUTPUT);
    pinMode(RELAY_CLOCK, OUTPUT);
    pinMode(RELAY_DATA, OUTPUT);

    pinMode(MTR_MDL_1_CTRL, INPUT);
    pinMode(MTR_MDL_2_CTRL, INPUT);

    digitalWrite(MTR_MDL_1_IN1, LOW);
    digitalWrite(MTR_MDL_1_IN2, LOW);

    // TCCR3B = (TCCR3B & 0b11111000) | 0x02; // 3.92kHz
    TCCR3B = (TCCR3B & 0b11111000) | 0x03; // 490Hz
    // TCCR3B = (TCCR3B & 0b11111000) | 0x04; // 122Hz
    // TCCR3B = (TCCR3B & 0b11111000) | 0x05; // 30.5Hz
    // analogWrite(MTR_MDL_1_ENA, 255);

    for (int i = 0; i < RELAY_COUNT; i++) {
        setRelay(i, false);
    }
}

void loop() {

    int percent = map(abs(ENC_MAIN_1_VALUE), 0, 255, 0, 100);

    // LCD PRINT TEST #############################################################################
    lcdPrint(lcd, 0, 3, 0, "SPD:");
    lcdPrint(lcd, 4, 6, 0, ENC_MAIN_1_VALUE > 0 ? ">" : "<");
    lcdPrint(lcd, 16, 18, 0, String((int)percent), "RTL");
    lcdPrint(lcd, 19, 19, 0, "%");
    // lcdPrint(17, 19, 1, String((int)random(1, 999)), "RTL");
    // lcdPrint(5, 15, 2, "Hello", "RTL");


    // lcdPrint(lcd, 0, 5, 3, "COUNT:");
    // REED SWITCH TEST ###########################################################################
    int switchState = digitalRead(REED_PIN1);
    if (switchState == LOW && lastSwitchState == HIGH) {
        counter++;
        // lcdPrint(lcd, 6, 19, 3, String((int)counter));
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

        lcdPrint(lcd, 0, 3, 3, String((int)realSpeed));
        lcdPrint(lcd, 7, 10, 3, "km/h");

        measurementStarted = false; // Messung zurücksetzen
        reed1Triggered = false;
        reed2Triggered = false;
    }

    // RELAY TEST #################################################################################
    // static unsigned long lastToggleTime = 0;
    // unsigned long currentTime = millis();

    // if (currentTime - lastToggleTime >= 5000) {
    //     toggleRelay(5);
    //     toggleRelay(1);
    //     toggleRelay(24);
    //     setRelay(17, true);
    //     lastToggleTime = currentTime;
    // }
    // setRelay(18, true);

    motorEncoderControl(ENC_MAIN_1_VALUE, MTR_MDL_1_ENA, MTR_MDL_1_IN1, MTR_MDL_1_IN2);

    if (ENC_MAIN_1_VALUE > 200) {
        setRelay(5, true);
    }
    if (ENC_MAIN_1_VALUE < 200) {
        setRelay(5, false);
    }
}

void processEncoder() {
    int currentCLKState = digitalRead(ENC_MAIN_1_CLK);
    int currentDTState = digitalRead(ENC_MAIN_1_DT);

    if (currentCLKState != ENC_MAIN_1_CLK_STATE) {
        ENC_MAIN_1_VALUE += (currentDTState == currentCLKState) ? 5 : -5;
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
