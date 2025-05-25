// USAGE:
// setup() -> setupSpeedMeasurement();
// loop() -> handleSpeedMeasurement();

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

void startMeasurement() {
    startTime = millis();
    measurementStarted = true;
}

void stopMeasurement() {
    endTime = millis();
}

void setupSpeedMeasurement() {
    pinMode(REED_PIN1, INPUT_PULLUP);
    pinMode(REED_PIN2, INPUT_PULLUP);
}

void handleSpeedMeasurement() {
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
}
