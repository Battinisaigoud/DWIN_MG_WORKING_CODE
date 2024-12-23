

#include "Variants.h"

#include "TriggerMessage.h"
#include "SimpleOcppOperationFactory.h"
#include "OcppEngine.h"


uint8_t flag_trigger_meter_values = false;
uint8_t flag_trigger_statusnotification = false;
uint8_t flag_trigger_bootnotification = false;
uint8_t flag_trigger_heartbeat = false;
uint8_t flag_trigger_firmwarestatusnotification = false;
uint8_t flag_trigger_diagnosticsstatusnotification = false;


// TriggerMessage Trigger_Message;

TriggerMessage::TriggerMessage(WebSocketsClient *webSocket) : webSocket(webSocket) {
  statusMessage = "NotImplemented"; //default value if anything goes wrong
}

const char* TriggerMessage::getOcppOperationType(){
    return "TriggerMessage";
}

void TriggerMessage::processReq(JsonObject payload) {

  // Serial.print(F("[TriggerMessage] Warning: TriggerMessage is not tested!\n"));
  Serial.print("[TriggerMessage] Warning: TriggerMessage is testing Now !\n");
#if 0
/*
  * @brief : Check if meter values were requested from trigger message. If yes, then set a flag
  */
  if (!strcmp(messageType, "MeterValues"))
  {
    flag_trigger_meter_values = true;
  }

    /*
  * @brief : Check if StatusNotification were requested from trigger message. If yes, then set a flag
  */
  else if (!strcmp(messageType, "StatusNotification"))
  {
    flag_trigger_statusnotification = true;
  }
  
 /*
  * @brief : Check if BootNotification were requested from trigger message. If yes, then set a flag
  */
  else if (!strcmp(messageType, "BootNotification"))
  {
    flag_trigger_bootnotification = true;
  }
  
 /*
  * @brief : Check if Heartbeat were requested from trigger message. If yes, then set a flag
  */
  else if (!strcmp(messageType, "Heartbeat"))
  {
    flag_trigger_heartbeat = true;
  }
 /*
  * @brief : Check if FirmwareStatusNotification were requested from trigger message. If yes, then set a flag
  */
  else if (!strcmp(messageType, "FirmwareStatusNotification"))
  {
    flag_trigger_firmwarestatusnotification = true;
  }

 /*
  * @brief : Check if FirmwareStatusNotification were requested from trigger message. If yes, then set a flag
  */ 
  else if (!strcmp(messageType, "DiagnosticsStatusNotification"))
  {
    flag_trigger_diagnosticsstatusnotification = true;
  }
  if (DEBUG_OUT) {
    Serial.print("[SimpleOcppOperationFactory] makeFromTriggerMessage for message type ");
    Serial.print(messageType);
    Serial.print("\n");
  }
  #endif

#if 1
  triggeredOperation = makeFromTriggerMessage(webSocket, payload);
  if (triggeredOperation != NULL) {
	initiateOcppOperation(triggeredOperation);
    statusMessage = "Accepted";
    Serial.print("[TriggerMessage] TriggerMessage is to be create conf.\n");
  } else {
    Serial.print("[TriggerMessage] Couldn't make OppOperation from TriggerMessage. Ignore request.\n");
    statusMessage = "NotImplemented";
  }
#endif

  statusMessage = "Accepted";
  

}

DynamicJsonDocument* TriggerMessage::createConf(){
  DynamicJsonDocument* doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1) + strlen(statusMessage));
  JsonObject payload = doc->to<JsonObject>();
  
  payload["status"] = statusMessage;

  //Serial.print(F("[TriggerMessage] Triggering messages is not implemented!\n"));

  return doc;
}
