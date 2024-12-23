#ifndef EVSE_WATCH_DOG_H_
#define EVSE_WATCH_DOG_H_
#include <Arduino.h>

#define WATCH_DOG_PIN (2)

class WATCH_DOG
{
private:
    /* data */
public:
    void evse_watch_dog_init(void);
    void evse_watch_dog_on(void);
    void evse_watch_dog_off(void);
};

#endif