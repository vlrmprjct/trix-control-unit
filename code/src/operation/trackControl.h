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

    // BBF OPERATIONS
    void stopBBF(Tracks& track, int relay, bool canDepart);
    void toggleBBF(Tracks& track, int relay, bool canDepart);
    void releasePendingBBF();

    // BBF ROUTING (RD_05 ENTRY / RD_BBFx_L EXIT)
    int  findFreeBBF();
    void setBBFEntryRoute(Adafruit_PWMServoDriver& driver, int slot);
    void setBBFExitRoute(Adafruit_PWMServoDriver& driver, int slot);
    void onBBFReedL(Adafruit_PWMServoDriver& driver, int slot);

    // HBF OPERATIONS
    // HBF DEPARTURE OWNER OF W2 / ZONE A (0=NONE, 1=HBF1, 2=HBF2)
    void setDepartingHBF(int n);
    int  getDepartingHBF();
    bool isHBFDeparting();
    void stopHBF(Tracks& track, int zoneRelay, int trackRelay);
    void toggleHBF(Tracks& track, int trackRelay);
    bool releasePendingHBF(Adafruit_PWMServoDriver& driver);

    // ZONE A RELEASE: CALLED FROM RD_10 OR RD_BBFx_L WHEN HBF DEPARTURE CLEARED ZONE A
    void releaseZoneA(Adafruit_PWMServoDriver& driver, Tracks& blockA);

    // SHARED (BBF + HBF)
    void cancelPending(Tracks& track, int relay);
}
