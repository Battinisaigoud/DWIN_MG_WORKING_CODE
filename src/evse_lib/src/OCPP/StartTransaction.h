

#ifndef STARTTRANSACTION_H
#define STARTTRANSACTION_H

#include <EEPROM.h>

#include "Variants.h"
//#include "EEPROMLayout.h"

#include "OcppMessage.h"

class StartTransaction : public OcppMessage {
private:
	String idTag = String(""); //FIXME change it to /0 NULL char.
	int connectorId = 1; 
	bool accepted = false;
public:
	StartTransaction();

	StartTransaction(String &idTag);
	
	StartTransaction(String &idTag, int &connectorId);

	const char* getOcppOperationType();

	DynamicJsonDocument* createReq();

	void processConf(JsonObject payload);

	void processReq(JsonObject payload);

	DynamicJsonDocument* createConf();
};

#endif
