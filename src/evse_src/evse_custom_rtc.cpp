/*
 * custom_rtc.cpp
 *
 * Copyright 2022 raja <raja@raja-IdeaPad-Gaming-3-15IMH05>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#include "evse_custom_rtc.h"
#include "evse_BLE_Commissioning.h"

/*******************/
// RTC Implementation

unsigned long last = 0;
void rtc_setup()
{

  Wire1.begin(23, 18);
  Serial.println();
  Serial.println("Autoprobing for a RTC...");
  if (rtc.autoprobe())
  {
    // Found something, hopefully a clock.
    Serial.print("Autoprobe found ");
    Serial.print(rtc.getDeviceName());
    Serial.print(" at 0x");
    Serial.println(rtc.getAddress(), HEX);
  }

  // Enable the battery backup. This happens by default on the DS1307
  // but needs to be enabled on the MCP7941x.
  rtc.enableBatteryBackup();

  // rtc.clearPowerFailFlag();

  // Ensure the oscillator is running.
  rtc.startClock();

  if (rtc.getDevice() == RTCx::MCP7941x)
  {
    Serial.println(rtc.getCalibration(), DEC);
    // rtc.setCalibration(-127);
  }

  rtc.setSQW(RTCx::freq4096Hz);
}

struct RTCx::tm tm;

RTCx::time_t rtc_timestamp()
{

  last = millis();
  rtc.readClock(tm);

  RTCx::printIsotime(Serial, tm).println();
  RTCx::time_t t = RTCx::mktime(&tm);

  Serial.print("unixtime = ");
  Serial.println(t);
  Serial.println("-----");

  return t;
}

void getTime(char *datestring)
{
  rtc.readClock(tm);

  // sprintf(datestring, "%04d-%02d-%02dT%02d:%02d:%02d.000Z", tm.tm_year + 1900,
  //     tm.tm_mon + 1,
  //     tm.tm_hour,
  //     tm.tm_min,
  //     tm.tm_sec);
  //     tm.tm_mday,
  // WORKS for 20
  //  sprintf(datestring, "%02d%02d%02d%02d%02d%02d", tm.tm_mday,tm.tm_mon + 1, ((tm.tm_year + 1900)-2000),tm.tm_hour,
  //   tm.tm_min,
  //   tm.tm_sec);
  // sprintf(datestring, "%02d-%02d-%04d-%02d-%02d-%02d", tm.tm_mday,tm.tm_mon + 1, ((tm.tm_year + 1900)),tm.tm_hour,
  // tm.tm_min,
  // tm.tm_sec);
  Serial.print("**********************************************************************:::");
  Serial.println(datestring);

  for (int i = 0; i < 6; i++)
  {
    switch (i)
    {
    case 0:
      datestring[i] = tm.tm_mday;
      break;
    case 1:
      datestring[i] = tm.tm_mon + 1;
      break;
    case 2:
      datestring[i] = ((tm.tm_year + 1900) - 2000);
      break;
    case 3:
      datestring[i] = tm.tm_hour;
      break;
    case 4:
      datestring[i] = tm.tm_min;
      break;
    case 5:
      datestring[i] = tm.tm_sec;
      break;
    default:
      break;
    }
  }

  Serial.print("***Debug Date***");
  for (int i = 0; i < 6; i++)
  {
    Serial.println(datestring[i], DEC);
  }
}

int getDate()
{
  rtc.readClock(tm);
  return tm.tm_mday;
}

int getMonth()
{
  rtc.readClock(tm);
  return tm.tm_mon + 1;
}

int getYear()
{
  rtc.readClock(tm);
  return (tm.tm_year + 1900);
}
