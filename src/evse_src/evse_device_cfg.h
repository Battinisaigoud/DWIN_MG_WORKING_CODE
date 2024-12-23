/*****************************************************************************************************
DEVELOPED DATE      : 24-08-2023
DEVELOPED BY        : GOPIKRISHNA S 
UNDER GUIDELINSE of : KRISHNA M 

Description : This file is used to store/retrieve following parameters into/from nvs memory.
              Connection type, WiFi-credentials, OCPP parameters and Threshold values.

******************************************************************************************************/
#ifndef EVSE_DEVICE_CFG_H
#define EVSE_DEVICE_CFG_H
#include "FS.h"
#include "FFat.h"

#include "SPIFFS.h"
#include <time.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define EVSE_DEVICE_CFG                     (1)
#define OCPP_PARAM_CFG                      (2)
#define WIFI_CFG                            (3)              

#define WIFI_SSID_MAX_LENGTH                (16)
#define WIFI_PSWD_MAX_LENGTH                (10)
#define OCPP_URL_MAX_LENGTH                 (128)    
#define OCPP_PROTOCOL_MAX_LENGTH            (16)
#define OCPP_HOST_MAX_LENGTH                (32)
#define OCPP_RESOURCE_PATH_MAX_LENGTH       (96)

#define DB_NO_ERROR                         (0x0)
#define DB_FS_MOUNT_ERROR                   (0x1)
#define DB_FS_OPEN_ERROR                    (0x2)
#define DB_FS_IS_DIR_ERROR                  (0x3)
#define DB_OPEN_ERROR                       (0x4)
#define DB_EXEC_ERROR                       (0x5)

#define TOTAL_NUMBER_OF_COFIG               (1)

#define FORMAT_FFAT true
//TO do 
//Add the ETHERNET
//Add Config_Threshold
//Add RFID Options
//Add Charger Details
//Add Time_Sync
//Add About Information
//Add Charger details

/*Config Threshold values Structure*/
typedef struct ThresholdConfig_tag
{
  uint16_t maxVoltage;
  uint16_t minVoltage;
  uint16_t maxCurrent;
  uint16_t minCurrent;
  uint8_t CTC;
  uint8_t CTT;
  uint8_t LPT;
  uint8_t OTT;
  uint8_t UTT;
}ThresholdCfg_t;

/*Config WiFi Credentials Structure*/
typedef struct wifi_cfg_tag
{
  char ssid[WIFI_SSID_MAX_LENGTH];     // WiFi ssid
  char pswd[WIFI_PSWD_MAX_LENGTH];     // WiFi password
} wifi_cfg_t;

/*Config OCPP parameters Structure*/
typedef struct ocpp_cfg_params_tag
{
  char url[OCPP_URL_MAX_LENGTH];             // OCPP url
  char protocol[OCPP_PROTOCOL_MAX_LENGTH];   // OCPP protocol
  char host[OCPP_HOST_MAX_LENGTH];           // OCPP Host
  uint16_t port;                             // OCPP port number
  char resource_path[OCPP_RESOURCE_PATH_MAX_LENGTH]; // OCPP Resource_path
} ocpp_cfg_params_t;

/*Config linked Device Config Structure*/
typedef struct evse_device_cfg_tag
{
  uint8_t connection_type;          // Communication connectivity parameter from ChargePoint to CMS
  wifi_cfg_t wifi_config;           // WiFi-Credentials
  ocpp_cfg_params_t ocpp_config;    // OCPP parameters
  ThresholdCfg_t ThresholdConfig;   // Threshold Configurations
} evse_device_cfg_t;

class EvseDeviceConfigStorage
{
private:
public:
  /* You only need to format FFAT/SPIFFS the first time you run a
   test or else use the FFAT/SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */

  int Device_Config_init(void);
  int Device_Config_open(void);
  int Device_Config_write(void);
  int Device_Config_read(void);
  int Device_Config_dump(void);
  void Device_Config_ConnType_write(uint8_t connection_type);
  void Device_Config_WifiCred_write(char *ssid, char *passwd);
  void Device_Config_ocppParam_write(String url, String protocol, String host, uint16_t port, String resource_path);
  void Device_Config_ThreshCfg_write(uint16_t, uint16_t, uint16_t, uint16_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
};

#endif