/*******************************************************************************
 *
 * program : evse_gfci.cpp
 * author  : krishna & rakesh
 *
 *
 *
 *******************************************************************************/
#include "evse_earth_dis.h"
#include "evse_config.h"
#include "evse_addr_led.h"
#include "../evse_lib/src/OCPP/ChargePointStatusService.h"
#include "evse_contacter_relay.h"
#include "evse_cp_out.h"

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

volatile uint8_t gu8_earth_discontion_count = 0;
uint8_t gu8_other_type_error_power_loss;

uint8_t gu8_earth_disconnect_threshold = 100;

extern uint8_t boot_flag_accepted;
// boot_flag_accepted = 0xff;

EARTH earth_obj;

extern enum EvseDevStatuse EvseDevStatus_connector_1;

void EARTH ::earth_init()
{

  analogReadResolution(12);
}

uint32_t EARTH ::earth_read()
{

  uint32_t analogValue = analogRead(EARTH_PIN);
  // Serial.println(analogValue);
  // Serial.printf(" Earth dis ADC analog value = %d\n", analogValue);

  return analogValue;
}