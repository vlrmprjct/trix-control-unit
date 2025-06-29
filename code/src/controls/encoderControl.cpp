#include "../../config.h"
#include "encoderControl.h"
#include <Arduino.h>

namespace EncoderControl {

    void process() {
        int currentCLKState = digitalRead(ENC_MAIN_1_CLOCK);
        int currentDTState = digitalRead(ENC_MAIN_1_DT);

        if (currentCLKState != ENC_MAIN_1_CLOCK_STATE) {
            ENC_MAIN_1_VALUE += (currentDTState == currentCLKState) ? 1 : -1;
        }

        ENC_MAIN_1_VALUE = constrain(ENC_MAIN_1_VALUE, -255, 255);
        ENC_MAIN_1_CLOCK_STATE = currentCLKState;
    }
}
