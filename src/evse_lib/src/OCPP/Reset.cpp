// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\Reset.cpp"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Variants.h"

#include "Reset.h"
#include "OcppEngine.h"
#include "../../../evse_src/evse_config.h"

uint8_t gu8_EVSE_CMS_reset = 0;

extern uint8_t gu8_evse_cms_reset_flag;

extern uint8_t reasonForStop;

Reset re_set;

Reset::Reset()
{
}

const char *Reset::getOcppOperationType()
{
	return "Reset";
}

void Reset::processReq(JsonObject payload)
{
	/*
	 * Process the application data here. Note: you have to implement the device reset procedure in your client code. You have to set
	 * a onSendConfListener in which you initiate a reset (e.g. calling ESP.reset() )
	 */
	const char *type = payload["type"] | "Invalid";
	if (!strcmp(type, "Hard"))
	{
		Serial.print(F("[Reset] Warning: received request to perform hard reset, but this implementation is only capable of soft reset!\n"));

		reasonForStop = 2;
		gu8_EVSE_CMS_reset = EVSE_CMS_RESET_HARD;
	}
	else if (!strcmp(type, "Soft"))
	{
		if (DEBUG_OUT)
			Serial.println(F("Soft Reset is Requested"));
		reasonForStop = 8;
		gu8_evse_cms_reset_flag = 0;
		gu8_EVSE_CMS_reset = EVSE_CMS_RESET_SOFT;
	}
}

DynamicJsonDocument *Reset::createConf()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();
	payload["status"] = "Accepted";
	return doc;
}
