

#include "Authorize.h"
#include "OcppEngine.h"
#include "Variants.h"

#include "../../../evse_src/evse_config.h"

extern enum EvseDevStatuse EvseDevStatus_connector_1;
extern String ocpp_rfid;
extern enum evse_authorize_stat_t evse_authorize_state; /* Authorize State machine */
extern uint8_t flag_rfid;
// Authorize authorize;  this class Object is created on runtime.

Authorize::Authorize()
{
	idTag = String("defaultCPIDTag"); // Use a default payload. In the typical use case of this library, you probably you don't even need Authorization at all
}

Authorize::Authorize(String &idTag)
{
	this->idTag = String(idTag);
	//	EEPROM.begin(sizeof(EEPROM_Data));
	//	EEPROM.put(28,idTag);
	//	EEPROM.commit();
	//	EEPROM.end();
}

const char *Authorize::getOcppOperationType()
{
	return "Authorize";
}

DynamicJsonDocument *Authorize::createReq()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1) + (idTag.length() + 1));
	// send to Authorize only when Available.
	JsonObject payload = doc->to<JsonObject>();
	payload["idTag"] = String(idTag);
	// payload["idTag"] = ocpp_rfid;
	return doc;
	/*
	ChargePointStatus inferencedStatus = getChargePointStatusService()->inferenceStatus();
	if(inferencedStatus == ChargePointStatus::Available)
	{
	JsonObject payload = doc->to<JsonObject>();
	payload["idTag"] = idTag;
	return doc;
	}
	*/
}

void Authorize::processConf(JsonObject payload)
{
	String idTagInfo = payload["idTagInfo"]["status"] | "Invalid";

	if (idTagInfo.equals("Accepted"))
	{
		evse_authorize_state = EVSE_AUTHORIZE_ACCEPTED;
		if (DEBUG_OUT)
			Serial.print("[Authorize] Request has been accepted!\n");

		getChargePointStatusService()->setReserved(false);
		ChargePointStatusService *cpStatusService = getChargePointStatusService();
		if (cpStatusService != NULL)
		{
			cpStatusService->authorize();
		}
		// evse_ChargePointStatus = Preparing;
		// EvseDevStatus_connector_1 = flag_EVSE_Start_Transaction;
	}
	else
	{

		Serial.print("[Authorize] Request has been denied!");
		evse_authorize_state = EVSE_AUTHORIZE_REJECTED;
		EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;
		 flag_rfid = 0;
	}
}

void Authorize::processReq(JsonObject payload)
{
	/*
	 * Ignore Contents of this Req-message, because this is for debug purposes only
	 */
}

DynamicJsonDocument *Authorize::createConf()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(2 * JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();
	JsonObject idTagInfo = payload.createNestedObject("idTagInfo");
	idTagInfo["status"] = "Accepted";
	return doc;
}
