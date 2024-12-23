

#include "Variants.h"

#include "RemoteStopTransaction.h"
#include "OcppEngine.h"
#include "OcppMessage.h"
#include "OcppOperation.h"
#include "SimpleOcppOperationFactory.h"

#include "ChargePointStatusService.h"
#include "StatusNotification.h"

#include "../../../evse_src/evse_config.h"

extern WebSocketsClient webSocket;
extern bool flag_evseIsBooted;
bool flag_evseReadIdTag;
bool flag_evseAuthenticate;
bool flag_evseStartTransaction;
bool flag_evRequestsCharge;
bool flag_evseStopTransaction;
bool flag_evseUnauthorise;


uint8_t reasonForStop;

bool disp_evse_A;

extern uint8_t gu8_evse_change_state;
extern enum EvseDevStatuse EvseDevStatus_connector_1;

RemoteStopTransaction Remote_StopTransaction;

RemoteStopTransaction::RemoteStopTransaction()
{
}

const char *RemoteStopTransaction::getOcppOperationType()
{
	return "RemoteStopTransaction";
}

// sample message: [2,"9f639cdf-8a81-406c-a77e-60dff3cb93eb","RemoteStopTransaction",{"transactionId":2042}]
void RemoteStopTransaction::processReq(JsonObject payload)
{
	transactionId = payload["transactionId"];

	if (transactionId == getChargePointStatusService()->getTransactionId())
	{

		switch (EvseDevStatus_connector_1)
		{
		case flag_EVSE_Request_Charge:
			//       evse_stop_txn_state= EVSE_STOP_TXN_INITIATED;
			//    gu8_evse_change_state = EVSE_STOP_TXN;
			//    EvseDevStatus_connector_1 = flag_EVSE_Stop_Transaction;
			Serial.println("Remote Stop- CMS Accepted");
			payload["status"] = "Accepted";
			EvseDevStatus_connector_1 = flag_EVSE_Stop_Transaction;
			evse_stop_txn_state = EVSE_STOP_TXN_INITIATED;
			gu8_evse_change_state = EVSE_STOP_TXN;
			
			break;
		default:
			Serial.println("default - Remote Stop-");
			break;
		}

		reasonForStop = 7; // Remote
		// getChargePointStatusService()->stopTransaction();
		accepted = true;
	}
	else
	{
		if (DEBUG_OUT)
			Serial.println(F("[RemoteStopTransaction] TXN id does not match!\n"));
		if (DEBUG_OUT)
			Serial.println(transactionId);
		if (DEBUG_OUT)
			Serial.println(getChargePointStatusService()->getTransactionId());
		accepted = false;
	}
}

DynamicJsonDocument *RemoteStopTransaction::createConf()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();
	if (accepted)
		payload["status"] = "Accepted";
	else
		payload["status"] = "Rejected";
	return doc;
}

DynamicJsonDocument *RemoteStopTransaction::createReq()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();

	payload["idTag"] = "fefed1d19876";

	return doc;
}

void RemoteStopTransaction::processConf(JsonObject payload)
{
	String status = payload["status"] | "Invalid";

	if (status.equals("Accepted"))
	{
		if (DEBUG_OUT)
			Serial.print("[RemoteStopTransaction] Request has been accepted!\n");
	}
	else
	{
		Serial.print("[RemoteStopTransaction] Request has been denied!");
	}
}
