

#include "Variants.h"

#include "StatusNotification.h"
#include "ChargePointStatusService.h"

#include "../../../evse_src/evse_config.h"
#include "../../../evse_src/evse_addr_led.h"

#include <string.h>

int8_t fault_code_A;
extern bool evse_A_unavail;
extern bool flag_stop_finishing;
extern int reserve_connectorId;

extern String csSerial;

extern enum led_col_config evse_led_status; // enum variable.
extern enum EvseChargePointErrorCode EvseChargePoint_ErrorCode;
extern enum EvseChargePointErrorCode_OtherError_t EvseChargePointErrorCode_OtherError;

StatusNotification Status_Notification;

StatusNotification::StatusNotification(ChargePointStatus currentStatus)
	: currentStatus(currentStatus)
{

	if (!getJsonDateStringFromSystemTime(timestamp, JSONDATE_LENGTH))
	{
		Serial.print(F("[StatusNotification] Error reading time string. Expect format like 2020-02-01T20:53:32.486Z\n"));
	}

	if (DEBUG_OUT)
	{
		Serial.print(F("[StatusNotification] New StatusNotification with timestamp "));
		Serial.print(timestamp);
		Serial.print(". New Status: ");
	}

	switch (currentStatus)
	{
	case (ChargePointStatus::Available):
		if (DEBUG_OUT)
			Serial.print("Available\n");

		break;
	case (ChargePointStatus::Preparing):
		if (DEBUG_OUT)
			Serial.print("Preparing\n");

		break;
	case (ChargePointStatus::Charging):
		if (DEBUG_OUT)
			Serial.print("Charging\n");

		break;
	case (ChargePointStatus::SuspendedEVSE):
		if (DEBUG_OUT)
			Serial.print("SuspendedEVSE\n");

		break;
	case (ChargePointStatus::SuspendedEV):
		if (DEBUG_OUT)
			Serial.print("SuspendedEV\n");

		break;
	case (ChargePointStatus::Finishing):
		if (DEBUG_OUT)
			Serial.print("Finishing\n");

		break;
	case (ChargePointStatus::Reserved):
		if (DEBUG_OUT)
			Serial.print("Reserved\n");

		break;
	case (ChargePointStatus::Unavailable):
		if (DEBUG_OUT)
			Serial.print("Unavailable\n");

		break;
	case (ChargePointStatus::Faulted):
		if (DEBUG_OUT)
			Serial.print("Faulted\n");

		break;
	case (ChargePointStatus::NOT_SET):
		currentStatus = getChargePointStatusService()->inferenceStatus();
		Serial.print("NOT_SET\n");
		break;
	default:
		Serial.print("[Error, invalid status code]\n");
		break;
	}
}

const char *StatusNotification::getOcppOperationType()
{
	return "StatusNotification";
}

// TODO if the status has changed again when sendReq() is called, abort the operation completely (note: if req is already sent, stick with listening to conf). The ChargePointStatusService will enqueue a new operation itself
DynamicJsonDocument *StatusNotification::createReq()
{
	int connectorId = 1;
	/*
	 * @brief :Adding vendor specific error codes and making changes to error codes.
	 * JSON_OBJECT_SIZE(6) to support future scope
	 */
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(6) + (JSONDATE_LENGTH + 25 + 1));
	JsonObject payload = doc->to<JsonObject>();

	payload["connectorId"] = 1; // Hardcoded to be one because only one connector is supported
	// if OtherError Other type of error. More information in vendorErrorCode.
	// payload["errorCode"] = "NoError";  //No error diagnostics support

	currentStatus = getChargePointStatusService()->inferenceStatus();
	// Part of Offline Implementation

	payload["errorCode"] = "NoError";

	switch (currentStatus)
	{
	case (ChargePointStatus::Available):
		payload["status"] = "Available";

		break;
	case (ChargePointStatus::Preparing):
		payload["status"] = "Preparing";

		break;
	case (ChargePointStatus::Charging):
		payload["status"] = "Charging";

		break;
	case (ChargePointStatus::SuspendedEVSE):
		payload["status"] = "SuspendedEVSE";

		break;
	case (ChargePointStatus::SuspendedEV):
		payload["status"] = "SuspendedEV";

		break;
	case (ChargePointStatus::Finishing):
		payload["status"] = "Finishing";

		break;
	case (ChargePointStatus::Reserved):
		payload["status"] = "Reserved";

		break;
	case (ChargePointStatus::Unavailable):
		payload["status"] = "Unavailable";

		break;
	case (ChargePointStatus::Faulted):
		payload["status"] = "Faulted";

		switch (EvseChargePoint_ErrorCode)
		{
		case EVSE_ConnectorLockFailure:

			break;

		case EVSE_EVCommunicationError:

			break;

		case EVSE_GroundFailure:
		{
			payload["errorCode"] = "GroundFailure";
		}

		break;

		case EVSE_HighTemperature:
		{
			payload["errorCode"] = "HighTemperature";
		}

		break;

		case EVSE_InternalError:

			break;

		case EVSE_LocalListConflict:

			break;

		case EVSE_NoError:

			break;

		case EVSE_OtherError:
		{
			payload["errorCode"] = "OtherError";
			payload["vendorId"] = csSerial;
			// payload["vendorId"] = "EVRE";

			switch (EvseChargePointErrorCode_OtherError)
			{
			case EVSE_Earth_Disconnect:
				payload["vendorErrorCode"] = "EARTH DISCONNECT";
				break;

			case EVSE_EmergencyStop:
				payload["vendorErrorCode"] = "EmergencyStop";

				break;

			case EVSE_PowerLoss:
				payload["vendorErrorCode"] = "PowerLoss";
				break;
			}
		}

		break;

		case EVSE_OverCurrentFailure:
		{
			payload["errorCode"] = "OverCurrentFailure";
		}

		break;

		case EVSE_OverVoltage:
		{
			payload["errorCode"] = "OverVoltage";
		}

		break;

		case EVSE_PowerMeterFailure:

			break;

		case EVSE_ReaderFailure:

			break;

		case EVSE_ResetFailure:

			break;

		case EVSE_UnderVoltage:
		{
			payload["errorCode"] = "UnderVoltage";
		}
		break;

		case EVSE_WeakSignal:

			break;

		default:

			break;
		}

#if 0
		switch(fault_code_A)
		{
			case 0: payload["errorCode"] = "OverVoltage";
			        break;
			case 1: payload["errorCode"] = "UnderVoltage";
			        break;
			case 2: payload["errorCode"] = "OverCurrentFailure";
			        break;
			case 3: payload["errorCode"] = "UnderCurrent";
			        break;
			case 4: payload["errorCode"] = "HighTemperature";
			        break;
			case 5: payload["errorCode"] = "UnderTemperature";
			        break;
			case 6: payload["errorCode"] = "GroundFailure";
			        break;
			case 7: 
					payload["errorCode"] = "OtherError";
					payload["vendorId"] = "EVRE";
					payload["vendorErrorCode"] = "EARTH DISCONNECT";
			        break;
			case 8 :payload["errorCode"] = "OtherError";
					payload["vendorId"] = "EVRE";
					payload["vendorErrorCode"] = "EmergencyStop";
			        break;
			case 9: payload["errorCode"] = "OtherError";
					payload["vendorId"] = "EVRE";
					payload["vendorErrorCode"] = "PowerLoss";
			default : payload["errorCode"] = "NoError"; //No error diagnostics support
			        break;
		}

		fault_code_A = -1; // clearing the fault code so that the next status notification will not be effected.
#endif

		break;
	default:
		currentStatus = getChargePointStatusService()->inferenceStatus();
		switch (currentStatus)
		{
		case (ChargePointStatus::Available):
			payload["status"] = "Available";
			break;
		case (ChargePointStatus::Preparing):
			payload["status"] = "Preparing";
			break;
		case (ChargePointStatus::Charging):
			payload["status"] = "Charging";
			break;

		case (ChargePointStatus::SuspendedEVSE):
			payload["status"] = "SuspendedEVSE";
			break;

		case (ChargePointStatus::SuspendedEV):
			payload["status"] = "SuspendedEV";
			break;
		case (ChargePointStatus::Finishing):
			payload["status"] = "Finishing";
			break;
		case (ChargePointStatus::Reserved):
			payload["status"] = "Reserved";
			break;
		case (ChargePointStatus::Unavailable):
			payload["status"] = "Unavailable";
			break;
		case (ChargePointStatus::Faulted):
			payload["status"] = "Faulted";
			break;
		default:
			payload["status"] = "Available";
			Serial.print(F("[StatusNotification] Error: Sending status is still NOT_SET!\n"));
			break;
		}

		break;
	}

	// if(flag_stop_finishing)
	// {
	// 	payload["status"] = "Finishing";
	// }

	if (!getJsonDateStringFromSystemTime(timestamp, JSONDATE_LENGTH))
	{
		Serial.print(F("[StatusNotification] Error reading time string. Expect format like 2020-02-01T20:53:32.486Z\n"));
	}

	payload["timestamp"] = timestamp;

	return doc;
}

void StatusNotification::processConf(JsonObject payload)
{
	/*
	 * Empty payload
	 */
}

/*
 * For debugging only
 */
StatusNotification::StatusNotification()
{
}

/*
 * For debugging only
 */
void StatusNotification::processReq(JsonObject payload)
{
}

/*
 * For debugging only
 */
DynamicJsonDocument *StatusNotification::createConf()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(0);
	JsonObject payload = doc->to<JsonObject>();
	return doc;
}
