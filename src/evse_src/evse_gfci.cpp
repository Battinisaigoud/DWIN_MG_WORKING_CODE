/*******************************************************************************
 *
 * program : evse_gfci.cpp
 * author  : krishna & rakesh
 *
 *
 *
 *******************************************************************************/

#include "evse_gfci.h"
#include "evse_config.h"
#include "evse_addr_led.h"
#include "../evse_lib/src/OCPP/ChargePointStatusService.h"
#include "evse_cp_out.h"
#include "evse_contacter_relay.h"

extern ChargePointStatus evse_ChargePointStatus;
extern enum EvseChargePointErrorCode EvseChargePoint_ErrorCode;
extern enum EvseChargePointErrorCode_OtherError_t EvseChargePointErrorCode_OtherError;
extern EVSE_states_enum EVSE_states;
extern uint8_t reasonForStop;

extern RELAY evse_relay;
extern CP_OUT evse_cp_out;
extern CP_OUT *ptr_cp_out;
extern RELAY *ptr_evse_relay;

extern uint8_t gu8_online_flag;
extern uint8_t boot_flag_accepted;
//  boot_flag_accepted = 0xff;

GFCI gfci_object;

extern enum EvseDevStatuse EvseDevStatus_connector_1;
// int read_gfci ;

void GFCI ::gfci_init()

{
  // analogReadResolution(12);
  pinMode(GFCI_PIN, INPUT);
}

uint8_t GFCI ::gfci_read()
// bool GFCI ::gfci_read()
{
  // bool stat = digitalRead(GFCI_PIN);
  // uint8_t stat = digitalRead(GFCI_PIN);
  bool read_gfci = digitalRead(GFCI_PIN);

  
  // Serial.printf(" GFCI  value = %d\n", read_gfci);
  return read_gfci;
}
