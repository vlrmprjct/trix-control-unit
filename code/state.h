#pragma once

struct Tracks {
    bool occupied;
    bool selected;
    bool powered;
    char name[32];
    char uid[32];
    int min;
    int max;
    int brake;
};

struct Routes {
    Tracks track[10];
};

inline Routes ROUTE;

inline Tracks& HBF1   = ROUTE.track[1];
inline Tracks& HBF2   = ROUTE.track[2];
inline Tracks& BBF1   = ROUTE.track[3];
inline Tracks& BBF2   = ROUTE.track[4];
inline Tracks& BBF3   = ROUTE.track[5];
inline Tracks& BBF4   = ROUTE.track[6];
inline Tracks& BBF5   = ROUTE.track[7];
inline Tracks& BLOCKA  = ROUTE.track[8];
inline Tracks& BLOCKB  = ROUTE.track[9];

struct RouteMeta {
    unsigned long magic;
    unsigned short version;
};
