#pragma once

#include "../../state.h"

struct BBFConfig {
    Tracks* track;
    int relay;
};

namespace BlockControl {
    extern const BBFConfig bbf[];
    extern const int BBF_COUNT;

    void stopBBF(Tracks& track, int relay);
    void toggleBBF(Tracks& track, int relay);
    void releasePendingBBF();
}
