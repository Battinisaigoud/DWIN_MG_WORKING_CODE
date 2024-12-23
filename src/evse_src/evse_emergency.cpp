/*******************************************************************************
 *
 * program : evse_gfci.cpp
 * author  : krishna & rakesh
 *
 *
 * Description :
 *
 *
 *******************************************************************************/

#include "evse_emergency.h"
#include "evse_config.h"
#include "evse_addr_led.h"
#include "evse_cp_out.h"
#include "evse_contacter_relay.h"
#include "../evse_lib/src/OCPP/ChargePointStatusService.h"

EMGY emgy_obj;
extern RELAY evse_relay;
extern CP_OUT evse_cp_out;
extern CP_OUT *ptr_cp_out;
extern RELAY *ptr_evse_relay;

extern ChargePointStatus evse_ChargePointStatus;
extern enum EvseChargePointErrorCode EvseChargePoint_ErrorCode;
extern enum EvseChargePointErrorCode_OtherError_t EvseChargePointErrorCode_OtherError;
extern EVSE_states_enum EVSE_states;
extern uint8_t reasonForStop;
extern uint8_t gu8_online_flag; // Connectivity Avaliable or not, can be indicated by gu8_online_flag

uint8_t boot_flag_accepted = 0xff;
//  boot_flag_accepted = 0xff;

extern enum EvseDevStatuse EvseDevStatus_connector_1; 
void EMGY ::emergency_init()
{

  pinMode(EMGY_PIN, INPUT);
}

// void EMGY ::emergency_read()
uint32_t EMGY ::emergency_read()
{

  uint32_t analogemergncy = analogRead(EMGY_PIN);
  // bool stat = digitalRead(GFCI_PIN);
  // Serial.printf(" EMGY ADC analog value = %d\n", analogemergncy);

  return analogemergncy;
}
