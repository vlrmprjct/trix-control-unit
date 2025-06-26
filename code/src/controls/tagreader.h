#pragma once

#include "../../profiles.h"
#include <MFRC522.h>

UIDProfile* getTag(MFRC522& rfid);
