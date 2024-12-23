

#ifndef UNLOCKCONNECTOR_H
#define UNLOCKCONNECTOR_H

#include "OcppMessage.h"


class UnlockConnector : public OcppMessage {
private:
	int connectorId = 1; 
    bool accepted = false;
public:
	UnlockConnector();

	const char* getOcppOperationType();

	DynamicJsonDocument* createReq();

	void processConf(JsonObject payload);

	void processReq(JsonObject payload);

	DynamicJsonDocument* createConf();
};

#endif
