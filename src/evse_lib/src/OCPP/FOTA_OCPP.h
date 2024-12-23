
 



#ifndef FOTA_OCPP_H
#define FOTA_OCPP_H

#include "OcppMessage.h"

#//include "fota_handler.h"


class FotaNotification : public OcppMessage {
public:
	FotaNotification();

	const char* getOcppOperationType();

	DynamicJsonDocument* createReq(JsonObject payload);

	void processConf(JsonObject payload);

	void processReq(JsonObject payload);

	DynamicJsonDocument* createConf();
	//void createConf(JsonObject payload);
};



#endif
