#include "../../config.h"
#include "../../state.h"
#include <EEPROM.h>

void initState() {
    EEPROM.get(EEPROM_ROUTE, HBF_ROUTE);
    EEPROM.get(EEPROM_ACTIVE, HBF_ACTIVE);

    bool needsInit = false;
    if (HBF_ROUTE.HBF1.name[0] == '\0' && HBF_ROUTE.HBF2.name[0] == '\0' && HBF_ROUTE.HBF3.name[0] == '\0')
        needsInit = true;
    if (HBF_ACTIVE.HBF1.name[0] == '\0' && HBF_ACTIVE.HBF2.name[0] == '\0' && HBF_ACTIVE.HBF3.name[0] == '\0')
        needsInit = true;

    if (needsInit) {
        HBF_ROUTE.HBF1.active = false;
        HBF_ROUTE.HBF2.active = false;
        HBF_ROUTE.HBF3.active = false;
        HBF_ROUTE.HBF1.name[0] = '\0';
        HBF_ROUTE.HBF2.name[0] = '\0';
        HBF_ROUTE.HBF3.name[0] = '\0';

        HBF_ACTIVE.HBF1.active = false;
        HBF_ACTIVE.HBF2.active = false;
        HBF_ACTIVE.HBF3.active = false;
        HBF_ACTIVE.HBF1.name[0] = '\0';
        HBF_ACTIVE.HBF2.name[0] = '\0';
        HBF_ACTIVE.HBF3.name[0] = '\0';

        EEPROM.put(EEPROM_ROUTE, HBF_ROUTE);
        EEPROM.put(EEPROM_ROUTE, HBF_ACTIVE);
    }
}
