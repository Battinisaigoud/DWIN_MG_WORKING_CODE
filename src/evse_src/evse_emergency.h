#ifndef _EVSE_EMERGENCY_H
#define _EVSE_EMERGENCY_H
#include <Arduino.h>

#define EMGY_PIN (36)

class EMGY
{
public:
  // void emergency_read(void);
  void emergency_init(void);
  uint32_t emergency_read(void);

};

#endif