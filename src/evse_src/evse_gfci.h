#ifndef _EVSE_GFCI_H_
#define _EVSE_GFCI_H_

#include <Arduino.h>

#define GFCI_PIN (34)

class GFCI
{
public:
    void gfci_init(void);
    uint8_t gfci_read(void);
    // void gfci_read(void);
};

extern GFCI gfci_object;

#endif