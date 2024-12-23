

#include "Variants.h"

#include "StartTransaction.h"
#include "TimeHelper.h"
#include "OcppEngine.h"
#include "MeteringService.h"

#include "../../../evse_src/evse_config.h"

extern enum EvseDevStatuse EvseDevStatus_connector_1;
extern enum evse_start_txn_stat_t evse_start_txn_state; /* Start Transaction State machine */

bool flag_start_accepted = false;

int globalmeterstartA = 0;
// unsigned long st_timeA = 0;
unsigned long evse_st_time = 0;

extern int evse_transactionId;
extern float evse_globalmeterstart;
extern volatile unsigned long int gs32_st_time;
StartTransaction Start_Transaction;

StartTransaction::StartTransaction()
{
	if (getChargePointStatusService() != NULL)
	{
		if (!getChargePointStatusService()->getIdTag().isEmpty())
		{
			idTag = String(getChargePointStatusService()->getIdTag());
			connectorId = getChargePointStatusService()->getConnectorId();
		}
	}
	// if (idTag.isEmpty()) idTag = String("wrongIDTag"); //Use a default payload. In the typical use case of this library, you probably you don't even need Authorization at all
}

StartTransaction::StartTransaction(String &idTag)
{
	this->idTag = String(idTag);
}

StartTransaction::StartTransaction(String &idTag, int &connectorId)
{
	this->idTag = String(idTag);
	this->connectorId = connectorId;
}

const char *StartTransaction::getOcppOperationType()
{
	return "StartTransaction";
}

DynamicJsonDocument *StartTransaction::createReq()
{

	/*if(getChargePointStatusService()->inferenceStatus() != ChargePointStatus::Available )
	{
		accepted = false;
		return;
	}*/

	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(5) + (JSONDATE_LENGTH + 1) + (idTag.length() + 1));
	JsonObject payload = doc->to<JsonObject>();

	payload["connectorId"] = connectorId;
	MeteringService *meteringService = getMeteringService();
	if (meteringService != NULL)
	{
		payload["meterStart"] = (int)meteringService->currentEnergy();
	}
	evse_globalmeterstart = payload["meterStart"];
	// st_timeA = millis();
	evse_st_time = millis();
	gs32_st_time = millis();
	char timestamp[JSONDATE_LENGTH + 1] = {'\0'};
	getJsonDateStringFromGivenTime(timestamp, JSONDATE_LENGTH + 1, now());
	payload["timestamp"] = timestamp;
	payload["idTag"] = idTag;

	return doc;
}

void StartTransaction::processConf(JsonObject payload)
{

	const char *idTagInfoStatus = payload["idTagInfo"]["status"] | "Invalid";
	int transactionId = payload["transactionId"] | -1;
	evse_transactionId = transactionId;
	

	if (!strcmp(idTagInfoStatus, "Accepted") || !strcmp(idTagInfoStatus, "ConcurrentTx"))
	{
		if (DEBUG_OUT)
			Serial.print("[StartTransaction] Request has been accepted!\n");
		flag_start_accepted = true;
		// EvseDevStatus_connector_1 = flag_EVSE_Request_Charge;   // Commented @ 31012024_174000hr
		evse_start_txn_state = EVSE_START_TXN_ACCEPTED;
		// evse_ChargePointStatus = Charging;
		ChargePointStatusService *cpStatusService = getChargePointStatusService();
		if (cpStatusService != NULL)
		{
			cpStatusService->startTransaction(transactionId);
			cpStatusService->startEnergyOffer();
		}

		SmartChargingService *scService = getSmartChargingService();
		if (scService != NULL)
		{
			scService->beginChargingNow();
		}
		// EvseDevStatus_connector_1 = flag_EVSE_Request_Charge;
		// evse_ChargePointStatus = Charging;
	}
	else
	{
		Serial.print(F("[StartTransaction] Request has been denied!\n"));
	}
}

void StartTransaction::processReq(JsonObject payload)
{

	/**
	 * Ignore Contents of this Req-message, because this is for debug purposes only
	 */
}

DynamicJsonDocument *StartTransaction::createConf()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2));
	JsonObject payload = doc->to<JsonObject>();

	JsonObject idTagInfo = payload.createNestedObject("idTagInfo");
	idTagInfo["status"] = "Accepted";
	payload["transactionId"] = 123456; // sample data for debug purpose

	return doc;
}
