
#ifndef GETCONFIGURATION_H
#define GETCONFIGURATION_H

#include "OcppMessage.h"

class GetConfiguration : public OcppMessage {
private:

public:
  GetConfiguration();

  const char* getOcppOperationType();

  DynamicJsonDocument* createReq();

  void processConf(JsonObject payload);
  
  DynamicJsonDocument* createConf();

  void processReq(JsonObject payload);


};

#endif
