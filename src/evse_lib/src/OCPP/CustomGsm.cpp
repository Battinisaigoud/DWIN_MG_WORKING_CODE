#include "CustomGsm.h"
#include <Preferences.h>

// #include "../Evse_Preferences/Evse_Preferences.h"
#include "Variants.h"
#include "../../../evse_src/evse_addr_led.h"
#include "../../../evse_src/evse_config.h"

#include "../../../evse_src/evse_BLE_Commissioning.h"

// #include "evse_addr_led.h"
extern String ws_url_prefix_m;
extern String host_m;
extern int port_m;
extern String path_m;
extern Preferences preferences;
extern enum led_col_config evse_led_status; // enum variable.
extern enum evseconnectivitystatus EVSE_Connectivity_status;
extern EvseDevStatuse EvseDevStatus_connector_1;
extern int startBLETime;
extern EVSE_states_enum EVSE_states;
extern uint8_t gu8_change_avaliable;

uint8_t gu8_offline_online_status_flag = 0;

#define DUMP_AT_COMMANDS (0)
#if DUMP_AT_COMMANDS
#include <StreamDebugger.h>
// TinyGsm ota_modem(Serial2);
// StreamDebugger debugger(SerialAT, SerialMon);

StreamDebugger debugger1(SerialAT, SerialMon);
TinyGsm modem(debugger1);
TinyGsmClient client(modem);
// TinyGsm        ota_modem(debugger);
#else
TinyGsm modem(SerialAT);
TinyGsmClient client(modem);

#endif

extern uint8_t gu8_online_flag;
extern bool flag_ping_sent;
extern int client_reconnect_flag;
extern bool flagswitchoffBLE;

uint32_t gsmloopCount = 0;
uint32_t pongprintcount = 0;
#if SIMCOM_A7672S
#define RXD2 16
#define TXD2 17

// Added for SIMCOM A7670C
#elif SIMCOM_A7670C
#define RXD2 17
#define TXD2 16
#endif

#if DWIN_ENABLED
extern unsigned char clun[22];
extern unsigned char not_avail[22];
extern unsigned char avail[22];
extern unsigned char g[22];
extern unsigned char clear_avail[28];
#endif

extern uint8_t gu8_online_flag;

extern int read_led_status;

uint8_t white_led_count3 = 0;



void SetupGsm_4g()
{

    // Added for SIMCOM A7670C
    SerialAT.begin(115200, SERIAL_8N1, RXD2, TXD2);

    if (DEBUG_OUT)
        Serial.println("[CustomSIM7672] Starting 4G Setup");
    // TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);          // Commented for SIMCOM A7670C
    delay(200);
    if (DEBUG_OUT)
        Serial.println("[CustomSIM7672] Initializing modem...");
    modem.restart();

    String modemInfo = modem.getModemInfo();
    if (DEBUG_OUT)
        Serial.print("[CustomSIM7672] Modem Info: ");
    Serial.println(modemInfo);

#if TINY_GSM_USE_GPRS && defined TINY_GSM_MODEM_XBEE
    // The XBee must run the gprsConnect function BEFORE waiting for network!
    modem.gprsConnect(apn, gprsUser, gprsPass);
#endif

    if (DEBUG_OUT)
        SerialMon.println("[CustomSIM7672] Waiting for network...");
    // if (!modem.waitForNetwork()) {
    //  if (!modem.waitForNetwork(120000)) {
    if (!modem.waitForNetwork(20000))
    {
        if (DEBUG_OUT)
            SerialMon.println("[CustomSIM7672] fail");
        delay(200);
        return;
    }
    if (DEBUG_OUT)
        SerialMon.println("[CustomSIM7672] success");

    if (modem.isNetworkConnected())
    {
        if (DEBUG_OUT)
            SerialMon.println("[CustomSIM7672] Network connected");
    }

#if TINY_GSM_USE_GPRS
    // GPRS connection parameters are usually set after network registration
    if (DEBUG_OUT)
        SerialMon.print("[CustomSIM7672] Connecting to ");
    SerialMon.print(apn);
    if (!modem.gprsConnect(apn, gprsUser, gprsPass))
    {
        if (DEBUG_OUT)
            SerialMon.println("[CustomSIM7672] fail");
        delay(200);
        return;
    }
    if (DEBUG_OUT)
        SerialMon.println("[CustomSIM7672] success");

    if (modem.isGprsConnected())
    {
        if (DEBUG_OUT)
            SerialMon.println("[CustomSIM7672]GPRS connected");
    }

#endif
    int csq = modem.getSignalQuality();
    Serial.println("Signal quality: " + String(csq));
    delay(1);
}

void ConnectToServer_4g()
{
    host_m.trim();
    const char *host = (host_m).c_str();
    Serial.println(host);

    if (DEBUG_OUT)
        SerialMon.println("[CustomSIM7672] Connecting to ");
    SerialMon.println(String(host_m) + "\n" + String(port_m));
    if (!client.connect(host, port_m))
    {
        if (DEBUG_OUT)
            SerialMon.println("[CustomSIM7672] Connection request failed");
        delay(1000);
        return;
    }
    if (DEBUG_OUT)
        SerialMon.println("[CustomSIM7672] Connected");

    // // Make a HTTP GET request:
    //   Serial.println(ws_url_prefix_m);
    //   String url = String(ws_url_prefix_m);
    //   url = url.substring(6);
    //   int a = url.indexOf('/');
    //   url = url.substring(a);
    //   String cpSerial = String("");
    //   preferences.begin("credentials",false);
    //   EVSE_A_getChargePointSerialNumber(cpSerial);
    //   Serial.println(cpSerial);
    //   url += cpSerial;

    //   preferences.end();

    Serial.println("[[Custom SIM7672]: ]" + String(path_m));
    SerialMon.println("Performing HTTPS GET request...");
    client.print(String("GET ") + path_m + " HTTP/1.1\r\n");
    client.print(String("Host: ") + host_m + "\r\n");
    client.print("Upgrade: websocket\r\n");  // delay(300);
    client.print("Connection: Upgrade\r\n"); // delay(300);
    //  client.print("Origin: http://13.233.136.157:8080\r\n");//delay(300);
    client.print("Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==\r\n"); // delay(300);
    client.print("Sec-WebSocket-Protocol: ocpp1.6\r\n");             // delay(300);
                                                                     // client.print("Sec-WebSocket-Protocol: ocpp2.0, ocpp1.6\r\n");//delay(300);

#if EVSE_CONNECTION_KEEP_ALIVE
    client.print("Connection: keep-alive\r\n");
#endif

    client.print("Sec-WebSocket-Version: 13\r\n\r\n"); // delay(300);

    delay(100);

    // printSerialData();
    //    gsm_Loop_4g();
}

void printSerialData()
{
    while (client.available() != 0)
        Serial.println(client.readString());
}

// int counter_clientConnection = 0;
void gsm_Loop_4g_ble()
{

    uint8_t err = 0;
    if (!client.connected())
    {
        // evse_ChargePointStatus = Unavailable;
        evse_led_status = Blinky_White;
        // EVSE_Connectivity_status = EVSE_DISCONNECTED;
        Serial.println("[CustomSIM7672] Client is disconnected. Trying to connect");

        // if ((evse_ChargePointStatus != Faulted) &&
        //     (evse_ChargePointStatus != Charging) &&
        //     (evse_ChargePointStatus != Preparing) &&
        //     (evse_ChargePointStatus != Finishing) &&
        //     (evse_ChargePointStatus != SuspendedEVSE) &&
        //     (evse_ChargePointStatus != SuspendedEV))
        // {

        //     if (EVSE_transaction_status == EVSE_START_TRANSACTION)
        //     {
        //         evse_ChargePointStatus = Charging;
        //     }
        // }
        gu8_online_flag = 0;
#if 0
         if ((millis() - startBLETime < TIMEOUT_BLE)&& (flagswitchoffBLE == false))
        {
            Serial.println(F("[CustomSIM7672] BLE loop"));
            BLE_loop();    
        }
#endif
        delay(100); // do not flood the network (have to create millis)
        SetupGsm_4g();
        ConnectToServer_4g();

        // BLE_loop();
    }
    else
    {

        Serial.println(F("[Custom SIM7672] Client Connected"));
        if (gu8_online_flag == 0)
        {
            gu8_online_flag = 1;
            if (EVSE_transaction_status == EVSE_START_TRANSACTION)
            {
                evse_ChargePointStatus = Charging;
            }
            else
            {
                evse_ChargePointStatus = NOT_SET;
            }
        }

        else
        {
            EVSE_Connectivity_status = EVSE_CONNECTED;
            if ((evse_ChargePointStatus != Faulted) &&
                (evse_ChargePointStatus != Charging) &&
                (evse_ChargePointStatus != Preparing) &&
                (evse_ChargePointStatus != Finishing) &&
                (evse_ChargePointStatus != SuspendedEVSE) &&
                (evse_ChargePointStatus != SuspendedEV))
            {

                // if (EvseDevStatus_connector_1 != flag_EVSE_is_Booted)
                // {
                //     evse_ChargePointStatus = Available;
                // }

                // if(EVSE_transaction_status == EVSE_START_TRANSACTION)
                // {
                //     evse_ChargePointStatus = Charging;
                // }

                if (gu8_change_avaliable == EVSE_CMS_CHANGE_AVAILABLITY_TWO)
                {

                    evse_ChargePointStatus = Unavailable;
                }
                // Serial.print("Available3");
            }

            // gu8_online_flag = 1;
        }
    }
#if 0 // for testing perpus
          if ((millis() - startBLETime < TIMEOUT_BLE)&& (flagswitchoffBLE == false))
        {
            Serial.println(F("[CustomSIM7672] BLE loop"));
            BLE_loop();    
        }
#endif
    // BLE_loop();
    gsmOnEvent_4g();
}

void gsm_Loop_4g()
{

    uint8_t err = 0;
    if (!client.connected())
    {
        // evse_ChargePointStatus = Unavailable;
        evse_led_status = Blinky_White;

        Serial.println("[CustomSIM7672] Client is disconnected. Trying to connect");
        gu8_online_flag = 0;

        gsmloopCount = 1;

        delay(100); // do not flood the network (have to create millis)
        SetupGsm_4g();
        ConnectToServer_4g();
    }
    else
    {

        if (gsmloopCount == 1)
        {
            gsmloopCount = 0;
            Serial.println("[Custom SIM7672] Client Connected");
        }
        if (gu8_online_flag == 0)
        {
            gu8_online_flag = 1;
            evse_ChargePointStatus = NOT_SET;
            gu8_offline_online_status_flag = 1;

            // if ((EVSE_transaction_status == EVSE_START_TRANSACTION) && (evse_ChargePointStatus != Faulted))
            // {

            //     evse_ChargePointStatus = Charging;
            // }
            // else
            // {
            //     evse_ChargePointStatus = NOT_SET;
            // }
            // Serial.println("not set>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
        }
        else
        {
            if (gu8_offline_online_status_flag == 1)
            {
                if ((EVSE_states == STATE_C) && (EVSE_transaction_status == EVSE_START_TRANSACTION) && (evse_ChargePointStatus != Faulted))
                {
                    evse_ChargePointStatus = Charging;
                    gu8_offline_online_status_flag = 0;
                }
                else if ((EVSE_states == STATE_SUS) && (EVSE_transaction_status == EVSE_START_TRANSACTION) && (evse_ChargePointStatus != Faulted))
                {
                    evse_ChargePointStatus = SuspendedEV;
                    gu8_offline_online_status_flag = 0;
                }
            }
            EVSE_Connectivity_status = EVSE_CONNECTED;
            if ((evse_ChargePointStatus != Faulted) &&
                (evse_ChargePointStatus != Charging) &&
                (evse_ChargePointStatus != Preparing) &&
                (evse_ChargePointStatus != Finishing) &&
                (evse_ChargePointStatus != SuspendedEVSE) &&
                (evse_ChargePointStatus != SuspendedEV) &&
                (evse_ChargePointStatus != Unavailable))
            {

                if (gu8_change_avaliable == EVSE_CMS_CHANGE_AVAILABLITY_TWO)
                {

                    evse_ChargePointStatus = Unavailable;
                }
            }
        }
    }

    gsmOnEvent_4g();
}

void processJson(String &IncomingDataString)
{
    int index1 = -1;
    int index2 = -1;
    int len;
    uint8_t *payload;
    size_t DataLen;
    String DataString;

    int csq = modem.getSignalQuality();
    Serial.println("Signal quality: " + String(csq));

    len = IncomingDataString.length();
    if (DEBUG_EXTRA)
        Serial.println("\nLen-->" + String(len));
    IncomingDataString.trim();
    len = IncomingDataString.length();
    if (DEBUG_EXTRA)
        Serial.println("\nAfter trim Len-->" + String(len));

    index1 = IncomingDataString.indexOf('[');
    index2 = IncomingDataString.lastIndexOf(']');
    delay(250);
    if (DEBUG_EXTRA)
         Serial.println(IncomingDataString);
        //  delay(1000);
        // index3 = IncomingDataString.indexOf(']');
        if (DEBUG_EXTRA)
             Serial.println("");
            if (DEBUG_EXTRA)
                Serial.println("Index 1--->" + String(index1));
                if (DEBUG_EXTRA)
                     Serial.println("Index 2--->" + String(index2));

                    if (index1 != -1 && index2 != -1)
                    {

                        DataString = IncomingDataString.substring(index1, index2 + 1);
                        DataLen = DataString.length();

                        if (DEBUG_EXTRA)
                                Serial.println(DataString + "\n Length:" + String(DataLen));
                            Serial.println("[CustomSIM7672] Wsc_Txt: " + DataString);

                            payload = (uint8_t *)DataString.c_str();

                        if (!processWebSocketEvent(payload, DataLen))
                        { // forward message to OcppEngine
                            if (DEBUG_OUT)
                                Serial.print("[CustomSIM7672] Processing WebSocket input event failed!\n");
                        }

                        IncomingDataString = ""; // Clearing the local buffer
                    }
    // if(DEBUG_EXTRA) Serial.println("It's not Json packet");
}

void printingUint(String IncomingDataString)
{
    int length = IncomingDataString.length();

    uint8_t *payload = (uint8_t *)IncomingDataString.c_str();

    Serial.println("");
    Serial.print("Payload Size-->" + String(sizeof(payload)));
    for (int i = 0; i < length; i++)
    {
        Serial.print(" ");
        Serial.print(payload[i], DEC);
    }
    Serial.println("");
}

void gsmOnEvent_4g()
{
    bool flagWebSktHdrTxt = 0;
    bool flagCounter = 0;
    bool flagJsonPkt = 1;
    char c;
    char header;
    int CounterOpenBracket = 0;
    int CounterCloseBracket = 0;
    String IncomingDataString = "";
    const char *pongResp = "rockybhai";
    uint8_t len1 = strlen(pongResp);
    uint8_t sum1 = 0;
    while (client.connected() && client.available())
    {
        c = client.read();
        if (c == 129 && flagJsonPkt == 1)
        { // Reading Headers
            flagJsonPkt = 0;
            flagWebSktHdrTxt = 1;
        }

        sum1 = (c == pongResp[sum1]) ? sum1 + 1 : 0;
        if (sum1 == len1)
        {
            flag_ping_sent = false;
            pongprintcount++;

            Serial.println("pong received!");

            EVSE_Connectivity_status = EVSE_CONNECTED;
            break;
        }

        if (flagWebSktHdrTxt == 1)
        {
            flagWebSktHdrTxt = 0;
            header = client.read(); // reading payload length
            if (header >= 126)
            {
                client.read(); // flushing Extra headers
                client.read();
            }
        }
        else
        { // End of Headers
            IncomingDataString += c;
            if (c == '[')
            {
                CounterOpenBracket++;
                flagCounter = 1;
            }
            if (c == ']')
                CounterCloseBracket++;

            if (CounterOpenBracket == CounterCloseBracket && flagCounter == 1)
            {

                if (DEBUG_EXTRA)
                    Serial.print(c);
                if (DEBUG_EXTRA)
                    printingUint(IncomingDataString);
                processJson(IncomingDataString);
                CounterOpenBracket = 0;
                flagCounter = 0;
                CounterCloseBracket = 0;
                flagJsonPkt = 1;
                flagWebSktHdrTxt = 0;
            }
            else
            {

                // if (1)
                Serial.print(c);
            }
        }
    }
}


/***********************************************************************************************/
bool sendTXTGsmStr(String &payload)
{

    return sendTXTGsm((uint8_t *)payload.c_str(), payload.length());
}

bool sendTXTGsm(uint8_t *payload, size_t length, bool headerToPayload)
{

    if (length == 0)
    {
        length = strlen((const char *)payload);
    }
    if (true)
    {
        return sendFrame(WSop_text, payload, length, true, headerToPayload);
    }
    return false;
}

bool sendPingGsmStr(String &payload)
{

    return sendPingGsm((uint8_t *)payload.c_str(), payload.length(), false);
}

bool sendPingGsm(uint8_t *payload, size_t length, bool headerToPayload)
{

    if (length == 0)
    {
        length = strlen((const char *)payload);
    }
    if (true)
    {
        return sendFrame(WSop_ping, payload, length, true, headerToPayload);
    }
    return false;
}

uint8_t createHeader(uint8_t *headerPtr, WSopcode_t opcode, size_t length, bool mask, uint8_t maskKey[4], bool fin)
{
    uint8_t headerSize;
    // calculate header Size
    if (length < 126)
    {
        headerSize = 2;
    }
    else if (length < 0xFFFF)
    {
        headerSize = 4;
    }
    else
    {
        headerSize = 10;
    }

    if (mask)
    {
        headerSize += 4;
    }

    // create header

    // byte 0
    *headerPtr = 0x00;
    if (fin)
    {
        *headerPtr |= bit(7); ///< set Fin
    }
    *headerPtr |= opcode; ///< set opcode
    headerPtr++;

    // byte 1
    *headerPtr = 0x00;
    if (mask)
    {
        *headerPtr |= bit(7); ///< set mask
    }

    if (length < 126)
    {
        *headerPtr |= length;
        headerPtr++;
    }
    else if (length < 0xFFFF)
    {
        *headerPtr |= 126;
        headerPtr++;
        *headerPtr = ((length >> 8) & 0xFF);
        headerPtr++;
        *headerPtr = (length & 0xFF);
        headerPtr++;
    }
    else
    {
        // Normally we never get here (to less memory)
        *headerPtr |= 127;
        headerPtr++;
        *headerPtr = 0x00;
        headerPtr++;
        *headerPtr = 0x00;
        headerPtr++;
        *headerPtr = 0x00;
        headerPtr++;
        *headerPtr = 0x00;
        headerPtr++;
        *headerPtr = ((length >> 24) & 0xFF);
        headerPtr++;
        *headerPtr = ((length >> 16) & 0xFF);
        headerPtr++;
        *headerPtr = ((length >> 8) & 0xFF);
        headerPtr++;
        *headerPtr = (length & 0xFF);
        headerPtr++;
    }

    if (mask)
    {
        *headerPtr = maskKey[0];
        headerPtr++;
        *headerPtr = maskKey[1];
        headerPtr++;
        *headerPtr = maskKey[2];
        headerPtr++;
        *headerPtr = maskKey[3];
        headerPtr++;
    }
    return headerSize;
}

bool sendFrame(WSopcode_t opcode, uint8_t *payload, size_t length, bool fin, bool headerToPayload)
{

    if (!client.connected())
    {

        if (DEBUG_OUT)
            Serial.println(F("[CustomSIM7672] Client is not connected"));
        return false;
    }

    /*
      if(client->tcp && !client->tcp->connected()) {
          DEBUG_WEBSOCKETS("[WS][%d][sendFrame] not Connected!?\n", client->num);
          return false;
      }

      if(client->status != WSC_CONNECTED) {
          DEBUG_WEBSOCKETS("[WS][%d][sendFrame] not in WSC_CONNECTED state!?\n", client->num);
          return false;
      }
  */
    //   DEBUG_WEBSOCKETS("[WS][%d][sendFrame] ------- send message frame -------\n", client->num);
    //   DEBUG_WEBSOCKETS("[WS][%d][sendFrame] fin: %u opCode: %u mask: %u length: %u headerToPayload: %u\n", client->num, fin, opcode, true, length, headerToPayload);

    //   if(opcode == WSop_text) {
    //       DEBUG_WEBSOCKETS("[WS][%d][sendFrame] text: %s\n", client->num, (payload + (headerToPayload ? 14 : 0)));
    //   }

    uint8_t maskKey[4] = {0x00, 0x00, 0x00, 0x00};
    uint8_t buffer[WEBSOCKETS_MAX_HEADER_SIZE] = {0};

    uint8_t headerSize;
    uint8_t *headerPtr;
    uint8_t *payloadPtr = payload;
    bool useInternBuffer = false;
    bool ret = true;

    // calculate header Size
    if (length < 126)
    {
        headerSize = 2;
    }
    else if (length < 0xFFFF)
    {
        headerSize = 4;
    }
    else
    {
        headerSize = 10;
    }

    if (true)
    {
        headerSize += 4;
    }

#ifdef WEBSOCKETS_USE_BIG_MEM

    // only for ESP since AVR has less HEAP
    // try to send data in one TCP package (only if some free Heap is there)
    if (!headerToPayload && ((length > 0) && (length < 1400)) && (GET_FREE_HEAP > 6000))
    {
        //  DEBUG_WEBSOCKETS("[WS][%d][sendFrame] pack to one TCP package...\n", client->num);
        uint8_t *dataPtr = (uint8_t *)malloc(length + WEBSOCKETS_MAX_HEADER_SIZE);
        if (dataPtr)
        {
            memcpy((dataPtr + WEBSOCKETS_MAX_HEADER_SIZE), payload, length);
            headerToPayload = true;
            useInternBuffer = true;
            payloadPtr = dataPtr;
        }
    }
#endif

    // set Header Pointer
    if (headerToPayload)
    {
        // calculate offset in payload
        headerPtr = (payloadPtr + (WEBSOCKETS_MAX_HEADER_SIZE - headerSize));
    }
    else
    {
        headerPtr = &buffer[0];
    }

    if (true && useInternBuffer)
    {
        // if we use a Intern Buffer we can modify the data
        // by this fact its possible the do the masking
        for (uint8_t x = 0; x < sizeof(maskKey); x++)
        {
            maskKey[x] = random(0xFF);
        }
    }

    createHeader(headerPtr, opcode, length, true, maskKey, fin);

    if (true && useInternBuffer)
    {
        uint8_t *dataMaskPtr;

        if (headerToPayload)
        {
            dataMaskPtr = (payloadPtr + WEBSOCKETS_MAX_HEADER_SIZE);
        }
        else
        {
            dataMaskPtr = payloadPtr;
        }

        for (size_t x = 0; x < length; x++)
        {
            dataMaskPtr[x] = (dataMaskPtr[x] ^ maskKey[x % 4]);
        }
    }

#ifndef NODEBUG_WEBSOCKETS
    unsigned long start = micros();
#endif

    if (headerToPayload)
    {
        // header has be added to payload
        // payload is forced to reserved 14 Byte but we may not need all based on the length and mask settings
        // offset in payload is calculatetd 14 - headerSize
        if (client.write(&payloadPtr[(WEBSOCKETS_MAX_HEADER_SIZE - headerSize)], (length + headerSize)) != (length + headerSize))
        {
            ret = false;
        }
    }
    else
    {
        // send header
        if (client.write(&buffer[0], headerSize) != headerSize)
        {
            ret = false;
        }

        if (payloadPtr && length > 0)
        {
            // send payload
            if (client.write(&payloadPtr[0], length) != length)
            {
                ret = false;
            }
        }
    }

    //  DEBUG_WEBSOCKETS("[WS][%d][sendFrame] sending Frame Done (%luus).\n", client->num, (micros() - start));
    // mySerial.write("\r\n");
    // mySerial.write(0x1a);
    // mySerial.flush();
    delay(100);
// printSerialData();
#ifdef WEBSOCKETS_USE_BIG_MEM
    if (useInternBuffer && payloadPtr)
    {
        free(payloadPtr);
    }
#endif
    if (DEBUG_OUT)
        Serial.println("[CustomGsm-frame] Return : " + String(ret));
    return ret;
}
