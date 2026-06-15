#pragma once

#include <Adafruit_PWMServoDriver.h>
#include "../core/state.h"

struct BBFConfig {
    Tracks* track;
    int relay;
};

struct HBFConfig {
    Tracks* track;
    int zoneRelay;
    int trackRelay;
    bool w2Diverging;
};

namespace TrackControl {
    extern const BBFConfig bbf[];
    extern const int BBF_COUNT;
    extern const HBFConfig hbf[];
    extern const int HBF_COUNT;

    // DEPARTURE SLOT TRACKING (W2 GUARD)
    void setDepartingSlot(int n);
    int  getDepartingSlot();
    bool isDeparting();

    // BBF OPERATIONS
    void stopBBF(Tracks& track, int relay, bool canDepart);
    void toggleBBF(Tracks& track, int relay, bool canDepart);
    void releasePendingBBF();
    bool releasePendingHBF(Adafruit_PWMServoDriver& driver);
    void cancelPending(Tracks& track, int relay);

    // HBF OPERATIONS
    void stopHBF(Tracks& track, int zoneRelay, int trackRelay);
    void toggleHBF(Tracks& track, int trackRelay);

    // ZONE A RELEASE: CALLED FROM RD_10 OR RD_BBFx_L WHEN HBF DEPARTURE CLEARED ZONE A
    void releaseZoneA(Adafruit_PWMServoDriver& driver, Tracks& blockA);
}
