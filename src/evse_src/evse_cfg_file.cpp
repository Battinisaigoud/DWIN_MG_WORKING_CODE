#include "evse_cfg_file.h"

OCPPconfigkeyStorage gOCPPConfigkeyStorage; // no of Id Tags

// static ocpp_config_keys_t lOCPP_config_keys_write;
// static ocpp_config_keys_t lOCPP_config_keys_read;

ocpp_config_keys_t gOCPP_config_keys_write;
ocpp_config_keys_t gOCPP_config_keys_read;

static uint8_t OCPP_config_keys_buf_write[sizeof(ocpp_config_keys_t)];
static uint8_t OCPP_config_keys_buf_read[sizeof(ocpp_config_keys_t)];

// We need to create a linkedlist of type strings to return RFID tags
//  Create a Linklist for
StoredConfigKeys_t StoredConfigKeys;

// Create a Linklist for
LinkedList<StoredConfigKeys_t> LL_Local_StoredConfig_List = LinkedList<StoredConfigKeys_t>();

File cfg_root;
File cfg_file;

#if 0
#if SPI_FFS_USED
char *ocpp_config_keys_filename = "/ocppconfigkey.txt";
#else
char *ocpp_config_keys_filename = "/ocppconfigkey.txt";
#endif
#endif

char *ocpp_config_keys_filename = "/ocppconfigkey.txt";

static int ocppcfgFilewrite(fs::FS &fs, const char *path);
static int ocppcfgFileread(fs::FS &fs, const char *path);

extern volatile uint8_t evse_fatfs_init_flag;

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */

int OCPPconfigkeyStorage::OCPP_config_keys_init(void)
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
    cfg_root = SPIFFS.open("/");
    if (!cfg_root)
    {
        Serial.println(F("- failed to open directory"));
        return DB_FS_OPEN_ERROR;
    }
    if (!cfg_root.isDirectory())
    {
        Serial.println(F(" - not a directory"));
        return DB_FS_IS_DIR_ERROR;
    }
    cfg_file = cfg_root.openNextFile();
    while (cfg_file)
    {
        if (cfg_file.isDirectory())
        {
            Serial.print(F("  DIR : "));
            Serial.println(cfg_file.name());
        }
        else
        {
            Serial.print(F("  FILE: "));
            Serial.print(cfg_file.name());
            Serial.print(F("\tSIZE: "));
            Serial.println(cfg_file.size());
        }
        cfg_file = cfg_root.openNextFile();
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
        mount_count = 2;
        if (FORMAT_FFAT) FFat.format();
        while ((!FFat.begin()) && mount_count)
        {

            mount_count--;
            Serial.println(F("Failed to mount file system"));
        }
         size_t len = 0;
        cfg_file = FFat.open(ocpp_config_keys_filename, FILE_WRITE);
        if (!cfg_file)
        {
            Serial.println("\r\n - failed to open file for writing");
            return 1;
        }
        cfg_file.close();
 
        //return DB_FS_MOUNT_ERROR;
    }

    fs::FS &fs = FFat;
    // File root = fs.open("/");
    cfg_root = fs.open("/");
    if (!cfg_root)
    {
        Serial.println("- failed to open directory");
        return DB_FS_OPEN_ERROR;
    }
    if (!cfg_root.isDirectory())
    {
        Serial.println(" - not a directory");


        return DB_FS_IS_DIR_ERROR;
    }

    // File file = cfg_root.openNextFile();
    cfg_file = cfg_root.openNextFile();
    while (cfg_file)
    {
        if (cfg_file.isDirectory())
        {
            Serial.print("  DIR : ");
            Serial.println(cfg_file.name());
            // if(levels){
            //     listDir(fs, file.name(), levels -1);
            // }
        }
        else
        {
            Serial.print("  FILE: ");
            Serial.print(cfg_file.name());
            Serial.print("\tSIZE: ");
            Serial.println(cfg_file.size());
        }
        cfg_file = cfg_root.openNextFile();
    }

#endif

    Serial.println(F("FREE HEAP"));
    Serial.println(ESP.getFreeHeap());

    return DB_NO_ERROR;
}

int OCPPconfigkeyStorage::OCPP_config_keys_write(void)
{
    memcpy(&OCPP_config_keys_buf_write, &gOCPP_config_keys_write, sizeof(ocpp_config_keys_t));
#if SPI_FFS_USED
    ocppcfgFilewrite(SPIFFS, ocpp_config_keys_filename);
#else
    ocppcfgFilewrite(FFat, ocpp_config_keys_filename);
#endif
    return 0;
}

int OCPPconfigkeyStorage::OCPP_config_keys_read(void)
{
    memset(&gOCPP_config_keys_read, 0x0, sizeof(ocpp_config_keys_t));
#if SPI_FFS_USED
    ocppcfgFileread(SPIFFS, ocpp_config_keys_filename);
#else
    ocppcfgFileread(FFat, ocpp_config_keys_filename);
#endif
    memcpy(&gOCPP_config_keys_read, &OCPP_config_keys_buf_read, sizeof(ocpp_config_keys_t));

    return 0;
}

int OCPPconfigkeyStorage::OCPP_config_keys_dump(void)
{
    Serial.printf("\r\nOCPP_config_keys_dump\r\n");
    Serial.printf("-----------------------------------------------------------------------------\r\n");
    // Serial.printf("|index |config_key | /* config_key_length | */ access_R_RW | type_int_bool | uints|\r\n");
    Serial.printf("|index |config_key |  access_R_RW | type_int_bool | uints|\r\n");
    Serial.printf("-----------------------------------------------------------------------------\r\n");

    for (uint8_t idx = 0; idx < TOTAL_NUMBER_OF_COFIG_KEYS; idx++)
    {
        // Serial.printf("|  %d  | %.s       |  %d               |  %d         |  %d           |  %d  |\r\n",idx+1, gOCPP_config_keys_read.ocpp_std_config_keys[idx].config_key,
        Serial.printf("|%.03d| %.03d | %.01d |  %.01d | %.01d |\r\n", idx + 1, gOCPP_config_keys_read.ocpp_std_config_keys[idx].config_key,
                      /* gOCPP_config_keys_read.ocpp_std_config_keys[idx].config_key_length, */
                      gOCPP_config_keys_read.ocpp_std_config_keys[idx].access_R_RW,
                      gOCPP_config_keys_read.ocpp_std_config_keys[idx].type_int_bool,
                      gOCPP_config_keys_read.ocpp_std_config_keys[idx].uints);

#if 0 
        Serial.printf("gOCPP_config_keys_read.ocpp_std_config_keys[%d].config_key: %.s\r\n", idx, gOCPP_config_keys_read.ocpp_std_config_keys[idx].config_key);
        Serial.printf("gOCPP_config_keys_read.ocpp_std_config_keys[%d].config_key_length: %d\r\n", idx, gOCPP_config_keys_read.ocpp_std_config_keys[idx].config_key_length);
        Serial.printf("gOCPP_config_keys_read.ocpp_std_config_keys[%d].access_R_RW : %d\r\n", idx, gOCPP_config_keys_read.ocpp_std_config_keys[idx].access_R_RW);
        Serial.printf("gOCPP_config_keys_read.ocpp_std_config_keys[%d].type_int_bool : %d\r\n", idx, gOCPP_config_keys_read.ocpp_std_config_keys[idx].type_int_bool);
        Serial.printf("gOCPP_config_keys_read.ocpp_std_config_keys[%d].uints : %d\r\n", idx, gOCPP_config_keys_read.ocpp_std_config_keys[idx].uints);
#endif
    }
    Serial.printf("-----------------------------------------------------------------------------\r\n");
    return 0;
}

int OCPPconfigkeyStorage::OCPP_config_keys_write_param(uint8_t config_key, uint8_t access_R_RW, int32_t type_int_bool, uint32_t uints, uint8_t idx)
{

    memcpy(&gOCPP_config_keys_write, &gOCPP_config_keys_read, sizeof(ocpp_config_keys_t));

    gOCPP_config_keys_write.ocpp_std_config_keys[idx].config_key = config_key;
    gOCPP_config_keys_write.ocpp_std_config_keys[idx].access_R_RW = access_R_RW;
    gOCPP_config_keys_write.ocpp_std_config_keys[idx].type_int_bool = type_int_bool;
    gOCPP_config_keys_write.ocpp_std_config_keys[idx].uints = uints;

    memcpy(&OCPP_config_keys_buf_write, &gOCPP_config_keys_write, sizeof(ocpp_config_keys_t));
#if SPI_FFS_USED
    ocppcfgFilewrite(SPIFFS, ocpp_config_keys_filename);
#else
    ocppcfgFilewrite(FFat, ocpp_config_keys_filename);
#endif
}

/***************************************************************************************************************************/
static int ocppcfgFilewrite(fs::FS &fs, const char *path)
{

    size_t len = 0;
    // File file = fs.open(path, FILE_WRITE);
    cfg_file = fs.open(path, FILE_WRITE);
    if (!cfg_file)
    {
        Serial.println("\r\n - failed to open file for writing");
        return 1;
    }

    size_t i = 0;
    Serial.print("- writing");
    uint32_t start = millis();
    // Serial.printf("\r\n(sizeof(ocpp_config_keys_t) / 1024) : %d \r\n",(sizeof(ocpp_config_keys_t) / 1024));
    for (i = 0; i < ((sizeof(ocpp_config_keys_t) / 1024)); i++)
    {
        cfg_file.write(&OCPP_config_keys_buf_write[(i * 1024)], 1024);
    }
    uint16_t remain_len = ((sizeof(ocpp_config_keys_t)) - ((sizeof(ocpp_config_keys_t) / 1024) * 1024));

    cfg_file.write(&OCPP_config_keys_buf_write[((i * 1024))], remain_len);

    uint32_t end = millis() - start;
    Serial.printf("\r\n - %u bytes written in %u ms\r\n", (((sizeof(ocpp_config_keys_t) / 1024) * 1024) + remain_len), end);
    cfg_file.close();
    return 0;
}

static int ocppcfgFileread(fs::FS &fs, const char *path)
{

    // File file = fs.open(path);
    cfg_file = fs.open(path);
    if (!cfg_file)
    {
        Serial.println("- failed to open file for reading");
        return 1;
    }
    uint32_t start = millis();
    size_t len = 0;
    size_t i = 0;
    uint8_t idx = 0;
    if (cfg_file && !cfg_file.isDirectory())
    {
        len = cfg_file.size();
        size_t flen = len;
        Serial.print("Size of file : ");
        Serial.println(len);
        Serial.print("- reading");
        while (len)
        {
            size_t toRead = len;
            if (toRead > 1024)
            {
                toRead = 1024;
            }

            cfg_file.read(&OCPP_config_keys_buf_read[(idx * 1024)], toRead);

            len -= toRead;
            idx++; // it is used to inde the buffer with respective to 1024 chunk of the data.
                   // Serial.printf("- %u bytes readed \r\n", len);
        }

        uint32_t end = millis() - start;
        Serial.printf("\r\n- %u bytes read in %u ms\r\n", flen, end);
        cfg_file.close();
    }
    else
    {
        Serial.println("\r\n- failed to open file for reading");
    }
    return 0;
}

uint8_t ocpp_std_write(uint32_t ocpp_key_uints, uint8_t idx)
{
  uint8_t config_key = 0;
  uint8_t access_R_RW = 0; 
  int32_t type_int_bool = 0; 
  uint32_t uints = 0; 
  switch( idx)
  {
    case HEARTBEATINTERVAL:
        config_key = HEARTBEATINTERVAL;
        access_R_RW = READ_WRITE;
        type_int_bool = TYPE_INTEGER;
        uints = ocpp_key_uints;
        break;
        
    default:
        break;

  }
  gOCPPConfigkeyStorage.OCPP_config_keys_write_param(config_key, access_R_RW, type_int_bool, uints, idx); 
  
}

uint8_t ocpp_std_read(void)
{
  gOCPPConfigkeyStorage.OCPP_config_keys_read();
}

/***************************************************************************************************************************/
