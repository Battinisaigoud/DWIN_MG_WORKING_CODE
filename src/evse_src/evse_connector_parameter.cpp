#include "evse_connector_parameter.h"
#include "evse_cfg_file.h"

evseNconnectorparametersStorage gEVSEconnectorparametersStorage;

evse_Nconnector_parameters_t gEVSE_connectorparameters_write;
evse_Nconnector_parameters_t gEVSE_connectorparameters_read;

static uint8_t EVSE_connectorparameters_buf_write[sizeof(evse_Nconnector_parameters_t)];
static uint8_t EVSE_connectorparameters_buf_read[sizeof(evse_Nconnector_parameters_t)];

File evse_connector_root;
File evse_connector_file;

char *evse_connector_filename = "/evse_connector.txt";

static int evse_connectorFilewrite(fs::FS &fs, const char *path);
static int evse_connectorFileread(fs::FS &fs, const char *path);

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */

int evseNconnectorparametersStorage::evse_connector_parameters_init(void)
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
    evse_connector_root = SPIFFS.open("/");
    if (!evse_connector_root)
    {
        Serial.println(F("- failed to open directory"));
        return DB_FS_OPEN_ERROR;
    }
    if (!evse_connector_root.isDirectory())
    {
        Serial.println(F(" - not a directory"));
        return DB_FS_IS_DIR_ERROR;
    }
    evse_connector_file = evse_connector_root.openNextFile();
    while (evse_connector_file)
    {
        if (evse_connector_file.isDirectory())
        {
            Serial.print(F("  DIR : "));
            Serial.println(evse_connector_file.name());
        }
        else
        {
            Serial.print(F("  FILE: "));
            Serial.print(evse_connector_file.name());
            Serial.print(F("\tSIZE: "));
            Serial.println(evse_connector_file.size());
        }
        evse_connector_file = evse_connector_root.openNextFile();
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
        return DB_FS_MOUNT_ERROR;
    }

    fs::FS &fs = FFat;
    // File root = fs.open("/");
    evse_connector_root = fs.open("/");
    if (!evse_connector_root)
    {
        Serial.println("- failed to open directory");
        return DB_FS_OPEN_ERROR;
    }
    if (!evse_connector_root.isDirectory())
    {
        Serial.println(" - not a directory");
        return DB_FS_IS_DIR_ERROR;
    }

    // File file = evse_connector_root.openNextFile();
    evse_connector_file = evse_connector_root.openNextFile();
    while (evse_connector_file)
    {
        if (evse_connector_file.isDirectory())
        {
            Serial.print("  DIR : ");
            Serial.println(evse_connector_file.name());
            // if(levels){
            //     listDir(fs, file.name(), levels -1);
            // }
        }
        else
        {
            Serial.print("  FILE: ");
            Serial.print(evse_connector_file.name());
            Serial.print("\tSIZE: ");
            Serial.println(evse_connector_file.size());
        }
        evse_connector_file = evse_connector_root.openNextFile();
    }

#endif

    Serial.println(F("FREE HEAP"));
    Serial.println(ESP.getFreeHeap());

    return DB_NO_ERROR;
}

int evseNconnectorparametersStorage::evse_connector_parameters_write(void)
{
    memcpy(&EVSE_connectorparameters_buf_write, &gEVSE_connectorparameters_write, sizeof(evse_Nconnector_parameters_t));
#if SPI_FFS_USED
    evse_connectorFilewrite(SPIFFS, evse_connector_filename);
#else
    evse_connectorFilewrite(FFat, evse_connector_filename);
#endif
    return 0;
}

int evseNconnectorparametersStorage::evse_connector_parameters_read(void)
{
    memset(&gEVSE_connectorparameters_read, 0x0, sizeof(evse_Nconnector_parameters_t));
#if SPI_FFS_USED
    evse_connectorFileread(SPIFFS, evse_connector_filename);
#else
    evse_connectorFileread(FFat, evse_connector_filename);
#endif
    memcpy(&gEVSE_connectorparameters_read, &EVSE_connectorparameters_buf_read, sizeof(evse_Nconnector_parameters_t));

    return 0;
}

int evseNconnectorparametersStorage::evse_connector_parameters_dump(void)
{
    Serial.printf("\r\n evse_connector_parameters_dump\r\n");
    Serial.printf("-----------------------------------------------------------------------------\r\n");
    Serial.printf("|index |idTag_number |  ongoing_txn | txn_id | meter_value|\r\n");
    Serial.printf("-----------------------------------------------------------------------------\r\n");

    for (uint8_t idx = 0; idx < TOTAL_NUMBER_OF_CONNECTOR; idx++)
    {
        Serial.printf("|%.03d| %.s | %.01d |  %.01d | %.01d |\r\n", idx + 1, gEVSE_connectorparameters_read.evse_connector_parameters[idx].idTag_number,
                      gEVSE_connectorparameters_read.evse_connector_parameters[idx].ongoing_txn,
                      gEVSE_connectorparameters_read.evse_connector_parameters[idx].txn_id,
                      gEVSE_connectorparameters_read.evse_connector_parameters[idx].meter_value);
    }
    Serial.printf("-----------------------------------------------------------------------------\r\n");
    return 0;
}

int evseNconnectorparametersStorage::evse_connector_parameters_write_param(uint8_t *idTag_number, uint8_t id_len, uint8_t ongoing_txn, int32_t txn_id, uint32_t meter_value, uint8_t idx)
{
    // strncpy((char *)gEVSE_connectorparameters_write.evse_connector_parameters[idx].idTag_number, idTag_number, id_len);
    // for (uint8_t i = 0; i < id_len; i++)
    // {
    //     gEVSE_connectorparameters_write.evse_connector_parameters[idx].idTag_number[i], *(idTag_number + i);

    // }

    gEVSE_connectorparameters_write.evse_connector_parameters[idx].ongoing_txn = ongoing_txn;
    gEVSE_connectorparameters_write.evse_connector_parameters[idx].txn_id = txn_id;
    gEVSE_connectorparameters_write.evse_connector_parameters[idx].meter_value = meter_value;

    memcpy(&EVSE_connectorparameters_buf_write, &gEVSE_connectorparameters_write, sizeof(evse_Nconnector_parameters_t));
#if SPI_FFS_USED
    evse_connectorFilewrite(SPIFFS, evse_connector_filename);
#else
    evse_connectorFilewrite(FFat, evse_connector_filename);
#endif
}

/***************************************************************************************************************************/
static int evse_connectorFilewrite(fs::FS &fs, const char *path)
{

    size_t len = 0;
    // File file = fs.open(path, FILE_WRITE);
    evse_connector_file = fs.open(path, FILE_WRITE);
    if (!evse_connector_file)
    {
        Serial.println("failed to open file for writing");
        return 1;
    }

    size_t i = 0;
    Serial.print(" writing");
    uint32_t start = millis();
    // Serial.printf("\r\n(sizeof(evse_Nconnector_parameters_t) / 1024) : %d \r\n",(sizeof(evse_Nconnector_parameters_t) / 1024));
    for (i = 0; i < ((sizeof(evse_Nconnector_parameters_t) / 1024)); i++)
    {
        evse_connector_file.write(&EVSE_connectorparameters_buf_write[(i * 1024)], 1024);
    }
    uint16_t remain_len = ((sizeof(evse_Nconnector_parameters_t)) - ((sizeof(evse_Nconnector_parameters_t) / 1024) * 1024));

    evse_connector_file.write(&EVSE_connectorparameters_buf_write[((i * 1024))], remain_len);

    uint32_t end = millis() - start;
    Serial.printf("  %u bytes written in %u ms\r\n", (((sizeof(evse_Nconnector_parameters_t) / 1024) * 1024) + remain_len), end);
    Serial.printf("file_closed 1\r\n");
    evse_connector_file.close();
    evse_connector_file.close();
    Serial.printf("file_closed 2\r\n");
}

static int evse_connectorFileread(fs::FS &fs, const char *path)
{

    // File file = fs.open(path);
    evse_connector_file = fs.open(path);
    if (!evse_connector_file)
    {
        Serial.println(" failed to open file for reading");
        return 1;
    }
    uint32_t start = millis();
    size_t len = 0;
    size_t i = 0;
    uint8_t idx = 0;
    if (evse_connector_file && !evse_connector_file.isDirectory())
    {
        len = evse_connector_file.size();
        size_t flen = len;
        Serial.print("Size of file : ");
        Serial.println(len);
        Serial.print("− reading");
        while (len)
        {
            size_t toRead = len;
            if (toRead > 1024)
            {
                toRead = 1024;
            }

            evse_connector_file.read(&EVSE_connectorparameters_buf_read[(idx * 1024)], toRead);

            len -= toRead;
            idx++; // it is used to inde the buffer with respective to 1024 chunk of the data.
                   // Serial.printf("- %u bytes readed \r\n", len);
        }

        uint32_t end = millis() - start;
        Serial.printf("\r\n- %u bytes read in %u ms\r\n", flen, end);
        evse_connector_file.close();
    }
    else
    {
        Serial.println("\r\n- failed to open file for reading");
    }
}
/***************************************************************************************************************************/
