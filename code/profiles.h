#pragma once

struct UIDProfile {
    const char* uid;
    const char* name;
    int min;
    int max;
    int brake;
};

inline UIDProfile profiles[] = {
    {
        "04 73 86 3A 25 55 80",
        "Test NFC 1"
    },
    {
        "A2 5E C0 AB",
        "Test NFC 2"
    },
    {
        "12 1E 90 AB",
        "Test NFC 3"
    },
    {
        "12 1E 90 AB",
        "BR132",
        48,
        80,
        68
    }
};

inline const int profileCount = sizeof(profiles) / sizeof(UIDProfile);
