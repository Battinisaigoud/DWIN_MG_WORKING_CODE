#ifndef _EVSE_CONNECTOR_PARAMETERS_H
#define _EVSE_CONNECTOR_PARAMETERS_H

#include "FS.h"
#include "FFat.h"

#include "SPIFFS.h"
#include <time.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <LinkedList.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define TOTAL_NUMBER_OF_CONNECTOR (1)

typedef struct evse_connector_parameters_tag
{
  uint8_t idTag_number[32];
  uint8_t ongoing_txn;
  int32_t txn_id;
  uint32_t meter_value;
} evse_connector_parameters_t;

typedef struct evse_Nconnector_parameters_tag
{
  evse_connector_parameters_t evse_connector_parameters[TOTAL_NUMBER_OF_CONNECTOR];
} evse_Nconnector_parameters_t;

class evseNconnectorparametersStorage
{
private:
public:
  /* You only need to format FFAT/SPIFFS the first time you run a
   test or else use the FFAT/SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */

  int evse_connector_parameters_init(void);
  int evse_connector_parameters_write(void);
  int evse_connector_parameters_read(void);
  int evse_connector_parameters_dump(void);
  int evse_connector_parameters_write_param(uint8_t *idTag_number, uint8_t id_len, uint8_t ongoing_txn, int32_t txn_id, uint32_t meter_value, uint8_t idx);
};

extern evseNconnectorparametersStorage gEVSEconnectorparametersStorage;

#endif