#pragma once
#include "../../config.h"

namespace MotorControl {
    void softSPIWrite(byte dataOut);
    void setValue(byte potNum, int value);
}
