#pragma once

namespace EncoderControl {

    enum Direction {
        STOP = 0,
        CW = 1,
        CCW = -1
    };

    void process();
    Direction getDirection();
}
