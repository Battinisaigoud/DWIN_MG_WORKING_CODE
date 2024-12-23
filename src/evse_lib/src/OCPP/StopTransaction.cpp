

#include "Variants.h"

#include "StopTransaction.h"
#include "OcppEngine.h"
#include "OcppMessage.h"
#include "OcppOperation.h"
#include "SimpleOcppOperationFactory.h"
#include "MeteringService.h"

#include "ChargePointStatusService.h"
#include "StatusNotification.h"
#include "ReserveNow.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <Preferences.h>


#include "../../../evse_src/evse_config.h"

extern enum EvseDevStatuse EvseDevStatus_connector_1; 
extern enum evse_stop_txn_stat_t evse_stop_txn_state;                           /* Stop Transaction State machine */ 

extern bool reservation_start_flag;

short int counter_drawingCurrent;

extern Preferences energymeter;

extern int globalmeterstartA;
extern unsigned long st_timeA;



extern bool disp_evse_A;
unsigned long stop_time = 0;
/*
* @brief: Feature added by Raja
* This feature will avoid hardcoding of messages. 
*/
//typedef enum resonofstop { EmergencyStop, EVDisconnected , HardReset, Local , Other , PowerLoss, Reboot,Remote, Softreset,UnlockCommand,DeAuthorized};
extern uint8_t reasonForStop;
static const char *resonofstop_str[] = { "EmergencyStop", "EVDisconnected" , "HardReset", "Local" , "Other" , "PowerLoss", "Reboot","Remote", "SoftReset","UnlockCommand","DeAuthorized"};

//extern bool flag_GFCI_set_here;

extern bool flag_start_accepted;

extern bool flag_stop_finishing;

extern WebSocketsClient webSocket;

StopTransaction Stop_Transaction;

StopTransaction::StopTransaction() {

}

const char* StopTransaction::getOcppOperationType(){
    return "StopTransaction";
}

DynamicJsonDocument* StopTransaction::createReq() {

  String idTag = String('\0');

  if (getChargePointStatusService() != NULL) {
    idTag += getChargePointStatusService()->getIdTag();
  }

  DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(5) + (idTag.length() + 1) + (JSONDATE_LENGTH + 1));
  JsonObject payload = doc->to<JsonObject>();
  
  if (!idTag.isEmpty()) { //if there is no idTag present, we shouldn't add a default one
    payload["idTag"] = idTag;
  }
  
  float meterStop = 0.0f;

#if 1
  MeteringService *meteringService = getMeteringService();
  // if (getMeteringService() != NULL) {

  if (meteringService != NULL)
  {
    meterStop = (int)meteringService->currentEnergy();
    if (meterStop > (200000000 - 100000))
    {
      energymeter.begin("MeterData", false);
      energymeter.putFloat("currEnergy_A", 0);
      energymeter.end();
      // Serial.println("[Metering init] Reinitialized currEnergy_A");
    }
    // meterStop =(int) getMeteringService()->currentEnergy();
  }
#endif

  payload["meterStop"] = (int) meterStop; //TODO meterStart is required to be in Wh, but measuring unit is probably inconsistent in implementation
  char timestamp[JSONDATE_LENGTH + 1] = {'\0'};
  getJsonDateStringFromSystemTime(timestamp, JSONDATE_LENGTH);
  payload["timestamp"] = timestamp;

  int transactionId = -1;
  if (getChargePointStatusService() != NULL) {
    transactionId = getChargePointStatusService()->getTransactionId();
  }
  payload["transactionId"] = transactionId;
 payload["reason"] = resonofstop_str[reasonForStop];


  if (getChargePointStatusService() != NULL) {
    getChargePointStatusService()->stopEnergyOffer();
  }
flag_start_accepted = false;

  return doc;
}

void StopTransaction::processConf(JsonObject payload) {

  //no need to process anything here

  ChargePointStatusService *cpStatusService = getChargePointStatusService();
  if (cpStatusService != NULL){
    //cpStatusService->stopEnergyOffer(); //No. This should remain in createReq
    cpStatusService->stopTransaction();
    cpStatusService->unauthorize();
  }

  SmartChargingService *scService = getSmartChargingService();
  if (scService != NULL){
    scService->endChargingNow();
  }

  if (DEBUG_OUT) Serial.print("[StopTransaction] Request has been accepted!\n");

  
  reasonForStop = 3;
  counter_drawingCurrent = 0;
 
  flag_stop_finishing = false;
  evse_stop_txn_state = EVSE_STOP_TXN_ACCEPTED;
  //  EvseDevStatus_connector_1 = flag_EVSE_UnAutharized;
  // evse_ChargePointStatus = Finishing;
/*
 * @brief : Feature added by G. Raja Sumant 29/07/2022
 * This will take the charge point to reserved state when ever it is available during the reservation loop
 */
#if 1
  if (reservation_start_flag)
  {
    getChargePointStatusService()->setReserved(true);
  }
#endif
}

void StopTransaction::processReq(JsonObject payload) {
  /**
   * Ignore Contents of this Req-message, because this is for debug purposes only
   */
}

DynamicJsonDocument* StopTransaction::createConf(){
  DynamicJsonDocument *doc = new DynamicJsonDocument(2 * JSON_OBJECT_SIZE(1));
  JsonObject payload = doc->to<JsonObject>();

  JsonObject idTagInfo = payload.createNestedObject("idTagInfo");
  idTagInfo["status"] = "Accepted";

  return doc;
}
