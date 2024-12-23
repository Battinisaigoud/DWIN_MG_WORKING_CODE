

#include "Variants.h"
#include "../../../evse_src/evse_config.h"
#include "../../../evse_src/evse_addr_led.h"

#include "ChargePointStatusService.h"
#include "StatusNotification.h"
#include "OcppEngine.h"

#include "SimpleOcppOperationFactory.h"
 #include <Preferences.h>

// #include "../Evse_Preferences/Evse_Preferences.h"

#include <string.h>


extern EvseDevStatuse EvseDevStatus_connector_1;

extern bool webSocketConncted;

extern uint8_t gu8_online_flag;

extern uint8_t gu8_evse_change_state;


// uint8_t //reasonForStop = 0;

int transactionId = -1;

//extern bool flag_GFCI_set_here;

 bool ongoingTxn_m;  //extern

extern bool flag_start_accepted;

bool flag_stop_finishing = false;


ChargePointStatus evse_ChargePointStatus;

Preferences resumeTxn2;

extern enum led_col_config evse_led_status; // enum variable.
extern enum EvseChargePointErrorCode EvseChargePoint_ErrorCode;
extern enum EvseChargePointErrorCode Current_EvseChargePoint_ErrorCode;


ChargePointStatusService::ChargePointStatusService(WebSocketsClient *webSocket) : webSocket(webSocket)
{
	setChargePointStatusService(this);
}


ChargePointStatus ChargePointStatusService::inferenceStatus()
{
		 return evse_ChargePointStatus;

}

/**
 * 
 * @brief 
 * implemeted by krishna
 * 
 * 
*/
void ChargePointStatusService::loop()
{
	ChargePointStatus inferencedStatus = inferenceStatus();

	if (inferencedStatus != currentStatus)
	{
		currentStatus = inferencedStatus;
        Serial.print("CP LOOP:");
		switch(currentStatus)
		{
			case Available:
				Serial.println("Available");
				break;
			case Preparing:
				Serial.print("Preparing");	
				break;
			case Charging:
			gu8_evse_change_state = EVSE_STOP_TXN;
				Serial.print("Charging");
				break;
			case SuspendedEVSE:
				Serial.print("SuspendedEVSE");
				break;
			case SuspendedEV:
				Serial.print("SuspendedEV");
				break;
			case Finishing:    //not supported by this client
				Serial.print("Finishing");
				break;
			case Reserved:     //Implemented reserve now
				Serial.print("Reserved");
				break;
			case Unavailable:  //Implemented Change Availability
				Serial.print("Unavailable");
				break;
			case Faulted: // Implemented Faulted.
				Serial.print("Faulted");

				break;
			case NOT_SET: // Implemented Faulted.
				Serial.print("NOT_SET.........");

				break;
			default:
					// evse_ChargePointStatus = Available;
					//Serial.print("Available");
			   break; 

		}

		if (DEBUG_OUT)
			Serial.print("[ChargePointStatusService] Status changed\n");
		#if 1
		// if(currentStatus != Unavailable)
		// {
		if ((gu8_online_flag == 1) && (currentStatus != NOT_SET))
		{
			if (EvseDevStatus_connector_1 != flag_EVSE_is_Booted)
			{

				// fire StatusNotification
				// TODO check for online condition: Only inform CS about status change if CP is online
				// TODO check for too short duration condition: Only inform CS about status change if it lasted for longer than MinimumStatusDuration
				OcppOperation *statusNotification = makeOcppOperation(webSocket,
																	  new StatusNotification(currentStatus));
				initiateOcppOperation(statusNotification);
			}
		}
#endif
	}
}
/*********************************need to optimatize***************************************************8*/

void ChargePointStatusService::authorize(String &idTag, int connectorId)
{
	this->idTag = String(idTag);
	this->connectorId = connectorId;
	authorize();
}

void ChargePointStatusService::authorize(String &idTag)
{
	this->idTag = String(idTag);
	authorize();
}

void ChargePointStatusService::authorize()
{
	if (authorized == true)
	{
		// if (DEBUG_OUT)
		// 	Serial.print(F("[ChargePointStatusService] Warning: authorized twice or didn't unauthorize before\n"));
		return;
	}
	authorized = true;
}

int &ChargePointStatusService::getConnectorId()
{
	return connectorId;
}

String &ChargePointStatusService::getIdTag()
{
	return idTag;
}

bool ChargePointStatusService::getOverVoltage()
{
	return overVoltage;
}

void ChargePointStatusService::setOverVoltage(bool ov)
{
	this->overVoltage = ov;
}

bool ChargePointStatusService::getUnderVoltage()
{
	return underVoltage;
}

void ChargePointStatusService::setUnderVoltage(bool uv)
{
	this->underVoltage = uv;
}

bool ChargePointStatusService::getOverTemperature()
{
	return overTemperature;
}

void ChargePointStatusService::setOverTemperature(bool ot)
{
	this->overTemperature = ot;
}

bool ChargePointStatusService::getUnderTemperature()
{
	return underTemperature;
}

void ChargePointStatusService::setUnderTemperature(bool ut)
{
	this->underTemperature = ut;
}

bool ChargePointStatusService::getOverCurrent()
{
	return overCurrent;
}

void ChargePointStatusService::setOverCurrent(bool oc)
{
	this->overCurrent = oc;
}

bool ChargePointStatusService::getUnderCurrent()
{
	return underCurrent;
}

void ChargePointStatusService::setUnderCurrent(bool uc)
{
	this->underCurrent = uc;
}

bool ChargePointStatusService::getEmergencyRelayClose()
{
	return emergencyRelayClose;
}
void ChargePointStatusService::setEmergencyRelayClose(bool erc)
{
	this->emergencyRelayClose = erc;
}

void ChargePointStatusService::unauthorize()
{
	// if (authorized == false)
	// {
	// 	if (DEBUG_OUT)
	// 		Serial.print(F("[ChargePointStatusService] Warning: unauthorized twice or didn't authorize before\n"));
	// 	return;
	// }
	// if (DEBUG_OUT)
	// 	Serial.print(F("[ChargePointStatusService] idTag value will be cleared.\n"));
	this->idTag.clear();
	authorized = false;
}

void ChargePointStatusService::startTransaction(int transId)
{
	// if (transactionRunning == true)
	// {
	// 	if (DEBUG_OUT)
	// 		Serial.print(F("[ChargePointStatusService] Warning: started transaction twice or didn't stop transaction before\n"));
	// }
	transactionId = transId;
	/*
	* @brief: Feature added by G. Raja Sumant 14/05/2022
			  Session should resume with this txn id itself.
			  Hence first store it.
	*/
	resumeTxn2.begin("resume", false); // opening preferences in R/W mode
	resumeTxn2.putInt("TxnIdData", transId);
	resumeTxn2.end();
	transactionRunning = true;
}

void ChargePointStatusService::boot()
{
	// TODO Review: Is it necessary to check in inferenceStatus(), if the CP is booted at all? Problably not ...
}

void ChargePointStatusService::stopTransaction()
{
	// if (transactionRunning == false)
	// {
	// 	if (DEBUG_OUT)
	// 		Serial.print(F("[ChargePointStatusService] Warning: stopped transaction twice or didn't start transaction before\n"));
	// }
	transactionRunning = false;
	resumeTxn2.begin("resume", false); // opening preferences in R/W mode
	resumeTxn2.putInt("TxnIdData", -1);
	resumeTxn2.end();
	transactionId = -1;
	
}

int ChargePointStatusService::getTransactionId()
{
// 	if (transactionId < 0)
// 	{
// 		if (DEBUG_OUT)
// 			Serial.print(F("[ChargePointStatusService] Warning: getTransactionId() returns invalid transactionId. Have you called it after stopTransaction()? (can only be called before) Have you called it before startTransaction?\n"));
// 	}
// 	if (transactionRunning == false)
// 	{
// 		if (DEBUG_OUT)
// 			Serial.print(F("[ChargePointStatusService] Warning: getTransactionId() called, but there is no transaction running. Have you called it after stopTransaction()? (can only be called before) Have you called it before startTransaction?\n"));
	// }
	return transactionId;
}

void ChargePointStatusService::startEvDrawsEnergy()
{
	// if (evDrawsEnergy == true)
	// {
	// 	if (DEBUG_OUT)
	// 		Serial.print(F("[ChargePointStatusService] Warning: startEvDrawsEnergy called twice or didn't call stopEvDrawsEnergy before\n"));
	// }
	evDrawsEnergy = true;
}

void ChargePointStatusService::stopEvDrawsEnergy()
{
	// if (evDrawsEnergy == false)
	// {
	// 	if (DEBUG_OUT)
	// 		Serial.print(F("[ChargePointStatusService] Warning: stopEvDrawsEnergy called twice or didn't call startEvDrawsEnergy before\n"));
	// }
	evDrawsEnergy = false;
}
void ChargePointStatusService::startEnergyOffer()
{
	// if (evseOffersEnergy == true)
	// {
	// 	if (DEBUG_OUT)
	// 		Serial.print(F("[ChargePointStatusService] Warning: startEnergyOffer called twice or didn't call stopEnergyOffer before\n"));
	// }
	evseOffersEnergy = true;
}

void ChargePointStatusService::stopEnergyOffer()
{
	// if (evseOffersEnergy == false)
	// {
	// 	if (DEBUG_OUT)
	// 		Serial.print(F("[ChargePointStatusService] Warning: stopEnergyOffer called twice or didn't call startEnergyOffer before\n"));
	// }
	evseOffersEnergy = false;
}

/*****Added new Definition @wamique***********/
bool ChargePointStatusService::getEvDrawsEnergy()
{
	return evDrawsEnergy;
}

/*********************************************/

/*
 * @brief : Feature added by G. Raja Sumant
 * 09/07/2022 For Change availability
 */
void ChargePointStatusService::setUnavailable(bool su)
{
	this->unavailable = su;
}

bool ChargePointStatusService::getUnavailable()
{
	return unavailable;
}

/*
 * @brief : Feature added by G. Raja Sumant
 * 19/07/2022 For ReserveNow
 */
void ChargePointStatusService::setReserved(bool re)
{
	this->reserved = re;
}

bool ChargePointStatusService::getReserved()
{
	return reserved;
}
