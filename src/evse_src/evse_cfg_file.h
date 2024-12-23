#ifndef EVSE_CFG_PARAMETER_H
#define EVSE_CFG_PARAMETER_H

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

#include "evse_ocpp_config_keys.h"

#define STD_NUMBER_OF_COFIG_KEYS (43)
#define CUSTOM_NUMBER_OF_COFIG_KEYS (0)

#define DB_NO_ERROR (0x0)
#define DB_FS_MOUNT_ERROR (0x1)
#define DB_FS_OPEN_ERROR (0x2)
#define DB_FS_IS_DIR_ERROR (0x3)
#define DB_OPEN_ERROR (0x4)
#define DB_EXEC_ERROR (0x5)

#define TOTAL_NUMBER_OF_COFIG_KEYS (STD_NUMBER_OF_COFIG_KEYS + CUSTOM_NUMBER_OF_COFIG_KEYS)

#define FORMAT_FFAT true

typedef struct ocpp_config_key_tag
{
  //   char *config_key;
  //   uint8_t config_key_length;
  // String config_key;
  uint8_t config_key;
  uint8_t access_R_RW;
  int32_t type_int_bool;
  uint32_t uints;
} ocpp_config_key_t;

typedef struct ocpp_config_keys_tag
{
  ocpp_config_key_t ocpp_std_config_keys[TOTAL_NUMBER_OF_COFIG_KEYS];
  //   ocpp_config_key_t ocpp_custom_config_keys;
} ocpp_config_keys_t;

/* extern ocpp_config_keys_t gOCPP_config_keys_write;
extern ocpp_config_keys_t gOCPP_config_keys_read; */

// extern uint8_t OCPP_config_keys_buf_write[sizeof(ocpp_config_keys_t)];
// extern uint8_t OCPP_config_keys_buf_read[sizeof(ocpp_config_keys_t)];

typedef struct StoredConfigKeys_tag
{
  String config_key;
  String access_R_RW;
  String type_int_bool;
  String uints;

} StoredConfigKeys_t;

class OCPPconfigkeyStorage
{
private:
public:
  /* You only need to format FFAT/SPIFFS the first time you run a
   test or else use the FFAT/SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */

  int OCPP_config_keys_init(void);
  int OCPP_config_keys_open(void);
  int OCPP_config_keys_write(void);
  int OCPP_config_keys_read(void);
  int OCPP_config_keys_dump(void);
  int OCPP_config_keys_write_param(uint8_t config_key, uint8_t access_R_RW, int32_t type_int_bool, uint32_t uints, uint8_t idx);
};


uint8_t ocpp_std_write(uint32_t ocpp_key_uints, uint8_t idx);
uint8_t ocpp_std_read(void); 

#endif