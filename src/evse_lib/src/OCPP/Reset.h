

#ifndef RESET_H
#define RESET_H

#include "Variants.h"

#include "OcppMessage.h"

class Reset : public OcppMessage {
public:
	Reset();

	const char* getOcppOperationType();

	void processReq(JsonObject payload);

	DynamicJsonDocument* createConf();


};
	/****Added new Function @Wamique*****/
	void softReset();
	/***********************************/
#endif
