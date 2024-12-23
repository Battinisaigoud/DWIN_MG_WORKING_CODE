/*****************************************************************************************************
DEVELOPED DATE      : 24-08-2023
DEVELOPED BY        : GOPIKRISHNA S 
UNDER GUIDELINSE of : KRISHNA M 

Description : This file is used to store/retrieve following parameters into/from nvs memory.
              Connection type, WiFi-credentials, OCPP parameters and Threshold values.

******************************************************************************************************/

#include "evse_device_cfg.h"

File dev_cfg_root;
File dev_cfg_file;

char *device_config_filename = "/evseDevConfig.txt";

EvseDeviceConfigStorage gEvseDeviceConfigStorage;
evse_device_cfg_t gEvse_Device_Config_write;
evse_device_cfg_t gEvse_Device_Config_read;

static uint8_t Evse_Device_Config_Buf_write[sizeof(evse_device_cfg_t)];
static uint8_t Evse_Device_Config_Buf_read[sizeof(evse_device_cfg_t)];

static int evseDeviceCfgFileWrite(fs::FS &fs, const char *path);
static int evseDeviceCfgFileRead(fs::FS &fs, const char *path);

int EvseDeviceConfigStorage::Device_Config_init(void)
{
    uint8_t levels = 0;
    uint8_t mount_count = 5;

    Serial.printf("Listing directory: %s\r\n", "/");

#if SPI_FFS_USED
    while (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED) && mount_count)
    {
        mount_count--;
        Serial.println(F("Failed to mount file system"));
    }
    if (!mount_count)
    {
        return DB_FS_MOUNT_ERROR;
    }

    // list SPIFFS contents
    dev_cfg_root = SPIFFS.open("/");
    if (!dev_cfg_root)
    {
        Serial.println(F("- failed to open directory"));
        return DB_FS_OPEN_ERROR;
    }
    if (!dev_cfg_root.isDirectory())
    {
        Serial.println(F(" - not a directory"));
        return DB_FS_IS_DIR_ERROR;
    }
    dev_cfg_file = dev_cfg_root.openNextFile();
    while (dev_cfg_file)
    {
        if (dev_cfg_file.isDirectory())
        {
            Serial.print("  DIR : ");
            Serial.println(dev_cfg_file.name());
        }
        else
        {
            Serial.print("  FILE: ");
            Serial.print(dev_cfg_file.name());
            Serial.print("\tSIZE: ");
            Serial.println(dev_cfg_file.size());
        }
        dev_cfg_file = dev_cfg_root.openNextFile();
    }

#else

#if 0
    if (FORMAT_FFAT) FFat.format();
#endif

    while ((!FFat.begin()) && mount_count)
    {
        mount_count--;
        Serial.println(F("Failed to mount file system"));
    }
    if (!mount_count)
    {
        if (FORMAT_FFAT) FFat.format(); 
        mount_count = 5;
        
        while ((!FFat.begin()) && mount_count)
        {
            mount_count--;
            Serial.println(F("Failed to mount file system"));
        }

        return DB_FS_MOUNT_ERROR;
    }

    fs::FS &fs = FFat;
    // File root = fs.open("/");
    dev_cfg_root = fs.open("/");
    if (!dev_cfg_root)
    {
        Serial.println("- failed to open directory");
        return DB_FS_OPEN_ERROR;
    }
    if (!dev_cfg_root.isDirectory())
    {
        Serial.println(" - not a directory");
        return DB_FS_IS_DIR_ERROR;
    }

    // File file = dev_cfg_root.openNextFile();
    dev_cfg_file = dev_cfg_root.openNextFile();
    while (dev_cfg_file)
    {
        if (dev_cfg_file.isDirectory())
        {
            Serial.print("  DIR : ");
            Serial.println(dev_cfg_file.name());
            // if(levels){
            //     listDir(fs, file.name(), levels -1);
            // }
        }
        else
        {
            Serial.print("  FILE: ");
            Serial.print(dev_cfg_file.name());
            Serial.print("\tSIZE: ");
            Serial.println(dev_cfg_file.size());
        }
        dev_cfg_file = dev_cfg_root.openNextFile();
    }
#endif

    Serial.println(F("FREE HEAP"));
    Serial.println(ESP.getFreeHeap());

    return DB_NO_ERROR;
}

int EvseDeviceConfigStorage::Device_Config_open(void)
{
}

int EvseDeviceConfigStorage::Device_Config_write(void)
{
    memcpy(&Evse_Device_Config_Buf_write, &gEvse_Device_Config_write, sizeof(evse_device_cfg_t));
#if SPI_FFS_USED
    evseDeviceCfgFileWrite(SPIFFS, device_config_filename);
#else
    evseDeviceCfgFileWrite(FFat, device_config_filename);
#endif
    return 0;
}

int EvseDeviceConfigStorage::Device_Config_read(void)
{
    memset(&gEvse_Device_Config_read, 0x0, sizeof(evse_device_cfg_t));
#if SPI_FFS_USED
    evseDeviceCfgFileRead(SPIFFS, device_config_filename);
#else
    evseDeviceCfgFileRead(FFat, device_config_filename);
#endif
    memcpy(&gEvse_Device_Config_read, &Evse_Device_Config_Buf_read, sizeof(evse_device_cfg_t));

    return 0;
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

int EvseDeviceConfigStorage::Device_Config_dump(void)
{
    Device_Config_read();
    // readFile(FFat, device_config_filename);
    Serial.printf("\r\nOCPP_config_keys_dump\r\n");
#if 0
    Serial.printf("-----------------------------------------------------------------------------\r\n");
    // Serial.printf("|index |config_key | /* config_key_length | */ access_R_RW | type_int_bool | uints|\r\n");
    Serial.printf("|index |config_key |  access_R_RW | type_int_bool | uints|\r\n");
    Serial.printf("-----------------------------------------------------------------------------\r\n");

    for (uint8_t idx = 0; idx < TOTAL_NUMBER_OF_COFIG; idx++)
    {
#if 0
        // Serial.printf("|  %d  | %.s       |  %d               |  %d         |  %d           |  %d  |\r\n",idx+1, gOCPP_config_keys_read.ocpp_std_config_keys[idx].config_key,
        Serial.printf("|%.03d| %.03d | %.01s |  %.01s | %.01s | %.01s |  %.01s | %.01d |  %.01s |\r\n", idx + 1, gEvse_Device_Config_read.connection_type,
                      /* gOCPP_config_keys_read.ocpp_std_config_keys[idx].config_key_length, */
                      gEvse_Device_Config_read.wifi_config.ssid,
                      gEvse_Device_Config_read.wifi_config.pswd,
                      gEvse_Device_Config_read.ocpp_config.url,
                      gEvse_Device_Config_read.ocpp_config.protocol,
                      gEvse_Device_Config_read.ocpp_config.host,
                      gEvse_Device_Config_read.ocpp_config.port,
                      gEvse_Device_Config_read.ocpp_config.resource_path);
#endif
#if 1 
        Serial.printf("gOCPP_config_keys_read.ocpp_std_config_keys[%d].config_key: %.s\r\n", idx, gOCPP_config_keys_read.ocpp_std_config_keys[idx].config_key);
        Serial.printf("gOCPP_config_keys_read.ocpp_std_config_keys[%d].config_key_length: %d\r\n", idx, gOCPP_config_keys_read.ocpp_std_config_keys[idx].config_key_length);
        Serial.printf("gOCPP_config_keys_read.ocpp_std_config_keys[%d].access_R_RW : %d\r\n", idx, gOCPP_config_keys_read.ocpp_std_config_keys[idx].access_R_RW);
        Serial.printf("gOCPP_config_keys_read.ocpp_std_config_keys[%d].type_int_bool : %d\r\n", idx, gOCPP_config_keys_read.ocpp_std_config_keys[idx].type_int_bool);
        Serial.printf("gOCPP_config_keys_read.ocpp_std_config_keys[%d].uints : %d\r\n", idx, gOCPP_config_keys_read.ocpp_std_config_keys[idx].uints);
#endif
    }
#endif

    Serial.printf("gEvse_Device_Config_read.connection_type: %d\r\n", gEvse_Device_Config_read.connection_type);
    Serial.printf("gEvse_Device_Config_read.wifi_config.ssid: %s\r\n", gEvse_Device_Config_read.wifi_config.ssid);
    Serial.printf("gEvse_Device_Config_read.wifi_config.pswd: %s\r\n", gEvse_Device_Config_read.wifi_config.pswd);
    Serial.printf("gEvse_Device_Config_read.ocpp_config.url: %s\r\n", gEvse_Device_Config_read.ocpp_config.url);
    Serial.printf("gEvse_Device_Config_read.ocpp_config.protocol: %s\r\n", gEvse_Device_Config_read.ocpp_config.protocol);
    Serial.printf("gEvse_Device_Config_read.ocpp_config.host: %s\r\n", gEvse_Device_Config_read.ocpp_config.host);
    Serial.printf("gEvse_Device_Config_read.ocpp_config.port: %d\r\n", gEvse_Device_Config_read.ocpp_config.port);
    Serial.printf("gEvse_Device_Config_read.ocpp_config.resource_path: %s\r\n", gEvse_Device_Config_read.ocpp_config.resource_path);
    Serial.printf("gEvse_Device_Config_write.ThresholdConfig.maxVoltage: %d\r\n", gEvse_Device_Config_write.ThresholdConfig.maxVoltage);
    Serial.printf("gEvse_Device_Config_write.ThresholdConfig.minVoltage: %d\r\n", gEvse_Device_Config_write.ThresholdConfig.minVoltage);
    Serial.printf("gEvse_Device_Config_write.ThresholdConfig.maxCurrent: %d\r\n", gEvse_Device_Config_write.ThresholdConfig.maxCurrent);
    Serial.printf("gEvse_Device_Config_write.ThresholdConfig.minCurrent: %d\r\n", gEvse_Device_Config_write.ThresholdConfig.minCurrent);
    Serial.printf("gEvse_Device_Config_write.ThresholdConfig.CTC: %d\r\n", gEvse_Device_Config_write.ThresholdConfig.CTC);
    Serial.printf("gEvse_Device_Config_write.ThresholdConfig.CTT: %d\r\n", gEvse_Device_Config_write.ThresholdConfig.CTT);
    Serial.printf("gEvse_Device_Config_write.ThresholdConfig.LPT: %d\r\n", gEvse_Device_Config_write.ThresholdConfig.LPT);
    Serial.printf("gEvse_Device_Config_write.ThresholdConfig.OTT: %d\r\n", gEvse_Device_Config_write.ThresholdConfig.OTT);
    Serial.printf("gEvse_Device_Config_write.ThresholdConfig.UTT: %d\r\n", gEvse_Device_Config_write.ThresholdConfig.UTT);
    Serial.printf("-----------------------------------------------------------------------------\r\n");
    return 0;
}

void EvseDeviceConfigStorage::Device_Config_ConnType_write(uint8_t connection_type)
{
    memcpy(&gEvse_Device_Config_write, &gEvse_Device_Config_read, sizeof(evse_device_cfg_t));

    gEvse_Device_Config_write.connection_type = connection_type;
    memcpy(&Evse_Device_Config_Buf_write, &gEvse_Device_Config_write, sizeof(evse_device_cfg_t));
#if SPI_FFS_USED
    evseDeviceCfgFileWrite(SPIFFS, device_config_filename);
#else
    evseDeviceCfgFileWrite(FFat, device_config_filename);
#endif
return;
}

void EvseDeviceConfigStorage::Device_Config_WifiCred_write(char *ssid, char *passwd)
{  
    memcpy(&gEvse_Device_Config_write, &gEvse_Device_Config_read, sizeof(evse_device_cfg_t)); 
    
    strcpy(gEvse_Device_Config_write.wifi_config.ssid, ssid);
    strcpy(gEvse_Device_Config_write.wifi_config.pswd, passwd);
    memcpy(&Evse_Device_Config_Buf_write, &gEvse_Device_Config_write, sizeof(evse_device_cfg_t));
#if SPI_FFS_USED
    evseDeviceCfgFileWrite(SPIFFS, device_config_filename);
#else
    evseDeviceCfgFileWrite(FFat, device_config_filename);
#endif
return;
}

void EvseDeviceConfigStorage::Device_Config_ThreshCfg_write(uint16_t maxVoltage, uint16_t minVoltage, uint16_t maxCurrent, uint16_t minCurrent, uint8_t CTC, uint8_t CTT, uint8_t LPT, uint8_t OTT, uint8_t UTT)
{
    memcpy(&gEvse_Device_Config_write, &gEvse_Device_Config_read, sizeof(evse_device_cfg_t));

    gEvse_Device_Config_write.ThresholdConfig.maxVoltage = maxVoltage;
    gEvse_Device_Config_write.ThresholdConfig.minVoltage = minVoltage;
    gEvse_Device_Config_write.ThresholdConfig.maxCurrent = maxCurrent;
    gEvse_Device_Config_write.ThresholdConfig.minCurrent = minCurrent;
    gEvse_Device_Config_write.ThresholdConfig.CTC = CTC;
    gEvse_Device_Config_write.ThresholdConfig.CTT = CTT;
    gEvse_Device_Config_write.ThresholdConfig.LPT = LPT;
    gEvse_Device_Config_write.ThresholdConfig.OTT = OTT;
    gEvse_Device_Config_write.ThresholdConfig.UTT = UTT;

    memcpy(&Evse_Device_Config_Buf_write, &gEvse_Device_Config_write, sizeof(evse_device_cfg_t));
#if SPI_FFS_USED
    evseDeviceCfgFileWrite(SPIFFS, device_config_filename);
#else
    evseDeviceCfgFileWrite(FFat, device_config_filename);
#endif
}

void EvseDeviceConfigStorage::Device_Config_ocppParam_write(String url, String protocol, String host, uint16_t port, String resource_path)
{
    memcpy(&gEvse_Device_Config_write, &gEvse_Device_Config_read, sizeof(evse_device_cfg_t));
    
    strcpy(gEvse_Device_Config_write.ocpp_config.url, url.c_str());
    strcpy(gEvse_Device_Config_write.ocpp_config.protocol, protocol.c_str());
    strcpy(gEvse_Device_Config_write.ocpp_config.host, host.c_str());
    strcpy(gEvse_Device_Config_write.ocpp_config.resource_path, resource_path.c_str());
    gEvse_Device_Config_write.ocpp_config.port = port;
    memcpy(&Evse_Device_Config_Buf_write, &gEvse_Device_Config_write, sizeof(evse_device_cfg_t));
#if SPI_FFS_USED
    evseDeviceCfgFileWrite(SPIFFS, device_config_filename);
#else
    evseDeviceCfgFileWrite(FFat, device_config_filename);
#endif
}

static int evseDeviceCfgFileWrite(fs::FS &fs, const char *path)
{
    size_t len = 0;
    // File file = fs.open(path, FILE_WRITE);
    dev_cfg_file = fs.open(path, FILE_WRITE);
    if (!dev_cfg_file)
    {
        Serial.println(" failed to open file for writing");
        return 1;
    }

    size_t i = 0;
    Serial.print("writing");
    uint32_t start = millis();
    // Serial.printf("\r\n(sizeof(evse_device_cfg_t) / 1024) : %d \r\n",(sizeof(evse_device_cfg_t) / 1024));
    for (i = 0; i < ((sizeof(evse_device_cfg_t) / 1024)); i++)
    {
        dev_cfg_file.write(&Evse_Device_Config_Buf_write[(i * 1024)], 1024);
    }

    uint16_t remain_len = ((sizeof(evse_device_cfg_t)) - ((sizeof(evse_device_cfg_t) / 1024) * 1024));

    dev_cfg_file.write(&Evse_Device_Config_Buf_write[((i * 1024))], remain_len);

    uint32_t end = millis() - start;
    Serial.printf("  %u bytes written in %u ms\r\n", (((sizeof(evse_device_cfg_t) / 1024) * 1024) + remain_len), end);
    dev_cfg_file.close();
return 0;
}

static int evseDeviceCfgFileRead(fs::FS &fs, const char *path)
{
    dev_cfg_file = fs.open(path);
    if (!dev_cfg_file)
    {
        Serial.println(" failed to open file for reading");
        return 1;
    }
    uint32_t start = millis();
    size_t len = 0;
    size_t i = 0;
    uint8_t idx = 0;
    if (dev_cfg_file && !dev_cfg_file.isDirectory())
    {
        len = dev_cfg_file.size();
        size_t flen = len;
        Serial.print("Size of file : ");
        Serial.println(len);
        Serial.print("reading");
        while (len)
        {
            size_t toRead = len;
            if (toRead > 1024)
            {
                toRead = 1024;
            }

            dev_cfg_file.read(&Evse_Device_Config_Buf_read[(idx * 1024)], toRead);

            len -= toRead;
            idx++; // it is used to inde the buffer with respective to 1024 chunk of the data.
                   // Serial.printf("- %u bytes readed \r\n", len);
        }

        uint32_t end = millis() - start;
        Serial.printf("\r\n- %u bytes read in %u ms\r\n", flen, end);
        dev_cfg_file.close();
    }
    else
    {
        Serial.println("\r\n- failed to open file for reading");
    }
return 0;
}
