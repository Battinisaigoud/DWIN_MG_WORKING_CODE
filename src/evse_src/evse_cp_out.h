#ifndef _EVSE_CP_OUT_H
#define _EVSE_CP_OUT_H

#include <Arduino.h>
#include "evse_config.h"



#define PWM_PIN (32)
#define PWM_FREQUENCY (1000) // 1KHZ
#define PWM_CHANNEL (0)
#define PWM_RESOLUTION (8)
#define PWM_DUTY (50)

class CP_OUT
{
public:
    void set_cp_init();
    void set_cp_begin(uint8_t chargingLimit);
    void set_cp_off();
    void set_cp_on();
    int CalculateDutyCycle(float chargingLimit);
    int CalculateDutyCycle_32Amp(void);
    void set_cp_begin_Amp(int ls32_dutycycle);
};

#endif