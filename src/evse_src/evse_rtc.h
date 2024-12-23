#ifndef _EVSE_RTC_H_
#define _EVSE_RTC_H_

#include "../evse_lib/src/PCF85063A/PCF85063A.h"


class Rtc : public PCF85063A{
    public:

void set_ext_rtc_time(void);
void get_ext_rtc_time(void);
//void print_time(tmElements_t t);

};

#endif
