/*******************************************************************************
 *
 * program : evse_gfci.cpp
 * author  :rakesh & pream
 *
 *
 * Description :
 *
 *
 *******************************************************************************/


#include "evse_cp_in.h"
#include <Arduino.h>

void CP_IN::read_cp_init(void)
{

  analogReadResolution(12);
}

int CP_IN::read_cp_in(void)
{
#if 1
  int ADC_Result = 0;
  for (uint16_t index = 0; index < CP_AVR_VALUE; index++)
  {

    ADC_Result += analogRead(CP_IN_PIN);
    delayMicroseconds(10); // pauses for 10 microseconds
  }
#endif

  //  return analogRead(CP_IN_PIN);
#if 1
  return (ADC_Result / CP_AVR_VALUE);
#endif
}
