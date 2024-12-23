#include "evse_contacter_relay.h"
/*******************************************************************************
 *
 * program : evse_gfci.cpp
 * author  : rakesh
 *
 *
 * Description :
 *
 *
 *******************************************************************************/
RELAY evse_relay;
/*
 * @brief: evse_relay_init(void)
 * This function is used to initialtion of the relay.
 */

void RELAY::evse_relay_init(void)
{
  pinMode(RELAY_PIN, OUTPUT);
}
void RELAY::evse_relay2_init(void)
{
  pinMode(RELAY2_PIN, OUTPUT);
}
/*
*@brief: evse_relay_on


*/

void RELAY::evse_relay_on(void)
{
  digitalWrite(RELAY_PIN, HIGH);
  //  Serial.println("Relay_ON");
}
void RELAY::evse_relay2_on(void)
{
  digitalWrite(RELAY2_PIN, HIGH);
  //  Serial.println("Relay_ON");
}

/*
*@brief: evse_relay_on


*/
void RELAY::evse_relay_off(void)
{
  digitalWrite(RELAY_PIN, LOW);
  //  Serial.println("Relay_OFF");
}
void RELAY::evse_relay2_off(void)
{
  digitalWrite(RELAY2_PIN, LOW);
  //  Serial.println("Relay_OFF");
}