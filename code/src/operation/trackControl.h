#pragma once

#include "../core/state.h"

struct BBFConfig {
    Tracks* track;
    int relay;
};

namespace TrackControl {
    extern const BBFConfig bbf[];
    extern const int BBF_COUNT;

    // BBF OPERATIONS
    void stopBBF(Tracks& track, int relay, bool canDepart);
    void toggleBBF(Tracks& track, int relay, bool canDepart);
    void releasePendingBBF();
    void cancelPending(Tracks& track, int relay);

    // HBF OPERATIONS
    void stopHBF(Tracks& track, int zoneRelay, int trackRelay);
    void toggleHBF(Tracks& track, int trackRelay);
}
