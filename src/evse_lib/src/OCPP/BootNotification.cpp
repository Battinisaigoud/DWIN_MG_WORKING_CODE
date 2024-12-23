
#include "Variants.h"

#include "../../../evse_src/evse_config.h"

#include "BootNotification.h"

#include "../../../evse_src/evse_cfg_file.h"

#include "ChargePointStatusService.h"

#include "OcppEngine.h"

#include <string.h>
#include "TimeHelper.h"

#include <nvs_flash.h>
#include <Preferences.h>

// #include "../Evse_Preferences/Evse_Preferences.h"

extern String cpSerial;
extern String csSerial;
extern String cscbSerial;
extern String cssnSerial;
extern String CP_Id_m;
//  unsigned int heartbeatInterval;
extern enum EvseDevStatuse EvseDevStatus_connector_1;

extern ocpp_config_keys_t gOCPP_config_keys_write;
extern OCPPconfigkeyStorage *ptrOCPPconfigKeys;

extern uint8_t gu8_evse_boot_flag;
extern enum evse_boot_stat_t evse_boot_state;

extern unsigned int heartbeatInterval;
extern ChargePointStatus evse_ChargePointStatus;

extern Preferences evse_preferences_ocpp;

// BootNotification bootNotification;  this class Object is created on runtime.

BootNotification::BootNotification()
{
}

const char *BootNotification::getOcppOperationType()
{
	return "BootNotification";
}

#if 0
DynamicJsonDocument *BootNotification::createReq()
{


	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(4) + csSerial.length() + 1 + cpSerial.length() + 1 + cscbSerial.length() + 25 + 1);
	JsonObject payload = doc->to<JsonObject>();
	payload["chargePointVendor"] = csSerial; 
	payload["chargePointSerialNumber"] = cpSerial;
	payload["chargePointModel"] = cscbSerial; 
	payload["firmwareVersion"] = EVSE_CHARGE_POINT_FIRMWARE_VERSION;
	// payload["chargeBoxSerialNumber"] = EVSE_CHARGE_BOX_SERIAL_NUMBER;

	return doc;
}
#endif
DynamicJsonDocument *BootNotification::createReq()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(5) + csSerial.length() + 1 + CP_Id_m.length() + 1 + cscbSerial.length() + 50 + 1 + 25 + 1);
	JsonObject payload = doc->to<JsonObject>();
	payload["chargePointVendor"] = csSerial;
	payload["chargePointSerialNumber"] =CP_Id_m;
	payload["chargePointModel"] = cscbSerial;
	payload["firmwareVersion"] = EVSE_CHARGE_POINT_FIRMWARE_VERSION;
	payload["chargeBoxSerialNumber"] = cpSerial;

	return doc;
}

void BootNotification::processConf(JsonObject payload)
{
	const char *currentTime = payload["currentTime"] | "Invalid";
	if (strcmp(currentTime, "Invalid"))
	{
		setTimeFromJsonDateString(currentTime);
		// EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;
	}
	else
	{
		Serial.print(F("[BootNotification] Error reading time string. Expect format like 2020-02-01T20:53:32.486Z\n"));
	}
	/*
	 * @brief interval - this is the the heartbeat interval.
	 * This feature was requested by JioBP. Making this change so that the heartbeat sampling interval would change.
	 */

	heartbeatInterval = payload["interval"] | 30; // requested by JioBP. Setting the default value as 50

	if (heartbeatInterval > 30)
	{
		heartbeatInterval = 30;
	}

	evse_preferences_ocpp.begin("heartbeat", false);
	evse_preferences_ocpp.putUInt("heartbeat_Interval", heartbeatInterval);
	evse_preferences_ocpp.end();

	// ptrOCPPconfigKeys->OCPP_config_keys_write_param(HEARTBEATINTERVAL, READ_WRITE, TYPE_INTEGER, heartbeatInterval, HEARTBEATINTERVAL);
	// heartbeat_write(heartbeatInterval);
	// ocpp_std_write(heartbeatInterval, HEARTBEATINTERVAL);
	//    Serial.print( "heartbeatInterval :" );
	//   Serial.println( heartbeatInterval );

	const char *status = payload["status"] | "Invalid";

	if (!strcmp(status, "Accepted"))
	{
		if (DEBUG_OUT)
			Serial.print("[BootNotification] Request has been accepted!\n");
		if (getChargePointStatusService() != NULL)
		{
			getChargePointStatusService()->boot();
			evse_boot_state = EVSE_BOOT_ACCEPTED;
			// evse_ChargePointStatus = Available;
		}
	}
	else
	{
		evse_boot_state = EVSE_BOOT_REJECTED;
		Serial.print(F("[BootNotification] Request unsuccessful!\n"));
	}
}

/*****************************no need*****************************************************/

void BootNotification::processReq(JsonObject payload)
{
	/*
	 * Ignore Contents of this Req-message, because this is for debug purposes only
	 */
}

DynamicJsonDocument *BootNotification::createConf()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(3) + (JSONDATE_LENGTH + 1));
	JsonObject payload = doc->to<JsonObject>();
	char currentTime[JSONDATE_LENGTH + 1] = {'\0'};
	getJsonDateStringFromSystemTime(currentTime, JSONDATE_LENGTH);
	payload["currentTime"] = currentTime; // currentTime
	payload["interval"] = 86400;		  // heartbeat send interval - not relevant for JSON variant of OCPP so send dummy value that likely won't break
	payload["status"] = "Accepted";

	ocpp_std_read();
	// ptrOCPPconfigKeys->OCPP_config_keys_read();

	return doc;
}
