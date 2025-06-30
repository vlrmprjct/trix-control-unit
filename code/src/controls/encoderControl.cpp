#include "../../config.h"
#include "encoderControl.h"
#include <Arduino.h>

namespace EncoderControl {

    void process() {
        int currentCLKState = digitalRead(ENC_MAIN_1_CLOCK);
        int currentDTState = digitalRead(ENC_MAIN_1_DT);

        if (currentCLKState != ENC_MAIN_1_CLOCK_STATE) {
            int delta = (currentDTState == currentCLKState) ? 1 : -1;
            ENC_MAIN_1_VALUE += delta;
        }

        ENC_MAIN_1_VALUE = constrain(ENC_MAIN_1_VALUE, -255, 255);
        ENC_MAIN_1_CLOCK_STATE = currentCLKState;
    }

    Direction getDirection() {
        if (ENC_MAIN_1_VALUE > 0) {
            return CW;
        } else if (ENC_MAIN_1_VALUE < 0) {
            return CCW;
        } else {
            return STOP;
        }
    }
}
