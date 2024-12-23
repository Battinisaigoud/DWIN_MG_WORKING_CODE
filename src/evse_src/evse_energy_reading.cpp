/*******************************************************************************
 *
 * program : evse_gfci.cpp
 * author  : krishna & rakesh
 *
 *
 *
 *******************************************************************************/

#include "evse_energy_reading.h"
#include "../evse_lib/src/ATM90E36/ATM90E36.h"

// ENERGY1 evse_energy;
ATM90E36 eic(5);
// ATM90E36 eic(19); //ver2

extern volatile double ipc_voltage_B;
extern volatile double ipc_voltage_A;
extern volatile double ipc_voltage_C;
extern volatile double ipc_current_A;
extern volatile double ipc_current_B;
extern volatile double ipc_current_C;
extern volatile double ipc_temp;

extern portMUX_TYPE voltage_A_mux;
extern portMUX_TYPE voltage_B_mux;
extern portMUX_TYPE voltage_C_mux;
extern portMUX_TYPE current_A_mux;
extern portMUX_TYPE current_B_mux;
extern portMUX_TYPE current_C_mux;

void ENERGY1::energy_meter_init(void)
{
  eic.begin();
}

void ENERGY1::energy_meter_voltageA(void)
{

  vTaskDelay(2000 / portTICK_PERIOD_MS);
  double voltageA = eic.GetLineVoltageA();
  voltageA = ((voltageA * 0.989649057) + 0.315538775); // new calbiration for testing
  ipc_voltage_A = voltageA;
  Serial.printf("VA : %.1f V \r\n", ipc_voltage_A);
}
#if THREE_PHASE
void ENERGY1::energy_meter_voltageB(void)
{
  vTaskDelay(500 / portTICK_PERIOD_MS);
  double voltageB = eic.GetLineVoltageB();
  voltageB = ((voltageB * 0.989649057) + 0.315538775);
  ipc_voltage_B = voltageB;
  Serial.printf("VB : %.1f V \r\n", ipc_voltage_B);
}

void ENERGY1::energy_meter_voltageC(void)
{
  vTaskDelay(500 / portTICK_PERIOD_MS);
  double voltageC = eic.GetLineVoltageC();
  voltageC = ((voltageC * 0.989649057) + 0.315538775);
  ipc_voltage_C = voltageC;
  Serial.printf("VC : %.1f V \r\n", ipc_voltage_C);
}
#endif

void ENERGY1::energy_meter_currentA(void)
{

#if 1
  vTaskDelay(2000 / portTICK_PERIOD_MS);
#else
  delay(10);
#endif
  double currentA = eic.GetLineCurrentA();

  ipc_current_A = currentA;

#if 1

  double currA;
  if (ipc_current_A > 8.5)
  {
    currA = ipc_current_A * 2.21;
    // currA = (currA * 0.967836) - 0.02957;
    currA = (currA * 0.98042) - 0.02957;
#if 0
					 currA = (currA * 0.981468587) - 0.000988791;
#endif
    if (currA < 0)
    {
      currA = 0;
    }
  }
  else if ((ipc_current_A > 0) && (ipc_current_A <= 8.5))
  {
    currA = ipc_current_A * 2.202;
    // currA = (currA * 0.967836) - 0.02957;
    currA = (currA * 0.98042) - 0.02957;
#if 0
					 currA = (currA * 0.981468587) - 0.000988791;
#endif
    if (currA < 0)
    {
      currA = 0;
    }
  }
  ipc_current_A = currA;

  Serial.printf("IA : %.2f A \r\n", ipc_current_A);
#endif
}
#if THREE_PHASE
void ENERGY1::energy_meter_currentB(void)
{
  vTaskDelay(500 / portTICK_PERIOD_MS);
  double currentB = eic.GetLineCurrentB();

  ipc_current_B = currentB;

#if 1

  double currB;
  if (ipc_current_A > 8.5)
  {
    currB = ipc_current_A * 2.21;
    currB = (currB * 0.967836) - 0.02957;
#if 0
					 currB = (currB * 0.981468587) - 0.000988791;
#endif
    if (currB < 0)
    {
      currB = 0;
    }
  }
  else if ((ipc_current_A > 0) && (ipc_current_A <= 8.5))
  {
    currB = ipc_current_A * 2.202;
    currB = (currB * 0.967836) - 0.02957;
#if 0
					 currB = (currB * 0.981468587) - 0.000988791;
#endif
    if (currB < 0)
    {
      currB = 0;
    }
  }
  ipc_current_B = currB;

  Serial.printf("IB : %.2f A \r\n", ipc_current_B);
#endif
}

void ENERGY1::energy_meter_currentC(void)
{
  vTaskDelay(500 / portTICK_PERIOD_MS);
  double currentC = eic.GetLineCurrentC();

  ipc_current_C = currentC;

#if 1

  double currC;
  if (ipc_current_A > 8.5)
  {
    currC = ipc_current_A * 2.21;
    currC = (currC * 0.967836) - 0.02957;
#if 0
					 currC = (currC * 0.981468587) - 0.000988791;
#endif
    if (currC < 0)
    {
      currC = 0;
    }
  }
  else if ((ipc_current_A > 0) && (ipc_current_A <= 8.5))
  {
    currC = ipc_current_A * 2.202;
    currC = (currC * 0.967836) - 0.02957;
#if 0
					 currC = (currC * 0.981468587) - 0.000988791;
#endif
    if (currC < 0)
    {
      currC = 0;
    }
  }
  ipc_current_C = currC;

  Serial.printf("IC : %.2f A \r\n", ipc_current_C);
#endif
}
#endif

void ENERGY1::energy_meter_temp(void)
{
  vTaskDelay(500 / portTICK_PERIOD_MS);
  double temp = eic.GetTemperature();
  ipc_temp = temp;
  // Serial.println(" Temp" + String(temp));
}

#if 0
  void ENERGY1:: energy_meter_frq()
  {
   double freq=eic.GetFrequency();
   Serial.println("f"+String(freq)+"Hz");
     vTaskDelay(500 / portTICK_PERIOD_MS );
    
  }

  void ENERGY1:: energy_meter_pf()
  {

  }

  void ENERGY1:: energy_meter_power()
  {
  }
#endif
