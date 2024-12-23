

#ifndef CHARGEPOINTSTATUSSERVICE_H
#define CHARGEPOINTSTATUSSERVICE_H

// #include "libraries/arduinoWebSockets-master/src/WebSocketsClient.h"
//#include "../arduinoWebSockets-master/src/WebSocketsClient.h"
//#include "src/evse_lib/src/WebSockets_Generic/src/WebSocketsClient_Generic.h"
//#include "../WebSockets_Generic/src/WebSocketsClient_Generic.h"
#include "../websocket/WebSocketsClient.h"

#if 0
enum class ChargePointStatus {
	Available,
	Preparing,
	Charging,
	SuspendedEVSE,
	SuspendedEV,
	Finishing,    //not supported by this client
	Reserved,     //Implemented reserve now
	Unavailable,  //Implemented Change Availability
	Faulted,      //Implemented Faulted.
	NOT_SET 	  //not part of OCPP 1.6
};

#else

enum ChargePointStatus {
	Available,
	Preparing,
	Charging,
	SuspendedEVSE,
	SuspendedEV,
	Finishing,    //not supported by this client
	Reserved,     //Implemented reserve now
	Unavailable,  //Implemented Change Availability
	Faulted,      //Implemented Faulted.
	NOT_SET 	  //not part of OCPP 1.6
};

#endif

extern enum ChargePointStatus evse_ChargePointStatus;

class ChargePointStatusService {
private:
	bool authorized = false;
	String idTag = String('\0');
	int connectorId = 1;
	bool transactionRunning = false;
	//Making this public 
	//int transactionId = -1;
	bool evDrawsEnergy = false;
	bool evseOffersEnergy = false;
	bool overVoltage = false;
	bool underVoltage = false;
	bool EarthDisconnect = false;
	bool overTemperature = false;
	bool underTemperature = false;
	bool overCurrent = false;
	bool underCurrent = false;
	bool emergencyRelayClose = false;
	//Feature added by G. Raja Sumant for charge point status service.
	bool unavailable = false;
	//Feature added by G. Raja Sumant for reservation status service.
	bool reserved = false;
	ChargePointStatus currentStatus = ChargePointStatus::NOT_SET;
	WebSocketsClient *webSocket;
public:
	
	ChargePointStatusService(WebSocketsClient *webSocket);
	void authorize(String &idTag, int connectorId);
	void authorize(String &idTag);
	void authorize();
	void unauthorize();
	String &getIdTag();
	int &getConnectorId();
	bool getOverVoltage();
	void setOverVoltage(bool ov);
	bool getUnderVoltage();
	void setUnderVoltage(bool uv);
	//Feature added by G. Raja Sumant for Change availabilty
	void setUnavailable(bool su);
	bool getUnavailable();
	//Feature added by G. Raja Sumant for reservation
	void setReserved(bool);
	bool getReserved();
	bool getOverCurrent();
	void setOverCurrent(bool oc);
	bool getUnderCurrent();
	void setUnderCurrent(bool uc);
	bool getOverTemperature();
	void setOverTemperature(bool ot);
	bool getUnderTemperature();
	void setUnderTemperature(bool ut);
	bool getEmergencyRelayClose();
	void setEmergencyRelayClose(bool erc);
	void startTransaction(int transactionId);
	void stopTransaction();
	int getTransactionId();
	void boot();
	void startEvDrawsEnergy();
	void stopEvDrawsEnergy();
	void startEnergyOffer();
	void stopEnergyOffer();
	void loop();
	void setEarthDisconnect(bool ed);

	ChargePointStatus inferenceStatus();

	/***Adding new funtion @wamique**/
	bool getEvDrawsEnergy();
	/*******************************/
};

#endif
