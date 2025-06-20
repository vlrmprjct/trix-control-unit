#ifndef BUTTON_CONTROL_H
#define BUTTON_CONTROL_H

#include <Arduino.h>

namespace ButtonControl {
    int readButtons();
    void pushButton(int buttonNr, void (*callback)());
    void updateStates();
    void setStates();
}

#endif
