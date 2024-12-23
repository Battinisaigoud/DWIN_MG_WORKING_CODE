

#include "Variants.h"

#include "RemoteStartTransaction.h"
#include "OcppEngine.h"
#include "ChargePointStatusService.h"

#include "../../../evse_src/evse_config.h"

extern bool flag_evseReadIdTag;
extern bool flag_evseAuthenticate;
extern bool flag_evseStartTransaction; // Entry condition for starting transaction.
extern bool flag_evRequestsCharge;
extern bool flag_evseStopTransaction;
extern bool flag_evseUnauthorise;
extern String currentIdTag;

bool reservation_start_flag;
extern String reserve_currentIdTag;
extern uint8_t reasonForStop_A;
extern bool flag_AuthorizeRemoteTxRequests;
extern uint8_t flag_rfid;
extern int evse_transactionId;
extern String evse_idTagData;

extern uint8_t gu8_evse_change_state;
extern enum EvseDevStatuse EvseDevStatus_connector_1;

extern EVSE_states_enum EVSE_states;

RemoteStartTransaction Remote_StartTransaction;

RemoteStartTransaction::RemoteStartTransaction()
{
}

const char *RemoteStartTransaction::getOcppOperationType()
{
	return "RemoteStartTransaction";
}

void RemoteStartTransaction::processReq(JsonObject payload)
{
	idTag = String(payload["idTag"].as<String>());
	// currentIdTag = idTag;
	evse_idTagData = idTag;
	connectorId = payload["connectorId"].as<int>();
	// if reserved
	// if strcmp of idTag and reservedidTag is 0
	//  set accepted = true
	// else accepted = false
	// else directly authoizie
	Serial.println(F("[RemoteStartTransaction] processReq 1"));
	if ((getChargePointStatusService()->inferenceStatus() != ChargePointStatus::Available) &&
		(getChargePointStatusService()->inferenceStatus() != ChargePointStatus::Preparing))
	{
		Serial.println(F("[RemoteStartTransaction] processReq 2"));
		if (getChargePointStatusService()->inferenceStatus() == ChargePointStatus::Reserved)
		{
			Serial.println(F("[RemoteStartTransaction] Trying remote start in reserved state"));
			Serial.println(F("[RemoteStartTransaction] processReq 3"));
		}
		else
		{
			Serial.println(F("[RemoteStartTransaction] processReq 4"));
			accepted = false;
			return;
		}
	}
	Serial.println(F("[RemoteStartTransaction] processReq 5"));
	if (reservation_start_flag)
	{
		
		Serial.println(F("[RemoteStartTransaction] processReq 6"));
		// if (currentIdTag.equals(reserve_currentIdTag) == true)
		if (evse_idTagData.equals(reserve_currentIdTag) == true)
		{
			Serial.println(F("[RemoteStartTransaction] processReq 7"));
			accepted = true;
			getChargePointStatusService()->authorize(idTag, connectorId);

			/*
			 * @brief : Change made by G. Raja Sumant for removing reservation.
			 */
			getChargePointStatusService()->setReserved(false);
		}
		else
		{
			Serial.println(F("[RemoteStartTransaction] processReq 8"));
			accepted = false;
			Serial.println(F("[RemoteStartTransaction] Reserved and failed due to mismatch of tag"));
		}
	}
	else
	{
		Serial.println("[RemoteStartTransaction] processReq 9");

		// accepted = true;
		// Add condition by checking if available or unavailable
		bool un = false;
		// if(getChargePointStatusService()->inferenceStatus() == ChargePointStatus::Preparing)
		if ((getChargePointStatusService()->inferenceStatus() == ChargePointStatus::Available) ||
			(getChargePointStatusService()->inferenceStatus() == ChargePointStatus::Preparing))
		{
			un = getChargePointStatusService()->getUnavailable();
			if (!un)
			{
				// if(EVSE_states == STATE_B)
				if ((EVSE_states == STATE_A) || (EVSE_states == STATE_B))
				{
					accepted = true;
					Serial.println(F("[RemoteStartTransaction] processReq 10"));
					getChargePointStatusService()->authorize(idTag, connectorId);
					getChargePointStatusService()->setReserved(false);
				}
				else
				{
					accepted = false;
				}
			}
		}
		else
		{
			Serial.println(F("[RemoteStartTransaction] processReq 11"));
			accepted = false;
		}
	}
}
DynamicJsonDocument *RemoteStartTransaction::createConf()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();
	if (accepted)
	{

		switch (EvseDevStatus_connector_1)
		{
		case flag_EVSE_Read_Id_Tag:
			EvseDevStatus_connector_1 = flag_EVSE_Start_Transaction;
			evse_start_txn_state = EVSE_START_TXN_INITIATED;
			gu8_evse_change_state = EVSE_START_TXN;
			flag_rfid = 1;
			Serial.println("Remote Start- CMS Accepted");
			payload["status"] = "Accepted";
			break;
		default:
			Serial.println("default - Remote Start-");
			break;
		}
	}
	else
	{
		payload["status"] = "Rejected";
	}
	// accepted = false;

	return doc;
}

DynamicJsonDocument *RemoteStartTransaction::createReq()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();

	payload["idTag"] = "fefed1d19876";
	payload["connectorId"] = 1;

	return doc;
}

void RemoteStartTransaction::processConf(JsonObject payload)
{
	String status = payload["status"] | "Invalid";

	if (status.equals("Accepted"))
	{
		if (DEBUG_OUT)
			Serial.print(F("[RemoteStartTransaction] Request has been accepted!\n"));
	}
	else
	{
		Serial.print(F("[RemoteStartTransaction] Request has been denied!"));
	}
}
