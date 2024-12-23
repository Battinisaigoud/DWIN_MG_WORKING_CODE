#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include <nvs_flash.h>
#include <Preferences.h>

// #include "../Evse_Preferences/Evse_Preferences.h"

#include "evse_config.h"

#include "evse_BLE_Commissioning.h"
#include "evse_RTCx.h"
#include "evse_custom_rtc.h"

#if 0
#include "src/BLE_Commissioning.h"
#include "src/RTCx.h"
#include "src/custom_rtc.h"
#endif
#include <ArduinoJson.h>
#include <WiFi.h>

#define BLE_OK (1)
#define BLE_FAIL (0)

#define BLE_GOTCONN (1)
#define BLE_CONN (2)
#define BLE_DISC (3)

extern unsigned long ble_timerDelay;

const char* ssid_1 = "shiva";
const char* key_1 = "shiva1933";

float value1 = 12345.123;
float value2 = 45678.123;
float value3 = 78901.789;

extern String ble_mac_add;
extern String cpSerial;
extern String csSerial;
extern String cscbSerial;
extern String cssnSerial;

extern struct RTCx::tm tm;

extern volatile uint8_t gu8_evse_ble_count;

StaticJsonDocument<1000> docBLE;

#pragma pack(1)
struct ThresholdConfig
{
  char header1[1] = { '$' };
  int8_t header2; // type
  uint16_t maxVoltage;
  uint16_t minVoltage;
  uint16_t maxCurrent;
  uint16_t minCurrent;
  uint8_t CTC;
  uint8_t CTT;
  uint8_t LPT;
  uint8_t OTT;
  uint8_t UTT;
  int16_t checksum;
  char footer[1] = { '#' };
};

ThresholdConfig CFT;

/*BLE*/
#pragma pack(1)
struct OverVoltage
{
  char header1[1] = { '$' };
  int16_t header2; // type
  int16_t over_vol1;
  int16_t over_vol2;
  int16_t checksum;
  char footer[1] = { '#' };
};

OverVoltage over_voltage;

#pragma pack(1)
struct UnderVoltage
{
  char header1[1] = { '$' };
  int16_t header2; // type
  int16_t under_vol1;
  int16_t under_vol2;
  int16_t checksum;
  char footer[1] = { '#' };
};

UnderVoltage under_voltage;

#pragma pack(1)
struct OverCurrent
{
  char header1[1] = { '$' };
  int16_t header2; // type
  int16_t over_cur1;
  int16_t over_cur2;
  int16_t checksum;
  char footer[1] = { '#' };
};

OverCurrent over_current;

#pragma pack(1)
struct Undercurrent
{
  char header1[1] = { '$' };
  int16_t header2; // type
  uint16_t under_cur1;
  int16_t checksum;
  char footer[1] = { '#' };
};

Undercurrent under_current;

#pragma pack(1)
struct Chargeterminationcurrent
{
  char header1[1] = { '$' };
  int16_t header2; // type
  float Chargeterminationcurrent1;
  int16_t checksum;
  char footer[1] = { '#' };
};

Chargeterminationcurrent chargeterminationcurrent;

#pragma pack(1)
struct Chargeterminationtime
{
  char header1[1] = { '$' };
  int16_t header2; // type
  uint16_t Chargeterminationtime1;
  uint16_t Chargeterminationtime2;
  uint16_t Chargeterminationtime3;
  int16_t checksum;
  char footer[1] = { '#' };
};

Chargeterminationtime chargeterminationtime;

#pragma pack(1)
struct OverTemparature
{
  char header1[1] = { '$' };
  int16_t header2; // type
  int16_t overtemparature1;
  int16_t overtemparature2;
  int16_t checksum;
  char footer[1] = { '#' };
};

OverTemparature over_temparature;

#pragma pack(1)
struct WiFic
{
  char header1[1] = { '$' };
  int8_t header2; // type
  uint8_t lenS;
  uint8_t lenP;
  char SSID_received[30];
  char PWD_received[63];
  int16_t checksum;
  char footer[1] = { '#' };
};

WiFic SW;

#pragma pack(1)
struct ethernetc
{
  char header1[1] = { '$' };
  int8_t header2; // type
  uint8_t req;
  int16_t checksum;
  char footer[1] = { '#' };
};

ethernetc eth;

#pragma pack(1)
struct RFIDc
{
  char header1[1] = { '$' };
  int8_t header2; // type
  uint8_t RFID_serial;
  uint8_t RFID_len;
  char RFID_num[8];
  int16_t checksum;
  char footer[1] = { '#' };
};

RFIDc rfid;

#pragma pack(1)
struct RFIDGET
{
  char header1[1] = { '$' };
  int8_t header2; // type
  char GET_RFID_num1[15];
  char GET_RFID_num2[15];
  char GET_RFID_num3[15];
  char GET_RFID_num4[15];
  char GET_RFID_num5[15];
  char GET_RFID_num6[15];
  char GET_RFID_num7[15];
  char GET_RFID_num8[15];
  int16_t checksum;
  char footer[1] = { '#' };
};

RFIDGET rfidget;

#pragma pack(1)
struct FactReset
{
  char header1[1] = { '$' };
  int8_t header2; // type
  uint8_t req;
  int16_t checksum;
  char footer[1] = { '#' };
};

FactReset fac;

#pragma pack(1)
struct CONN_CONF
{
  char header1[1] = { '$' };
  int8_t header2; // type
  uint8_t req;
  int16_t checksum;
  char footer[1] = { '#' };
};

CONN_CONF conn_conf;

#pragma pack(1)
struct offline
{
  char header1[1] = { '$' };
  int8_t header2; // type
  uint8_t req;
  int16_t checksum;
  char footer[1] = { '#' };
};

offline OFF;

#pragma pack(1)
struct Rff
{
  char header1[1] = { '$' };
  int8_t header2; // type
  uint8_t req;
  int16_t checksum;
  char footer[1] = { '#' };
};

Rff rFF;

#pragma pack(1)
struct OCPP
{
  char header1[1] = { '$' };
  int8_t header2; // type
  char url_received[150];
  int16_t checksum;
  char footer[1] = { '#' };
};

OCPP ocpp;

#pragma pack(1)
struct Date_Comm
{
  char header1[1] = { '$' };
  int8_t header2; // type
  char date[20];
  int16_t checksum;
  char footer[1] = { '#' };
};
Date_Comm date_comm;

#pragma pack(1)
struct Date_Sync
{
  char header1[1] = { '$' };
  int8_t header2; // type
  int16_t day;
  int16_t month;
  int16_t year;
  int16_t hour;
  int16_t minute;
  int16_t second;
  int16_t checksum;
  char footer[1] = { '#' };
};
Date_Sync date_sync;

#pragma pack(1)
struct RTC_TIME_SET
{
  char header1[1] = { '$' };
  int8_t header2; // type
  int day;
  int month;
  int year;
  int hour;
  int minute;
  int second;
  int16_t checksum;
  char footer[1] = { '#' };
};
RTC_TIME_SET rtc_time_set;

#pragma pack(1)
struct Cssn
{
  char header1[1] = { '$' };
  int8_t header2; // type
  char num[20];
  int16_t checksum;
  char footer[1] = { '#' };
};

Cssn cssn;

#pragma pack(1)
struct CPSN
{
  char header1[1] = { '$' };
  int8_t header2; // type
  char num[16];
  int16_t checksum;
  char footer[1] = { '#' };
};

CPSN cpsn;

#pragma pack(1)
struct Csn
{
  char header1[1] = { '$' };
  int8_t header2; // type
  char name[30];
  int16_t checksum;
  char footer[1] = { '#' };
};

Csn csn;

#pragma pack(1)
struct CommBy
{
  char header1[1] = { '$' };
  int8_t header2; // type
  char name[30];
  int16_t checksum;
  char footer[1] = { '#' };
};

CommBy comm_by;

#pragma pack(1)
struct GETCHARGEDETAILS
{
  char header1[1] = { '$' };
  int8_t header2; // type
  char GET_Station_Name[15];
  char GET_Station_Num[15];
  char GET_Comm_date[20];
  char GET_Comm_By[15];
  int16_t checksum;
  char footer[1] = { '#' };
};

GETCHARGEDETAILS getchargedetails;

#pragma pack(1)
struct Tariff
{
  char header1[1] = { '$' };
  int8_t header2; // type
  int amount1;
  int amount2;
  int16_t checksum;
  char footer[1] = { '#' };
};

Tariff tarrif;

#pragma pack(1)
struct resp
{
  char header1[1] = { '$' };
  int8_t header2; // type
  int8_t ack;
  int16_t checksum;
  char footer[1] = { '#' };
};

resp res;

#pragma pack(1)
struct LAST_MONTH_CUMULATIVE
{
  char header1[1] = { '$' };
  int8_t header2; // type
  char last_month_cum[10];
  // String last_month_cum;
  int16_t checksum;
  char footer[1] = { '#' };
};
LAST_MONTH_CUMULATIVE last_month_cumulative;

#pragma pack(1)
struct THIS_MONTH_CUMULATIVE
{
  char header1[1] = { '$' };
  int8_t header2; // type
  char this_month_cum[10];
  // String this_month_cum;
  int16_t checksum;
  char footer[1] = { '#' };
};
THIS_MONTH_CUMULATIVE this_month_cumulative;

#pragma pack(1)
struct TOTAL_CUMULATIVE
{
  char header1[1] = { '$' };
  int8_t header2; // type
  char total_cum[10];
  // String total_cum;
  int16_t checksum;
  char footer[1] = { '#' };
};
TOTAL_CUMULATIVE total_cumulative;

#pragma pack(1)
struct TOTAL_AMOUNT
{
  char header1[1] = { '$' };
  int8_t header2; // type
  char total_cum[10];
  int16_t checksum;
  char footer[1] = { '#' };
};
TOTAL_AMOUNT total_amount;

#pragma pack(1)
struct plugandcharge
{

  char header1[1] = { '$' };
  int16_t header2; // type
  bool plug_and_charge1;
  bool plug_and_charge2;
  int16_t checksum;
  char footer[1] = { '#' };
};

plugandcharge Plug_and_Charge; // adding for plug and play

#pragma pack(1)
struct SessionResume
{

  char header1[1] = { '$' };
  int16_t header2; // type
  bool session_resume1;
  bool session_resume2;
  int16_t checksum;
  char footer[1] = { '#' };
};

#pragma pack(1)
struct CPcurrentLimit
{
  char header1[1] = { '$' };
  int16_t header2; // type
  int16_t current_limit1;
  int16_t current_limit2;
  int16_t checksum;
  char footer[1] = { '#' };
};

CPcurrentLimit CP_current_limit;

SessionResume session_resume; // for session resume

extern uint8_t gu8_cp_serial_update;

extern uint8_t gu8_cs_serial_update;

extern uint8_t gu8_cscb_update;

extern uint8_t gu8_cssn_serial_update;

// Preferences preferences_ble;
Preferences evse_preferences_ble;

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic_COM = NULL;
BLECharacteristic* pCharacteristic_RES = NULL;
std::string Value;

uint8_t typeOfpacket = 0;

#define SERVICE_UUID "06122111-0820-20A1-20B2-40F520AB4C86"
#define COM_UUID "06122111-0820-20A1-20B2-40F520AB4C87"
#define RES_UUID "06122111-0820-20A1-20B2-40F520AB4C88"

bool deviceConnected = false;
bool oldDeviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer* pServer)
  {
    ble_timerDelay = 120000; // if BLE app connected, extend ble config time upto 2 minutes
    Serial.print(F("BLE Configuration Time Duration is Extended to: "));
    Serial.print(ble_timerDelay);
    Serial.print(F("ms.\r\n"));
    deviceConnected = true;
    BLEDevice::startAdvertising();
  };

  void onDisconnect(BLEServer* pServer)
  {
    deviceConnected = false;
  }
};

class MySecurity : public BLESecurityCallbacks
{

  bool onConfirmPIN(uint32_t pin)
  {
    Serial.println(F("confirming pin"));
    return false;
  }

  uint32_t onPassKeyRequest()
  {
    Serial.println(F("PassKeyRequest"));
    ESP_LOGI(LOG_TAG, "PassKeyRequest");
    // return 123456;    @modified by Gopi
  }
  /*
    Here in this callback function Display the Security Key through Serial window
  */
  void onPassKeyNotify(uint32_t pass_key)
  {
    Serial.print("On passkey Notify number:");
    Serial.println(pass_key);
    ESP_LOGI(LOG_TAG, "On passkey Notify number:%d", pass_key);
    /* 2 lines @Added by Gopi*/
    delay(50);
    Serial.print("Connecting...\r\n");
  }

  bool onSecurityRequest()
  {
    Serial.println("On Security Request");
    ESP_LOGI(LOG_TAG, "On Security Request");
    return true;
  }

  void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl)
  {
    ESP_LOGI(LOG_TAG, "Starting BLE work!");
    delay(10);
    if (cmpl.success)
    {
      Serial.println("EVCP Connected Successfully");
    }
    else
    {
      Serial.print("Failed auth");
      Serial.println(cmpl.fail_reason);
    }
  }
};

using std::cout;
using std::endl;
using std::hex;
using std::string;
using std::stringstream;

bool dataChanged = false;
bool dataValid = false;

void BLE_setup()
{
  // put your setup code here, to run once:

  // Serial.begin(9600);

  // BLEDevice::init("EVRE_BT_COMMISSIONING");
  // BLEDevice::init("EVRE_BLE_COMMISSIONING");
  // if(cpSerial.equals("12345") == true)
  if (gu8_cp_serial_update == 0)
  {
    BLEDevice::init(ble_mac_add.c_str());
  }
  else
  {
    BLEDevice::init(cpSerial.c_str());
  }

  if (gu8_cs_serial_update == 0)
  {
    BLEDevice::init(EVSE_CHARGE_POINT_VENDOR.c_str());
  }
  else
  {
    BLEDevice::init(csSerial.c_str());
  }

  if (gu8_cscb_update == 0)
  {
    BLEDevice::init(EVSE_CHARGE_POINT_MODEL.c_str());
  }
  else
  {
    BLEDevice::init(cscbSerial.c_str());
  }

  if (gu8_cssn_serial_update == 0)
  {
    BLEDevice::init(cssnSerial.c_str());
  }
  else
  {
    BLEDevice::init(cssnSerial.c_str());
  }

  BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
  /*
     Required in authentication process to provide displaying and/or input passkey or yes/no butttons confirmation
  */
  BLEDevice::setSecurityCallbacks(new MySecurity());

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService* pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic : AC2M
  pCharacteristic_COM = pService->createCharacteristic(
    COM_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE |
    BLECharacteristic::PROPERTY_NOTIFY |
    BLECharacteristic::PROPERTY_INDICATE);

  // Create a BLE Characteristic : M2AC
  pCharacteristic_RES = pService->createCharacteristic(
    RES_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE |
    BLECharacteristic::PROPERTY_NOTIFY |
    BLECharacteristic::PROPERTY_INDICATE);

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic_COM->addDescriptor(new BLE2902());

  pCharacteristic_RES->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0); // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  // Use the below code for BLE password protected approach.
  BLESecurity* pSecurity = new BLESecurity();
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_ONLY);
  pSecurity->setCapability(ESP_IO_CAP_OUT);
  pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
  /*
    while setting your device password start with Non_Zero
    @modified and Tested by Gopi
  */
  uint32_t passkey = 112233;
  esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t));
  Serial.println(F("*****Started EVRE BLE Commissioning*****"));
}

/*
   @brief: BLE_connectionCheck  - This function checks for active connections
   Unlike global functions in C, access to static functions is restricted to the file where they are declared.
   Therefore, when we want to restrict access to functions, we make them static.
   Another reason for making functions static can be reuse of the same function name in other files.
*/

static uint8_t BLE_connectionCheck()
{
  if (deviceConnected)
  {
    if (!oldDeviceConnected)
    {
      // requestLed(BLUE, START, 1);
      delay(100);
      // requestLed(GREEN, START, 1);
      return BLE_GOTCONN;
    }
  }
  // disconnecting
  if (!deviceConnected && oldDeviceConnected)
  {
    // requestLed(BLUE, START, 1);
    delay(50);                   // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    // Serial.println(F("******EVRE Commissioning connection established****"));
    // Serial.println(F("*****EVCP is Ready to Connect*****"));              //@modified by Gopi
    oldDeviceConnected = deviceConnected;
    return BLE_CONN;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected)
  {
    // do stuff here on connecting
    Serial.print("*****EOld Device Connect*****");
    oldDeviceConnected = deviceConnected;
    return BLE_DISC;
  }
}

void BLE_loop()
{
  // put your main code here, to run repeatedly:
  BLE_connectionCheck();
  uint8_t errCode;
  errCode = state_read();
  if (errCode == 1)
  {
    if (dataChanged)
      dataValid = true;
    else
      dataValid = false;
  }
  else if (errCode == 0)
  {
    dataValid = false;
  }

  if (dataValid)
  {
    switch (typeOfpacket)
    {
    case ConfigFaultThresholds:
      if (DEBUG_EN)
        Serial.println(F("config fault thresholds"));
      errCode = setCFT();
      break;
    case WiFiConfig:
      if (DEBUG_EN)
        Serial.println(F("config WiFiConfig "));
      errCode = setWiFi();
      gu8_evse_ble_count = 1;
      break;
    case ethernetR:
      if (DEBUG_EN)
        Serial.println(F("config ethernetR "));
      errCode = setETH();
      break;
    case OCPP_parameters:
      if (DEBUG_EN)
        Serial.println(F("config OCPP_parameters"));
      errCode = setOCPP();
      break;
    case ConnectivityConfig:
      if (DEBUG_EN)
        Serial.println(F("config ConnectivityConfig"));
      errCode = setConnConfg();
      break;
    case StoreRFID:
      if (DEBUG_EN)
        Serial.println(F("config StoreRFID"));
      errCode = Store_RFID();
      break;
    case DeleteRFID:
      if (DEBUG_EN)
        Serial.println(F("config DeleteRFID"));
      errCode = Delete_RFID();
      break;
    case GetRFID:
      if (DEBUG_EN)
        Serial.println(F("config GetRFID"));
      errCode = Get_RFID();
      break;
    case ChargerSerialNum:
      if (DEBUG_EN)
        Serial.println(F("config Set Charging Station Serial Number"));
      errCode = setChargingStationSerialNumber();
      break;
    case ChargePointVendorName:
      if (DEBUG_EN)
        Serial.println(F("config SetCharging Station Vendor name"));
      errCode = setChargingStationVendorName();
      break;
    case CommissioningDate:
      if (DEBUG_EN)
        Serial.println(F("config SetCharging Station Commission date"));
      errCode = setDateOfCommissioning();
      break;
    case CommissioningBy:
      if (DEBUG_EN)
        Serial.println(F("config SetCharging Station Commission by"));
      errCode = setChargingStationCommissionBy();
      break;
    case FactoryReset:
      if (DEBUG_EN)
        Serial.println(F("config FactoryReset"));
      errCode = factoryResetD();
      break;
    case restoreSession:
      if (DEBUG_EN)
        Serial.println(F("config restoreSession"));
      errCode = restoreFromFault();
      break;
    case TariffAmount:
      if (DEBUG_EN)
        Serial.println(F("config TariffAmount"));
      errCode = Store_Tariff_Amount();
      break;
    case About:
      if (DEBUG_EN)
        Serial.println(F("About Charger"));
      errCode = About_charger();
      break;
    case LastMonthCumulative:
      if (DEBUG_EN)
        Serial.println(F("Last month cumulative energy"));
      errCode = Last_Month_Cumulative_Energy();
      break;
    case ThisMonthCumulative:
      if (DEBUG_EN)
        Serial.println(F("This month cumulative energy"));
      errCode = This_Month_Cumulative_Energy();
      break;
    case TotalCumulative:
      if (DEBUG_EN)
        Serial.println(F("Total cumulative energy"));
      errCode = Total_Cumulative_Energy();
      break;
    case DateTimeSet:
      if (DEBUG_EN)
        Serial.println(F("Sync Real Time"));
      errCode = setDate_Time();
      break;

    case ESP32RESET:
      if (DEBUG_EN)
        Serial.println(F("Resetting ESP32"));
      ESP.restart();
      break;

    case CP_SERIAL:
      if (DEBUG_EN)
        Serial.println(F("Charge point serial number"));
      ChargepointSerialNumber();
      break;
    case PLUG_AND_CHARGE:
      if (DEBUG_EN)
        Serial.println(("Plug and charge packet sent"));
      setPlugAndCharge();
      break;
    case SESSIONRESUME:
      if (DEBUG_EN)
        Serial.println(("Session resume packet sent"));
      setSessionResume();
      break;
    case OVERVOLTAGE:
      if (DEBUG_EN)
        Serial.println(("Over Voltage"));
      setOverVoltage();
      break;
    case UNDERVOLTAGE:
      if (DEBUG_EN)
        Serial.println(("under Voltage"));
      setunderVoltage();
      break;
    case OVERCURRENT:
      if (DEBUG_EN)
        Serial.println(("over  current"));
      setovercurrent();
      break;

    case UNDERCURRENT:
      if (DEBUG_EN)
        Serial.println(("under  current"));
      setundercurrent();
      break;
    case TERMINATIONCURRENT:
      if (DEBUG_EN)
        Serial.println(("Terminationcurrent"));
      setterminationcurrent();
      break;
    case TERMINATIONTIME:
      if (DEBUG_EN)
        Serial.println(("Terminationtime"));
      setterminationtime();

      break;

    case OVERTEMPERATURE:
      if (DEBUG_EN)
        Serial.println(("overtemparature"));
      setovertemparature();
      break;
    case CP_CURRENT_LIMIT:
      if (DEBUG_EN)
        Serial.println(("CP current limit"));
      setCPCurrentLimit();
      break;
    default:
      Serial.println(F("Invalid type of packet sent!"));
      break;
    }
  }
}

uint8_t state_read()
{
  // Read diagnostics and check the current state of the machine
  int16_t checkSum = 0;

  static int16_t prev_checkSum = 0;

  Value = pCharacteristic_COM->getValue();
  // int Value2 = pCharacteristic_COM->getLength();
  // Serial.println(Value2);

  // for(uint8_t idx = 0; idx < value2; idx++)
  //{
  //   uint8_t Value3 = *(pCharacteristic_COM->getData()+idx);
  //   Serial.print(Value3);
  // }

  // uint8_t Value3 = *pCharacteristic_COM->getData();
  // Serial.println(Value3);
  delay(10);
  if (Value.length() > 4)
  {
    if (Value[0] == '$' && Value[Value.length() - 1] == '#')
    {
      int temp[2];
      int j = 0;
      for (int i = Value.length() - 3; i < Value.length() - 1; i++)
      {
        temp[j] = Value[i];
        j++;
      }
      checkSum = temp[0] << 8 | temp[1];
      if (checkSum != prev_checkSum)
      {
        dataChanged = true;
        // Serial.println(F("******* data changed*******"));
      }
      else
      {
        dataChanged = false;
        // Serial.println(F("******* data unchanged*******"));
      }
      prev_checkSum = checkSum;
      typeOfpacket = Value[1];
      return BLE_OK;
    }
  }
  return BLE_FAIL;
}

/*
   @brief: setCFT  - This function sets the config of fault thresholds
*/
uint8_t setCFT()
{
  CFT.maxVoltage = (Value[2] << 8 | Value[3]) / 100; // To divide by the threshold
  CFT.maxCurrent = Value[4];
  CFT.minVoltage = (Value[5] << 8 | Value[6]) / 100; // To divide by the threshold
  CFT.minCurrent = Value[7];
  CFT.CTC = Value[8];
  CFT.CTT = Value[9];
  CFT.LPT = Value[10];
  CFT.OTT = Value[11];
  CFT.UTT = Value[12];
  evse_preferences_ble.begin("thresholds", false);
  evse_preferences_ble.putUInt("maxVolt", CFT.maxVoltage);
  evse_preferences_ble.putUInt("maxCurrent", CFT.maxCurrent);
  evse_preferences_ble.putUInt("minVoltage", CFT.minVoltage);
  evse_preferences_ble.putUInt("minCurrent", CFT.minCurrent);
  evse_preferences_ble.putUInt("CTC", CFT.CTC);
  evse_preferences_ble.putUInt("CTT", CFT.CTT);
  evse_preferences_ble.putUInt("LPT", CFT.LPT);
  evse_preferences_ble.putUInt("OTT", CFT.OTT);
  evse_preferences_ble.putUInt("UTT", CFT.UTT);
  evse_preferences_ble.end();
  return sendRes(1, ConfigFaultThresholds);
}

/*
   @brief: setOverVoltage  - This function stores the Over volatge
*/
/*BLE*/
uint8_t setOverVoltage()
{
  over_voltage.over_vol1 = 0;
  over_voltage.over_vol2 = 0;
  over_voltage.over_vol1 = Value[2];
  over_voltage.over_vol2 = Value[3];

  evse_preferences_ble.begin("FaultThreshold", false);
  bool res = evse_preferences_ble.putInt("overvoltage", over_voltage.over_vol1 + (over_voltage.over_vol2 * 256));
  Serial.print("OVER VOLTAGE:");
  Serial.println(over_voltage.over_vol1 + (over_voltage.over_vol2 * 256));
  // Serial.println(String(preferences.getString("cssn", "")));
  evse_preferences_ble.end();
  if (res)
  {
    Serial.println("nvs set success");
  }
  else
  {
    Serial.println("nvs set fail");
  }
  Value.clear();
  return sendRes(1, OVERVOLTAGE);
}

uint8_t setunderVoltage()
{
  Serial.println("UNDER VOLTAGE:");
  under_voltage.under_vol1 = 0;
  under_voltage.under_vol2 = 0;
  under_voltage.under_vol1 = Value[2];
  under_voltage.under_vol2 = Value[3];
  evse_preferences_ble.begin("FaultThreshold", false);
  evse_preferences_ble.putInt("undervoltage", under_voltage.under_vol1 + (under_voltage.under_vol2 * 256));
  Serial.print("UNDER VOLTAGE:");
  Serial.println(under_voltage.under_vol1 + (under_voltage.under_vol2 * 256));
  evse_preferences_ble.end();

  Value.clear();
  return sendRes(1, UNDERVOLTAGE);
}

uint8_t setovercurrent()
{
  over_current.over_cur1 = 0;
  over_current.over_cur2 = 0;
  over_current.over_cur1 = Value[2];
  over_current.over_cur2 = Value[3];
  evse_preferences_ble.begin("FaultThreshold", false);
  // preferences.putInt("overcurrent", over_current.over_cur1);
  evse_preferences_ble.putInt("overcurrent", over_current.over_cur1 + (over_current.over_cur2 * 256));
  Serial.print("overcurrent : ");
  Serial.println(over_current.over_cur1 + (over_current.over_cur2 * 256));
  // Serial.println("overcurrent : " + over_current.over_cur1);
  evse_preferences_ble.end();
  Value.clear();
  return sendRes(1, OVERCURRENT);
}

uint8_t setundercurrent()
{
  under_current.under_cur1 = 0;
  under_current.under_cur1 = Value[2];
  evse_preferences_ble.begin("FaultThreshold", false);
  evse_preferences_ble.putInt("undercurrent", under_current.under_cur1);
  Serial.println("undercurrent : " + under_current.under_cur1);
  evse_preferences_ble.end();
  Value.clear();
  return sendRes(1, UNDERCURRENT);
}

uint8_t setterminationcurrent()
{
  chargeterminationcurrent.Chargeterminationcurrent1 = 0;
  chargeterminationcurrent.Chargeterminationcurrent1 = Value[2];
  evse_preferences_ble.begin("FaultThreshold", false);
  evse_preferences_ble.putInt("terminationcurrent", chargeterminationcurrent.Chargeterminationcurrent1);
  // Serial.println("terminationcurrent : %f ", chargeterminationcurrent.Chargeterminationcurrent1);
  Serial.print("terminationcurrent : ");
  Serial.println(chargeterminationcurrent.Chargeterminationcurrent1);
  evse_preferences_ble.end();
  Value.clear();
  return sendRes(1, TERMINATIONCURRENT);
}

uint8_t setterminationtime()
{
  chargeterminationtime.Chargeterminationtime1 = 0;
  chargeterminationtime.Chargeterminationtime2 = 0;
  chargeterminationtime.Chargeterminationtime3 = 0;
  chargeterminationtime.Chargeterminationtime1 = Value[2];
  chargeterminationtime.Chargeterminationtime2 = Value[3];
  // chargeterminationtime.Chargeterminationtime3 = Value[4];
  evse_preferences_ble.begin("FaultThreshold", false);
  // preferences.putInt("terminationcurrent", chargeterminationtime.Chargeterminationtime1 + (chargeterminationtime.Chargeterminationtime2 * 256) + (chargeterminationtime.Chargeterminationtime3 * 256));
  evse_preferences_ble.putInt("terminationtime", chargeterminationtime.Chargeterminationtime1 + (chargeterminationtime.Chargeterminationtime2 * 256));
  Serial.print("Termination time:");
  // Serial.println(chargeterminationtime.Chargeterminationtime1 + (chargeterminationtime.Chargeterminationtime2 * 256) + (chargeterminationtime.Chargeterminationtime3 * 256));
  Serial.println(chargeterminationtime.Chargeterminationtime1 + (chargeterminationtime.Chargeterminationtime2 * 256));
  evse_preferences_ble.end();
  Value.clear();
  return sendRes(1, TERMINATIONTIME);
}

uint8_t setovertemparature()
{
  over_temparature.overtemparature1 = 0;
  over_temparature.overtemparature2 = 0;
  over_temparature.overtemparature1 = Value[2];
  over_temparature.overtemparature2 = Value[3];
  evse_preferences_ble.begin("FaultThreshold", false);
  // preferences.putInt("overtemparature", overtemparature.overtemparature1);
  evse_preferences_ble.putInt("overtemparature", over_temparature.overtemparature1 + (over_temparature.overtemparature2 * 256));
  Serial.print("overtemparature : ");
  Serial.println(over_temparature.overtemparature1 + (over_temparature.overtemparature2 * 256));
  // Serial.println("undercurrent : " + overtemparature.overtemparature1);
  evse_preferences_ble.end();
  Value.clear();
  return sendRes(1, OVERTEMPERATURE);
}

/*
   @brief: setETH  - This function sets the ethernet
*/
uint8_t setETH()
{
  eth.req = Value[2];
  evse_preferences_ble.begin("eth", false);
  evse_preferences_ble.putUInt("eth_req", eth.req);
  evse_preferences_ble.end();
  return sendRes(1, ethernetR);
}

/*
   @brief: setOCPP  - This function sets the OCPP url
*/
uint8_t setOCPP()
{
  memset(&ocpp.url_received[0], 0, sizeof(ocpp.url_received));
  int i = 3; // OCPP Websocket URL starts from index 3 in packet
  while (Value[i] != '\0')
  {
    ocpp.url_received[i - 3] = Value[i];
    i++;
  }
  ocpp.url_received[i] = '\0';
  Serial.println(ocpp.url_received);
  evse_preferences_ble.begin("wifi_cred", false);
  evse_preferences_ble.putString("ws_url_prefix", ocpp.url_received);
  evse_preferences_ble.end();
  Value.clear();
  return sendRes(1, OCPP_parameters);
}

/*
   @brief: setConnConfg  - This function stores the RFID
*/
uint8_t Store_RFID()
{
  memset(&rfid.RFID_num[0], 0, sizeof(rfid.RFID_num));
  rfid.RFID_serial = Value[3];
  int i = 4; // RFID data starts from index 4 in packet
  while (Value[i] != '\0')
  {
    rfid.RFID_num[i - 4] = Value[i];
    i++;
  }
  rfid.RFID_num[i] = '\0';
  evse_preferences_ble.begin("rfid", false);
  switch (rfid.RFID_serial)
  {
  case 1:

    evse_preferences_ble.putString("rfid_1", rfid.RFID_num);
    Serial.println(rfid.RFID_num);
    break;

  case 2:

    evse_preferences_ble.putString("rfid_2", rfid.RFID_num);
    Serial.println(rfid.RFID_num);
    break;

  case 3:
    evse_preferences_ble.putString("rfid_3", rfid.RFID_num);
    Serial.println(rfid.RFID_num);
    break;

  case 4:
    evse_preferences_ble.putString("rfid_4", rfid.RFID_num);
    Serial.println(rfid.RFID_num);
    break;

  case 5:
    evse_preferences_ble.putString("rfid_5", rfid.RFID_num);
    Serial.println(rfid.RFID_num);
    break;

  case 6:
    evse_preferences_ble.putString("rfid_6", rfid.RFID_num);
    Serial.println(rfid.RFID_num);
    break;

  case 7:
    evse_preferences_ble.putString("rfid_7", rfid.RFID_num);
    Serial.println(rfid.RFID_num);
    break;

  case 8:
    evse_preferences_ble.putString("rfid_8", rfid.RFID_num);
    Serial.println(rfid.RFID_num);
    break;
  case 9:
    evse_preferences_ble.putString("rfid_9", rfid.RFID_num);
    Serial.println(rfid.RFID_num);
    break;
  case 10:
    evse_preferences_ble.putString("rfid_10", rfid.RFID_num);
    Serial.println(rfid.RFID_num);
    break;

  default:
    break;
  }
  evse_preferences_ble.end();
  return sendRes(1, StoreRFID);
}

/*
   @brief: setConnConfg  - This function deletes the RFID
*/
uint8_t Delete_RFID()
{
  memset(&rfid.RFID_num[0], 0, sizeof(rfid.RFID_num));
  rfid.RFID_serial = Value[3];
  int i = 4; // RFID data starts from index 4 in packet
  while (Value[i] != '\0')
  {
    rfid.RFID_num[i - 4] = Value[i];
    i++;
  }
  rfid.RFID_num[i] = '\0';
  evse_preferences_ble.begin("rfid", false);
  switch (rfid.RFID_serial)
  {
  case 1:
    evse_preferences_ble.putString("rfid_1", rfid.RFID_num);
    break;

  case 2:
    evse_preferences_ble.putString("rfid_2", rfid.RFID_num);
    break;

  case 3:
    evse_preferences_ble.putString("rfid_3", rfid.RFID_num);
    break;

  case 4:
    evse_preferences_ble.putString("rfid_4", rfid.RFID_num);
    break;

  case 5:
    evse_preferences_ble.putString("rfid_5", rfid.RFID_num);
    break;

  case 6:
    evse_preferences_ble.putString("rfid_6", rfid.RFID_num);
    break;

  case 7:
    evse_preferences_ble.putString("rfid_7", rfid.RFID_num);
    break;

  case 8:
    evse_preferences_ble.putString("rfid_8", rfid.RFID_num);
    break;

  default:
    break;
  }
  evse_preferences_ble.end();
  return sendRes(1, DeleteRFID);
}

/*
   @brief: setConnConfg  - This function Gets the RFID
*/
uint8_t Get_RFID()
{
  String get_rfid_num1, get_rfid_num2, get_rfid_num3, get_rfid_num4, get_rfid_num5, get_rfid_num6, get_rfid_num7, get_rfid_num8;
  evse_preferences_ble.begin("rfid", false);
  get_rfid_num1 = evse_preferences_ble.getString("rfid_1", "");
  get_rfid_num2 = evse_preferences_ble.getString("rfid_2", "");
  get_rfid_num3 = evse_preferences_ble.getString("rfid_3", "");
  get_rfid_num4 = evse_preferences_ble.getString("rfid_4", "");
  get_rfid_num5 = evse_preferences_ble.getString("rfid_5", "");
  get_rfid_num6 = evse_preferences_ble.getString("rfid_6", "");
  get_rfid_num7 = evse_preferences_ble.getString("rfid_7", "");
  get_rfid_num8 = evse_preferences_ble.getString("rfid_8", "");
  evse_preferences_ble.end();

  strcpy(rfidget.header1, "$");
  rfidget.header2 = GetRFID; // type
  strncpy(rfidget.GET_RFID_num1, get_rfid_num1.c_str(), sizeof(get_rfid_num1));
  strncpy(rfidget.GET_RFID_num2, get_rfid_num2.c_str(), sizeof(get_rfid_num2));
  strncpy(rfidget.GET_RFID_num3, get_rfid_num3.c_str(), sizeof(get_rfid_num3));
  strncpy(rfidget.GET_RFID_num4, get_rfid_num4.c_str(), sizeof(get_rfid_num4));
  strncpy(rfidget.GET_RFID_num5, get_rfid_num5.c_str(), sizeof(get_rfid_num5));
  strncpy(rfidget.GET_RFID_num6, get_rfid_num6.c_str(), sizeof(get_rfid_num6));
  strncpy(rfidget.GET_RFID_num7, get_rfid_num7.c_str(), sizeof(get_rfid_num7));
  strncpy(rfidget.GET_RFID_num8, get_rfid_num8.c_str(), sizeof(get_rfid_num8));
  rfidget.checksum = random(1, 100);
  strcpy(rfid.footer, "#");

  pCharacteristic_RES->setValue((uint8_t*)&rfidget, sizeof(rfidget));
  pCharacteristic_RES->notify();

  delay(10);

  return BLE_OK;
}

/*
   @brief: factoryResetD  - This function resets the device to factory settings
*/
uint8_t factoryResetD()
{
  fac.req = Value[2];
  evse_preferences_ble.begin("fac", false);
  evse_preferences_ble.putUInt("fac", fac.req);
  evse_preferences_ble.end();
  return sendRes(1, FactoryReset);
}

/*
   @brief: restoreFromFault  - This function restores from fault
*/
uint8_t restoreFromFault()
{
  rFF.req = Value[2];

  evse_preferences_ble.begin("rFF", false);
  evse_preferences_ble.putUInt("rFF", rFF.req);
  evse_preferences_ble.end();
  return sendRes(1, restoreSession);
}

/*
   @brief: restoreFromFault  - This function store the tariff amount
*/
uint8_t Store_Tariff_Amount()
{
  tarrif.amount1 = 0;
  tarrif.amount2 = 0;
  tarrif.amount1 = Value[3];
  tarrif.amount2 = Value[4];

  evse_preferences_ble.begin("tariff", false);
  evse_preferences_ble.putInt("tariff", (tarrif.amount1 + (tarrif.amount2) * 256));
  Serial.println(tarrif.amount1 + (tarrif.amount2) * 256);
  Serial.println(String(evse_preferences_ble.getInt("tariff")));
  evse_preferences_ble.end();
  return sendRes(1, TariffAmount);
}

#if 1
/*
   @brief: setConnConfg  - This function sets the ConnConfg
*/
uint8_t setConnConfg()
{
  uint32_t connnection_type = 0;
  // conn_conf.req = Value[2]; // Connection configuration starts at index 2
  connnection_type = Value[2]; // Connection configuration starts at index 2
  Serial.print("connection request number***********************>>>:::  ");
  Serial.println(connnection_type);
  // Serial.println(conn_conf.req);
  evse_preferences_ble.begin("connectivity", false);

  // switch(conn_conf.req)
  switch (connnection_type)
  {
  case 0:
    Serial.print("connection request number::  0");
    // evse_preferences_ble.putBool("wifi", 0);
    // evse_preferences_ble.putBool("gsm", 0);
    evse_preferences_ble.putUInt("conn_type", connnection_type);
    break;
  case 1:
    Serial.print("connection request number:: 1 ");
    // evse_preferences_ble.putBool("gsm", 0);
    // evse_preferences_ble.putBool("wifi", 1);
    evse_preferences_ble.putUInt("conn_type", connnection_type);
    break;

  case 2:
    Serial.print("connection request number:: 2 ");
    // evse_preferences_ble.putBool("wifi", 0);
    // evse_preferences_ble.putBool("gsm", 1);
    evse_preferences_ble.putUInt("conn_type", connnection_type);
    break;

  case 3:
    evse_preferences_ble.putUInt("conn_type", connnection_type);
    Serial.print("connection request number:: 3 ");

    break;

  case 4:
    Serial.print("connection request number:: 4 ");
    // evse_preferences_ble.putBool("wifi", 1);
    // evse_preferences_ble.putBool("gsm", 1);
    evse_preferences_ble.putUInt("conn_type", connnection_type);
    break;

  case 5:
    evse_preferences_ble.putUInt("conn_type", connnection_type);
    Serial.print("connection request number:: 5 ");
    break;

  case 6:
    evse_preferences_ble.putUInt("conn_type", connnection_type);
    Serial.print("connection request number:: 6 ");
    break;

  case 7:
    evse_preferences_ble.putUInt("conn_type", connnection_type);
    Serial.print("connection request number:: 7 ");
    break;

  default:
    evse_preferences_ble.putUInt("conn_type", connnection_type);
    Serial.print("connection request number:: default ");
    break;
  }
  evse_preferences_ble.end();

  evse_preferences_ble.begin("connectivity", false);
  // bool lb_wifi_status = evse_preferences_ble.getBool("wifi", 1);
  // bool lb_gsm_status = evse_preferences_ble.getBool("gsm", 1);
  uint32_t lb_conection_status = evse_preferences_ble.getUInt("conn_type", connnection_type);
  evse_preferences_ble.end();

  // Serial.println("lb_wifi_status :" + String(lb_wifi_status));
  // Serial.println("lb_gsm_status :" + String(lb_gsm_status));
  Serial.println("lb_conection_status :" + String(lb_conection_status));

  Value.clear();
  return sendRes(1, ConnectivityConfig);
}
#endif

/*
   @brief: setCSSN  - This function stores the charging station serial number
*/
uint8_t setChargingStationSerialNumber()
{
  memset(&cssn.num[0], 0, sizeof(cssn.num));
  int i = 3;
  while (Value[i] != '\0')
  {
    cssn.num[i - 3] = Value[i];
    i++;
  }

  cssn.num[i] = '\0';
  evse_preferences_ble.begin("ChargerDetails", false);
  evse_preferences_ble.putString("cssn", cssn.num);
  Serial.println(cssn.num);
  Serial.print("CHARGING STATION SERIAL NUMBER:");
  Serial.println(String(evse_preferences_ble.getString("cssn", "")));
  evse_preferences_ble.end();
  Value.clear();
  return sendRes(1, ChargerSerialNum);
}

/*
   @brief: setCSN  - This function stores the name of charging station
*/
uint8_t setChargingStationVendorName()
{
  memset(&csn.name[0], 0, sizeof(csn.name));
  int i = 3;
  while (Value[i] != '\0')
  {
    csn.name[i - 3] = Value[i];
    i++;
  }

  csn.name[i] = '\0';
  evse_preferences_ble.begin("ChargerDetails", false);
  evse_preferences_ble.putString("csn", csn.name);
  Serial.println(csn.name);
  Serial.print("CHARGING STATION NAME ::");
  Serial.println(String(evse_preferences_ble.getString("csn", "")));
  evse_preferences_ble.end();
  Value.clear();
  return sendRes(1, ChargePointVendorName);
}

/*
   @brief: setDOC  - This function stores the date of commissioning
*/
uint8_t setDateOfCommissioning()
{
  memset(&date_comm.date[0], 0, sizeof(date_comm.date));
  int i = 3;
  while (Value[i] != '\0')
  {
    date_comm.date[i - 3] = Value[i];
    i++;
  }

  date_comm.date[i] = '\0';
  evse_preferences_ble.begin("ChargerDetails", false);
  evse_preferences_ble.putString("doc", date_comm.date);
  Serial.println(date_comm.date);
  Serial.print("COMMISSIONING DATE  ::");
  Serial.println(String(evse_preferences_ble.getString("doc", "")));
  evse_preferences_ble.end();
  Value.clear();
  return sendRes(1, CommissioningDate);
}

uint8_t setDate_Time()
{
  tm.tm_mon = 0;
  tm.tm_mday = Value[2];
  tm.tm_mon = Value[3];
  tm.tm_year = Value[4];
  tm.tm_hour = Value[5];
  tm.tm_min = Value[6];
  tm.tm_sec = Value[7];
  for (int i = 2; i < 8; i++)
  {
    Serial.println(Value[i]);
  }
  bool suc = rtc.setClock(&tm);
  if (suc)
  {
    Serial.println(F("Time was set"));
    Serial.println(suc);
  }
  else
  {
    Serial.println(F("Failed"));
    Serial.println(suc);
  }
  char datestring[6];
  getTime(datestring);
  return sendRes(1, DateTimeSet);
}

/*
  This function sync the current Time & Date from central to peripheral
*/
uint8_t syncDateOfCommissioning()
{
  tm.tm_mon = 0;
  tm.tm_mday = Value[2];
  tm.tm_mon = Value[3];
  tm.tm_year = Value[4];
  tm.tm_hour = Value[5];
  tm.tm_min = Value[6];
  tm.tm_sec = Value[7];
  for (int i = 2; i < 8; i++)
  {
    Serial.println(Value[i]);
  }
  bool suc = rtc.setClock(&tm);
  if (suc)
  {
    Serial.println(F("Time was set"));
    Serial.println(suc);
  }
  else
  {
    Serial.println(F("Failed"));
    Serial.println(suc);
  }
  char datestring[6];
  getTime(datestring);
  return sendRes(1, DateTimeSet);
}

/*
   @brief: setCSSN  - This function stores the charging station commissioned by
*/
uint8_t setChargingStationCommissionBy()
{
  memset(&comm_by.name[0], 0, sizeof(comm_by.name));
  int i = 3;
  while (Value[i] != '\0')
  {
    comm_by.name[i - 3] = Value[i];
    Serial.println(Value[i]);
    i++;
  }

  comm_by.name[i] = '\0';
  evse_preferences_ble.begin("ChargerDetails", false);
  evse_preferences_ble.putString("cscb", comm_by.name);
  Serial.println(comm_by.name);
  Serial.print("COMMISSIONING BY  $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$:::::::");
  Serial.println(String(evse_preferences_ble.getString("cscb", "")));
  evse_preferences_ble.end();
  Value.clear();
  return sendRes(1, CommissioningBy);
}

/*
   @brief: WiFi config  - This function stores the SSID and password
   Unlike global functions in C, access to static functions is restricted to the file where they are declared.
   Therefore, when we want to restrict access to functions, we make them static.
   Another reason for making functions static can be reuse of the same function name in other files.
*/
uint8_t setWiFi()
{
  memset(&SW.SSID_received[0], 0, sizeof(SW.SSID_received));
  memset(&SW.PWD_received[0], 0, sizeof(SW.PWD_received));
  SW.lenS = Value[2]; // Length of SSID in index 2
  SW.lenP = Value[3]; // Length if Password in index 3
  /*for (int i = 2; i < 2 + SW.lenS ; i++)
    {
    if (Value[i] != '\0')
    {
      SW.SSID_received[i - 2] = Value[i];
    }
    else
    {
      break;
    }
    }*/
    // Serial.print("SSID LENGTH>>>>>>>>>>>>>>>>>>:");
    // Serial.print(SW.lenS);
    // Serial.print("PWD LENGTH>>>>>>>>>>>>>>>>>>:");
    // Serial.print(SW.lenP);
  int i = 4; // SSID data starts from index 4
  while (Value[i] != '\0')
  {
    SW.SSID_received[i - 4] = Value[i];
    i++;
  }
  SW.SSID_received[i] = '\0';

  int j = SW.lenS + 5; // Password starts from SSID length+5... Ref packet format document for more
  while (Value[j] != '\0')
  {
    SW.PWD_received[j - (SW.lenS + 5)] = Value[j];
    j++;
  }
  // SW.PWD_received[i] = '\0';       commented as modification done by Gopi
  SW.PWD_received[j] = '\0'; //
  Serial.print("SSID:");
  Serial.println(SW.SSID_received);

  Serial.print("PASSWORD:");
  Serial.println(SW.PWD_received);

  evse_preferences_ble.begin("wifi_cred", false);
  evse_preferences_ble.putString("ssid", SW.SSID_received);
  evse_preferences_ble.putString("KEY", SW.PWD_received);
  // evse_preferences_ble.putString("ssid", ssid_1);
  // evse_preferences_ble.putString("key", key_1);
  evse_preferences_ble.end();
  Value.clear();
  return sendRes(1, WiFiConfig);
}

uint8_t About_charger()
{
  String get_station_name;
  String get_station_num;
  String get_comm_date;
  String get_comm_by;
  evse_preferences_ble.begin("ChargerDetails", false);
  get_station_name = evse_preferences_ble.getString("csn", "");
  get_station_num = evse_preferences_ble.getString("cssn", "");
  get_comm_date = evse_preferences_ble.getString("doc", "");
  get_comm_by = evse_preferences_ble.getString("cscb", "");
  evse_preferences_ble.end();
  strcpy(getchargedetails.header1, "$");
  getchargedetails.header2 = About; // type
  strncpy(getchargedetails.GET_Station_Name, get_station_name.c_str(), sizeof(get_station_name));
  strncpy(getchargedetails.GET_Station_Num, get_station_num.c_str(), sizeof(get_station_num));
  // strncpy(getchargedetails.GET_Comm_date,get_comm_date.c_str(),sizeof(get_comm_date));
  strcpy(getchargedetails.GET_Comm_date, get_comm_date.c_str());
  strncpy(getchargedetails.GET_Comm_By, get_comm_by.c_str(), sizeof(get_comm_by));
  getchargedetails.checksum = 22;
  strcpy(getchargedetails.footer, "#");
  // send the data
  pCharacteristic_RES->setValue((uint8_t*)&getchargedetails, sizeof(getchargedetails));
  pCharacteristic_RES->notify();
  delay(10);
  return BLE_OK;
}

uint8_t sendRes(uint8_t ack, uint8_t typeOP)
{
  // Prepare acknowledgement
  strcpy(res.header1, "$");
  res.header2 = typeOP; // type
  res.ack = ack;        // 1 - success 2 - fail
  res.checksum = res.ack;
  strcpy(res.footer, "#");
  // send the data
  pCharacteristic_RES->setValue((uint8_t*)&res, sizeof(res));
  pCharacteristic_RES->notify();
  delay(10);
  return BLE_OK;
}

/*
   @brief: setCSSN  - This function stores the last month cumulative energy
*/

uint8_t Last_Month_Cumulative_Energy()
{
  strcpy(last_month_cumulative.header1, "$");
  last_month_cumulative.header2 = LastMonthCumulative; // type
  // strcpy(last_month_cumulative.last_month_cum,"456.45");
  dtostrf(value2, 10, 3, last_month_cumulative.last_month_cum);
  last_month_cumulative.checksum = random(1, 100);
  ;
  strcpy(last_month_cumulative.footer, "#");
  // send the data
  pCharacteristic_RES->setValue((uint8_t*)&last_month_cumulative, sizeof(last_month_cumulative));
  pCharacteristic_RES->notify();
  delay(10);
  return BLE_OK;
}

/*
   @brief: setCSSN  - This function stores the this month cumulative energy
*/

uint8_t This_Month_Cumulative_Energy()
{
  strcpy(this_month_cumulative.header1, "$");
  this_month_cumulative.header2 = ThisMonthCumulative; // type
  // strcpy(this_month_cumulative.this_month_cum,"123.12");
  dtostrf(value1, 10, 3, this_month_cumulative.this_month_cum);
  this_month_cumulative.checksum = random(1, 100);
  strcpy(this_month_cumulative.footer, "#");
  // send the data
  pCharacteristic_RES->setValue((uint8_t*)&this_month_cumulative, sizeof(this_month_cumulative));
  pCharacteristic_RES->notify();
  delay(10);
  return BLE_OK;
}

/*
   @brief: setCSSN  - This function stores the total cumulative energy
*/

uint8_t Total_Cumulative_Energy()
{
  strcpy(total_cumulative.header1, "$");
  total_cumulative.header2 = TotalCumulative; // type
  // strcpy(total_cumulative.total_cum,"789.78");
  dtostrf(value3, 10, 3, total_cumulative.total_cum);
  total_cumulative.checksum = random(1, 100);
  strcpy(total_cumulative.footer, "#");
  // send the data
  pCharacteristic_RES->setValue((uint8_t*)&total_cumulative, sizeof(total_cumulative));
  pCharacteristic_RES->notify();
  delay(10);
  return BLE_OK;
}

uint8_t ChargepointSerialNumber()
{
  memset(&cpsn.num[0], 0, sizeof(cpsn.num));
  int i = 2;
  while (Value[i] != '\0')
  {
    cpsn.num[i - 2] = Value[i];
    i++;
  }

  cpsn.num[i] = '\0';
  evse_preferences_ble.begin("ChargerDetails", false);
  evse_preferences_ble.putString("cpsn", cpsn.num);
  Serial.println(cpsn.num);
  Serial.print("CHARGE POINT SERIAL NUMBER$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$:::::::");
  Serial.println(String(evse_preferences_ble.getString("cpsn", "")));
  evse_preferences_ble.end();
  Value.clear();
  return sendRes(1, CP_SERIAL);
}

uint8_t setPlugAndCharge()
{
  Serial.println("plug and charge function called");

  Plug_and_Charge.plug_and_charge1 = 0;
  Plug_and_Charge.plug_and_charge2 = 0;
  Plug_and_Charge.plug_and_charge1 = Value[2];
  Plug_and_Charge.plug_and_charge2 = Value[3];

  evse_preferences_ble.begin("FaultThreshold", false);
  bool res = evse_preferences_ble.putBool("plugandcharge", Plug_and_Charge.plug_and_charge1 + (Plug_and_Charge.plug_and_charge2 * 1));
  Serial.print("PLUG AND CHARGE:");

  Serial.println(Plug_and_Charge.plug_and_charge1 + (Plug_and_Charge.plug_and_charge2 * 1));
  evse_preferences_ble.end();
  if (res)
  {
    Serial.println(" PLUG_AND_CHARGE nvs set success");
  }
  else
  {
    Serial.println("PLUG_AND_CHARGE nvs set fail");
  }
  Value.clear();
  return sendRes(1, PLUG_AND_CHARGE);
}

uint8_t setSessionResume()
{
  Serial.println("session resume function called");
  session_resume.session_resume1 = 0;
  session_resume.session_resume2 = 0;
  session_resume.session_resume1 = Value[2];
  session_resume.session_resume2 = Value[3];

  evse_preferences_ble.begin("FaultThreshold", false);
  bool res = evse_preferences_ble.putBool("sessionresum", session_resume.session_resume1 + (session_resume.session_resume2 * 1));
  Serial.print("SESSION RESUME:");

  Serial.println(session_resume.session_resume1 + (session_resume.session_resume2 * 1));
  evse_preferences_ble.end();
  if (res)
  {
    Serial.println(" sessionresume nvs set success");
  }
  else
  {
    Serial.println("sessionresume nvs set fail");
  }
  Value.clear();
  return sendRes(1, SESSIONRESUME);
}

uint8_t setCPCurrentLimit()
{
  CP_current_limit.current_limit1 = 0;
  CP_current_limit.current_limit2 = 0;
  CP_current_limit.current_limit1 = Value[2];
  CP_current_limit.current_limit2 = Value[3];
  evse_preferences_ble.begin("FaultThreshold", false);
  evse_preferences_ble.putInt("CPcurrentLimit", CP_current_limit.current_limit1 + (CP_current_limit.current_limit2 * 256));
  Serial.print("CPcurrentLimit : ");
  Serial.println(CP_current_limit.current_limit1 + (CP_current_limit.current_limit2 * 256));
  evse_preferences_ble.end();
  Value.clear();
  return sendRes(1, CP_CURRENT_LIMIT);
}

/*******************************************************************************************************************/

/**
 * @brief de-Initialize the BLE environment.
 */
void BLE_deinit(void)
{
  BLEDevice::deinit(false);
}

/*******************************************************************************************************************/
