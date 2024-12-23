

#ifndef REMOTESTOPTRANSACTION_H
#define REMOTESTOPTRANSACTION_H

#include "Variants.h"

#include "OcppMessage.h"
#include "ChargePointStatusService.h"

class RemoteStopTransaction : public OcppMessage {
private:
	int transactionId;
	bool accepted = false;
public:
	RemoteStopTransaction();

	const char* getOcppOperationType();

	DynamicJsonDocument* createReq();

	void processConf(JsonObject payload);

	void processReq(JsonObject payload);

	DynamicJsonDocument* createConf();
};

#endif
