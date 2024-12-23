#include "evse_rtc.h"

#include "Arduino.h"

PCF85063A rtc;
time_t t;

void Rtc::set_ext_rtc_time(void)
{
    tmElements_t tm;
    breakTime(1478648990, tm); /* change unix time accordingly  or get it from 4g module*/
    rtc.time_set(&tm);
}

void Rtc::get_ext_rtc_time(void)
{
    tmElements_t t;
    /* Get and print current time */
    rtc.time_get(&t);

    // Serial.print((1970 + t.Year));
    // Serial.print(String(" , ") + t.Month);
    // Serial.print(String(" , ") + t.Day);
    // Serial.print(String(" , ") + t.Hour);
    // Serial.print(String(" : ") + t.Minute);
    // Serial.print(String(" : ") + t.Second);
    // Serial.print("\n");
}

#if 0
void Rtc::print_time(tmElements_t t) 
{
    Serial.print(String("Year ") + (1970 + t.Year));
    Serial.print(String(" Month ") + t.Month);
    Serial.print(String(" Day ") + t.Day);
    Serial.print(String(" Wday ") + t.Wday);
    Serial.print(String(" Hour ") + t.Hour);
    Serial.print(String(" Minute ") + t.Minute);
    Serial.print(String(" Second ") + t.Second);
    Serial.print("\n");
}
#endif