

#include "Variants.h"

#include "FirmwareStatus_OCPP.h"

#include "OcppEngine.h"

#include "SimpleOcppOperationFactory.h"

#include <string.h>
#include "TimeHelper.h"

// FirmwareStatus Firmware_Status;

/*
 * @brief: Refer ChargePointStatus
 */

/*
 * @breif: Instantiate an object FirmwareStatus
 */

FirmwareStatus::FirmwareStatus(WebSocketsClient *webSocket) : webSocket(webSocket)
{
	statusMessage = firmwarestatus_str[NotImplemented]; // default value if anything goes wrong
}

/*
 * @breif: Method - getOcppOperationType => This method gives the type of Ocpp operation
 */

const char *FirmwareStatus::getOcppOperationType()
{
	return "FirmwareStatus";
}

/*
 * @breif:
 * Method - createReq => This method creates Fota request to be sent to the OCPP server
 * Field Name   | Field Type         | Card. | Description
 * status       | FirmwareStatus enum| 1..1  | Required. This contains the progress status of the firmware installation.
 */

DynamicJsonDocument *FirmwareStatus::createReq()
{
	// For now the object size is 2, but with custom data, it will increase
	// DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(3) + strlen(EVSE_getChargePointVendor()) + 1 + cpSerial.length() + 1 + strlen(EVSE_getChargePointModel()) + 1);
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();
	payload["status"] = firmwarestatus_str[Downloaded]; // fill this enum
	// Need nested json for customData here -

	return doc;
}

void FirmwareStatus::processConf(JsonObject payload)
{
	/*
	 * OTA update should be processed.
	 */
}

void FirmwareStatus::processReq(JsonObject payload)
{
	/*
	 * Ignore Contents of this Req-message, because this is for debug purposes only
	 */
}

DynamicJsonDocument *FirmwareStatus::createConf()
{
	/*
	 * Ignore Contents of this Req-message, because this is for debug purposes only
	 */
}
