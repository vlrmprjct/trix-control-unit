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
        "04 3D 36 6A 40 15 90",
        "BR132-x",
        62,
        82,
        76
    },
    {
        "A2 5E C0 AB",
        "BR132-1",
        54,
        80,
        74
    },
    {
        "12 1E 90 AB",
        "BR132",
        56,
        80,
        76
    }
};

inline const int profileCount = sizeof(profiles) / sizeof(UIDProfile);
