#include "evse_espnow_master.h"

#include "evse_config.h"
#include "../evse_lib/src/OCPP/ChargePointStatusService.h"

extern enum EvseDevStatuse EvseDevStatus_connector_1;
extern uint8_t gu8_evse_change_state;
extern EVSE_states_enum EVSE_states;
extern enum evse_authorize_stat_t evse_authorize_state;
extern enum ChargePointStatus evse_ChargePointStatus;

uint8_t slaveAddress1[] = {0x0C, 0xB8, 0x15, 0xED, 0x20, 0x34};
uint8_t slaveAddress2[] = {0xA0, 0xA3, 0xB3, 0x2A, 0x4F, 0xE8};
// uint8_t slaveAddress3[] = {0x40, 0x91, 0x51, 0xC5, 0x3D, 0x58};

// uint8_t slaveAddress1[] = {0C, B8, 15, ED, 20, 34};
// uint8_t slaveAddress2[] = {CC, DB, A7, 5B, 70, 6C};

extern uint8_t flag_rfid;

String alpr_data = "";

esp_now_peer_info_t slave;
int chan = 0;
enum MessageType
{
  PAIRING,
  DATA,
};
MessageType messageType;

uint8_t alpr_device_id = 0;

// Must match the receiver structure
// Structure ALPRDATA to send alpr data
#pragma pack(1)
struct GETALPRDATA
{
  uint8_t msgType;
  char header1[2];
  uint8_t deviceID;
  uint8_t packTYPE;
  uint8_t packLEN;
  uint16_t checksum;
  char footer[2];
};
GETALPRDATA get_alpr_data;

// Structure RECIEVEDDATA getting data from another node
#pragma pack(1)
struct RECIEVEDDATA
{
  uint8_t msgType;
  char header1[1];
  uint8_t deviceID;
  uint8_t packTYPE;
  uint8_t packLEN;
  char packDATA[15];
  uint16_t checksum;
  char footer[1];
};
RECIEVEDDATA recieved_data;

typedef struct struct_pairing
{ // new structure for pairing
  uint8_t msgType;
  uint8_t id;
  uint8_t macAddr[6];
  uint8_t channel;
} struct_pairing;
struct_pairing pairingData;

// ---------------------------- esp_ now -------------------------
void printMAC(const uint8_t *mac_addr)
{
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
}

bool addPeer(const uint8_t *peer_addr)
{ // add pairing
  memset(&slave, 0, sizeof(slave));
  const esp_now_peer_info_t *peer = &slave;
  memcpy(slave.peer_addr, peer_addr, 6);

  slave.channel = chan; // pick a channel
  slave.encrypt = 0;    // no encryption
  // check if the peer exists
  bool exists = esp_now_is_peer_exist(slave.peer_addr);
  if (exists)
  {
    // Slave already paired.
    Serial.println("Already Paired");
    return true;
  }
  else
  {
    esp_err_t addStatus = esp_now_add_peer(peer);
    if (addStatus == ESP_OK)
    {
      // Pair success
      Serial.println("Pair success");
      return true;
    }
    else
    {
      Serial.println("Pair failed");
      return false;
    }
  }
}

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("Last Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success to " : "Delivery Fail");
  printMAC(mac_addr);
  Serial.println();
}

// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{

  Serial.print(len);
  Serial.print(" bytes of data received from : ");
  printMAC(mac);

  uint8_t type = incomingData[0]; // first message byte is the type of message
  switch (type)
  {
  case DATA:
    memcpy(&recieved_data, incomingData, sizeof(recieved_data));
    // Serial.println(recieved_data.msgType);
    // Serial.println(recieved_data.header1);
    // Serial.println(recieved_data.deviceID);
    // Serial.println(recieved_data.packTYPE);
    // Serial.println(recieved_data.packLEN);
    // Serial.println(recieved_data.packDATA);
    // Serial.println(recieved_data.checksum);
    // Serial.println(recieved_data.footer);

    Read_Response(len);
    // read_Data();
    break;

  case PAIRING:
    memcpy(&pairingData, incomingData, sizeof(pairingData));
    Serial.println(pairingData.msgType);
    Serial.println(pairingData.id);
    Serial.print("Pairing request from: ");
    printMAC(mac);
    Serial.println();
    Serial.println(pairingData.channel);
    if (pairingData.id > 0)
    { // do not replay to server itself
      if (pairingData.msgType == PAIRING)
      {
        pairingData.id = 0; // 0 is server
        chan = WiFi.channel();
        // Server is in AP_STA mode: peers need to send data to server soft AP MAC address
        WiFi.softAPmacAddress(pairingData.macAddr);
        pairingData.channel = chan;
        Serial.println("send response");
        esp_err_t result = esp_now_send(mac, (uint8_t *)&pairingData, sizeof(pairingData));
        addPeer(mac);
      }
    }
    break;
  }
}
void ask_Alpr_Data(uint8_t device_ID, uint8_t packTYPE)
{
  esp_err_t result = 0;
#if EXCTIMEDEBUG
  unsigned long startTIME = 0;
  unsigned long endTIME = 0;
  unsigned long totalTIME = 0;
  startTIME = millis();
#endif
  strcpy(get_alpr_data.header1, "U");
  get_alpr_data.deviceID = device_ID;
  get_alpr_data.packTYPE = packTYPE;
  get_alpr_data.packLEN = 1;
  get_alpr_data.checksum = (get_alpr_data.deviceID + get_alpr_data.packTYPE + get_alpr_data.packLEN);
  strcpy(get_alpr_data.footer, "#");

  switch (device_ID)
  {
  case DEVICE1:
    Serial.print(F("Request to DeviceID: "));
    Serial.print(device_ID);
    Serial.print(F("\t"));
    // Send message via ESP-NOW
    result = esp_now_send(slaveAddress1, (uint8_t *)&get_alpr_data, sizeof(get_alpr_data));
    break;

  case DEVICE2:
    Serial.print(F("Request to DeviceID: "));
    Serial.print(device_ID);
    Serial.print(F("\t"));

    result = esp_now_send(slaveAddress2, (uint8_t *)&get_alpr_data, sizeof(get_alpr_data));
    break;

    // case DEVICE3:
    // Serial.print(F("Request to DeviceID: "));
    // Serial.print(device_ID);
    // Serial.print(F("\t"));
    // result = esp_now_send(broadcastAddress3, (uint8_t *)&get_alpr_data, sizeof(get_alpr_data));
    // break;
  }

  if (result == ESP_OK)
  {
    Serial.println("Sent with success");
  }
  else
  {
    Serial.println("Error sending the data");
  }

#if EXCTIMEDEBUG
  endTIME = millis();
  totalTIME = endTIME - startTIME;
  Serial.print(F("Time taken to Send_Car_DATA function: "));
  Serial.println(totalTIME);
  Serial.println();
#endif
  return;
}

void read_Data(void)
{
  switch (recieved_data.packTYPE)
  {
  case CAMERA_INFO:
    Serial.print(F("CAM INFO\r\n"));
    Serial.println(F("*********************************\r\n"));
    break;
  case CARAVAIL:
    if (strcmp(recieved_data.packDATA, "\0"))
    {
      Serial.print(F("VEHICLE IS AVAIBLE\r\n"));
      Serial.println(F("*********************************\r\n"));
    }
    break;
  case CARNOTAVAIL:
    Serial.print(F("VEHICLE IS NOT AVAIBLE\r\n"));
    Serial.println(F("*********************************\r\n"));
    break;
  case CARPALTENUM:
    Serial.println(recieved_data.packDATA);
    if (strcmp(recieved_data.packDATA, "NONE") != 0)
    {
      alpr_data = recieved_data.packDATA;
      if (evse_ChargePointStatus != Charging)
      {
        EvseDevStatus_connector_1 = falg_EVSE_Authentication;
        gu8_evse_change_state = EVSE_AUTHENTICATION;
        evse_authorize_state = EVSE_AUTHORIZE_INITIATED;
        flag_rfid = 2;
      }
    }

    Serial.println(F("*********************************\r\n"));
    break;
  }
  return;
}

void Read_Response(int len)
{
  // if (strcmp(recieved_data.header1, "U") && strcmp(recieved_data.footer, "#")) {
  if (len == 23)
  {
    // Serial.println(recieved_data.deviceID);
    switch (recieved_data.deviceID)
    {
    case 0x01:
      Serial.println(F("*********************************"));
      Serial.print(F("Received Packet from Device: "));
      Serial.println(recieved_data.deviceID);
      read_Data();
      break;
    case 0x02:
      Serial.println(F("*********************************"));
      Serial.print(F("Received Packet from Device: "));
      Serial.println(recieved_data.deviceID);
      read_Data();
      break;
    case 0x03:
      Serial.println(F("*********************************"));
      Serial.print(F("Received Packet from Device: "));
      Serial.println(recieved_data.deviceID);
      read_Data();
      break;
    default:
      Serial.print(F("Unknown Device.."));
    }
  }
  else
  {
    Serial.print(F("Unknown Data.. "));
  }
  return;
}

void init_esp_now(void)
{
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}
#if 0
void init_esp_now(void)
{
  WiFi.disconnect();
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  slave.channel = 1;  
  slave.encrypt = false;
  
  // Add peer 
  memcpy(slave.peer_addr, slaveAddress1, 6);       
  if (esp_now_add_peer(&slave) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  memcpy(slave.peer_addr, slaveAddress2, 6);       
  if (esp_now_add_peer(&slave) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}
#endif