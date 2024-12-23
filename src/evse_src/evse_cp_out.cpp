/*******************************************************************************
 *
 * program : evse_gfci.cpp
 * author  : rakesh & pream
 *
 *
 * Description :
 *
 *
 *******************************************************************************/
#include "evse_cp_out.h"

CP_OUT evse_cp_out;

void CP_OUT::set_cp_init()
{

  ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(PWM_PIN, PWM_CHANNEL);
}

void CP_OUT::set_cp_begin(uint8_t chargingLimit)
{
 uint8_t charging_current_limit = chargingLimit;
  int dutycycle = CalculateDutyCycle(charging_current_limit);

  // uint8_t per = map(PWM_DUTY, 0, 100, 0, 255);
  uint8_t per = map(dutycycle, 0, 100, 0, 255);
    Serial.println("[ControlPilot] Mapped value for 8-Bit resolution = " + String(per));
  ledcWrite(PWM_CHANNEL, per);

}
  
void CP_OUT::set_cp_on()
{

  ledcWrite(PWM_CHANNEL, 255);
}
void CP_OUT::set_cp_off()

{

  ledcWrite(PWM_CHANNEL, 0);
}

int CP_OUT::CalculateDutyCycle(float chargingLimit)

{

  int dutycycle_l = 0;

  if ((chargingLimit <= 51) && (chargingLimit > 5))
  {

    // dutycycle_l = ((chargingLimit / 0.6) * 2.55); // duty cycle  and mapping it's duty cycle percentage to PWM resolution of 255 calculated here
    dutycycle_l = ((chargingLimit / 0.6)); // only duty cycle calculated here
    Serial.println("[ControlPilot] Duty Cycle is = " + String(dutycycle_l));
  }
  else if ((chargingLimit < 80) && (chargingLimit > 51))
  {

    // dutycycle_l = (((chargingLimit / 2.5) + 64 ) * 2.55 );// duty cycle  and mapping it's duty cycle percentage to PWM resolution of 255 calculated here
    dutycycle_l = (((chargingLimit / 2.5) + 64)); // only duty cycle calculated here
    Serial.println("[ControlPilot] Duty Cycle is = " + String(dutycycle_l));
  }
  else
  {

    Serial.println("[ControlPilot] chargingLimit is not in range");
  }

  return dutycycle_l;
}

int CP_OUT::CalculateDutyCycle_32Amp(void)
{
  float chargingLimit = CHARGE_CURRENT_IN_AMPS;
  int dutycycle_l = 0;

  if ((chargingLimit <= 51) && (chargingLimit > 5))
  {

    // dutycycle_l = ((chargingLimit / 0.6) * 2.55); // duty cycle  and mapping it's duty cycle percentage to PWM resolution of 255 calculated here
    dutycycle_l = ((chargingLimit / 0.6)); // only duty cycle calculated here
    Serial.println("[ControlPilot] Duty Cycle is = " + String(dutycycle_l));
  }
  else if ((chargingLimit < 80) && (chargingLimit > 51))
  {

    // dutycycle_l = (((chargingLimit / 2.5) + 64 ) * 2.55 );// duty cycle  and mapping it's duty cycle percentage to PWM resolution of 255 calculated here
    dutycycle_l = (((chargingLimit / 2.5) + 64)); // only duty cycle calculated here
    Serial.println("[ControlPilot] Duty Cycle is = " + String(dutycycle_l));
  }
  else
  {

    Serial.println("[ControlPilot] chargingLimit is not in range");
  }

  return dutycycle_l;
}

void CP_OUT::set_cp_begin_Amp(int ls32_dutycycle)
{
  // uint8_t per = map(PWM_DUTY, 0, 100, 0, 255);
  uint8_t per = map(ls32_dutycycle, 0, 100, 0, 255);
  Serial.println("[ControlPilot] Duty Cycle is Mapped value for 8-Bit resolution = " + String(per));
  ledcWrite(PWM_CHANNEL, per);
  Serial.println(F("[ControlPilot] Duty Cycle setting"));
}