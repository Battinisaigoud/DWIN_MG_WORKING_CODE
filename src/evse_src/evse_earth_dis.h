#ifndef _EVSE_EARTH_H_
#define _EVSE_EARTH_H_

#include <Arduino.h>

#define EARTH_PIN (39)

#define EARTH_DISCONTION_COUNT (100)

class EARTH
{
public:
    void earth_init(void);
    // void earth_read(void);
    uint32_t earth_read();
};

#endif