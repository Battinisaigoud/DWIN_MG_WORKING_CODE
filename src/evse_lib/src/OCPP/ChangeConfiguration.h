

#ifndef CHANGECONFIGURATION_H
#define CHANGECONFIGURATION_H

#include "OcppMessage.h"

#include <LinkedList.h>

class ChangeConfiguration : public OcppMessage {
private:
    bool acceptance;
    bool rejected; //added by G. Raja Sumant 08/07/2022 to reject invalid values.
public:
  ChangeConfiguration();

  const char* getOcppOperationType();

  DynamicJsonDocument* createReq();

  void processConf(JsonObject payload);
  
  DynamicJsonDocument* createConf();
  
  void processReq(JsonObject payload);

};



#if 1

typedef struct Change_configuration_tag
{

  String ocppkey;

}Change_configuration_tag_t;

extern Change_configuration_tag_t Change_configuration_instance;

#endif






#endif