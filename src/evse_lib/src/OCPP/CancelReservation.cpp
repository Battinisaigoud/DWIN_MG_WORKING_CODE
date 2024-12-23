

#include "Variants.h"

#include "CancelReservation.h"
#include "OcppEngine.h"
#include "TimeHelper.h"

extern bool flag_evseReadIdTag ;
extern bool flag_evseAuthenticate ;
extern bool flag_evseStartTransaction ; //Entry condition for starting transaction.
extern bool flag_evRequestsCharge ;
extern bool flag_evseStopTransaction ;
extern bool flag_evseUnauthorise ;
extern bool flag_evseSoftReset;
extern bool flag_rebootRequired;
extern bool flag_evseReserveNow; //added by @mkrishna
 bool flag_evseCancelReservation; //added by @mkrishna

extern String currentIdTag;
extern String expiryDate;
extern int reservationId;
extern int reserve_connectorId;
extern time_t reservation_start_time;
extern time_t reserveDate;
extern time_t reservedDuration;     // 15 minutes slot Duration 15 * (1 * 60)
extern bool reserve_state;

CancelReservation Cancel_Reservation;

CancelReservation::CancelReservation() {


}

const char* CancelReservation::getOcppOperationType(){
	return "CancelReservation";
}

void CancelReservation::processReq(JsonObject payload) {

   
	reservationId = payload["reservationId"].as<int>();
		
	Serial.print(F("\r\n***********************************[CancelReservation] ************************************\r\n "));

}

DynamicJsonDocument* CancelReservation::createConf(){
	DynamicJsonDocument* doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();

    bool accepted = true;
    //if (flag_evseReserveNow) // Todo
	if (accepted)
	{
		payload["status"] = "Accepted";
		/*
		* @brief : Change made by G. Raja Sumant for removing reservation.
		*/
		getChargePointStatusService()->setReserved(false);
		flag_evseCancelReservation = true;
	}
	else
	{
		
		payload["status"] = "Rejected";
	}
	accepted = false; // reset the flag
	Serial.print(F("\r\n***********************************[CancelReservation] ************************************\r\n "));
	Serial.print(F("[CancelReservation] \r\n "));

	return doc;
}

DynamicJsonDocument* CancelReservation::createReq() {
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();

	payload["idTag"] = "fefed1d19876";
	payload["connectorId"] = 1;

	return doc;
}

void CancelReservation::processConf(JsonObject payload){
	String status = payload["status"] | "Invalid";

	if (status.equals("Accepted")) {
		if (DEBUG_OUT) Serial.print("[CancelReservation] Request has been accepted!\n");
		} else {
			Serial.print(F("[CancelReservation] Request has been denied!"));
	}
}


CancelReservationstatus CancelReservation::getCancelReservation(){
	return currentStatus;
}

void CancelReservation::setCancelReservation(CancelReservationstatus cancelreservationStatus){
	this->currentStatus = cancelreservationStatus;
}
