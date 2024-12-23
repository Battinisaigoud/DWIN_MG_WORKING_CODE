#ifndef _EVSE_CONTACTER_RELAY_H
#define _EVSE_CONTACTER_RELAY_H
#include <Arduino.h>

#define RELAY_PIN (18)
#define RELAY2_PIN (25)

class RELAY
{
public:
    void evse_relay_init(void);
    void evse_relay2_init(void);
    void evse_relay_on(void);
    void evse_relay_off(void);
    void evse_relay2_on(void);
    void evse_relay2_off(void);
};

#endif