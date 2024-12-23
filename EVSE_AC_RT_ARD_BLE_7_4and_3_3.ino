/***********************************************************************************
 * Code for the EVSE (Electric Vehicle Supply Equipment) device.
 *
 * This code is responsible for controlling the EVSE device. It includes functions
 * for initializing the device, handling communication, and performing various tasks
 * related to charging.
 *
 * Company: EVRE
 * Author : Krishna& Rakesh (EVRE Embedded software team)
 * Date : March,2023
 *
 *https://github.com/Amplify-Mobility/EVSE_AC_RT_ARD_BLE.git
 *
 **********************************************************************************/
#include <Arduino.h>
#include <WiFi.h>
#include <string.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>
#include <Preferences.h>

#include <EEPROM.h>

#include <nvs_flash.h>

 /***************************************************************************************************************/

#include "FFat.h"
#include "FS.h"
#include "SPIFFS.h"

#include "esp32-hal-cpu.h"
#include "ESP32Time.h"
#include "Esp.h"

#include "core_version.h"

/*************************************************************************************************************/
#define SerialAT Serial2
#define TINY_GSM_MODEM_SIM7600
#include <TinyGsmClient.h>
#include <CRC32.h>

// extern TinyGsm ota_modem(Serial2);
// extern TinyGsmClient ota_client(ota_modem);

extern TinyGsm modem();
extern TinyGsmClient client();
TinyGsm ota_modem(Serial2);
TinyGsmClient ota_client(ota_modem);

/******************************************ocpp_lib***********************************************************/

#include "src/evse_lib/src/websocket/WebSocketsClient.h" /***@websockt library***/

#include "src/evse_lib/src/OCPP/OcppEngine.h"

#include "src/evse_lib/src/OCPP/OcppMessage.h"

#include "src/evse_lib/src/OCPP/OcppOperation.h"

#include "src/evse_lib/src/OCPP/BootNotification.h"

#include "src/evse_lib/src/OCPP/Authorize.h"

#include "src/evse_lib/src/OCPP/StartTransaction.h"

#include "src/evse_lib/src/OCPP/StopTransaction.h"

#include "src/evse_lib/src/OCPP/StatusNotification.h"

#include "src/evse_lib/src/OCPP/MeterValues.h"

#include "src/evse_lib/src/OCPP/SimpleOcppOperationFactory.h"

#include "src/evse_lib/src/OCPP/ChargePointStatusService.h"

#include "src/evse_lib/src/OCPP/ChangeConfiguration.h"

#include "src/evse_lib/src/OCPP/Heartbeat.h"

#include "src/evse_src/evse_cfg_file.h"

#include "src/evse_src/evse_earth_dis.h"

#include "src/evse_src/evse_gfci.h"

#include "src/evse_lib/src/OCPP/CustomGsm.h"

#include "src/evse_lib/src/OCPP/TimeHelper.h"

/**********************************************evse_src***********************************************************************/
#include "src/evse_src/evse_contacter_relay.h"

#include "src/evse_src/evse_rfid.h"

#include "src/evse_lib/src/MFRC522/MFRC522.h"

#include "src/evse_src/evse_energy_reading.h"

#include "src/evse_src/evse_i2c_display.h"

#include "src/evse_src/evse_dwin.h"

#include "src/evse_src/evse_rtc.h"

#include "src/evse_src/evse_cp_out.h"

#include "src/evse_src/evse_cp_in.h"

#include "src/evse_src/evse_config.h"

#include "src/evse_src/evse_watch_dog.h"

#include "src/evse_src/evse_addr_led.h"

#include "src/evse_src/evse_connector_parameter.h"

#include "src/evse_src/evse_emergency.h"

#include "src/evse_src/evse_BLE_Commissioning.h"

#include "src/evse_src/evse_device_cfg.h"

#include "src/evse_lib/src/OCPP/urlparse.h"

#include "src/evse_src/evse_wifi.h"

#include "src/evse_src/evse_espnow_master.h"

/******************************************************************************************************/

#if !defined LWIP_TCPIP_CORE_LOCKING || defined __DOXYGEN__
#define LWIP_TCPIP_CORE_LOCKING 1
#endif

#define DEBUG_OUT (1)

#define TEST_OTA (1)

#define WATCH_DOG_RESET_COUNT (120)
#define WATCH_DOG_RESET_INTERVAL (1)

int wifi_counter = 0;
int counter_wifiNotConnected = 0;
bool webSocketConncted = false;
bool offline_connect = false;
bool internet = false;
extern bool wifi_connect;

extern String CP_Id_m;

// extern THREE_PHASE;
const char* ssid_ = "EVRE";
const char* password = "Amplify5";

enum led_col_config evse_led_status; // enum variable.

extern String alpr_data;

#if 1

String host_fota = "";
int port_fota = 0;
String path_fota = "";

char fota_host[32] = { 0 };
int fota_port = 80;
char fota_path[64] = { 0 };
#endif

#if 1
String host_m = "";
int port_m = 0;
String protocol_m = "ocpp1.6j";
String path_m = "";
#endif

#if 1
char host_ocpp[32] = { 0 };
// int port_ocpp = 0;
char protocol_ocpp[16] = "ocpp1.6j";
char path_ocpp[64] = { 0 };
#endif

PILOT_readings_t PILOT_reading;
EVSE_states_enum EVSE_state;

String rfid_buf = "";
String ocpp_rfid = "";

volatile uint8_t gu8_evse_suspend_state_count = 0;
volatile uint8_t evse_spi_reading_states = 0;
volatile uint32_t gu32read_rfid_timer_count = 0;
volatile uint32_t gu32read_energy_meter_timer_count = 0;
volatile uint8_t evse_spi_energy_reading_states = 0;

// volatile uint8_t evse_i2c_reading_states = 0;
// volatile uint32_t gu32read_rtc_timer_count = 0;
// volatile uint32_t gu32read_lcd_timer_count = 0;
volatile uint32_t gu32reset_watch_dog_timer_count = 0;

#if RFID_I2C
volatile uint32_t gu32read_i2c_rfid_timer_count = 0;
#endif

volatile uint8_t gu8_evse_ble_count = 0;

volatile uint8_t evse_fatfs_init_flag = 0;

volatile int32_t gs32EVSE_SMW_Control_pilot_Window[EVSE_CONTROL_PILOT_SM_WINDOW_SIZE];
volatile int32_t gs32EVSE_SMW_Control_pilot_Average = 0.0;
volatile int32_t gs32EVSE_SMW_Control_pilot_Total = 0.0;
volatile uint8_t gu8EVSE_SMW_Control_pilot_WindowIndex = 0;
volatile uint32_t gu32EVSE_SMW_Control_pilot_SampleCnt = 0;

double ocpp_voltage_A = 0;
double ocpp_voltage_B = 0;
double ocpp_voltage_C = 0;
double ocpp_current_A = 0;
double ocpp_current_B = 0;
double ocpp_current_C = 0;

double disp_voltage_A = 0;
double disp_voltage_B = 0;
double disp_voltage_C = 0;
double disp_current_A = 0;
double disp_current_B = 0;
double disp_current_C = 0;

/*inter process communication*/
volatile double ipc_voltage_A = 0;
volatile double ipc_voltage_B = 0;
volatile double ipc_voltage_C = 0;
volatile double ipc_current_A = 0;
volatile double ipc_current_B = 0;
volatile double ipc_current_C = 0;
volatile double ipc_temp = 0;

/*************Preferences***********************/
Preferences evse_preferences;

Preferences evse_resumeTxn;

extern Preferences evse_preferences_ble;

Preferences evse_preferences_ws;

extern Preferences evse_preferences_ocpp;

/**********************************/

uint8_t gu8_cp_serial_update = 0;

uint8_t gu8_cs_serial_update = 0;

uint8_t gu8_cscb_update = 0;

uint8_t gu8_cssn_serial_update = 0;

String cpSerial = String("12345");

String csSerial = String("EVRE");

String cscbSerial = String("7.4_3.3KW");

String cssnSerial = String("SERIAL_NUMBER");

String ble_mac_add;

String evse_idTagData;
bool evse_ongoingTxn;
int evse_transactionId;
float evse_globalmeterstart;

/***********************************/

extern uint8_t gu8_change_avaliable;

char rfid_tag_val[16];
volatile uint8_t gu8_rfid_flag = 0;

// WebSocketsClient webSocket;

uint8_t gu8_online_flag = 0; // Connectivity Avaliable or not, can be indicated by gu8_online_flag

uint8_t gu8_check_online_count = 52;

uint8_t gu8_check_online_count2 = 52;

uint8_t gu8_OTA_update_flag = 0;

extern uint8_t reasonForStop;

extern uint8_t boot_flag_accepted;

// const char resource_get_config[] = "/evse-config-update.php";

enum EvseChargePointErrorCode_OtherError_t EvseChargePointErrorCode_OtherError;
enum EvseChargePointErrorCode EvseChargePoint_ErrorCode;
enum EvseChargePointErrorCode Current_EvseChargePoint_ErrorCode = EVSE_NoError;

enum evsetransactionstatus EVSE_transaction_status;
enum evseconnectivitystatus EVSE_Connectivity_status;

// OcppEvseState OcppEvseStates;
EvseDevStatuse EvseDevStatus_connector_1;
EvseDevStatuse currentEvseDevStatus_connector_1;
static uint8_t gu8_state_change = 0;

uint8_t gu8_evse_change_state = EVSE_BOOT;
// uint8_t gu8_evse_change_state = EVSE_CHANGE_STATE_ZERO;

uint8_t gu8_evse_boot_flag = false;
enum evse_boot_stat_t evse_boot_state;								/* Boot Notification State machine */
enum evse_status_notification_stat_t evse_status_notification_stat; /* Status Notification State machine */
enum evse_authorize_stat_t evse_authorize_state;					/* Authorize State machine */
enum evse_start_txn_stat_t evse_start_txn_state;					/* Start Transaction State machine */
enum evse_stop_txn_stat_t evse_stop_txn_state;						/* Stop Transaction State machine */

int32_t connecter_id = 1;

bool flagswitchoffBLE = false;
int startBLETime = 0;

extern uint8_t gu8_other_type_error_power_loss;

extern bool fota_available;
extern uint8_t gu8_EVSE_CMS_reset;

EVSE_states_enum EVSE_states;

WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

extern enum ChargePointStatus evse_ChargePointStatus; // To get Charge point status

extern enum ChargePointStatus lenum_evse_ChargePointStatus;
extern enum EvseChargePointErrorCode_OtherError_t dip_EvseChargePointErrorCode_OtherError;
// ChargePointStatus Service declarations
ChargePointStatusService* chargePointStatusService;

// Mertering Service declarations
MeteringService* meteringService;

CP_OUT* ptr_cp_out; /*@brife object for cp out*/

unsigned int heartbeatInterval = 30;
extern unsigned int meterSampleInterval;

extern Preferences change_config;

unsigned int evse_suspended_ev_interval = 600; // 600 seconds that is 10 minuts

unsigned int evse_no_load_interval = 120;

uint32_t gu32counter_wifiNotConnected = 0;

uint8_t alternet_10d_flag = 0;
uint8_t Charging_3_3 = 0;

typedef struct Authorised_RFID_tag
{
	String rfid;
} Authorised_RFID_t;

Authorised_RFID_t Authorised_RFID[TOTAL_NO_OF_RFID_TAGS];

#if EVSE_DWIN_ENABLE
extern volatile unsigned long int gs32_st_time;
#endif

extern uint32_t evse_ocpp_config_HeartbeatInterval;
extern bool evse_ocpp_config_AuthorizeRemoteTxRequests;

/*****************************************OCPP CONFIGURATION KEY****************************************************************/
OCPPconfigkeyStorage* ptrOCPPconfigKeys;
extern ocpp_config_keys_t gOCPP_config_keys_write;
extern ocpp_config_keys_t gOCPP_config_keys_read;

/*****************************************OCPP CONFIGURATION KEY****************************************************************/

evseNconnectorparametersStorage* ptr_evse_connector_parameter;

/***********************************************websockt_test***********************************************************************/

ulong timerHb = 0;
ulong timerMv_count = 0;

bool flag_ping_sent = false;

extern bool wifi_connect;
extern bool gsm_connect;

void OTA_4G_setup4G(void);
void OTA_4G_setup_4G_OTA_get(void);
void OTA_4G_setup_4G_OTA(void);

void management_task(void* p);
void Read_rfid_energy(void* p);
void Read_rtc_lcd(void* p);
void ocpp_task(void* p);
void ControlPilot(void* p);

void hexdump(const void* mem, const uint32_t& len, const uint8_t& cols);
void webSocketEvent(const WStype_t& type, uint8_t* payload, const size_t& length);

void setup_WIFI_OTA_get_1(void);
void performUpdate_WiFi_1(WiFiClient& updateSource, size_t updateSize);
void setup_WIFI_OTA_1(void);
void printPercent_1(uint32_t readLength, uint32_t contentLength);
void setup_WIFI_OTA_getconfig_1(void);

void cloudConnectivityLed_Loop(void);

bool Authorization_done(String Received_RFID);

void Update_Authorized_RFID(void);
/*****************************************************************************************************************************************/

String messageToServer = String("Message from Client on ") + String(ARDUINO_BOARD);

void hexdump(const void* mem, const uint32_t& len, const uint8_t& cols = 16)
{
	const uint8_t* src = (const uint8_t*)mem;

	Serial.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);

	for (uint32_t i = 0; i < len; i++)
	{
		if (i % cols == 0)
		{
			Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
		}

		Serial.printf("%02X ", *src);
		src++;
	}

	Serial.printf("\n");
}

bool alreadyConnected = false;
uint32_t wscDis_counter = 0;
uint32_t wscConn_counter = 0;

void webSocketEvent(const WStype_t& type, uint8_t* payload, const size_t& length)
{
	switch (type)
	{
	case WStype_DISCONNECTED:
		if (alreadyConnected)
		{
			Serial.println("[WSc] Disconnected!");
			alreadyConnected = false;

			webSocketConncted = false;

			gu8_online_flag = 0;
			Serial.println("Counter:" + String(wscDis_counter));
			if (DEBUG_OUT)
				Serial.print("[WSc] Disconnected!!!\n");
			if (wscDis_counter++ > 2)
			{
				delay(200);
				// vTaskDelay(2000 / portTICK_PERIOD_MS );
				Serial.println("Trying to reconnect to WSC endpoint");
				wscDis_counter = 0;

				String host_m = "";
				int port_m = 0;
				String protocol_m = "";
				String path_m = "";

				char host_evse[32] = "";
				int port_evse = 0;
				char protocol_evse[8] = "ocpp1.6";
				char path_evse[128] = "";

				evse_preferences_ws.begin("ws_cred", false);
				host_m = evse_preferences_ws.getString("host_m", "");
				path_m = evse_preferences_ws.getString("path_m", "");
				protocol_m = evse_preferences_ws.getString("protocol_m", "");
				port_m = evse_preferences_ws.getUInt("port_m", 0);
				evse_preferences_ws.end();

				strcpy(&host_evse[0], host_m.c_str());
				// strcpy(&protocol_evse[0], protocol_m.c_str());    // Disabled the Dynamic update from End-user
				strcpy(&path_evse[0], path_m.c_str());
				port_evse = port_m;

				Serial.println("host_evse:" + String(host_evse));
				Serial.println("path_evse:" + String(path_evse));
				Serial.println("port_evse:" + String(port_evse));
				Serial.println("protocol_evse:" + String(protocol_evse));

				webSocket.begin(host_evse, port_evse, path_evse, protocol_evse);

				while (!webSocketConncted)
				{ // how to take care if while loop fails
					Serial.print("..***..");
					delay(100); // bit**
					// vTaskDelay(100 / portTICK_PERIOD_MS );
					webSocket.loop(); // after certain time stop relays and set fault state
					if (wscConn_counter++ > 30)
					{
						wscConn_counter = 0;
						Serial.println("[Wsc] Unable To Connect");
						break;
					}
				}
			}
		}

		break;

	case WStype_CONNECTED:
	{
		alreadyConnected = true;
		wifi_connect = true;
		Serial.print("[WSc] Connected to url: ");
		Serial.println((char*)payload);
		if (gu8_online_flag == 0)
		{
			gu8_online_flag = 1;
			evse_ChargePointStatus = NOT_SET;
			// if ((EVSE_transaction_status == EVSE_START_TRANSACTION) && (evse_ChargePointStatus != Faulted))
			// {

			// 	evse_ChargePointStatus = Charging;
			// }
			// else
			// {
			// 	evse_ChargePointStatus = NOT_SET;
			// }
		}
	}
	break;

	case WStype_TEXT:
		Serial.printf("[WSc] get text: %s\n", payload);

		// send message to server
		if (!processWebSocketEvent(payload, length))
		{ // forward message to OcppEngine
			if (DEBUG_OUT)
				Serial.print("[WSc] Processing WebSocket input event failed!\n");
		}

		break;

	case WStype_BIN:
		Serial.printf("[WSc] get binary length: %u\n", length);
		hexdump(payload, length);

		// send data to server
		// webSocket.sendBIN(payload, length);
		Serial.print("[WSc] Incoming binary data stream not supported");

		break;

	case WStype_PING:
		// pong will be send automatically
		Serial.printf("[WSc] get ping\n");

		break;

	case WStype_PONG:
		// answer to a ping we send
		Serial.printf("[WSc] get pong\n");
		EVSE_Connectivity_status = EVSE_CONNECTED;
		gu8_online_flag = 1;
		break;

	case WStype_ERROR:
	case WStype_FRAGMENT_TEXT_START:
	case WStype_FRAGMENT_BIN_START:
	case WStype_FRAGMENT:
	case WStype_FRAGMENT_FIN:

		break;

	default:
		break;
	}
}

bool Authorization_done(String Received_RFID)
{
	for (int i = 0; i < TOTAL_NO_OF_RFID_TAGS; i++)
	{
		if (Authorised_RFID[i].rfid.equals(Received_RFID))
		{
			return 1;
		}
	}
	return 0;
}

void Update_Authorized_RFID(void)
{

	evse_preferences_ble.begin("rfid", false);
	Authorised_RFID[0].rfid = evse_preferences_ble.getString("rfid_1", "");
	Authorised_RFID[1].rfid = evse_preferences_ble.getString("rfid_2", "");
	Authorised_RFID[2].rfid = evse_preferences_ble.getString("rfid_3", "");
	Authorised_RFID[3].rfid = evse_preferences_ble.getString("rfid_4", "");
	Authorised_RFID[4].rfid = evse_preferences_ble.getString("rfid_5", "");
	Authorised_RFID[5].rfid = evse_preferences_ble.getString("rfid_6", "");
	Authorised_RFID[6].rfid = evse_preferences_ble.getString("rfid_7", "");
	Authorised_RFID[7].rfid = evse_preferences_ble.getString("rfid_8", "");
	evse_preferences_ble.end();
}

/************************************************************************************************************ */
String evse_ble_ssid;
String evse_ble_ssid_key;
/***********************************************************************************************************************/
#if 1
RELAY* ptr_evse_relay;
#endif

/* FreeRTOS config TICK RATE Hz 100*/
#define configTICK_RATE_HZ 100

/*************TaskHandler's*********************/
TaskHandle_t taskHandle_1;
TaskHandle_t taskHandle_2;
TaskHandle_t taskHandle_3;
TaskHandle_t taskHandle_4;
TaskHandle_t taskHandle_5;

SemaphoreHandle_t preferencesMutex;

/***********************************************/
portMUX_TYPE rfid_tag_mux = portMUX_INITIALIZER_UNLOCKED;

portMUX_TYPE upload__mux = portMUX_INITIALIZER_UNLOCKED;

/****************************** Atomic Operations *******************/
// void webSocketEvent( WStype_t type, uint8_t *payload, size_t length);
uint8_t evse_earth_count = 0;

uint8_t gu8_change_conf_flag;

uint32_t gu32_evse_suspended_count = 0;

uint32_t gu32_evse_no_load_intervel_count = 0;

//  String change_config ;

enum evseconnectivity_offline_to_online evseconnectivity_offline_to_online_status;

extern Change_configuration_tag_t Change_configuration_instance;
extern LinkedList<Change_configuration_tag_t> LL_Change_configuration_tag;
extern Change_configuration_tag_t get_Change_configuration_instance;
// Change_configuration_tag_t Change_configuration_instance;

unsigned long ble_startTime = 0;
unsigned long ble_lastTime = 0;
unsigned long ble_timerDelay = 30000;

// enum evse_authorize_stat_t evse_authorize_state;

extern uint8_t rfid_read_flag;

ulong gu32InternaltimerMv_count = 0;
unsigned int gu32InternalmeterSampleInterval = 30;
uint8_t gu8_offline_blink_count;

uint8_t gu8_evse_ev_disconnect;

uint8_t gu8_offline_start_flag = 0;

uint8_t gu8_offline_blink_count_1;

uint8_t dis_connection_flag;

uint8_t gu8_offline_flag = 0;

uint8_t gu8_wifi_4g_flag = 0;

uint8_t gu8_evse_cms_reset_flag = 0;

uint8_t gu8_i2cnotfound_flag = 0;

uint32_t evse_over_voltage_threshold = 0;

uint32_t evse_under_voltage_high_threshold = 0;

uint32_t evse_over_current_threshold = 0;

uint32_t evse_high_temperature_threshold = 0;
String strH;

bool plugandcharge_1 = 0;
uint8_t plugandcharge_7_4 = 0;

bool sessionresume = 0;

SemaphoreHandle_t mutexControlPilot;

uint8_t cpcurrentlimit = 0;

volatile uint32_t evse_state_a_upper_threshold = 0;
volatile uint32_t evse_state_a_lower_threshold = 0;
volatile uint32_t evse_state_b_upper_threshold = 0;
volatile uint32_t evse_state_b_lower_threshold = 0;
volatile uint32_t evse_state_c_upper_threshold = 0;
volatile uint32_t evse_state_c_lower_threshold = 0;
volatile uint32_t evse_state_d_upper_threshold = 0;
volatile uint32_t evse_state_d_lower_threshold = 0;
volatile uint32_t evse_state_e_threshold = 0;
volatile uint32_t evse_state_sus_upper_threshold = 0;
volatile uint32_t evse_state_sus_lower_threshold = 0;
volatile uint32_t evse_state_e2_upper_threshold = 0;
volatile uint32_t evse_state_e2_lower_threshold = 0;
volatile uint32_t evse_state_dis_upper_threshold = 0;
volatile uint32_t evse_state_dis_lower_threshold = 0;

uint8_t ev_disconnect_flag = 0;

extern uint8_t flag_rfid;


extern volatile float current_energy_A;

void setup()
{
	// enbale the UART for serial communication
	Serial.begin(115200);
	/***************************************************************************/
	Serial.println(F("*** EVSE Booting ......!***"));
	Serial.print(F("EVSE FIRMWARE VERSION:  "));
	Serial.print(F("[SETUP] ESP32 Freertos tick rate "));
	Serial.println(configTICK_RATE_HZ);
	Serial.print(F("[SETUP] ESP32 Freertos portTICK_PERIOD_MS 0"));
	Serial.println(portTICK_PERIOD_MS);
	Serial.print(F("[SETUP] ESP32 Cpu Frequency  "));
	Serial.println(getCpuFrequencyMhz());
	Serial.print(F("[SETUP] ESP32 Xtal Frequency "));
	Serial.println(getXtalFrequencyMhz());
	Serial.print(F("[SETUP] ESP32 Apb Frequency "));
	Serial.println(getApbFrequency());
	Serial.print(F("[SETUP] ESP32 total heap size "));
	Serial.println(ESP.getHeapSize());
	Serial.print(F("[SETUP] ESP32 available heap  "));
	Serial.println(ESP.getFreeHeap());
	Serial.print(F("[SETUP] ESP32 lowest level of free heap since boot "));
	Serial.println(ESP.getMinFreeHeap());
	Serial.print(F("[SETUP] ESP32 largest block of heap that can be allocated at once"));
	Serial.println(ESP.getMaxAllocPsram());
	Serial.print(F("[SETUP] ESP32 chip Reversion  "));
	Serial.println(ESP.getChipRevision());
	Serial.print(F("[SETUP] ESP32 chip Model "));
	Serial.println(ESP.getChipModel());
	Serial.print(F("[SETUP] ESP32 chip Cores "));
	Serial.println(ESP.getChipCores());
	Serial.print(F("[SETUP] ESP32 SDK Version  "));
	Serial.println(ESP.getSdkVersion());
	Serial.print(F("[SETUP] ESP32 Flash Chip Size  "));
	Serial.println(ESP.getFlashChipSize());
	Serial.print(F("[SETUP] ESP32 Flash Chip Speed  "));
	Serial.println(ESP.getFlashChipSpeed());
	Serial.print(F("[SETUP] ESP32 Sketch Size  "));
	Serial.println(ESP.getSketchSize());
	Serial.print(F("[SETUP] ESP32 Sketch MD5"));
	Serial.println(ESP.getSketchMD5());
	Serial.print(F("[SETUP] ESP32 Free Sketch Space  "));
	Serial.println(ESP.getFreeSketchSpace());
	ble_mac_add = WiFi.macAddress();
	Serial.print(F("[SETUP] ESP32 WIFI MAC "));
	Serial.println(ble_mac_add);
	Serial.print(F("[SETUP]EVSE DEVICE VERSION :"));
	Serial.println(EVSE_CHARGE_POINT_FIRMWARE_VERSION);
	Serial.print(F("[SETUP]EVSE CHARGE POINT VENDOR : "));
	Serial.println(EVSE_CHARGE_POINT_VENDOR);
	Serial.print(F("[SETUP]EVSE CHARGE POINT MODEL : "));
	Serial.println(EVSE_CHARGE_POINT_MODEL);
	// Serial.println("[SETUP]EVSE ARDUINO ESP32 GIT DESC RELEASE VERSION : " +String(ARDUINO_ESP32_GIT_DESC));
	Serial.print(F("[SETUP]EVSE ARDUINO ESP32 RELEASE VERSION :"));
	Serial.println(ARDUINO_ESP32_RELEASE);
	Serial.print(F("[SETUP]EVSE ARDUINO ESP32 GIT VERSION : "));
	Serial.println(ARDUINO_ESP32_GIT_VER);
	Serial.print(F("[SETUP]EVSE Compiled: "));
	Serial.print(__DATE__);
	Serial.println(__TIME__);
	// Serial.println(/* __VERSION__ */);
	Serial.print(F("[SETUP] FILE NAME : "));
	Serial.println(__FILE__);
	Serial.print(F("[SETUP] Arduino IDE version: "));
	Serial.println(ARDUINO, DEC);

	Serial.println("Code Design and development by : Krishna & Rakesh");
	Serial.println("All rights Recived by : EVRE");
	/*****************************************************************************************/
	// Disconnect the wifi
	WiFi.disconnect();
	/****************************************************************************************************/

#if 1
	evse_preferences_ble.begin("fota_url", false);
	String evse_fota_uri = evse_preferences_ble.getString("fota_uri", "");
	uint32_t evse_fota_retries = evse_preferences_ble.getUInt("fota_retries", 0);
	String evse_fota_date = evse_preferences_ble.getString("fota_date", "");
	bool evse_fota_avail = evse_preferences_ble.getBool("fota_avial", false);
	evse_preferences_ble.end();

	Serial.println("FOTA UPDATE states ");
	Serial.println("evse_fota_uri :  " + String(evse_fota_uri));
	Serial.println("evse_fota_retries :  " + String(evse_fota_retries));
	Serial.println("evse_fota_date :  " + String(evse_fota_date));
	Serial.println("evse_fota_avail :  " + String(evse_fota_avail));

	// evse_preferences_ble.begin("change_conf", false);
	// String evse_change_config = evse_preferences_ble.getString("change_conf", change_config);
	// evse_preferences_ble.end();

	// Serial.println("evse_change_config:  " + String(evse_change_config));

	evse_preferences_ble.begin("connectivity", false);
	// bool evse_ble_wifi = evse_preferences_ble.getBool("wifi", "");
	// bool evse_ble_gsm = evse_preferences_ble.getBool("gsm", "");
	uint32_t connnection_type = evse_preferences_ble.getUInt("conn_type", 0);
	evse_preferences_ble.end();

	// Serial.println("evse_ble_wifi :  " + String(evse_ble_wifi));
	// Serial.println("evse_ble_gsm :  " + String(evse_ble_gsm));
	Serial.println("connnection_type :  " + String(connnection_type));
#endif

	evse_preferences_ws.begin("fota_url_parser", false);
	host_fota = evse_preferences_ws.getString("fota_host", "");
	path_fota = evse_preferences_ws.getString("fota_path", "");
	port_fota = evse_preferences_ws.getUInt("fota_port", 0);
	evse_preferences_ws.end();

	Serial.println("host_fota : " + String(host_fota));
	Serial.println("path_fota : " + String(path_fota));
	Serial.println("port_fota : " + String(port_fota));

	Serial.println("connnection_type :  " + String(connnection_type));

	ADDR_LEDs* ptr_leds;
	ptr_leds->ADDR_LEDs_Init();
	ptr_leds->White_led();

#if EVSE_LCD_ENABLE
	evse_lcd_init();
	evse_lcd_initializing();

#endif
#if EVSE_DWIN_ENABLE
	evse_dwin_setup();
	delay(200);
#endif

#if EVSE_DWIN_ENABLE
	// evse_page_change(EVSE_SET_ZERO);//commented by MG removed
	evse_page_change(EVSE_SET_ELEVEN);
	vTaskDelay(200 / portTICK_PERIOD_MS);
#endif

	/***********************************BLE********************************************** */

	evse_preferences_ble.begin("ChargerDetails", false);
	String evse_ble_cssn = evse_preferences_ble.getString("cssn", "");
	String evse_ble_csn = evse_preferences_ble.getString("csn", "");
	String evse_ble_doc = evse_preferences_ble.getString("doc", "");
	String evse_ble_cpserial = evse_preferences_ble.getString("cpsn", "");
	String evse_ble_cscb = evse_preferences_ble.getString("cscb", "");
	evse_preferences_ble.end();

	evse_preferences_ble.begin("eth", false);
	uint32_t evse_ble_eth = evse_preferences_ble.getUInt("eth_req", 0);
	evse_preferences_ble.end();

	if (evse_ble_cpserial.equals("") == true)
	{
		gu8_cp_serial_update = 0;
	}
	else
	{
		gu8_cp_serial_update = 1;
		cpSerial = evse_ble_cpserial;
	}

	Serial.println("cpSerial" + String(cpSerial));

	if (evse_ble_csn.equals("") == true)
	{
		gu8_cs_serial_update = 0;
	}
	else
	{
		gu8_cs_serial_update = 1;
		csSerial = evse_ble_csn;
	}
	Serial.println("csSerial" + String(csSerial));

	if (evse_ble_cscb.equals("") == true)
	{
		gu8_cscb_update = 0;
	}
	else
	{
		gu8_cscb_update = 1;
		cscbSerial = evse_ble_cscb;
	}
	Serial.println("cscb" + String(cscbSerial));

	if (evse_ble_cssn.equals("") == true)
	{
		gu8_cssn_serial_update = 0;
	}
	else
	{
		gu8_cssn_serial_update = 1;
		cssnSerial = evse_ble_cssn;
	}
	Serial.println("cssnSerial" + String(cssnSerial));

	/*BLE*/
	evse_preferences_ble.begin("FaultThreshold", false);
	uint32_t over_voltage = evse_preferences_ble.getInt("overvoltage", 0);
	uint32_t under_voltage = evse_preferences_ble.getInt("undervoltage", 0);
	uint32_t over_current = evse_preferences_ble.getInt("overcurrent", 0); // overtemparature
	uint32_t over_temperature = evse_preferences_ble.getInt("overtemperature", 0);
	plugandcharge_1 = evse_preferences_ble.getBool("plugandcharge", 0);
	sessionresume = evse_preferences_ble.getBool("sessionresum", 0);
	cpcurrentlimit = evse_preferences_ble.getInt("CPcurrentLimit", 0);
	evse_preferences_ble.end();

	/*BLE*/
	Serial.println("over_voltage : " + String(over_voltage));
	Serial.println("under_voltage : " + String(under_voltage));
	Serial.println("over_current : " + String(over_current));
	Serial.println("over_temperature : " + String(over_temperature));
	Serial.println("plugandcharge_1 : " + String(plugandcharge_1));
	Serial.println("sessionresume : " + String(sessionresume));
	Serial.println("currentLimit : " + String(cpcurrentlimit));

	if (plugandcharge_1 == 1)
	{
		plugandcharge_7_4 = 1;
	}

	if (cpcurrentlimit == 10)
	{
		evse_state_a_upper_threshold = 4096;
		evse_state_a_lower_threshold = 3900;
		evse_state_b_upper_threshold = 3500;
		evse_state_b_lower_threshold = 2600;
		evse_state_c_upper_threshold = 444;
		evse_state_c_lower_threshold = 356;
		evse_state_d_upper_threshold = 355;
		evse_state_d_lower_threshold = 300;
		evse_state_e_threshold = 299;
		evse_state_sus_upper_threshold = 522;
		evse_state_sus_lower_threshold = 445;
		evse_state_e2_upper_threshold = 2399;
		evse_state_e2_lower_threshold = 2100;
		evse_state_dis_upper_threshold = 650;
		evse_state_dis_lower_threshold = 521;
	}
	else if (cpcurrentlimit == 16)
	{
		evse_state_a_upper_threshold = 4096;
		evse_state_a_lower_threshold = 3900;
		evse_state_b_upper_threshold = 3500;
		evse_state_b_lower_threshold = 2600;
		evse_state_c_upper_threshold = 747;
		evse_state_c_lower_threshold = 626;
		evse_state_d_upper_threshold = 625;
		evse_state_d_lower_threshold = 500;
		evse_state_e_threshold = 499;
		evse_state_sus_upper_threshold = 877;
		evse_state_sus_lower_threshold = 748;
		evse_state_e2_upper_threshold = 2399;
		evse_state_e2_lower_threshold = 2100;
		evse_state_dis_upper_threshold = 1050;
		evse_state_dis_lower_threshold = 878;
	}
	else if (cpcurrentlimit == 20)
	{
		evse_state_a_upper_threshold = 4096;
		evse_state_a_lower_threshold = 3900;
		evse_state_b_upper_threshold = 3500;
		evse_state_b_lower_threshold = 2600;
		evse_state_c_upper_threshold = 960;
		evse_state_c_lower_threshold = 802;
		evse_state_d_upper_threshold = 801;
		evse_state_d_lower_threshold = 650;
		evse_state_e_threshold = 649;
		evse_state_sus_upper_threshold = 1128;
		evse_state_sus_lower_threshold = 961;
		evse_state_e2_upper_threshold = 2399;
		evse_state_e2_lower_threshold = 2100;
		evse_state_dis_upper_threshold = 1300;
		evse_state_dis_lower_threshold = 1129;
	}
	else if (cpcurrentlimit == 25)
	{
		evse_state_a_upper_threshold = 4096;
		evse_state_a_lower_threshold = 3900;
		evse_state_b_upper_threshold = 3500;
		evse_state_b_lower_threshold = 2600;
		evse_state_c_upper_threshold = 1207;
		evse_state_c_lower_threshold = 1008;
		evse_state_d_upper_threshold = 1007;
		evse_state_d_lower_threshold = 800;
		evse_state_e_threshold = 799;
		evse_state_sus_upper_threshold = 1415;
		evse_state_sus_lower_threshold = 1208;
		evse_state_e2_upper_threshold = 2399;
		evse_state_e2_lower_threshold = 2100;
		evse_state_dis_upper_threshold = 1650;
		evse_state_dis_lower_threshold = 1416;
	}
	else if (cpcurrentlimit == 32)
	{
		evse_state_a_upper_threshold = 4096;
		evse_state_a_lower_threshold = 3900;
		evse_state_b_upper_threshold = 3500;
		evse_state_b_lower_threshold = 2600;
		evse_state_c_upper_threshold = 1575;
		evse_state_c_lower_threshold = 1317;
		evse_state_d_upper_threshold = 1316;
		evse_state_d_lower_threshold = 1000;
		evse_state_e_threshold = 999;
		evse_state_sus_upper_threshold = 1799;
		evse_state_sus_lower_threshold = 1576;
		evse_state_e2_upper_threshold = 2399;
		evse_state_e2_lower_threshold = 2100;
		evse_state_dis_upper_threshold = 2100;
		evse_state_dis_lower_threshold = 1800;
	}
	else
	{
		evse_state_a_upper_threshold = EVSE_STATE_A_UPPER_THRESHOULD;
		evse_state_a_lower_threshold = EVSE_STATE_A_LOWER_THRESHOULD;
		evse_state_b_upper_threshold = EVSE_STATE_B_UPPER_THRESHOULD;
		evse_state_b_lower_threshold = EVSE_STATE_B_LOWER_THRESHOULD;
		evse_state_c_upper_threshold = EVSE_STATE_C_UPPER_THRESHOULD;
		evse_state_c_lower_threshold = EVSE_STATE_C_LOWER_THRESHOULD;
		evse_state_d_upper_threshold = EVSE_STATE_D_UPPER_THRESHOULD;
		evse_state_d_lower_threshold = EVSE_STATE_D_LOWER_THRESHOULD;
		evse_state_e_threshold = EVSE_STATE_E_THRESHOULD;
		evse_state_sus_upper_threshold = EVSE_STATE_SUS_UPPER_THRESHOULD;
		evse_state_sus_lower_threshold = EVSE_STATE_SUS_LOWER_THRESHOULD;
		evse_state_e2_upper_threshold = EVSE_STATE_E2_UPPER_THRESHOULD;
		evse_state_e2_lower_threshold = EVSE_STATE_E2_LOWER_THRESHOULD;
		evse_state_dis_upper_threshold = EVSE_STATE_DIS_UPPER_THRESHOULD;
		evse_state_dis_lower_threshold = EVSE_STATE_DIS_LOWER_THRESHOULD;
	}

	// if ((evse_fota_avail == false) && (connnection_type != CONNECT_WIFI_GSM_ETHERNET_DISABLED))
	if (evse_fota_avail == false)
	{

		Serial.println("BLE Configuration...");
		BLE_setup();
		// digitalWrite(GREEN_LED_GPIO_NUM, HIGH);
		ble_startTime = millis();
		ble_lastTime = millis();
		while ((ble_lastTime - ble_startTime) <= ble_timerDelay)
		{
			ble_lastTime = millis();
			BLE_loop();
			// esp_task_wdt_reset();
		}

		BLE_deinit();
		// digitalWrite(GREEN_LED_GPIO_NUM, LOW);
		Serial.print(F("Available FREE HEAP: "));
		Serial.println(ESP.getFreeHeap());
		Serial.println("");
	}

	switch (connnection_type)
	{

	case CONNECT_WIFI_ENABLED:
	{
		gu8_wifi_4g_flag = 1;

		evse_preferences_ble.begin("wifi_cred", false);
		evse_ble_ssid = evse_preferences_ble.getString("ssid", "");
		evse_ble_ssid_key = evse_preferences_ble.getString("KEY", "");
		evse_preferences_ble.end();

		Serial.println("wifi_ssid " + String(evse_ble_ssid));
		Serial.println("wifi_key " + String(evse_ble_ssid_key));

		if ((evse_ble_ssid.equals("") == true))
		{
			Serial.println("evse_ble_ssid ...!");
			BLE_setup();
			do
			{
				BLE_loop();
			} while (gu8_evse_ble_count == 0);

			if (gu8_evse_ble_count == 1)
			{

				ESP.restart();
			}
		}
		//  evse_fota_avail = true;    //remove after test
		if (evse_fota_avail == true)
		{

#if EVSE_FOTA_ENABLE
			setup_WIFI_OTA_get_1();
			evse_preferences_ble.begin("fota_url", false);
			evse_preferences_ble.putString("fota_uri", "");
			evse_preferences_ble.putUInt("fota_retries", 0);
			evse_preferences_ble.putString("fota_date", "");
			evse_preferences_ble.putBool("fota_avial", false);
			evse_preferences_ble.end();

			switch (gu8_OTA_update_flag)
			{
			case 2:
				Serial.println("OTA update available, In Switch ...!");

				ADDR_LEDs* ptr_leds;
				ptr_leds->ADDR_LEDs_Init();
				ptr_leds->White_led();

				setup_WIFI_OTA_1();
				break;
			case 3:
				Serial.println("No OTA update available, In Switch ...!");
				break;
			default:
				Serial.println("default case OTA update, In Switch ...! \r\n gu8_OTA_update_flag :" + String(gu8_OTA_update_flag));
				break;
			}
#endif
		}
#if EVSE_LCD_ENABLE
		evse_lcd_connecting();
#endif
		// WiFi.disconnect(true);
		// WiFi.mode(WIFI_STA); /*wifi station mode*/
		// 					 //  WiFi.begin(ssid_, password);
		// 					 //  WiFi.begin("EVRE1", "Amplify5");
		// WiFi.begin(evse_ble_ssid.c_str(), evse_ble_ssid_key.c_str());
		wifi_init();
		// init_esp_now();

		Serial.println("\nConnecting");

#if 1

		gu32counter_wifiNotConnected = 50;
		while ((WiFi.status() != WL_CONNECTED) && (gu32counter_wifiNotConnected-- > 0))
		{
			// Serial.print(".");
			Serial.println("cheking_wifi.....");
			// delay(100);
			vTaskDelay(10000 / portTICK_PERIOD_MS);
		}
#endif
		// if (gu32counter_wifiNotConnected++ > 50)
		// {
		//   Serial.print("cheking_wifi.....");
		//   gu32counter_wifiNotConnected = 0;
		// }

		Serial.println("\nConnected to the WiFi network");
		Serial.print("Local ESP32 IP: ");
		Serial.println(WiFi.localIP());

		vTaskDelay(5000 / portTICK_PERIOD_MS);

		Serial.print("\nStarting ESP32_WebSocketClient on ");
		Serial.println(ARDUINO_BOARD);
		// Serial.println(WEBSOCKETS_GENERIC_VERSION);

		Serial.setDebugOutput(true);

		// WiFiMulti.addAP("EVRE", "Amplify5");
		// WiFiMulti.addAP(evse_ble_ssid.c_str(), evse_ble_ssid_key.c_str());  // commneted 08092023_113500

		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
	break;

	case CONNECT_GSM_ENABLED:
		gu8_wifi_4g_flag = 1;
		// evse_fota_avail = true;    //remove after test
		if (evse_fota_avail == true)
		{
			if (!FFat.begin(true))
			{
				Serial.println("Mount Failed");
			}
			else
			{
				Serial.println("File system mounted");
				ADDR_LEDs* ptr_leds;
				ptr_leds->ADDR_LEDs_Init();
				ptr_leds->White_led();

				OTA_4G_setup4G();
				Serial.println("******checking for OTA******");
				OTA_4G_setup_4G_OTA_get();
				Serial.println("******OTA check done******");
				delay(1000);
				evse_preferences_ble.begin("fota_url", false);
				evse_preferences_ble.putString("fota_uri", "");
				evse_preferences_ble.putUInt("fota_retries", 0);
				evse_preferences_ble.putString("fota_date", "");
				evse_preferences_ble.putBool("fota_avial", false);
				evse_preferences_ble.end();
				OTA_4G_setup_4G_OTA();
				Serial.println("******OTA update done******");
			}
		}

		// SetupGsm();

		break;

	case CONNECT_WIFI_GSM_ENABLED:

		break;

	case CONNECT_ETHERNET_ENABLED:

		break;

	case CONNECT_WIFI_ETHERNET_ENABLED:
	{
		gu8_wifi_4g_flag = 1;

		evse_preferences_ble.begin("wifi_cred", false);
		evse_ble_ssid = evse_preferences_ble.getString("ssid", "");
		evse_ble_ssid_key = evse_preferences_ble.getString("KEY", "");
		evse_preferences_ble.end();

		Serial.println("wifi_ssid " + String(evse_ble_ssid));
		Serial.println("wifi_key " + String(evse_ble_ssid_key));

		if ((evse_ble_ssid.equals("") == true))
		{
			Serial.println("evse_ble_ssid ...!");
			BLE_setup();
			do
			{
				BLE_loop();
			} while (gu8_evse_ble_count == 0);

			if (gu8_evse_ble_count == 1)
			{
				ESP.restart();
			}
		}

		if (evse_fota_avail == true)
		{

#if EVSE_FOTA_ENABLE
			setup_WIFI_OTA_get_1();
			evse_preferences_ble.begin("fota_url", false);
			evse_preferences_ble.putString("fota_uri", "");
			evse_preferences_ble.putUInt("fota_retries", 0);
			evse_preferences_ble.putString("fota_date", "");
			evse_preferences_ble.putBool("fota_avial", false);
			evse_preferences_ble.end();

			switch (gu8_OTA_update_flag)
			{
			case 2:
				Serial.println("OTA update available, In Switch ...!");

				ADDR_LEDs* ptr_leds;
				ptr_leds->ADDR_LEDs_Init();
				ptr_leds->White_led();

				setup_WIFI_OTA_1();
				break;
			case 3:
				Serial.println("No OTA update available, In Switch ...!");
				break;
			default:
				Serial.println("default case OTA update, In Switch ...! \r\n gu8_OTA_update_flag :" + String(gu8_OTA_update_flag));
				break;
			}
#endif
		}
#if EVSE_LCD_ENABLE
		evse_lcd_connecting();
#endif
		// WiFi.disconnect(true);
		// WiFi.mode(WIFI_STA); /*wifi station mode*/
		// 					 //  WiFi.begin(ssid_, password);
		// 					 //  WiFi.begin("EVRE1", "Amplify5");
		// WiFi.begin(evse_ble_ssid.c_str(), evse_ble_ssid_key.c_str());
		wifi_init();

		Serial.println("\nConnecting");
#if 1

		gu32counter_wifiNotConnected = 50;
		while ((WiFi.status() != WL_CONNECTED) && (gu32counter_wifiNotConnected-- > 0))
		{
			// Serial.print(".");
			Serial.println("cheking_wifi.....");
			// delay(100);
			vTaskDelay(10000 / portTICK_PERIOD_MS);
		}
#endif
		// if (gu32counter_wifiNotConnected++ > 50)
		// {
		//   Serial.print("cheking_wifi.....");
		//   gu32counter_wifiNotConnected = 0;
		// }

		Serial.println("\nConnected to the WiFi network");
		Serial.print("Local ESP32 IP: ");
		Serial.println(WiFi.localIP());

		vTaskDelay(5000 / portTICK_PERIOD_MS);

		Serial.print("\nStarting ESP32_WebSocketClient on ");
		Serial.println(ARDUINO_BOARD);
		// Serial.println(WEBSOCKETS_GENERIC_VERSION);

		Serial.setDebugOutput(true);

		// WiFiMulti.addAP("EVRE", "Amplify5");
		// WiFiMulti.addAP(evse_ble_ssid.c_str(), evse_ble_ssid_key.c_str());  // commneted 08092023_113500

		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
	break;

	case CONNECT_GSM_ETHERNET_ENABLED:
		if (evse_fota_avail == true)
		{
			if (!FFat.begin(true))
			{
				Serial.println("Mount Failed");
			}
			else
			{
				Serial.println("File system mounted");
				ADDR_LEDs* ptr_leds;
				ptr_leds->ADDR_LEDs_Init();
				ptr_leds->White_led();

				OTA_4G_setup4G();
				Serial.println("******checking for OTA******");
				OTA_4G_setup_4G_OTA_get();
				Serial.println("******OTA check done******");
				delay(1000);
				evse_preferences_ble.begin("fota_url", false);
				evse_preferences_ble.putString("fota_uri", "");
				evse_preferences_ble.putUInt("fota_retries", 0);
				evse_preferences_ble.putString("fota_date", "");
				evse_preferences_ble.putBool("fota_avial", false);
				evse_preferences_ble.end();
				OTA_4G_setup_4G_OTA();
				Serial.println("******OTA update done******");
			}
		}
		break;

	case CONNECT_WIFI_GSM_ETHERNET_ENABLED:
	{
		gu8_wifi_4g_flag = 1;

		evse_preferences_ble.begin("wifi_cred", false);
		evse_ble_ssid = evse_preferences_ble.getString("ssid", "");
		evse_ble_ssid_key = evse_preferences_ble.getString("KEY", "");
		evse_preferences_ble.end();

		Serial.println("wifi_ssid " + String(evse_ble_ssid));
		Serial.println("wifi_key " + String(evse_ble_ssid_key));

		if ((evse_ble_ssid.equals("") == true))
		{
			Serial.println("evse_ble_ssid ...!");
			BLE_setup();
			do
			{
				BLE_loop();
			} while (gu8_evse_ble_count == 0);

			if (gu8_evse_ble_count == 1)
			{
				ESP.restart();
			}
		}
		//  evse_fota_avail = true;    //remove after test
		if (evse_fota_avail == true)
		{

#if EVSE_FOTA_ENABLE
			setup_WIFI_OTA_get_1();
			evse_preferences_ble.begin("fota_url", false);
			evse_preferences_ble.putString("fota_uri", "");
			evse_preferences_ble.putUInt("fota_retries", 0);
			evse_preferences_ble.putString("fota_date", "");
			evse_preferences_ble.putBool("fota_avial", false);
			evse_preferences_ble.end();

			switch (gu8_OTA_update_flag)
			{
			case 2:
				Serial.println("OTA update available, In Switch ...!");

				ADDR_LEDs* ptr_leds;
				ptr_leds->ADDR_LEDs_Init();
				ptr_leds->White_led();

				setup_WIFI_OTA_1();
				break;
			case 3:
				Serial.println("No OTA update available, In Switch ...!");
				break;
			default:
				Serial.println("default case OTA update, In Switch ...! \r\n gu8_OTA_update_flag :" + String(gu8_OTA_update_flag));
				break;
			}
#endif
		}
#if EVSE_LCD_ENABLE
		evse_lcd_connecting();
#endif
		evse_charger_initialization();
		// WiFi.disconnect(true);
		// WiFi.mode(WIFI_STA); /*wifi station mode*/
		// 					 //  WiFi.begin(ssid_, password);
		// 					 //  WiFi.begin("EVRE1", "Amplify5");
		// WiFi.begin(evse_ble_ssid.c_str(), evse_ble_ssid_key.c_str());
		wifi_init();

		Serial.println("\nConnecting");
#if 1

		gu32counter_wifiNotConnected = 50;
		while ((WiFi.status() != WL_CONNECTED) && (gu32counter_wifiNotConnected-- > 0))
		{
			// Serial.print(".");
			Serial.println("cheking_wifi.....");
			// delay(100);
			vTaskDelay(10000 / portTICK_PERIOD_MS);
		}
#endif
		// if (gu32counter_wifiNotConnected++ > 50)
		// {
		//   Serial.print("cheking_wifi.....");
		//   gu32counter_wifiNotConnected = 0;
		// }

		Serial.println("\nConnected to the WiFi network");
		Serial.print("Local ESP32 IP: ");
		Serial.println(WiFi.localIP());

		vTaskDelay(5000 / portTICK_PERIOD_MS);

		Serial.print("\nStarting ESP32_WebSocketClient on ");
		Serial.println(ARDUINO_BOARD);
		// Serial.println(WEBSOCKETS_GENERIC_VERSION);

		Serial.setDebugOutput(true);

		// WiFiMulti.addAP("EVRE", "Amplify5");
		// WiFiMulti.addAP(evse_ble_ssid.c_str(), evse_ble_ssid_key.c_str());  // commneted 08092023_113500

		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
	break;

	case CONNECT_WIFI_GSM_ETHERNET_DISABLED:
	{
		gu8_offline_flag = 1;
#if 0
		evse_preferences_ble.begin("wifi_cred", false);
		evse_ble_ssid = evse_preferences_ble.getString("ssid", "");
		evse_ble_ssid_key = evse_preferences_ble.getString("KEY", "");
		evse_preferences_ble.end();

		Serial.println("wifi_ssid " + String(evse_ble_ssid));
		Serial.println("wifi_key " + String(evse_ble_ssid_key));

		if ((evse_ble_ssid.equals("") == true))
		{
			evse_ble_ssid = "evre_admin";
			evse_ble_ssid_key = "evre_password";
		}

		evse_fota_avail = true;
		if (evse_fota_avail == true)
		{

#if EVSE_FOTA_ENABLE
			setup_WIFI_OTA_get_1();
			evse_preferences_ble.begin("fota_url", false);
			evse_preferences_ble.putString("fota_uri", "");
			evse_preferences_ble.putUInt("fota_retries", 0);
			evse_preferences_ble.putString("fota_date", "");
			evse_preferences_ble.putBool("fota_avial", false);
			evse_preferences_ble.end();

			switch (gu8_OTA_update_flag)
			{
			case 2:
				Serial.println("OTA update available, In Switch ...!");

				ADDR_LEDs* ptr_leds;
				ptr_leds->ADDR_LEDs_Init();
				ptr_leds->White_led();

				setup_WIFI_OTA_1();
				break;
			case 3:
				Serial.println("No OTA update available, In Switch ...!");
				WiFi.disconnect(true);
				break;
			default:
				Serial.println("default case OTA update, In Switch ...! \r\n gu8_OTA_update_flag :" + String(gu8_OTA_update_flag));
				break;
			}
#endif
		}
#endif
	}
	break;

	default:

		break;
	}
	ptr_evse_relay->evse_relay_init();
	ptr_evse_relay->evse_relay_off();
	ptr_evse_relay->evse_relay2_init();
	ptr_evse_relay->evse_relay2_off();
	// evse_ChargePointStatus = Unavailable;
	// evse_led_status = Blinky_White;
	// EVSE_Connectivity_status = EVSE_NOCONNECTION;
	urlparser();

	Update_Authorized_RFID();
#if 0
	if (connnection_type == CONNECT_WIFI_GSM_ETHERNET_DISABLED)
	{
		Serial.println("BLE Configuration...");
		BLE_setup();
		// digitalWrite(GREEN_LED_GPIO_NUM, HIGH);
		ble_startTime = millis();
		ble_lastTime = millis();
		while ((ble_lastTime - ble_startTime) <= ble_timerDelay)
		{
			ble_lastTime = millis();
			BLE_loop();
			// esp_task_wdt_reset();
		}

		BLE_deinit();
		// digitalWrite(GREEN_LED_GPIO_NUM, LOW);
		Serial.print(F("Available FREE HEAP: "));
		Serial.println(ESP.getFreeHeap());
		Serial.println("");
	}
#endif

	EvseDevStatus_connector_1 = flag_EVSE_is_Booted;
	evse_boot_state = EVSE_BOOT_INITIATED;
	EVSE_transaction_status = EVSE_NO_TRANSACTION;

	evse_ChargePointStatus = NOT_SET;
	evse_led_status = Blinky_White;

	// if (gu8_offline_flag == 0) // plug and play only works in offline
	// {
	// 	plugandcharge_1 = 0;
	// }
#if EVSE_DWIN_ENABLE
	evse_page_change(EVSE_SET_ELEVEN);
	vTaskDelay(200 / portTICK_PERIOD_MS);
#endif

	/***************************************************wifi***********************************************/

#if 0
  /*****************************************OCPP CONFIGURATION KEY****************************************************************/
	Serial.println("OCPP_config_keys_init");
	ptrOCPPconfigKeys->OCPP_config_keys_init();
	ptrOCPPconfigKeys->OCPP_config_keys_dump();
#endif
	// gOCPP_config_keys_read.ocpp_std_config_keys[0].access_R_RW

	// evse_preferences_ocpp.begin("AuthorizeRemoteTxRequests", false);
	// evse_ocpp_config_AuthorizeRemoteTxRequests = evse_preferences_ocpp.getBool("AuthorizeRemote_TxRequests", false);
	// Serial.printf("AuthorizeRemoteTxRequests: %d \n", evse_ocpp_config_AuthorizeRemoteTxRequests);
	// evse_preferences_ocpp.end();

	// evse_preferences_ocpp.begin("HeartbeatInterval", false);
	// evse_ocpp_config_HeartbeatInterval = evse_preferences_ocpp.getUInt("Heartbeat_Interval", 0);
	// Serial.printf("HeartbeatInterval: %d \n", evse_ocpp_config_HeartbeatInterval);
	// evse_preferences_ocpp.end();

	/*****************************************OCPP CONFIGURATION KEY****************************************************************/

#if 0
	Serial.println("evse_connector_parameters_init");
	ptr_evse_connector_parameter->evse_connector_parameters_init();
	ptr_evse_connector_parameter->evse_connector_parameters_dump();
#endif
	// ptr_evse_connector_parameter->evse_connector_parameters_write_param((uint8_t *)"123455678", sizeof("123455678"), 1,  654321,  1254, 0);
	// ptr_evse_connector_parameter->evse_connector_parameters_read();
	// ptr_evse_connector_parameter->evse_connector_parameters_dump();

	change_config.begin("configurations", false);
	int metersample = change_config.getInt("meterSampleInterval", 0);
	change_config.end();

	meterSampleInterval = metersample;

	/**************************************************************TASK1******************************************************************/
#if TASK1_CONTROLPOILT
	xTaskCreatePinnedToCore( // Use xTaskCreate() in vanilla FreeRTOS
		ControlPilot,		 // Function to be called
		"ControlPilot",		 // Name of task
		2048,				 // Stack size (bytes in ESP32, words in FreeRTOS)
		NULL,				 // Parameter to pass
		1,					 // Task priority
		&taskHandle_5,		 // Task handle
		// NULL,
		1);
	// Run on core one (ESP32 only)
#endif

#if TASK2_MANAGEMENT
	xTaskCreatePinnedToCore( // Use xTaskCreate() in vanilla FreeRTOS
		management_task,	 // Function to be called
		"management",		 // Name of task
		2048,				 // Stack size (bytes in ESP32, words in FreeRTOS)
		NULL,				 // Parameter to pass
		1,					 // Task priority
		&taskHandle_1,		 // Task handle
		// NULL,
		1);
	// Run on core one (ESP32 only)
#endif

/************************************************************** TASK2******************************************************************/
#if TASK3_RFID_ENERGY
	xTaskCreatePinnedToCore( // Use xTaskCreate() in vanilla FreeRTOS
		Read_rfid_energy,	 // Function to be called
		"Read_rfid_energy",	 // Name of task
		4096,				 // Stack size (bytes in ESP32, words in FreeRTOS)
		NULL,				 // Parameter to pass
		1,					 // Task priority
		&taskHandle_2,		 // Task handle
		1);
#endif // Run on core one (ESP32 only)

	/**************************************************************TASK3******************************************************************/

#if TASK4_RTC_LCD
	xTaskCreatePinnedToCore( // Use xTaskCreate() in vanilla FreeRTOS
		Read_rtc_lcd,		 // Function to be called
		"read_rtc_lcd",		 // Name of task
		2048,				 // Stack size (bytes in ESP32, words in FreeRTOS)
		NULL,				 // Parameter to pass
		1,					 // Task priority
		&taskHandle_3,		 // Task handle
		1);
#endif

	/**************************************************************TASK4******************************************************************/
#if TASK5_OCPP
	xTaskCreatePinnedToCore( // Use xTaskCreate() in vanilla FreeRTOS
		ocpp_task,			 // Function to be called
		"ocpp_task",		 // Name of task
		4096,				 // Stack size (bytes in ESP32, words in FreeRTOS)
		NULL,				 // Parameter to pass
		1,					 // Task priority
		&taskHandle_4,		 // Task handle
		1);
#endif // Run on core one (ESP32 only)
}
/**
 * The main execution loop of the program.
 *
 */
void loop()
{
	vTaskDelay(2000 / portTICK_PERIOD_MS);
}

/**
 * Task function for managing various EVSE operations including fault handling and LED indications.
 *
 * @param p Pointer to the task input parameter (not used in this function)
 *
 * @return None
 *
 * @throws None
 */
void management_task(void* p)
{

	uint32_t delayTime = 500;
	TickType_t xLastWakeTime;

	GFCI evse_gfci;
	evse_gfci.gfci_init();

	EARTH evse_earth;
	evse_earth.earth_init();

	EMGY evse_emer;
	evse_emer.emergency_init();

	// ptr_cp_in->read_cp_init();

	WATCH_DOG* ptr_watch_dog;
	ptr_watch_dog->evse_watch_dog_init();

	ADDR_LEDs* ptr_leds;
	ptr_leds->ADDR_LEDs_Init();

	uint32_t lu_relay_count = 0;
	int32_t ADC_Result = 0;
	Serial.print(" management_task running on core ");
	Serial.println(xPortGetCoreID());

	const TickType_t xFrequency_Task1 = 200; // delay for mS
	// ptr_cp_out->set_cp_begin();
	// evse_led_status = Green;
	uint32_t loopCount_fult = 0;
	xLastWakeTime = xTaskGetTickCount();
	while (1)
	{

		uint32_t lu32_emergency = evse_emer.emergency_read();
		if (lu32_emergency > 2047)
		{
			reasonForStop = 0;
			ptr_evse_relay->evse_relay_off();
			ptr_evse_relay->evse_relay2_off();
			ptr_cp_out->set_cp_on();

			evse_ChargePointStatus = Faulted;
			EvseChargePointErrorCode_OtherError = EVSE_EmergencyStop;
			EvseChargePoint_ErrorCode = EVSE_OtherError;

			// Serial.println("EMGY Faulted  ");
		}
		else
		{

			if (EvseChargePointErrorCode_OtherError == EVSE_EmergencyStop)
			{
				EvseChargePointErrorCode_OtherError = EVSE_NO_Other_Error;
				EvseChargePoint_ErrorCode = EVSE_NoError;

				evse_ChargePointStatus = NOT_SET;
			}
		}

		uint8_t lu8_gfci = evse_gfci.gfci_read();

		if (lu8_gfci == 1)
		{
			ptr_evse_relay->evse_relay_off();
			ptr_evse_relay->evse_relay2_off();
			ptr_cp_out->set_cp_on();

			EvseChargePoint_ErrorCode = EVSE_GroundFailure;
			evse_ChargePointStatus = Faulted;
			reasonForStop = 4;
			// Serial.println("GFCI Faulted");
		}
		else
		{

			if (EvseChargePoint_ErrorCode == EVSE_GroundFailure)
			{

				EvseChargePointErrorCode_OtherError = EVSE_NO_Other_Error;
				EvseChargePoint_ErrorCode = EVSE_NoError;
				evse_ChargePointStatus = NOT_SET;
			}
		}

		uint32_t lu32_earth_disconnect = evse_earth.earth_read();

		if (lu32_earth_disconnect > 2047)
		{

			vTaskDelay(1000 / portTICK_PERIOD_MS);
			ptr_evse_relay->evse_relay_off();
			ptr_evse_relay->evse_relay2_off();
			ptr_cp_out->set_cp_on();

			EvseChargePointErrorCode_OtherError = EVSE_Earth_Disconnect;
			EvseChargePoint_ErrorCode = EVSE_OtherError;
			evse_ChargePointStatus = Faulted;
			reasonForStop = 4;
			// Serial.println("earth dis Faulted");
		}

		else
		{
			if (EvseChargePointErrorCode_OtherError == EVSE_Earth_Disconnect)
			{
				EvseChargePointErrorCode_OtherError = EVSE_NO_Other_Error;
				EvseChargePoint_ErrorCode = EVSE_NoError;

				evse_ChargePointStatus = NOT_SET;
			}
		}

		loopCount_fult++;

		if (loopCount_fult % 30 == 0)
		{

			if (lu8_gfci == 1)
			{
				Serial.println("GFCI Faulted");
			}
			if (lu32_emergency > 2047)
			{
				Serial.println("EMGY Faulted  ");
			}
			if (lu32_earth_disconnect > 2047)
			{

				Serial.println("earth dis Faulted");
			}
		}

		/* Below switch or state machine for Charge point status wise - LED Indiaction  */
		switch (evse_ChargePointStatus)
		{
		case Available:

			if (gu8_online_flag == 0)
			{
				gu8_offline_blink_count++;
				if ((gu8_offline_blink_count > 15) && (gu8_offline_blink_count < 25))
				{

					// evse_led_status = Blinky_White;
					evse_led_status = Green;
					if (gu8_offline_blink_count == 24)
					{
						gu8_offline_blink_count = 0;
					}
				}
				else
				{
					evse_led_status = Green;
				}
			}
			else if (flag_rfid == 1)
			{
				evse_led_status = Blinky_Blue;
			}
			else
			{
				evse_led_status = Green;
			}

			break;
		case Preparing:

			evse_led_status = Blue;

			switch (EvseDevStatus_connector_1)
			{
			case falg_EVSE_Authentication:
			{
				evse_led_status = Blinky_Blue;
				break;
			}
			}
			break;

		case Charging:

			if (gu8_online_flag == 0)
			{
				gu8_offline_blink_count++;
				if ((gu8_offline_blink_count > 7) && (gu8_offline_blink_count < 15))
				{

					// evse_led_status = Blinky_White;
					evse_led_status = Blinky_Green;
					if (gu8_offline_blink_count == 14)
					{
						gu8_offline_blink_count = 0;
					}
				}
				else
				{
					evse_led_status = Blinky_Green;
				}
			}

			else
			{
				evse_led_status = Blinky_Green;
			}

			break;
		case SuspendedEVSE:

			break;
		case SuspendedEV:
			evse_led_status = Blinky_Blue;

			break;
		case Finishing: // not supported by this client

			evse_led_status = Blue;
			break;
		case Reserved: // Implemented reserve now

			break;
		case Unavailable: // Implemented Change Availability

			evse_led_status = Blinky_White;
			break;
		case Faulted: // Implemented Faulted.

			evse_led_status = Red;
			switch (EvseChargePoint_ErrorCode)
			{
			case EVSE_ConnectorLockFailure:
				break;

			case EVSE_EVCommunicationError:
				break;

			case EVSE_GroundFailure:
				break;

			case EVSE_HighTemperature:
				break;

			case EVSE_InternalError:
				break;

			case EVSE_LocalListConflict:
				break;

			case EVSE_NoError:

				break;

			case EVSE_OtherError:
			{

				switch (EvseChargePointErrorCode_OtherError)
				{
				case EVSE_Earth_Disconnect:

					break;

				case EVSE_EmergencyStop:

					evse_led_status = Blinky_Red;
					break;

				case EVSE_PowerLoss:

					break;
				}
			}

			break;

			case EVSE_OverCurrentFailure:

				break;

			case EVSE_OverVoltage:

				break;

			case EVSE_PowerMeterFailure:

				break;

			case EVSE_ReaderFailure:

				break;

			case EVSE_ResetFailure:

				break;

			case EVSE_UnderVoltage:

				break;

			case EVSE_WeakSignal:

				break;

			default:
				break;
			}

			break;
		default:
			break;
		}
		ptr_leds->ADDR_LEDs_main(evse_led_status);
		/*********************************************WATCH_DOG*****************************************************************/
		gu32reset_watch_dog_timer_count++;
		if (gu32reset_watch_dog_timer_count > WATCH_DOG_RESET_COUNT + WATCH_DOG_RESET_INTERVAL)
		{
			gu32reset_watch_dog_timer_count = 0;
			ptr_watch_dog->evse_watch_dog_off();
		}
		else if (gu32reset_watch_dog_timer_count > WATCH_DOG_RESET_COUNT)
		{
			// gu32reset_watch_dog_timer_count=0;
			ptr_watch_dog->evse_watch_dog_on();
		}

		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}

/**
 * ControlPilot is a function that reads the ADC value of the Control Pilot and
 * updates the EVSE states and the Charge Point status based on the ADC value.
 *
 * @param p a pointer to a void, not used in the function
 *
 * @return void
 *
 * @throws None
 */
void ControlPilot(void* p)
{

	mutexControlPilot = xSemaphoreCreateMutex();

	uint32_t delayTime = 500;
	TickType_t xLastWakeTime;
	CP_OUT* ptr_cp_out;
	ptr_cp_out->set_cp_init();

	ptr_cp_out->set_cp_on();

	CP_IN* ptr_cp_in;
	ptr_cp_in->read_cp_init();
	int32_t ADC_Result = 0;
	const TickType_t xFrequency_Task1 = 200; // delay for mS
	// ptr_cp_out->set_cp_begin();
	// evse_led_status = Green;
	uint32_t loopCount = 0;
	xLastWakeTime = xTaskGetTickCount();
	while (1)
	{
		// ADC_Result = ptr_cp_in->read_cp_in();
		xSemaphoreTake(mutexControlPilot, portMAX_DELAY);
		ADC_Result = ptr_cp_in->read_cp_in();
		/**********************************************************************************************************************************************************/
		gs32EVSE_SMW_Control_pilot_Average = ADC_Result;
#if 0
		if (gu32EVSE_SMW_Control_pilot_SampleCnt >= EVSE_CONTROL_PILOT_SM_SKIP_COUNT)
		{
			if (gu32EVSE_SMW_Control_pilot_SampleCnt < (EVSE_CONTROL_PILOT_SM_SKIP_COUNT + EVSE_CONTROL_PILOT_SM_WINDOW_SIZE))
			{
				gs32EVSE_SMW_Control_pilot_Window[gu8EVSE_SMW_Control_pilot_WindowIndex] = ADC_Result;
				gs32EVSE_SMW_Control_pilot_Total += gs32EVSE_SMW_Control_pilot_Window[gu8EVSE_SMW_Control_pilot_WindowIndex];

				gu8EVSE_SMW_Control_pilot_WindowIndex++;
				if (gu8EVSE_SMW_Control_pilot_WindowIndex >= EVSE_CONTROL_PILOT_SM_WINDOW_SIZE)
				{
					gu8EVSE_SMW_Control_pilot_WindowIndex = 0;
					gs32EVSE_SMW_Control_pilot_Average = gs32EVSE_SMW_Control_pilot_Total / EVSE_CONTROL_PILOT_SM_WINDOW_SIZE;
				}

				gu32EVSE_SMW_Control_pilot_SampleCnt++;
			}
			else
			{
				gs32EVSE_SMW_Control_pilot_Total = gs32EVSE_SMW_Control_pilot_Total - gs32EVSE_SMW_Control_pilot_Window[gu8EVSE_SMW_Control_pilot_WindowIndex];
				gs32EVSE_SMW_Control_pilot_Window[gu8EVSE_SMW_Control_pilot_WindowIndex] = ADC_Result;
				gs32EVSE_SMW_Control_pilot_Total = gs32EVSE_SMW_Control_pilot_Total + gs32EVSE_SMW_Control_pilot_Window[gu8EVSE_SMW_Control_pilot_WindowIndex];
				gs32EVSE_SMW_Control_pilot_Average = gs32EVSE_SMW_Control_pilot_Total / EVSE_CONTROL_PILOT_SM_WINDOW_SIZE;

				gu8EVSE_SMW_Control_pilot_WindowIndex++;
				if (gu8EVSE_SMW_Control_pilot_WindowIndex >= EVSE_CONTROL_PILOT_SM_WINDOW_SIZE)
					gu8EVSE_SMW_Control_pilot_WindowIndex = 0;
			}
		}
		else
		{
			gu32EVSE_SMW_Control_pilot_SampleCnt++;
		}
#endif

		/**********************************************EVSE_CONTROL_POILET_LOGIC***************************************************************************/
		/* ADC result is between 3.2 and 2.8 volts */
		if ((gs32EVSE_SMW_Control_pilot_Average < evse_state_a_upper_threshold) &&
			(gs32EVSE_SMW_Control_pilot_Average > evse_state_a_lower_threshold))
		{
			PILOT_reading = V_12;
			EVSE_states = STATE_A;
			// ptr_evse_relay->evse_relay_off();
			if ((evse_ChargePointStatus != Faulted) &&
				(evse_ChargePointStatus != Unavailable) &&
				//  (evse_ChargePointStatus != NOT_SET)&&
				(EvseDevStatus_connector_1 != flag_EVSE_is_Booted))
			{

				if (ev_disconnect_flag == 1)
				{
					ev_disconnect_flag = 0;
					reasonForStop = 1;
					flag_rfid = 0;
					OcppOperation* stopTransaction = makeOcppOperation(&webSocket, new StopTransaction());
					initiateOcppOperation(stopTransaction);
					evse_resumeTxn.begin("resume_conn_1", false);
					evse_resumeTxn.putString("idTagData", "");
					evse_resumeTxn.putBool("ongoingTxn", false);
					evse_resumeTxn.putInt("TxnIdData", -1);
					evse_resumeTxn.putFloat("meterStart", 0);
					evse_resumeTxn.end();
					EVSE_transaction_status = EVSE_NO_TRANSACTION;
					EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;
				}
				else if (flag_rfid == 0)
				{
					EVSE_transaction_status = EVSE_NO_TRANSACTION;
					evse_ChargePointStatus = Available;
					gu8_evse_change_state = EVSE_READ_RFID;
					EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;
					alternet_10d_flag = 1;
				}
			}
		}

		/* ADC result is between 2.8 and 2.33 volts */
		else if ((gs32EVSE_SMW_Control_pilot_Average < evse_state_b_upper_threshold) &&
			(gs32EVSE_SMW_Control_pilot_Average > evse_state_b_lower_threshold))
		{
			PILOT_reading = V_9;
			EVSE_states = STATE_B;
			ptr_evse_relay->evse_relay_off();
			if ((evse_ChargePointStatus != Faulted) &&
				(evse_ChargePointStatus != Unavailable)) /* &&
				(evse_ChargePointStatus != NOT_SET)  */
			{
				// Serial.print("[ControlPilot] ADC Value : ");
				// 	Serial.println(ADC_Result);
				// if (gu8_evse_change_state == EVSE_STOP_TXN)
				// {
				// 	// Serial.println("ControlPilot Issue...............");
				// 	EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;
				// }

				if (EVSE_transaction_status == EVSE_STOP_TRANSACTION)
				{
					evse_ChargePointStatus = Finishing;
				}
				else if (EVSE_transaction_status == EVSE_NO_TRANSACTION)
				{
					if (EvseDevStatus_connector_1 != flag_EVSE_is_Booted)
					{
						if (Charging_3_3 == 0)
						{
							evse_ChargePointStatus = Preparing;
							alternet_10d_flag = 2;
						}

						if (plugandcharge_1 == 1)
						{
							gu8_evse_change_state = EVSE_START_TXN;
							evse_start_txn_state = EVSE_START_TXN_INITIATED;
							EvseDevStatus_connector_1 = flag_EVSE_Start_Transaction;
						}
					}
				}
			}
		}

		/* ADC result is between 1.49 and 1.32 volts */ // implemented based on observation
		else if ((gs32EVSE_SMW_Control_pilot_Average < evse_state_sus_upper_threshold) &&
			(gs32EVSE_SMW_Control_pilot_Average > evse_state_sus_lower_threshold)) // 1750 working
		{

			EVSE_states = STATE_SUS;
			evse_ChargePointStatus = SuspendedEV;
			PILOT_reading = V_SUS;
			ptr_evse_relay->evse_relay_off();

			// Serial.print("[ControlPilot] ADC Value : ");
			// Serial.println(ADC_Result);
		}
		else if ((gs32EVSE_SMW_Control_pilot_Average < evse_state_e2_upper_threshold) &&
			(gs32EVSE_SMW_Control_pilot_Average > evse_state_e2_lower_threshold))
		{
			PILOT_reading = V_UNKNOWN;
			EVSE_states = STATE_E;
			ptr_evse_relay->evse_relay_off();
			// Serial.print("[ControlPilot] ADC Value : ");
			// Serial.println(ADC_Result);

			if (plugandcharge_1 == 1)
			{
				gu8_evse_change_state = EVSE_START_TXN;
				evse_start_txn_state = EVSE_START_TXN_INITIATED;
				EvseDevStatus_connector_1 = flag_EVSE_Start_Transaction;
			}
		}
		/* ADC result is between 1.73 and 1.49 volts */
		// DIS state changed from 2100 to 2700 by chandu observation
		else if ((gs32EVSE_SMW_Control_pilot_Average < evse_state_dis_upper_threshold) &&
			(gs32EVSE_SMW_Control_pilot_Average > evse_state_dis_lower_threshold)) // 1750 working
		{
			ev_disconnect_flag = 1;
			PILOT_reading = V_DIS;
			EVSE_states = STATE_DIS;
			ptr_evse_relay->evse_relay_off();
			ptr_cp_out->set_cp_on();
			// Serial.print("[ControlPilot] ADC Value : ");
			// Serial.println(ADC_Result);
		}

		/* ADC result is between 1.32 and 1.08 volts */
		else if ((gs32EVSE_SMW_Control_pilot_Average < evse_state_c_upper_threshold) &&
			(gs32EVSE_SMW_Control_pilot_Average > evse_state_c_lower_threshold))
		{
			PILOT_reading = V_6;
			EVSE_states = STATE_C;
			// Serial.print("[ControlPilot] ADC Value : ");
			// Serial.println(ADC_Result);

			if ((evse_ChargePointStatus != Faulted)) //&& (EVSE_transaction_status == EVSE_START_TRANSACTION))
			{
				ptr_evse_relay->evse_relay_on();
			}
			else if (evse_ChargePointStatus == Faulted)
			{
				ptr_evse_relay->evse_relay_off();
			}
			evse_ChargePointStatus = Charging;
		}
		/* ADC result is between 1.08 and 0.60 volts */
		else if ((gs32EVSE_SMW_Control_pilot_Average < evse_state_d_upper_threshold) &&
			(gs32EVSE_SMW_Control_pilot_Average > evse_state_d_lower_threshold))
		{
			PILOT_reading = V_3;
			EVSE_states = STATE_D;
			// Serial.print("[ControlPilot] ADC Value : ");
			// Serial.println(ADC_Result);

			if (evse_ChargePointStatus != Faulted)
			{
				ptr_evse_relay->evse_relay_on();
			}
			else if (evse_ChargePointStatus == Faulted)
			{
				ptr_evse_relay->evse_relay_off();
			}
		}

		else if (gs32EVSE_SMW_Control_pilot_Average < evse_state_e_threshold)
		{
			PILOT_reading = V_UNKNOWN;
			EVSE_states = STATE_E;
			ptr_evse_relay->evse_relay_off();
		}

		else
		{
			PILOT_reading = V_DEFAULT;
			ptr_evse_relay->evse_relay_off();
		}
		xSemaphoreGive(mutexControlPilot);

		loopCount++;
		if (loopCount % 30 == 0)
		{
			Serial.print("[ControlPilot] ADC Value : ");
			Serial.println(ADC_Result);

			if ((gs32EVSE_SMW_Control_pilot_Average < evse_state_a_upper_threshold) &&
				(gs32EVSE_SMW_Control_pilot_Average > evse_state_a_lower_threshold))
			{
				Serial.println("STATE_A");
			}

			/* ADC result is between 2.8 and 2.33 volts */
			else if ((gs32EVSE_SMW_Control_pilot_Average < evse_state_b_upper_threshold) &&
				(gs32EVSE_SMW_Control_pilot_Average > evse_state_b_lower_threshold))
			{

				Serial.println("STATE_B");
			}

			/* ADC result is between 1.49 and 1.32 volts */ // implemented based on observation
			else if ((gs32EVSE_SMW_Control_pilot_Average < evse_state_sus_upper_threshold) &&
				(gs32EVSE_SMW_Control_pilot_Average > evse_state_sus_lower_threshold)) // 1750 working
			{
				Serial.println("STATE_SUS");
			}
			else if ((gs32EVSE_SMW_Control_pilot_Average < evse_state_e2_upper_threshold) &&
				(gs32EVSE_SMW_Control_pilot_Average > evse_state_e2_lower_threshold))
			{
				Serial.println("STATE_E_2");
			}
			/* ADC result is between 1.73 and 1.49 volts */
			// DIS state changed from 2100 to 2700 by chandu observation
			else if ((gs32EVSE_SMW_Control_pilot_Average < evse_state_dis_upper_threshold) &&
				(gs32EVSE_SMW_Control_pilot_Average > evse_state_dis_lower_threshold)) // 1750 working
			{
				Serial.println("STATE_DIS");
			}

			/* ADC result is between 1.32 and 1.08 volts */
			else if ((gs32EVSE_SMW_Control_pilot_Average < evse_state_c_upper_threshold) &&
				(gs32EVSE_SMW_Control_pilot_Average > evse_state_c_lower_threshold))
			{
				Serial.println("STATE_C");
			}
			/* ADC result is between 1.08 and 0.60 volts */
			else if ((gs32EVSE_SMW_Control_pilot_Average < evse_state_d_upper_threshold) &&
				(gs32EVSE_SMW_Control_pilot_Average > evse_state_d_lower_threshold))
			{
				Serial.println("STATE_D");
			}

			else if (gs32EVSE_SMW_Control_pilot_Average < evse_state_e_threshold)
			{
				Serial.println("STATE_E_1");
			}

			else
			{
				Serial.println("ADC values are not in range");
			}
		}

		if (EVSE_states == STATE_SUS)
		{

			delayTime = 40;
		}
		else
		{
			delayTime = 500;
		}
		// vTaskDelayUntil(&xLastWakeTime_Task1, xFrequency_Task1);
		//  vTaskDelay(delayTime / portTICK_PERIOD_MS);
		vTaskDelayUntil(&xLastWakeTime, (delayTime / portTICK_PERIOD_MS));
	}
}

/**
 * Reads RFID energy data and handles BLE preferences and error conditions.
 *
 * @param p pointer to the function
 *
 * @return void
 *
 * @throws None
 */
void Read_rfid_energy(void* p)
{

#if RFID_SPI
	RFID* ptr_evse_rfid;
#if 0
	RFID* ptr_evse_rfid;
	ptr_evse_rfid->rfid_hspi_init();
	ptr_evse_rfid->rfid_init();
#endif
	uint8_t start_rfid;
#endif
	uint32_t lu_energy_VA_OV_count = 0;
	uint32_t lu_energy_VA_UV_count = 0;
	uint32_t lu_energy_VA_PL_count = 0;
	uint32_t lu_energy_VA_previous_value1 = 0;
	uint32_t lu_energy_VA_previous_value2 = 0;
	uint32_t lu_energy_VA_previous_value3 = 0;

	uint32_t lu_energy_IA_OV_count = 0;

	double ld32_previous_IA = 0;
	double ld32_present_IA = 0;

	ENERGY1* ptr_evse_energy;

	/***************************************************BLE*********************************************************************************/
	evse_preferences_ble.begin("FaultThreshold", false);
	uint32_t over_voltage = evse_preferences_ble.getInt("overvoltage", 0);
	uint32_t under_voltage = evse_preferences_ble.getInt("undervoltage", 0);
	uint32_t over_current = evse_preferences_ble.getInt("overcurrent", 0); // overtemparature
	uint32_t over_temperature = evse_preferences_ble.getInt("overtemperature", 0);
	evse_preferences_ble.end();

	/*BLE*/
	Serial.println("over_voltage : " + String(over_voltage));
	Serial.println("under_voltage : " + String(under_voltage));
	Serial.println("over_current : " + String(over_current));
	Serial.println("over_temperature : " + String(over_temperature));

	if (over_voltage != 0)
	{
		evse_over_voltage_threshold = over_voltage;
	}
	else
	{
		evse_over_voltage_threshold = EVSE_OVER_VOLTAGE_THRESHOLD;
	}
	Serial.println("evse_over_voltage_threshold : " + String(evse_over_voltage_threshold));

	if (under_voltage != 0)
	{
		evse_under_voltage_high_threshold = under_voltage;
	}
	else
	{
		evse_under_voltage_high_threshold = EVSE_UNDER_VOLTAGE_HIGH_THRESHOLD_2;
	}
	Serial.println("evse_under_voltage_high_threshold : " + String(evse_under_voltage_high_threshold));
	if (over_current != 0)
	{
		evse_over_current_threshold = over_current;
	}
	else
	{
		if (cpcurrentlimit == 0)
		{
			cpcurrentlimit = EVSE_OVER_CURRENT_THRESHOLD;
		}

		evse_over_current_threshold = (cpcurrentlimit + 2);
	}
	Serial.println("evse_over_current_threshold : " + String(evse_over_current_threshold));
	if (over_temperature != 0)
	{
		evse_high_temperature_threshold = over_temperature;
	}
	else
	{
		evse_high_temperature_threshold = EVSE_HIGH_TEMPERATURE_THRESHOLD;
	}
	Serial.println("evse_high_temperature_threshold : " + String(evse_high_temperature_threshold));
	/**************************************************************************************************************************************/
#if RFID_SPI
	uint32_t lu_rfid_count = 0;

#endif

	TickType_t xLastWakeTime_Task2 = xTaskGetTickCount();
	const TickType_t xFrequency_Task2 = 100; // delay for mS
	// ptr_evse_energy->energy_meter_init();
// ptr_evse_rfid->rfid_hspi_init();
// ptr_evse_rfid->rfid_init(); // ptr_evse_energy-> energy_register_config();
	while (1)
	{
		if (rfid_read_flag == 1)
		{
			static uint8_t count_rfid = 0;
			count_rfid++;
			if (count_rfid >= 20)
			{
				rfid_read_flag = 0;
				count_rfid = 0;
			}
		}

		switch (evse_spi_reading_states)
		{
#if RFID_SPI
		case RFID_SCANNER:
		{

#if EVSE_RFID_ENABLE

			// Serial.printf("[communication task] ESP32 heap size  %d \r\n", ESP.getHeapSize());
			ptr_evse_rfid->rfid_hspi_init();
			ptr_evse_rfid->rfid_init();
			if (((rfid_read_flag == 0) && (plugandcharge_7_4 != 2)))
			{
				ptr_evse_rfid->rfid_scan();
			}
			evse_spi_reading_states = ENERGY_METER;

#endif
		}
		break;

#endif

		case ENERGY_METER:
		{
#if EVSE_ENERGY_METER_ENABLE

			ptr_evse_energy->energy_meter_init();

			switch (evse_spi_energy_reading_states)
			{
			case EVSE_READING_VA:

				ptr_evse_energy->energy_meter_voltageA();
#if THREE_PHASE
				ptr_evse_energy->energy_meter_voltageB();
				ptr_evse_energy->energy_meter_voltageC();
#endif
				evse_spi_energy_reading_states = EVSE_READING_IA;
				evse_spi_reading_states = RFID_SCANNER;
				if (ipc_voltage_A > evse_over_voltage_threshold)
				{

					EvseChargePoint_ErrorCode = EVSE_OverVoltage;
					reasonForStop = 4;
					evse_ChargePointStatus = Faulted;
					Serial.printf("OVER VOLTAGE \r\n");
				}

				else if ((ipc_voltage_A >= EVSE_UNDER_VOLTAGE_HIGH_THRESHOLD_1) && (ipc_voltage_A <= evse_under_voltage_high_threshold))

				{
					EvseChargePoint_ErrorCode = EVSE_UnderVoltage;
					reasonForStop = 4;
					evse_ChargePointStatus = Faulted;
					Serial.printf("UNDER VOLTAGE \r\n");
				}
				else if ((ipc_voltage_A < EVSE_POWER_LOSS_THRESHOLD))
				{

					EvseChargePointErrorCode_OtherError = EVSE_PowerLoss;
					reasonForStop = 5;
					EvseChargePoint_ErrorCode = EVSE_OtherError;
					evse_ChargePointStatus = Faulted;
					Serial.printf("POWER LOSS \r\n");
				}
				else
				{

					if ((EvseChargePointErrorCode_OtherError == EVSE_PowerLoss) ||
						(EvseChargePoint_ErrorCode == EVSE_UnderVoltage) ||
						(EvseChargePoint_ErrorCode == EVSE_OverVoltage))
					{

						EvseChargePoint_ErrorCode = EVSE_NoError;
						evse_ChargePointStatus = NOT_SET;
					}
				}

				break;

			case EVSE_READING_IA:

				ptr_evse_energy->energy_meter_currentA();
#if THREE_PHASE
				ptr_evse_energy->energy_meter_currentB();
				ptr_evse_energy->energy_meter_currentC();
#endif
				evse_spi_energy_reading_states = EVSE_READING_TEMP;
				evse_spi_reading_states = RFID_SCANNER;

				if (ipc_current_A > evse_over_current_threshold)
				{
					EvseChargePoint_ErrorCode = EVSE_OverCurrentFailure;
					evse_ChargePointStatus = Faulted;
					reasonForStop = 4;
					Serial.printf("OVER CURRENT \r\n");
				}

				else
				{
					if (EvseChargePoint_ErrorCode == EVSE_OverCurrentFailure)
					{
						EvseChargePoint_ErrorCode = EVSE_NoError;
						evse_ChargePointStatus = NOT_SET;
					}
				}

				break;

			case EVSE_READING_TEMP:

				ptr_evse_energy->energy_meter_temp();

				evse_spi_energy_reading_states = EVSE_READING_VA;
				evse_spi_reading_states = RFID_SCANNER;
#if 1

				if (ipc_temp > evse_high_temperature_threshold)
				{
					EvseChargePoint_ErrorCode = EVSE_HighTemperature;
					evse_ChargePointStatus = Faulted;
				}
				else
				{
					if (EvseChargePoint_ErrorCode == EVSE_HighTemperature)
					{
						EvseChargePoint_ErrorCode = EVSE_NoError;
						evse_ChargePointStatus = NOT_SET;
					}
				}
#endif
				break;

			default:
				break;
			}

#endif
		}
		break;
		}

		// evse_spi_reading_states = RFID_SCANNER;

		vTaskDelay(500 / portTICK_PERIOD_MS);
		// vTaskDelayUntil(&xLastWakeTime_Task2, xFrequency_Task2);
	}
}

/**
 * Reads the real-time clock (RTC) and LCD display states and updates the corresponding
 * states based on the reading states. This function is executed in an infinite loop.
 *
 * @param p pointer to the task handle
 *
 * @return void
 *
 * @throws None
 */
void Read_rtc_lcd(void* p)
{

	volatile uint32_t gu32read_rtc_timer_count = 0;
	volatile uint32_t gu32read_lcd_timer_count = 0;
	volatile uint8_t evse_i2c_reading_states = 0;

#if EVSE_LCD_ENABLE
	evse_lcd_init();
#endif

#if EVSE_DWIN_ENABLE
	evse_dwin_setup();
#endif
	Rtc* ptr_evse_rtc_read;

	ptr_evse_rtc_read->set_ext_rtc_time();

#if RFID_I2C
	setup_pn532();
#endif

	uint32_t lu_12c_count = 0;
	Serial.print(" communication1 running on core ");
	Serial.println(xPortGetCoreID());
	TickType_t xLastWakeTime_Task3 = xTaskGetTickCount();
	const TickType_t xFrequency_Task3 = 250; // delay for mS

	while (1)
	{

		gu32read_rtc_timer_count++;
		gu32read_lcd_timer_count++;

#if RFID_I2C
		gu32read_i2c_rfid_timer_count++;
#endif

		if (gu32read_rtc_timer_count > RTC_READING_COUNT)
		{
			gu32read_rtc_timer_count = 0;
			evse_i2c_reading_states = RTC_READING;
		}
#if RFID_I2C
		else if (gu32read_i2c_rfid_timer_count > I2C_RFID_COUNT)
		{
			gu32read_i2c_rfid_timer_count = 0;
			evse_i2c_reading_states = I2C_RFID_READ;
		}
#endif
		else if (gu32read_lcd_timer_count > LCD_DISPLAY_COUNT)
		{
			gu32read_lcd_timer_count = 0;
			evse_i2c_reading_states = LCD_DISPLAY;
		}

		switch (evse_i2c_reading_states)
		{

		case LCD_DISPLAY:
		{
#if EVSE_LCD_ENABLE
			byte address = 39;

			Wire1.beginTransmission(address);
			byte error = Wire1.endTransmission();

			if (error == 0)
			{
				if (gu8_i2cnotfound_flag == 1)
				{
					lenum_evse_ChargePointStatus = NOT_SET;
					dip_EvseChargePointErrorCode_OtherError = EVSE_NO_Other_Error;
					evse_lcd_init();

					gu8_i2cnotfound_flag = 0;
				}
			}
			else
			{

				gu8_i2cnotfound_flag = 1;
			}

			evse_lcd_state_update();

#endif

#if EVSE_DWIN_ENABLE
			evse_dwin_state_update();

#endif
		}
		break;

		case RTC_READING:
		{
#if EVSE_RTC_ENABLE
			ptr_evse_rtc_read->get_ext_rtc_time();

#endif
		}
		break;
#if RFID_I2C
		case I2C_RFID_READ:
		{

			pn532_loop();
		}
		break;
#endif
		default:

			break;
		}

		vTaskDelay(1000 / portTICK_PERIOD_MS);
		// vTaskDelayUntil(&xLastWakeTime_Task3, xFrequency_Task3);
	}
}

/**
 * The OCPP_task function is the main task of the OCPP protocol. It runs on the ESP32 processor and handles the OCPP protocol operations.
 * The function is executed on the specified core and runs continuously until it is stopped.
 *
 * @param p void pointer to the parameter.
 * @return void.
 */
void ocpp_task(void* p)
{
	Serial.print(" OCPP_task running on core ");
	Serial.println(xPortGetCoreID());
	TickType_t xLastWakeTime_Task1 = xTaskGetTickCount();

	String evse_ble_url = "";

	Serial.print("WebSockets Client started @ IP address: ");
	Serial.println(WiFi.localIP());
	// server address, port and URL
	Serial.print("Connecting to WebSockets Server @ ");
	// Serial.println(WS_SERVER);
	Serial.println(host_m);

	String host_m = "";
	int port_m = 0;
	String protocol_m = "";
	String path_m = "";

	char host_evse[32] = { 0 };
	int port_evse = 0;
	char protocol_evse[8] = "ocpp1.6";
	char path_evse[128] = { 0 }; // changed from 64byts to 128

	evse_preferences_ws.begin("ws_cred", false);
	host_m = evse_preferences_ws.getString("host_m", "");
	path_m = evse_preferences_ws.getString("path_m", "");
	port_m = evse_preferences_ws.getUInt("port_m", 0);
	protocol_m = evse_preferences_ws.getString("protocol_m", "");
	evse_preferences_ws.end();

	strcpy(&host_evse[0], host_m.c_str());
	// strcpy(&protocol_evse[0], protocol_m.c_str());        // Disabled the Dynamic update from End-user
	strcpy(&path_evse[0], path_m.c_str());
	port_evse = port_m;

	Serial.println("host_evse:" + String(host_evse));
	Serial.println("path_evse:" + String(path_evse));
	Serial.println("port_evse:" + String(port_evse));
	Serial.println("protocol_evse:" + String(protocol_evse));

	evse_preferences_ble.begin("connectivity", false);
	// bool evse_ble_wifi = evse_preferences_ble.getBool("wifi", "");
	// bool evse_ble_gsm = evse_preferences_ble.getBool("gsm", "");
	uint32_t connnection_type = evse_preferences_ble.getUInt("conn_type", 0);
	evse_preferences_ble.end();

	// Serial.println("evse_ble_wifi :  " + String(evse_ble_wifi));
	// Serial.println("evse_ble_gsm :  " + String(evse_ble_gsm));
	Serial.println("connnection_type :  " + String(connnection_type));

	evse_led_status = Blinky_White;

	dis_connection_flag = connnection_type;

	Serial.println("connnection_type :  " + String(connnection_type));

	String evse_ble_ssid;
	String evse_ble_ssid_key;

#if EVSE_LCD_ENABLE
	evse_lcd_init();
#endif

	switch (connnection_type)
	{

	case CONNECT_WIFI_ENABLED:
	{
		gu8_wifi_4g_flag = 1;
		evse_preferences_ble.begin("wifi_cred", false);
		evse_ble_ssid = evse_preferences_ble.getString("ssid", "");
		evse_ble_ssid_key = evse_preferences_ble.getString("KEY", "");
		evse_preferences_ble.end();

		Serial.println("wifi_ssid " + String(evse_ble_ssid));
		Serial.println("wifi_key " + String(evse_ble_ssid_key));

		gsm_connect = false;
		wifi_connect = true;
		Serial.println(" CONNECT_WIFI_ENABLED");

		// server address, port and URL
#if USE_SSL
		webSocket.beginSSL(WS_SERVER, WS_PORT);
#else

		// webSocket.begin(WS_SERVER, WS_PORT, "/websocket/amplify/jublihills1phase", "ocpp1.6");

		webSocket.begin(host_evse, port_evse, path_evse, protocol_evse);

		// webSocket.begin("192.168.1.8", 8080, "/steve/websocket/CentralSystemService/singlephase_evse", "ocpp1.6");
		// webSocket.begin(host_m.c_str(), port_m, path_m.c_str(), protocol_m.c_str());
#endif

		// event handler
		webSocket.onEvent(webSocketEvent);

		// use HTTP Basic Authorization this is optional remove if not needed
		// webSocket.setAuthorization("user", "Password");

		// try ever 5000 again if connection has failed
		webSocket.setReconnectInterval(45000);

		// start heartbeat (optional)
		// ping server every 15000 ms
		// expect pong from server within 3000 ms
		// consider connection disconnected if pong is not received 2 times
		// webSocket.enableHeartbeat(15000, 3000, 2);
		// webSocket.enableHeartbeat(60000, 9000, 3);
		webSocket.enableHeartbeat(25000, 9000, 3);

		// server address, port and URL
		Serial.print("Connected to WebSockets Server @ IP address: ");
		// Serial.println(WS_SERVER);
		Serial.println(host_evse);
	}
	break;

	case CONNECT_GSM_ENABLED:
		gu8_wifi_4g_flag = 1;
		// Serial.println("gsm loop>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>.");
		// OTA_4G_setup4G();
		// SetupGsm();
		gsm_connect = true;
		wifi_connect = false;
		Serial.println(" CONNECT_GSM_ENABLED");
		SetupGsm_4g();
		//  ConnectToServer();
		ConnectToServer_4g();
		// #if ALPR_ENABLED
		// 		WiFi.mode(WIFI_AP_STA);
		// 		WiFi.begin("EVRE", "PASSWORD");
		// #endif
#if EVSE_LCD_ENABLE
		evse_lcd_connecting_gsm();
#endif
		// gsm_Loop_4g_ble();
		break;

	case CONNECT_WIFI_GSM_ENABLED:
	{
		// Serial.println("wifi_gsm loop>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>.");
		evse_preferences_ble.begin("wifi_cred", false);
		evse_ble_ssid = evse_preferences_ble.getString("ssid", "");
		evse_ble_ssid_key = evse_preferences_ble.getString("KEY", "");
		evse_preferences_ble.end();

		Serial.println("wifi_ssid " + String(evse_ble_ssid));
		Serial.println("wifi_key " + String(evse_ble_ssid_key));

		wifi_connect = true;
		gsm_connect = false;
		Serial.println("CONNECT_WIFI_GSM_ENABLED ");

#if USE_SSL
		webSocket.beginSSL(WS_SERVER, WS_PORT);
#else

		// webSocket.begin(WS_SERVER, WS_PORT, "/websocket/amplify/jublihills1phase", "ocpp1.6");

		webSocket.begin(host_evse, port_evse, path_evse, protocol_evse);

		// webSocket.begin("192.168.1.8", 8080, "/steve/websocket/CentralSystemService/singlephase_evse", "ocpp1.6");
		// webSocket.begin(host_m.c_str(), port_m, path_m.c_str(), protocol_m.c_str());
#endif

		// event handler
		webSocket.onEvent(webSocketEvent);

		// use HTTP Basic Authorization this is optional remove if not needed
		// webSocket.setAuthorization("user", "Password");

		// try ever 45000 again if connection has failed
		webSocket.setReconnectInterval(45000);

		// start heartbeat (optional)
		// ping server every 15000 ms
		// expect pong from server within 3000 ms
		// consider connection disconnected if pong is not received 2 times
		webSocket.enableHeartbeat(15000, 3000, 2); //
		// webSocket.enableHeartbeat(60000, 9000, 3);

		// server address, port and URL
		Serial.print("Connected to WebSockets Server @ IP address: ");
		// Serial.println(WS_SERVER);
		Serial.println(host_evse);
	}
	break;

	case CONNECT_ETHERNET_ENABLED:
		break;

	case CONNECT_WIFI_ETHERNET_ENABLED:
	{
		evse_preferences_ble.begin("wifi_cred", false);
		evse_ble_ssid = evse_preferences_ble.getString("ssid", "");
		evse_ble_ssid_key = evse_preferences_ble.getString("KEY", "");
		evse_preferences_ble.end();

		Serial.println("wifi_ssid " + String(evse_ble_ssid));
		Serial.println("wifi_key " + String(evse_ble_ssid_key));

		wifi_connect = true;
		gsm_connect = false;
		Serial.println("CONNECT_WIFI_GSM_ENABLED ");

#if USE_SSL
		webSocket.beginSSL(WS_SERVER, WS_PORT);
#else

		// webSocket.begin(WS_SERVER, WS_PORT, "/websocket/amplify/jublihills1phase", "ocpp1.6");

		webSocket.begin(host_evse, port_evse, path_evse, protocol_evse);

		// webSocket.begin("192.168.1.8", 8080, "/steve/websocket/CentralSystemService/singlephase_evse", "ocpp1.6");
		// webSocket.begin(host_m.c_str(), port_m, path_m.c_str(), protocol_m.c_str());
#endif

		// event handler
		webSocket.onEvent(webSocketEvent);

		// use HTTP Basic Authorization this is optional remove if not needed
		// webSocket.setAuthorization("user", "Password");

		// try ever 45000 again if connection has failed
		webSocket.setReconnectInterval(45000);

		// start heartbeat (optional)
		// ping server every 15000 ms
		// expect pong from server within 3000 ms
		// consider connection disconnected if pong is not received 2 times
		webSocket.enableHeartbeat(15000, 3000, 2); //
		// webSocket.enableHeartbeat(60000, 9000, 3);

		// server address, port and URL
		Serial.print("Connected to WebSockets Server @ IP address: ");
		// Serial.println(WS_SERVER);
		Serial.println(host_evse);
	}
	break;

	case CONNECT_GSM_ETHERNET_ENABLED:
	{
		// Serial.println("gsm loop>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>.");
		// OTA_4G_setup4G();
		// SetupGsm();
		gsm_connect = true;
		wifi_connect = false;
		Serial.println(" CONNECT_GSM_ENABLED");
		SetupGsm_4g();
		//  ConnectToServer();
		ConnectToServer_4g();
	}
	break;

	case CONNECT_WIFI_GSM_ETHERNET_ENABLED:
		break;

	case CONNECT_WIFI_GSM_ETHERNET_DISABLED:
	{
		gu8_offline_flag = 1;
	}

	break;

	default:

		break;
	}
#if ALPR_ENABLED
	WiFi.mode(WIFI_AP_STA);
	WiFi.begin("EVRE", "PASSWORD");
#endif
#if ALPR_ENABLED
	init_esp_now();
#endif
	// EvseDevStatus_connector_1 = flag_EVSE_is_Booted;
	// evse_boot_state = EVSE_BOOT_INITIATED;
	// EVSE_transaction_status = EVSE_NO_TRANSACTION;

	// ocppEngine_initialize(&webSocket, 4096); // default JSON document size = 2048

	ocppEngine_initialize(&webSocket, 4096);

	chargePointStatusService = new ChargePointStatusService(&webSocket); // adds itself to ocppEngine in constructor
	meteringService = new MeteringService(&webSocket);

	// set system time to default value; will be without effect as soon as the BootNotification conf arrives
	// setTimeFromJsonDateString("2021-22-12T11:59:55.123Z"); // use if needed for debugging

	const TickType_t xFrequency_Task1 = 500; // delay for mS

	// EvseDevStatus_connector_1 = flag_EVSE_is_Booted;
	// evse_boot_state = EVSE_BOOT_INITIATED;
	// EVSE_transaction_status = EVSE_NO_TRANSACTION;

#if 1
	OcppOperation* startTransaction = makeOcppOperation(&webSocket, new StartTransaction());
#endif

	// startBLETime = millis();
	while (1)
	{

		switch (connnection_type)
		{

		case CONNECT_WIFI_ENABLED:
		{

#if WIFI_ENABLED
			if (gu8_offline_start_flag == 0)
			{
				webSocket.loop();
			}
#endif
		}
		break;

		case CONNECT_GSM_ENABLED:
#if GSM_ENABLED
			if (gu8_offline_start_flag == 0)
			{
				gsm_Loop_4g();
			}

#endif
			break;

		case CONNECT_WIFI_GSM_ENABLED:
		{

#if WIFI_ENABLED
			if (gu8_offline_start_flag == 0)
			{
				webSocket.loop();
			}
#endif
		}
		break;

		case CONNECT_ETHERNET_ENABLED:

			break;

		case CONNECT_WIFI_ETHERNET_ENABLED:
		{

#if WIFI_ENABLED
			if (gu8_offline_start_flag == 0)
			{
				webSocket.loop();
			}
#endif
		}
		break;

		case CONNECT_GSM_ETHERNET_ENABLED:
		{
#if GSM_ENABLED
			if (gu8_offline_start_flag == 0)
			{
				gsm_Loop_4g();
			}

#endif
		}
		break;

		case CONNECT_WIFI_GSM_ETHERNET_ENABLED:

			break;

		case CONNECT_WIFI_GSM_ETHERNET_DISABLED:
		{
		}

		break;

		default:

			break;
		}

		switch (EvseDevStatus_connector_1)
		{

		case flag_EVSE_is_Booted:
		{
			// Serial.println("boot 1");
			switch (evse_boot_state)
			{
			case EVSE_BOOT_INITIATED:
			{

				Serial.println("CP LOOP: boot notification ");
				OcppOperation* bootNotification = makeOcppOperation(&webSocket, new BootNotification());
				initiateOcppOperation(bootNotification);
				meteringService->init(meteringService);
				// evse_boot_state = EVSE_BOOT_SENT;
#if EVSE_LCD_ENABLE
				evse_lcd_state_Unavailable();
#endif

#if 1
				switch (connnection_type)
				{

				case CONNECT_WIFI_ENABLED:
				{
					evse_boot_state = EVSE_BOOT_SENT;
				}
				break;
				case CONNECT_GSM_ENABLED:
				{
					evse_boot_state = EVSE_BOOT_SENT;
				}
				break;
				case CONNECT_WIFI_GSM_ENABLED:
				{
					evse_boot_state = EVSE_BOOT_SENT;
				}
				break;
				case CONNECT_ETHERNET_ENABLED:
				{
					evse_boot_state = EVSE_BOOT_SENT;
				}
				break;
				case CONNECT_WIFI_ETHERNET_ENABLED:
				{
					if (gu8_online_flag == 1)
					{
						evse_boot_state = EVSE_BOOT_SENT;
					}
					else
					{

						evse_boot_state = EVSE_BOOT_ACCEPTED;
					}
				}
				break;
				case CONNECT_GSM_ETHERNET_ENABLED:
				{
					if (gu8_online_flag == 1)
					{
						evse_boot_state = EVSE_BOOT_SENT;
					}
					else
					{

						evse_boot_state = EVSE_BOOT_ACCEPTED;
					}
				}
				break;
				case CONNECT_WIFI_GSM_ETHERNET_ENABLED:
				{
					evse_boot_state = EVSE_BOOT_SENT;
				}
				break;
				case CONNECT_WIFI_GSM_ETHERNET_DISABLED:
				{
					evse_boot_state = EVSE_BOOT_ACCEPTED;
				}
				break;
				default:
				{
				}
				break;
				}
#endif
				Serial.print("\r\nevse_boot_state  :EVSE_BOOT_INITIATED\r\n");
				break;
			}

			case EVSE_BOOT_SENT:
			{
				if (gu8_state_change == 0)
				{
					gu8_state_change = 1;
					Serial.print("\r\nevse_boot_state  :EVSE_BOOT_SENT\r\n");
				}

				break;
			}

			case EVSE_BOOT_ACCEPTED:
			{

				gu8_evse_change_state = EVSE_READ_RFID;
				EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;
				// meteringService->init(meteringService);
				gu8_state_change = 0;
				Serial.print("\r\nevse_boot_state  :EVSE_BOOT_ACCEPTED\r\n");

				vTaskDelay(1000 / portTICK_PERIOD_MS);

#if 1

				if (DEBUG_OUT)
					Serial.print("EVSE_Sending heartbeat signal...\n");
				OcppOperation* heartbeat = makeOcppOperation(&webSocket, new Heartbeat());
				initiateOcppOperation(heartbeat);
				heartbeat->setOnReceiveConfListener([](JsonObject payload)
					{
						const char* currentTime = payload["currentTime"] | "Invalid";
						if (strcmp(currentTime, "Invalid")) {
							if (setTimeFromJsonDateString(currentTime)) {
								if (DEBUG_OUT) Serial.print("EVSE_setOnsendHeartbeat Callback: Request has been accepted!\n");
							}
							else {
								Serial.print("EVSE Expect format like 2020-02-01T20:53:32.486Z\n");
							}
						}
						else {
							Serial.print("EVSE Missing field currentTime. Expect format like 2020-02-01T20:53:32.486Z\n");
						} });

#endif

						vTaskDelay(1000 / portTICK_PERIOD_MS);

#if PREFERENCE_USED

						/**********Preferences Block For Restarting previously running Txn [Power Cycle]***********/
						evse_resumeTxn.begin("resume_conn_1", false); // opening preferences in R/W mode
						evse_idTagData = evse_resumeTxn.getString("idTagData", "");
						evse_ongoingTxn = evse_resumeTxn.getBool("ongoingTxn", false);
						evse_transactionId = evse_resumeTxn.getInt("TxnIdData", -1);
						evse_globalmeterstart = evse_resumeTxn.getFloat("meterStart", false);
						evse_resumeTxn.end();

#endif

						Serial.println("evse_idTagData : " + String(evse_idTagData));
						Serial.println("evse_ongoingTxn : " + String(evse_ongoingTxn));
						Serial.println("evse_transactionId : " + String(evse_transactionId));
						Serial.println("evse_globalmeterstart : " + String(evse_globalmeterstart));

						if (evse_ongoingTxn)
						{
							if (DEBUG_OUT)
								Serial.print("[StartTransaction] Request has been accepted!\n");

							ptr_cp_out->set_cp_on();
							chargePointStatusService->authorize(evse_idTagData);
							chargePointStatusService->startTransaction(evse_transactionId);
							chargePointStatusService->startEnergyOffer();
							// EVSE_transaction_status = EVSE_STOP_TRANSACTION;
#if EVSE_SESSION_RESUME_ENEABLED
							if (sessionresume == 1)
							{
								if ((EVSE_states == STATE_B) || ((EVSE_states == STATE_E) && (plugandcharge_1 == 1)))
								{
									EvseDevStatus_connector_1 = flag_EVSE_Request_Charge;
									EVSE_transaction_status = EVSE_START_TRANSACTION;
									gu8_evse_change_state = EVSE_CHARGING;
									evse_ChargePointStatus = Charging;
									gu32_evse_suspended_count = 0;

									// chargePointStatusService->authorize(evse_idTagData);
									// chargePointStatusService->startTransaction(evse_transactionId);
									// chargePointStatusService->startEnergyOffer();
									if (cpcurrentlimit == 0)
									{
										cpcurrentlimit = CHARGE_CURRENT_IN_AMPS;
									}

									ptr_cp_out->set_cp_begin(cpcurrentlimit);

									evse_led_status = Blinky_Green;
								}

								else if (EVSE_states == STATE_A)
								{

									ptr_cp_out->set_cp_on();

									reasonForStop = 5;
									OcppOperation* stopTransaction = makeOcppOperation(&webSocket, new StopTransaction());
									initiateOcppOperation(stopTransaction);
									if (DEBUG_OUT)
										Serial.print("EVSE_setOnStopTransaction  before Callback: Closing Relays.\n");

									stopTransaction->setOnReceiveConfListener([](JsonObject payload)
										{
											evse_ChargePointStatus = Finishing;
											EVSE_transaction_status = EVSE_STOP_TRANSACTION;

#if PREFERENCE_USED
											/**********Preferences Block For Restarting previously running Txn [Power Cycle]***********/
											evse_resumeTxn.begin("resume_conn_1", false); // opening preferences in R/W mode
											evse_resumeTxn.putString("idTagData", "");
											evse_resumeTxn.putBool("ongoingTxn", false);
											evse_resumeTxn.putInt("TxnIdData", -1);
											evse_resumeTxn.putFloat("meterStart", 0);
											evse_resumeTxn.end();

#endif
											evse_idTagData = ""; // Clear the RFID for next Start Transaction
										});

									gu8_evse_change_state = EVSE_UNAUTHENTICATION;
									ocpp_rfid = "";
									evse_led_status = Green;
								}
							}
#endif
#if EVSE_SESSION_RESUME_DISABLED
							if (sessionresume == 0)
							{
								reasonForStop = 5;
								OcppOperation* stopTransaction = makeOcppOperation(&webSocket, new StopTransaction());
								initiateOcppOperation(stopTransaction);
								if (DEBUG_OUT)
									Serial.print("EVSE_setOnStopTransaction  before Callback: Closing Relays.\n");

								stopTransaction->setOnReceiveConfListener([](JsonObject payload)
									{
										evse_ChargePointStatus = Finishing;
										EVSE_transaction_status = EVSE_STOP_TRANSACTION;

#if PREFERENCE_USED
										/**********Preferences Block For Restarting previously running Txn [Power Cycle]***********/
										evse_resumeTxn.begin("resume_conn_1", false); // opening preferences in R/W mode
										evse_resumeTxn.putString("idTagData", "");
										evse_resumeTxn.putBool("ongoingTxn", false);
										evse_resumeTxn.putInt("TxnIdData", -1);
										evse_resumeTxn.putFloat("meterStart", 0);
										evse_resumeTxn.end();

#endif
										evse_idTagData = ""; // Clear the RFID for next Start Transaction
									});

								gu8_evse_change_state = EVSE_UNAUTHENTICATION;
								ocpp_rfid = "";
								evse_led_status = Green;
							}
#endif

							evse_ChargePointStatus = NOT_SET;
							chargePointStatusService->loop();
							break;
						}
			}
			case EVSE_BOOT_REJECTED:
			{

				evse_boot_state = EVSE_BOOT_INITIATED;

				break;
			}

			default:
				break;
			}

			break;
		}
		case flag_EVSE_Read_Id_Tag:
		{
			// Serial.println("read rfid 1");
			gu8_evse_cms_reset_flag = 1;
			if (gu8_evse_change_state == EVSE_READ_RFID)
			{
				if (evse_ChargePointStatus != Faulted)
				{

					if (gu8_state_change == 0)
					{
						gu8_state_change = 1;

						chargePointStatusService->setUnavailable(false);
					}

					if (fota_available == true)
					{
						fota_available = false;
						ESP.restart();
					}
				}
			}

			break;
		}

		case falg_EVSE_Authentication:
		{
			// Serial.println("auth 1");
			if (gu8_evse_change_state == EVSE_AUTHENTICATION)
			{

				switch (evse_authorize_state)
				{
				case EVSE_AUTHORIZE_INITIATED:
				{
					Serial.println("auth 2");
					/*brief using mutex to update values*/
					portENTER_CRITICAL(&rfid_tag_mux);
					ocpp_rfid = rfid_buf;
					rfid_buf = " ";
					portEXIT_CRITICAL(&rfid_tag_mux);

					if ((flag_rfid == 2) && (evse_ChargePointStatus != Charging))
					{
						ocpp_rfid = alpr_data;
						alpr_data = "";
					}

					if (ocpp_rfid.equals("") == true)
					{
						gu8_evse_change_state = EVSE_READ_RFID;
					}
					else
					{
						Serial.println("auth 3");
						if (EVSE_transaction_status != EVSE_STOP_TRANSACTION)
						{

							OcppOperation* authorize = makeOcppOperation(&webSocket, new Authorize(ocpp_rfid));
							initiateOcppOperation(authorize);

							chargePointStatusService->authorize(ocpp_rfid);
							evse_idTagData = ocpp_rfid;
							evse_lcd_state_authentication();
							// evse_authorize_state = EVSE_AUTHORIZE_SENT;
#if 0
							if (gu8_online_flag == 1)
							{
								evse_authorize_state = EVSE_AUTHORIZE_SENT;
							}
							else
							{
#if EVSE_OFFLINE_FUNCTIONALITY_ENABLE
								evse_authorize_state = EVSE_AUTHORIZE_ACCEPTED;
								gu8_offline_start_flag = 1;
#endif
							}
#endif
#if 1
							switch (connnection_type)
							{

							case CONNECT_WIFI_ENABLED:
							{
								evse_authorize_state = EVSE_AUTHORIZE_SENT;
							}
							break;
							case CONNECT_GSM_ENABLED:
							{
								evse_authorize_state = EVSE_AUTHORIZE_SENT;
							}
							break;
							case CONNECT_WIFI_GSM_ENABLED:
							{
								evse_authorize_state = EVSE_AUTHORIZE_SENT;
							}
							break;
							case CONNECT_ETHERNET_ENABLED:
							{
								evse_authorize_state = EVSE_AUTHORIZE_SENT;
							}
							break;
							case CONNECT_WIFI_ETHERNET_ENABLED:
							{
								if (gu8_online_flag == 1)
								{
									evse_authorize_state = EVSE_AUTHORIZE_SENT;
								}
								else
								{

									evse_authorize_state = EVSE_AUTHORIZE_ACCEPTED;
									gu8_offline_start_flag = 1;
									// if (Authorization_done(evse_idTagData) == true)
									// {
									// 	evse_authorize_state = EVSE_AUTHORIZE_ACCEPTED;
									// 	gu8_offline_start_flag = 1;
									// }
									// else
									// {
									// 	evse_authorize_state = EVSE_AUTHORIZE_REJECTED;
									// }
								}
							}
							break;
							case CONNECT_GSM_ETHERNET_ENABLED:
							{
								if (gu8_online_flag == 1)
								{
									evse_authorize_state = EVSE_AUTHORIZE_SENT;
								}
								else
								{

									evse_authorize_state = EVSE_AUTHORIZE_ACCEPTED;
									gu8_offline_start_flag = 1;
									// if (Authorization_done(evse_idTagData) == true)
									// {
									// 	evse_authorize_state = EVSE_AUTHORIZE_ACCEPTED;
									// 	gu8_offline_start_flag = 1;
									// }
									// else
									// {
									// 	evse_authorize_state = EVSE_AUTHORIZE_REJECTED;
									// }
								}
							}
							break;
							case CONNECT_WIFI_GSM_ETHERNET_ENABLED:
							{
								evse_authorize_state = EVSE_AUTHORIZE_SENT;
							}
							break;
							case CONNECT_WIFI_GSM_ETHERNET_DISABLED:
							{
								evse_authorize_state = EVSE_AUTHORIZE_ACCEPTED;
								gu8_offline_start_flag = 1;
								// if (Authorization_done(evse_idTagData) == true)
								// {
								// 	evse_authorize_state = EVSE_AUTHORIZE_ACCEPTED;
								// 	gu8_offline_start_flag = 1;
								// }
								// else
								// {
								// 	evse_authorize_state = EVSE_AUTHORIZE_REJECTED;
								// }
							}
							break;
							default:
							{
								// if (Authorization_done(evse_idTagData) == true)
								// {
								// 	evse_authorize_state = EVSE_AUTHORIZE_ACCEPTED;
								// 	gu8_offline_start_flag = 1;
								// }
								// else
								// {
								// 	evse_authorize_state = EVSE_AUTHORIZE_REJECTED;
								// }
							}
							break;
							}
#endif
							gu8_state_change = 0;
						}
					}
					break;
				}

				case EVSE_AUTHORIZE_SENT:
				{
					if (gu8_state_change == 0)
					{
						gu8_state_change = 1;
					}

					break;
				}
				case EVSE_AUTHORIZE_ACCEPTED:
				{
					EvseDevStatus_connector_1 = flag_EVSE_Start_Transaction;
					evse_start_txn_state = EVSE_START_TXN_INITIATED;
					gu8_evse_change_state = EVSE_START_TXN;

					gu8_state_change = 1;
					break;
				}
				case EVSE_AUTHORIZE_REJECTED:
				{
					EvseDevStatus_connector_1 = flag_EVSE_UnAutharized;
					gu8_evse_change_state = EVSE_READ_RFID;
					evse_authorize_state = EVSE_AUTHORIZE_INITIATED;
					// EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;

					break;
				}
				case EVSE_AUTHORIZE_DEFAULT:
				{
					break;
				}
				default:
				{
					break;
				}
				}
			}

			break;
		}
		case flag_EVSE_Start_Transaction:
		{
			// Serial.println("start 1");
			switch (evse_start_txn_state)
			{

			case EVSE_START_TXN_INITIATED:
			{

				gu8_evse_change_state = EVSE_START_TXN;
				if (gu8_evse_change_state == EVSE_START_TXN)
				{

					vTaskDelay(1500 / portTICK_PERIOD_MS);

					OcppOperation* startTransaction = makeOcppOperation(&webSocket, new StartTransaction());
					initiateOcppOperation(startTransaction);

					// evse_start_txn_state = EVSE_START_TXN_SENT;
#if 0
					if (gu8_online_flag == 1)
					{
						evse_start_txn_state = EVSE_START_TXN_SENT;
					}
					else
					{
#if EVSE_OFFLINE_FUNCTIONALITY_ENABLE
						evse_start_txn_state = EVSE_START_TXN_ACCEPTED;
#endif
					}
#endif
#if 1
					switch (connnection_type)
					{

					case CONNECT_WIFI_ENABLED:
					{
						evse_start_txn_state = EVSE_START_TXN_SENT;
					}
					break;
					case CONNECT_GSM_ENABLED:
					{
						evse_start_txn_state = EVSE_START_TXN_SENT;
					}
					break;
					case CONNECT_WIFI_GSM_ENABLED:
					{
						evse_start_txn_state = EVSE_START_TXN_SENT;
					}
					break;
					case CONNECT_ETHERNET_ENABLED:
					{
						evse_start_txn_state = EVSE_START_TXN_SENT;
					}
					break;
					case CONNECT_WIFI_ETHERNET_ENABLED:
					{
						if (gu8_online_flag == 1)
						{
							evse_start_txn_state = EVSE_START_TXN_SENT;
						}
						else
						{

							evse_start_txn_state = EVSE_START_TXN_ACCEPTED;
						}
					}
					break;
					case CONNECT_GSM_ETHERNET_ENABLED:
					{
						if (gu8_online_flag == 1)
						{
							evse_start_txn_state = EVSE_START_TXN_SENT;
						}
						else
						{

							evse_start_txn_state = EVSE_START_TXN_ACCEPTED;
						}
					}
					break;
					case CONNECT_WIFI_GSM_ETHERNET_ENABLED:
					{
					}
					break;
					case CONNECT_WIFI_GSM_ETHERNET_DISABLED:
					{
						evse_start_txn_state = EVSE_START_TXN_ACCEPTED;
					}
					break;
					default:
					{
					}
					break;
					}
#endif

					// vTaskDelay(500 / portTICK_PERIOD_MS);
				}

				break;
			}
			case EVSE_START_TXN_SENT:
			{
				//  Serial.println("CP LOOP: start transaction ");
				break;
			}
			case EVSE_START_TXN_ACCEPTED:
			{
				// vTaskDelay(700 / portTICK_PERIOD_MS);

				// if (cpcurrentlimit == 0)
				// {
				// 	cpcurrentlimit = CHARGE_CURRENT_IN_AMPS;
				// }
				// xSemaphoreTake(mutexControlPilot, portMAX_DELAY);
				// ptr_cp_out->set_cp_begin(cpcurrentlimit);
				// Serial.println("CP LOOP: start transaction accepted ");
				// xSemaphoreGive(mutexControlPilot);

				gu32_evse_suspended_count = 0;

#if EVSE_DWIN_ENABLE
				gs32_st_time = millis();
				vTaskDelay(200 / portTICK_PERIOD_MS);

#endif

#if PREFERENCE_USED
				/**********Preferences Block For Restarting previously running Txn [Power Cycle]***********/
				evse_resumeTxn.begin("resume_conn_1", false); // opening preferences in R/W mode
				evse_resumeTxn.putString("idTagData", evse_idTagData);
				evse_resumeTxn.putBool("ongoingTxn", true);
				evse_resumeTxn.putInt("TxnIdData", evse_transactionId);
				evse_resumeTxn.putFloat("meterStart", evse_globalmeterstart);
				evse_resumeTxn.end();
#endif
				gu8_evse_change_state = EVSE_CHARGING;

				EvseDevStatus_connector_1 = flag_EVSE_Request_Charge;
				break;
			}
			case EVSE_START_TXN_REJECTED:
			{
				break;
			}
			case EVSE_START_TXN_DEFAULT:
			{
				break;
			}

			default:
				break;
			}
		}

		case flag_EVSE_Request_Charge:
		{
			// Serial.println("request charge 1");
			// vTaskDelay(700 / portTICK_PERIOD_MS);
			if ((alternet_10d_flag == 2) && (Charging_3_3 != 1))
			{
				if (((EVSE_states == STATE_B) || (EVSE_states == STATE_E)) && (evse_ChargePointStatus != Faulted))
				{
					if (plugandcharge_1 == 1)
					{
						plugandcharge_7_4 = 2;
					}

					if (cpcurrentlimit == 0)
					{
						cpcurrentlimit = CHARGE_CURRENT_IN_AMPS;
					}
					xSemaphoreTake(mutexControlPilot, portMAX_DELAY);
					ptr_cp_out->set_cp_begin(cpcurrentlimit);
					Serial.println("CP LOOP: start transaction accepted ");
					xSemaphoreGive(mutexControlPilot);
				}
			}

			if ((alternet_10d_flag == 1) && (evse_ChargePointStatus != Faulted))
			{

				ptr_evse_relay->evse_relay2_on();
				// ptr_evse_relay->evse_relay_on();
				evse_ChargePointStatus = Charging;
				Charging_3_3 = 1;
			}

			// Serial.println("CP LOOP: EVSE Request Charge ");
			if (gu8_evse_change_state == EVSE_CHARGING)
			{

				gu8_evse_change_state = EVSE_STOP_TXN;
				// evse_ChargePointStatus = Charging;
			}

			if (evse_ChargePointStatus == Faulted)
			{
				ptr_evse_relay->evse_relay_off();
				ptr_evse_relay->evse_relay2_off();
				EvseDevStatus_connector_1 = flag_EVSE_Stop_Transaction;

				evse_stop_txn_state = EVSE_STOP_TXN_INITIATED;
				ptr_cp_out->set_cp_on();
			}
			if ((EVSE_states == STATE_SUS) || (EVSE_states == STATE_C))
			{
				EVSE_transaction_status = EVSE_START_TRANSACTION;
			}

#if EVSE_SUS_COUNT
			if (EVSE_states == STATE_SUS)
			{

				if (gu32_evse_suspended_count == 0)
				{
					gu32_evse_suspended_count = millis();
				}
				else if (millis() - gu32_evse_suspended_count > (evse_suspended_ev_interval * 1000))
				{
					gu32_evse_suspended_count = 0;
					ptr_evse_relay->evse_relay_off();
					EvseDevStatus_connector_1 = flag_EVSE_Stop_Transaction;
					evse_stop_txn_state = EVSE_STOP_TXN_INITIATED;
					reasonForStop = 4;
					Serial.println("EVSE_SUS: Sending stop for suspended ev");
					ptr_cp_out->set_cp_on();
				}
			}

#endif
#if EVSE_LOW_CURRENT
			if (Charging_3_3 == 1)
			{
				if (ipc_current_A <= 0.1)
				{

					if (gu32_evse_no_load_intervel_count == 0)
					{
						gu32_evse_no_load_intervel_count = millis();
						Serial.println("LOW CURRENT ");
					}
					else if (millis() - gu32_evse_no_load_intervel_count > (evse_no_load_interval * 1000))
					{
						gu32_evse_no_load_intervel_count = 0;
						ptr_evse_relay->evse_relay2_off();
						EvseDevStatus_connector_1 = flag_EVSE_Stop_Transaction;
						evse_stop_txn_state = EVSE_STOP_TXN_INITIATED;
						reasonForStop = 1;
						Serial.println("LOW CURRENT1");
						// ptr_cp_out->set_cp_on();
					}
				}
			}
#endif

			break;
		}

		case flag_EVSE_Stop_Transaction:
		{
			// Serial.println("stop txn 1");
#if 1
			switch (evse_stop_txn_state)
			{
			case EVSE_STOP_TXN_INITIATED:
			{
				// if (gu8_evse_change_state == EVSE_STOP_TXN)
				// {
				EVSE_transaction_status = EVSE_STOP_TRANSACTION;

				/*brief using mutex to update values*/
				portENTER_CRITICAL(&rfid_tag_mux);
				ocpp_rfid = rfid_buf;
				rfid_buf = " ";
				portEXIT_CRITICAL(&rfid_tag_mux);

				flag_rfid = 0;
				Charging_3_3 = 0;
				plugandcharge_7_4 = 1;
				int txnId = getChargePointStatusService()->getTransactionId();

				ptr_evse_relay->evse_relay_off();
				ptr_evse_relay->evse_relay2_off();
				// vTaskDelay(500 / portTICK_PERIOD_MS);
				ptr_cp_out->set_cp_on();

				OcppOperation* stopTransaction = makeOcppOperation(&webSocket, new StopTransaction());
				initiateOcppOperation(stopTransaction);

#if 0
				if (gu8_online_flag == 1)
				{
					evse_stop_txn_state = EVSE_STOP_TXN_SENT;
				}
				else
				{
#if EVSE_OFFLINE_FUNCTIONALITY_ENABLE
					evse_stop_txn_state = EVSE_STOP_TXN_ACCEPTED;
					gu8_offline_start_flag = 0;
#endif
				}
#endif

#if 1
				switch (connnection_type)
				{

				case CONNECT_WIFI_ENABLED:
				{
					evse_stop_txn_state = EVSE_STOP_TXN_SENT;
				}
				break;
				case CONNECT_GSM_ENABLED:
				{
					evse_stop_txn_state = EVSE_STOP_TXN_SENT;
				}
				break;
				case CONNECT_WIFI_GSM_ENABLED:
				{
					evse_stop_txn_state = EVSE_STOP_TXN_SENT;
				}
				break;
				case CONNECT_ETHERNET_ENABLED:
				{
					evse_stop_txn_state = EVSE_STOP_TXN_SENT;
				}
				break;
				case CONNECT_WIFI_ETHERNET_ENABLED:
				{
					if (gu8_online_flag == 1)
					{
						evse_stop_txn_state = EVSE_STOP_TXN_SENT;
					}
					else
					{

						evse_stop_txn_state = EVSE_STOP_TXN_ACCEPTED;
						gu8_offline_start_flag = 0;
					}
				}
				break;
				case CONNECT_GSM_ETHERNET_ENABLED:
				{
					if (gu8_online_flag == 1)
					{
						evse_stop_txn_state = EVSE_STOP_TXN_SENT;
					}
					else
					{

						evse_stop_txn_state = EVSE_STOP_TXN_ACCEPTED;
						gu8_offline_start_flag = 0;
					}
				}
				break;
				case CONNECT_WIFI_GSM_ETHERNET_ENABLED:
				{
				}
				break;
				case CONNECT_WIFI_GSM_ETHERNET_DISABLED:
				{
					evse_stop_txn_state = EVSE_STOP_TXN_ACCEPTED;
					gu8_offline_start_flag = 0;
				}
				break;
				default:
				{
				}
				break;
				}
#endif

				break;
			}
			case EVSE_STOP_TXN_SENT:
			{

				break;
			}

			case EVSE_STOP_TXN_ACCEPTED:
			{
				evse_ChargePointStatus = Finishing;
				chargePointStatusService->loop();

				Serial.print("EVSE_setOnStopTransaction Callback: Closing Relays & StopTransaction was successful\r\n");
				// evse_ChargePointStatus = Finishing;
#if PREFERENCE_USED
				/**********Preferences Block For Restarting previously running Txn [Power Cycle]***********/
				evse_resumeTxn.begin("resume_conn_1", false); // opening preferences in R/W mode
				evse_resumeTxn.putString("idTagData", "");
				evse_resumeTxn.putBool("ongoingTxn", false);
				evse_resumeTxn.putInt("TxnIdData", -1);
				evse_resumeTxn.putFloat("meterStart", 0);
				evse_resumeTxn.end();

#endif
				evse_idTagData = ""; // Clear the RFID for next Start Transaction
				// }

				gu8_evse_change_state = EVSE_UNAUTHENTICATION;
				ocpp_rfid = "";
				evse_led_status = Green;

				EvseDevStatus_connector_1 = flag_EVSE_UnAutharized;

				break;
			}

			case EVSE_STOP_TXN_REJECTED:
			{
				break;
			}

			case EVSE_STOP_TXN_DEFAULT:
			{
				break;
			}

			default:
				break;
			}
#endif

			break;
		}

		case flag_EVSE_UnAutharized:
		{
			// Serial.println("unauthorize 1");
			if (gu8_evse_change_state == EVSE_UNAUTHENTICATION)
			{
				if (gu8_state_change++ > 10)
				{
					Serial.println("CP LOOP: UnAutharized ");
					gu8_evse_change_state = EVSE_READ_RFID;
					EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;

					gu8_state_change = 0;
				}
				if (gu8_change_avaliable == EVSE_CMS_CHANGE_AVAILABLITY_ONE)
				{
					evse_ChargePointStatus = Unavailable;
					gu8_change_avaliable = EVSE_CMS_CHANGE_AVAILABLITY_TWO;
				}
			}
			break;
		}

		case flag_EVSE_Reboot_Request:
		{
			Serial.println("CP LOOP: Reboot request");
			break;
		}

		case flag_EVSE_Reserve_Now:
		{
			Serial.println("CP LOOP: reserver now");
			break;
		}
		case flag_EVSE_Cancle_Reservation:
		{

			Serial.println("CP LOOP: cancle reservation ");
			break;
		}
		case flag_EVSE_Local_Authantication:
		{
			Serial.println("CP LOOP: local authentication");
			break;
		}
		default:
		{
			Serial.println("CP LOOP: default");
			gu8_evse_change_state = EVSE_READ_RFID;
			break;
		}
		}

		chargePointStatusService->loop();

		switch (evse_ChargePointStatus)
		{
		case Charging:

			if (millis() - timerMv_count >= (meterSampleInterval))
			{
				timerMv_count = millis();

				uint32_t lu32_start = millis();
				// Serial.println("start meter time:" + String(lu32_start));

				meteringService->loop();

				uint32_t lu32_end = millis();
				// Serial.println("end meter time:" + String(lu32_end));
			}

			break;

		default:

			if (millis() - timerHb > ((heartbeatInterval * 1000) - 200)) //
			{
				timerHb = millis();

				if (gu8_online_flag == 1)
				{
					if (DEBUG_OUT)
						Serial.print("EVSE_Sending heartbeat signal...\n");
					OcppOperation* heartbeat = makeOcppOperation(&webSocket, new Heartbeat());
					initiateOcppOperation(heartbeat);
					heartbeat->setOnReceiveConfListener([](JsonObject payload)
						{
							const char* currentTime = payload["currentTime"] | "Invalid";
							if (strcmp(currentTime, "Invalid")) {
								if (setTimeFromJsonDateString(currentTime)) {
									if (DEBUG_OUT) Serial.print("EVSE_setOnsendHeartbeat Callback: Request has been accepted!\n");
								}
								else {
									Serial.print("EVSE Expect format like 2020-02-01T20:53:32.486Z\n");
								}
							}
							else {
								Serial.print("EVSE Missing field currentTime. Expect format like 2020-02-01T20:53:32.486Z\n");
							} });
				}
			}

			break;
		}

		if (gu8_online_flag == 1)
		{
			ocppEngine_loop();
		}

		switch (gu8_EVSE_CMS_reset)
		{
		case EVSE_CMS_RESET_HARD:
		{

			ESP.restart();
		}
		break;

		case EVSE_CMS_RESET_SOFT:
			if ((evse_ChargePointStatus == Charging) || (evse_ChargePointStatus == SuspendedEV))
			{
				EvseDevStatus_connector_1 = flag_EVSE_Stop_Transaction;

				if (gu8_evse_cms_reset_flag == 1)
				{
					ESP.restart();
				}
			}

			else
			{
				ESP.restart();
			}
			break;

		default:

			break;
		}

		switch (connnection_type)
		{

		case CONNECT_GSM_ENABLED:
		case CONNECT_GSM_ETHERNET_ENABLED:
		{
#if EVSE_WEBSOCKET_PING_PONG_ENABLE
			// vTaskDelay(100 / portTICK_PERIOD_MS);
			// cloudConnectivityLed_Loop();

			if ((evse_start_txn_state != EVSE_START_TXN_INITIATED) && (EvseDevStatus_connector_1 != flag_EVSE_is_Booted))
			{
				cloudConnectivityLed_Loop();
			}

#endif
		}
		break;
		default:

			break;
		}

		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}

/***********************************************WIFI_OTA********************************************************************/

void setup_WIFI_OTA_get_1(void)
{
	HTTPClient http;

	WiFi.disconnect(true);
	WiFi.mode(WIFI_STA); /*wifi station mode*/
	// WiFi.begin(ssid_, password);
	WiFi.begin(evse_ble_ssid.c_str(), evse_ble_ssid_key.c_str());

	Serial.println("\nConnecting");

	// while (WiFi.status() != WL_CONNECTED)
	gu32counter_wifiNotConnected = 50;
	while ((WiFi.status() != WL_CONNECTED) && (gu32counter_wifiNotConnected-- > 0))
	{
		Serial.print(".");
		// delay(100);
		vTaskDelay(10000 / portTICK_PERIOD_MS);
	}

	Serial.println("\nConnected to the WiFi network");
	Serial.print("Local ESP32 IP: ");
	Serial.println(WiFi.localIP());

	vTaskDelay(5000 / portTICK_PERIOD_MS);

	Serial.setDebugOutput(true);

	// WiFiMulti.addAP("EVRE", "Amplify5");

	// Client address
	Serial.print("WebSockets Client started @ IP address: ");
	Serial.println(WiFi.localIP());

	//  String URI = String("http://34.100.138.28/fota2.php");

#if TEST_OTA

#if EVSE_3S_CP_SR
	String URI = String("http://34.100.138.28/evse_rt_type2_3_3kw_ota.php");
#endif

#if EVSE_7_4_
	String URI = String("http://34.100.138.28/evse_adani_7_ota_test.php");
#endif

#if EVSE_7S_RT_OST
	String URI = String("http://34.100.138.28/7S_RT_OST_ota.php");
#endif

	Serial.println("[OTA] Test OTA Begin...");
#else
	String URI = String("http://34.100.138.28/evse_ota.php");
	Serial.println("[OTA] OTA Begin...");
#endif

	Serial.println("[HTTP] begin...");

	Serial.print("The URL given is:");
	//   Serial.println(uri);
	Serial.println(URI);

#if 1
	int updateSize = 0;

	// configure server and url
	// String post_data = "{\"version\":\"CP001/hello.ino.esp32\", \"deviceId\":\"CP001\"}";
	// String post_data = "{\"version\":\"display_TestUART.ino.esp32\",\"deviceId\":\"CP001\"}";
	/*http.begin("https://us-central1-evre-iot-308216.cloudfunctions.net/otaUpdate");
	http.addHeader("Content-Type", "application/json");
	http.addHeader("Connection", "keep-alive");*/

	// http.begin("http://34.93.75.210/fota2.php");
	http.begin(URI);

	http.setUserAgent(F("ESP32-http-Update"));
	http.addHeader(F("x-ESP32-STA-MAC"), WiFi.macAddress());
	http.addHeader(F("x-ESP32-AP-MAC"), WiFi.softAPmacAddress());
	http.addHeader(F("x-ESP32-sketch-md5"), String(ESP.getSketchMD5()));
	http.addHeader(F("x-ESP32-STA-MAC: "), ESP.getSdkVersion());
	http.addHeader(F("x-ESP32-STA-MAC: "), String(ESP.getFreeSketchSpace()));
	// http.addHeader(F("x-ESP32-sketch-size"), String(ESP.getSketchSize()));
	// http.addHeader(F("x-ESP32-device-id: "), DEVICE_ID);
	http.addHeader(F("x-ESP32-device-test-id: "), CP_Id_m);
	http.addHeader(F("x-ESP32-firmware-version: "), EVSE_CHARGE_POINT_FIRMWARE_OTA_VERSION /* + "\r\n" */);

	// int httpCode = http.POST(post_data);
	// int httpCode = http.GET(post_data);
	int httpCode = http.GET();
	// int httpCode = http.POST();

	if (httpCode > 0)
	{
		// HTTP header has been send and Server response header has been handled
		Serial.println("Checking for new firmware updates...");

		// If file found at server
		if (httpCode == HTTP_CODE_OK)
		{

			int len = http.getSize();
			updateSize = len;
			Serial.printf("[OTA] Update found, File size(bytes) : %d\n", len);
			String get_response = http.getString();
			Serial.printf("[OTA] http response : %s\n", get_response);
			Serial.println("[HTTP] connection closed or file end.\n");

			if (get_response.equals("true") == true)
			{
				Serial.print("OTA update available");
				gu8_OTA_update_flag = 2;
			}
			else if (get_response.equals("false") == false)
			{
				gu8_OTA_update_flag = 3;
				Serial.print("no OTA update");
			}
		}
		// If there is no file at server
		if (httpCode == HTTP_CODE_INTERNAL_SERVER_ERROR)
		{
			Serial.println("[HTTP] No Updates");
			Serial.println();
			// ESP.restart();
		}
		Serial.println("[HTTP] Other response code");
		Serial.println(httpCode);
		Serial.println();
	}
	http.end();

#endif
}

void setup_WIFI_OTA_1(void)
{
	HTTPClient http;

#if 1
	WiFi.disconnect(true);
	WiFi.mode(WIFI_STA); /*wifi station mode*/
	// WiFi.begin(ssid_, password);
	WiFi.begin(evse_ble_ssid.c_str(), evse_ble_ssid_key.c_str());

	Serial.println("\nConnecting");

	// while (WiFi.status() != WL_CONNECTED)
	gu32counter_wifiNotConnected = 50;
	while ((WiFi.status() != WL_CONNECTED) && (gu32counter_wifiNotConnected-- > 0))

	{
		Serial.print(".");
		// delay(100);
		vTaskDelay(10000 / portTICK_PERIOD_MS);
	}
#endif
	Serial.println("\nConnected to the WiFi network");
	Serial.print("Local ESP32 IP: ");
	Serial.println(WiFi.localIP());

	vTaskDelay(5000 / portTICK_PERIOD_MS);

	Serial.setDebugOutput(true);

	// WiFiMulti.addAP("EVRE", "Amplify5");

	// Client address
	Serial.print("WebSockets Client started @ IP address: ");
	Serial.println(WiFi.localIP());

	//  String URI = String("http://34.100.138.28/fota2.php");

#if TEST_OTA

#if EVSE_3S_CP_SR
	String URI = String("http://34.100.138.28/evse_rt_type2_3_3kw_ota.php");
#endif

#if EVSE_7S_RT_OST
	String URI = String("http://34.100.138.28/7S_RT_OST_ota.php");
#endif

#if EVSE_7_4_
	String URI = String("http://34.100.138.28/evse_adani_7_ota_test.php");
#endif
	// String URI = String("http://34.100.138.28/evse_rt_type2_3_3kw_ota.php"); //
	// String URI = String("http://34.100.138.28/7S_RT_OST_ota.php");
	// String URI = String("http://34.100.138.28/evse_adani_7_ota_test.php");
	Serial.println("[OTA] Test OTA Begin...");
#else
	String URI = String("http://34.100.138.28/evse_ota.php");
	Serial.println("[OTA] OTA Begin...");
#endif

	Serial.println("[HTTP] begin...");

	Serial.print("The URL given is:");
	//   Serial.println(uri);
	Serial.println(URI);

#if 1
	int updateSize = 0;

	// configure server and url
	// String post_data = "{\"version\":\"CP001/hello.ino.esp32\", \"deviceId\":\"CP001\"}";
	// String post_data = "{\"version\":\"display_TestUART.ino.esp32\",\"deviceId\":\"CP001\"}";
	/*http.begin("https://us-central1-evre-iot-308216.cloudfunctions.net/otaUpdate");
	http.addHeader("Content-Type", "application/json");
	http.addHeader("Connection", "keep-alive");*/

	// http.begin("http://34.93.75.210/fota2.php");
	http.begin(URI);

	http.setUserAgent(F("ESP32-http-Update"));
	http.addHeader(F("x-ESP32-STA-MAC"), WiFi.macAddress());
	http.addHeader(F("x-ESP32-AP-MAC"), WiFi.softAPmacAddress());
	http.addHeader(F("x-ESP32-sketch-md5"), String(ESP.getSketchMD5()));
	http.addHeader(F("x-ESP32-STA-MAC: "), ESP.getSdkVersion());
	http.addHeader(F("x-ESP32-STA-MAC: "), String(ESP.getFreeSketchSpace()));
	// http.addHeader(F("x-ESP32-sketch-size"), String(ESP.getSketchSize()));
	// http.addHeader(F("x-ESP32-device-id: "), DEVICE_ID);
	http.addHeader(F("x-ESP32-device-test-id: "), CP_Id_m);
	http.addHeader(F("x-ESP32-firmware-version: "), EVSE_CHARGE_POINT_FIRMWARE_OTA_VERSION /* + "\r\n" */);

	//   int httpCode = http.POST(post_data);
	// int httpCode = http.GET(post_data);
	// int httpCode = http.GET();
	int httpCode = http.POST(CP_Id_m);

	if (httpCode > 0)
	{
		// HTTP header has been send and Server response header has been handled
		Serial.println("Checking for new firmware updates...");

		// If file found at server
		if (httpCode == HTTP_CODE_OK)
		{
			// get lenght of document (is -1 when Server sends no Content-Length header)
			int len = http.getSize();
			updateSize = len;
			Serial.printf("[OTA] Update found, File size(bytes) : %d\n", len);

#if 1
			// get tcp stream
			WiFiClient* client = http.getStreamPtr();
			// Serial.println();
			performUpdate_WiFi_1(*client, (size_t)updateSize);
			Serial.println("[HTTP] connection closed or file end.\n");
#endif
			Serial.println("[HTTP] connection closed or file end.\n");
		}
		// If there is no file at server
		if (httpCode == HTTP_CODE_INTERNAL_SERVER_ERROR)
		{
			Serial.println("[HTTP] No Updates");
			Serial.println();
			// ESP.restart();
		}
		Serial.println("[HTTP] Other response code");
		Serial.println(httpCode);
		Serial.println();
	}
	http.end();
#endif
}

void performUpdate_WiFi_1(WiFiClient& updateSource, size_t updateSize)
{
	if (Update.begin(updateSize))
	{
		Serial.println("...Downloading File...");
		Serial.println();

		// Writing Update
		size_t written = Update.writeStream(updateSource);

		printPercent_1(written, updateSize);

		if (written == updateSize)
		{
			Serial.println("Written : " + String(written) + "bytes successfully");
		}
		else
		{
			Serial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
		}
		if (Update.end())
		{
			Serial.println("OTA done!");
			if (Update.isFinished())
			{
				Serial.println("Update successfully completed. Rebooting...");
				Serial.println();
				ESP.restart();
			}
			else
			{
				Serial.println("Update not finished? Something went wrong!");
			}
		}
		else
		{
			Serial.println("Error Occurred. Error #: " + String(Update.getError()));
		}
	}
	else
	{
		Serial.println("Not enough space to begin OTA");
	}
}

void printPercent_1(uint32_t readLength, uint32_t contentLength)
{
	// If we know the total length
	if (contentLength != (uint32_t)-1)
	{
		Serial.print("\r ");
		Serial.print((100.0 * readLength) / contentLength);
		Serial.print('%');
	}
	else
	{
		Serial.println(readLength);
	}
}

void setup_WIFI_OTA_getconfig_1(void)
{
	HTTPClient http;

	WiFi.disconnect(true);
	WiFi.mode(WIFI_STA); /*wifi station mode*/
	// WiFi.begin(ssid_, password);
	WiFi.begin(evse_ble_ssid.c_str(), evse_ble_ssid_key.c_str());
	Serial.println("\nConnecting");

	while (WiFi.status() != WL_CONNECTED)
	{
		Serial.print(".");
		// delay(100);
		vTaskDelay(10000 / portTICK_PERIOD_MS);
	}

	Serial.println("\nConnected to the WiFi network");
	Serial.print("Local ESP32 IP: ");
	Serial.println(WiFi.localIP());

	vTaskDelay(5000 / portTICK_PERIOD_MS);

	Serial.setDebugOutput(true);

	// WiFiMulti.addAP("EVRE", "Amplify5");

	// Client address
	Serial.print("WebSockets Client started @ IP address: ");
	Serial.println(WiFi.localIP());

	//  String URI = String("http://34.100.138.28/fota2.php");

#if TEST_OTA
	// String URI = String("http://34.100.138.28/evse_test_ota.php");
	String URI_GET_CONFIG = String("http://34.100.138.28/evse-config-update.php");

	Serial.println("[OTA]  Get config Begin...");
#else
	String URI = String("http://34.100.138.28/evse_ota.php");
	Serial.println("[OTA] OTA Begin...");
#endif

	Serial.println("[HTTP] begin...");

	Serial.print("The URL given is:");
	//   Serial.println(uri);
	Serial.println(URI_GET_CONFIG);

#if 1
	int updateSize = 0;

	// configure server and url
	// String post_data = "{\"version\":\"CP001/hello.ino.esp32\", \"deviceId\":\"CP001\"}";
	// String post_data = "{\"version\":\"display_TestUART.ino.esp32\",\"deviceId\":\"CP001\"}";
	/*http.begin("https://us-central1-evre-iot-308216.cloudfunctions.net/otaUpdate");
	http.addHeader("Content-Type", "application/json");
	http.addHeader("Connection", "keep-alive");*/

	// http.begin("http://34.93.75.210/fota2.php");
	http.begin(URI_GET_CONFIG);

	http.setUserAgent(F("ESP32-http-Update"));
	http.addHeader(F("x-ESP32-STA-MAC"), WiFi.macAddress());
	http.addHeader(F("x-ESP32-AP-MAC"), WiFi.softAPmacAddress());
	http.addHeader(F("x-ESP32-sketch-md5"), String(ESP.getSketchMD5()));
	http.addHeader(F("x-ESP32-STA-MAC: "), ESP.getSdkVersion());
	http.addHeader(F("x-ESP32-STA-MAC: "), String(ESP.getFreeSketchSpace()));
	// http.addHeader(F("x-ESP32-sketch-size"), String(ESP.getSketchSize()));
	http.addHeader(F("x-ESP32-device-id: "), DEVICE_ID);
	// http.addHeader(F("x-ESP32-device-test-id: "), DEVICE_ID);
	http.addHeader(F("x-ESP32-firmware-version: "), EVSE_CHARGE_POINT_FIRMWARE_OTA_VERSION /* + "\r\n" */);

	// int httpCode = http.POST(post_data);
	// int httpCode = http.GET(post_data);
	int httpCode = http.GET();
	// int httpCode = http.POST();

	if (httpCode > 0)
	{
		// HTTP header has been send and Server response header has been handled
		Serial.println("Checking for new configs...");

		// If file found at server
		if (httpCode == HTTP_CODE_OK)
		{
			// get lenght of document (is -1 when Server sends no Content-Length header)
			int len = http.getSize();
			// updateSize = len;
			Serial.printf("[OTA] get config ,  : %d\n", len);
			String get_response = http.getString();
			Serial.printf("[OTA] http response : %s\n", get_response);
			Serial.println("[HTTP] connection closed or file end.\n");

#if 0 
			DeserializationError error = deserializeJson(server_config, get_response);

			//{"wifi":"EVRE","port":"80","otaupdatetime":"86400"}

			if (error)
			{
				Serial.print(F("DeserializeJson() failed: "));
				Serial.println(error.f_str());
				// return connectedToWifi;
			}
			if (server_config.containsKey("wifi"))
			{
				wifi_server = server_config["wifi"];
			}
			if (server_config.containsKey("port"))
			{
				port_server = server_config["port"];
			}

			if (server_config.containsKey("otaupdatetime"))
			{
				ota_update_time = server_config["otaupdatetime"];
			}

			get_response = "";
			// put_server_config(); 
			Serial.println("\r\nclient disconnected....");
#endif
		}
		// If there is no file at server
		if (httpCode == HTTP_CODE_INTERNAL_SERVER_ERROR)
		{
			Serial.println("[HTTP] No Updates");
			Serial.println();
			// ESP.restart();
		}
		Serial.println("[HTTP] Other response code");
		Serial.println(httpCode);
		Serial.println();
	}
	http.end();

#endif
}

/************************************************************4G_OTA*********************************************************/

#if 1

#if EVSE_3S_CP_SR
const char resource[] = "/evse_rt_type2_3_3kw_ota.php";
#endif

#if EVSE_7_4_
const char resource[] = "/evse_adani_7_ota_test.php";
#endif

#if EVSE_7S_RT_OST
const char resource[] = "/7S_RT_OST_ota.php";
#endif

const char server[] = "34.100.138.28";
const int port = 80;
#endif

// const char gprsUser[] = "";
// const char gprsPass[] = "";

uint32_t knownCRC32 = 0x6f50d767;
uint32_t knownFileSize = 1148544;

// extern TinyGsm modem;
/// extern TinyGsmClient client;

// extern TinyGsm modem(Serial2);
// extern TinyGsmClient client(modem);

String APN = "m2misafe";
char g_apn[8] = { 'm', '2', 'm', 'i', 's', 'a', 'f', 'e' };
// char g_apn[14] = {'a', 'i', 'r', 't', 'e', 'l', 'g', 'p', 'r', 's', '.', 'c', 'o', 'm'};

void OTA_4G_setup_4G_OTA_get(void)
{
	// modem.gprsConnect(apn, gprsUser, gprsPass);
	// APN = getAPN();

	// strcpy(g_apn,APN.c_str());
	// setup4G();
	OTA_4G_setup4G();
	Serial.print("Waiting for network...");
	if (!ota_modem.waitForNetwork())
	{
		Serial.println(" fail");

#if FREE_RTOS_THREAD
		vTaskDelay(10000 / portTICK_PERIOD_MS);
#else
		delay(10000);
#endif
		return;
	}
	Serial.println(" success");

	if (ota_modem.isNetworkConnected())
	{
		Serial.println("Network connected");
	}
#if TINY_GSM_USE_GPRS | 1
	// GPRS connection parameters are usually set after network registration
	Serial.print(F("Connecting to "));
	Serial.print(g_apn);
	if (!ota_modem.gprsConnect(g_apn, gprsUser, gprsPass))
	{
		Serial.println(" fail");
#if FREE_RTOS_THREAD
		vTaskDelay(10000 / portTICK_PERIOD_MS);
#else
		delay(10000);
#endif
		return;
	}
	Serial.println(" success");

	if (ota_modem.isGprsConnected())
	{
		Serial.println("GPRS connected");
	}
#endif
	Serial.print("Connecting to ");

	Serial.print(server);

	strcpy(&fota_host[0], host_fota.c_str());
	strcpy(&fota_path[0], path_fota.c_str());
	fota_port = port_fota;

	// Serial.print(fota_host);

	if (!ota_client.connect(server, port))
		// if (!ota_client.connect(fota_host, fota_port))
	{
		Serial.println(" fail");
#if FREE_RTOS_THREAD
		vTaskDelay(10000 / portTICK_PERIOD_MS);
#else
		delay(10000);
#endif
		return;
	}
	Serial.println(" success");

	// ota_client.print(String("POST ") + resource + " HTTP/1.0\r\n");
	ota_client.print(String("GET ") + resource + " HTTP/1.1\r\n");
	ota_client.print(String("Host: ") + server + "\r\n");

	// dynamic update
	//  ota_client.print(String("GET ") + fota_path + " HTTP/1.1\r\n");
	//  ota_client.print(String("Host: ") + fota_host + "\r\n");
	ota_client.print(String("x-ESP32-STA-MAC: ") + String(WiFi.macAddress()) + "\r\n");
	ota_client.print(String("x-ESP32-AP-MAC: ") + String(WiFi.softAPmacAddress()) + "\r\n");
	ota_client.print(String("x-ESP32-sketch-md5: ") + String(ESP.getSketchMD5()) + "\r\n");
	ota_client.print(String("x-ESP32-STA-MAC: ") + String(ESP.getSdkVersion()) + "\r\n");
	ota_client.print(String("x-ESP32-STA-MAC: ") + String(ESP.getFreeSketchSpace()) + "\r\n");

	// ota_client.print(String("x-ESP32-firmware-version: ") + VERSION + "\r\n");
	// ota_client.print(String("x-ESP32-device-id: ") + "evse_001" + "\r\n");

	ota_client.print(String("x-ESP32-device-test-id: ") + CP_Id_m + "\r\n");
	ota_client.print(String("x-ESP32-firmware-version: ") + EVSE_CHARGE_POINT_FIRMWARE_OTA_VERSION + "\r\n");

	ota_client.print("Connection: close\r\n\r\n");

	// Let's see what the entire elapsed time is, from after we send the request.
	uint32_t timeElapsed = millis();
	Serial.println(F("Waiting for response header"));

	// While we are still looking for the end of the header (i.e. empty line
	// FOLLOWED by a newline), continue to read data into the buffer, parsing each
	// line (data FOLLOWED by a newline). If it takes too long to get data from
	// the ota_client, we need to exit.

	const uint32_t ota_clientReadTimeout = 600000;
	uint32_t ota_clientReadStartTime = millis();
	String headerBuffer;
	bool finishedHeader = false;
	uint32_t contentLength = 0;

	while (!finishedHeader)
	{
		int nlPos;

		if (ota_client.available())
		{
			ota_clientReadStartTime = millis();
			while (ota_client.available())
			{
				char c = ota_client.read();
				headerBuffer += c;

				// Uncomment the lines below to see the data coming into the buffer
				if (c < 16)
					Serial.print('0');
				Serial.print(c, HEX);
				Serial.print(' ');
				/*if (isprint(c))
					Serial.print(reinterpret_cast<char> c);
					else
					Serial.print('*');*/
				Serial.print(' ');

				// Let's exit and process if we find a new line
				if (headerBuffer.indexOf(F("\r\n")) >= 0)
					break;
			}
		}
		else
		{
			if (millis() - ota_clientReadStartTime > ota_clientReadTimeout)
			{
				// Time-out waiting for data from ota_client
				Serial.println(F(">>> ota_client Timeout !"));
				break;
			}
		}
		// See if we have a new line.
		nlPos = headerBuffer.indexOf(F("\r\n"));

		if (nlPos > 0)
		{
			headerBuffer.toLowerCase();
			// Check if line contains content-length
			if (headerBuffer.startsWith(F("content-length:")))
			{
				contentLength = headerBuffer.substring(headerBuffer.indexOf(':') + 1).toInt();
				// Serial.print(F("Got Content Length: "));  // uncomment for
				// Serial.println(contentLength);            // confirmation
			}

			headerBuffer.remove(0, nlPos + 2); // remove the line
		}
		else if (nlPos == 0)
		{
			// if the new line is empty (i.e. "\r\n" is at the beginning of the line),
			// we are done with the header.
			finishedHeader = true;
		}
	}

	// The two cases which are not managed properly are as follows:
	// 1. The ota_client doesn't provide data quickly enough to keep up with this
	// loop.
	// 2. If the ota_client data is segmented in the middle of the 'Content-Length: '
	// header,
	//    then that header may be missed/damaged.
	//

	uint32_t readLength = 0;
	CRC32 crc;
	// File file = FFat.open("/update.bin", FILE_APPEND);
	// if (finishedHeader && contentLength == knownFileSize) {
	if (finishedHeader)
	{

		Serial.println(F("Reading response data"));
		ota_clientReadStartTime = millis();

		String get_response = "";
		// printPercent(readLength, contentLength);
		while (readLength < contentLength && ota_client.connected() &&
			millis() - ota_clientReadStartTime < ota_clientReadTimeout)
		{
			ota_clientReadStartTime = millis();
			while (ota_client.available())
			{
				// uint8_t c = ota_client.read();
				char c = ota_client.read();
				if (c == 'f')
				{
					Serial.print("no OTA update");
					gu8_OTA_update_flag = 3;
				}
				if (c == 't')
				{
					Serial.print("OTA update available");
					gu8_OTA_update_flag = 2;
				}
				// Serial.print(c);
				get_response += c;
				// Serial.print(reinterpret_cast<char>c);  // Uncomment this to show
				// data
				// crc.update(c);
				readLength++;
			}
			Serial.println("\r\n" + get_response);

			if (get_response.equals("true") == true)
			{
				// gu8_OTA_update_flag = 2;
				// Serial.print("OTA update available");
			}
			else if (get_response.equals("false") == false)
			{
				// gu8_OTA_update_flag = 3;
				// Serial.print("no OTA update");
			}
			get_response = "";
			Serial.println("\r\nota_client disconnected....");
		}
		// printPercent(readLength, contentLength);
	}

	timeElapsed = millis() - timeElapsed;
	Serial.println();

	ota_client.stop();
	Serial.println(F("Server disconnected"));

#if TINY_GSM_USE_GPRS
	ota_modem.gprsDisconnect();
	Serial.println(F("GPRS disconnected"));
#endif

	float duration = float(timeElapsed) / 1000;

	Serial.println();
	Serial.print("Content-Length: ");
	Serial.println(contentLength);
	Serial.print("Actually read:  ");
	Serial.println(readLength);
	Serial.print("Calc. CRC32:    0x");
	Serial.println(crc.finalize(), HEX);
	Serial.print("Known CRC32:    0x");
	Serial.println(knownCRC32, HEX);
	Serial.print("Duration:       ");
	Serial.print(duration);
	Serial.println("s");
}
///////////////////////////get_response///////////////////

//------------------------------------------ 4G OTA ----------------------------------------//
void OTA_4G_setup_4G_OTA(void)
{
	// strcpy(g_apn,APN.c_str());
	OTA_4G_setup4G();
	Serial.print("Waiting for network...");
	if (!ota_modem.waitForNetwork())
	{
		Serial.println(" fail");
#if FREE_RTOS_THREAD
		vTaskDelay(10000 / portTICK_PERIOD_MS);
#else
		delay(10000);
#endif
		return;
	}
	Serial.println(" success");

	if (ota_modem.isNetworkConnected())
	{
		Serial.println("Network connected");
	}
#if TINY_GSM_USE_GPRS
	// GPRS connection parameters are usually set after network registration
	Serial.print("Connecting to ");
	Serial.print(g_apn);
	if (!ota_modem.gprsConnect(g_apn, gprsUser, gprsPass))
	{
		Serial.println(" fail");
#if FREE_RTOS_THREAD
		vTaskDelay(10000 / portTICK_PERIOD_MS);
#else
		delay(10000);
#endif
		return;
	}
	Serial.println(" success");

	if (ota_modem.isGprsConnected())
	{
		Serial.println("GPRS connected");
	}
#endif
	Serial.print("Connecting to ");
	Serial.print(server);
	// Serial.print(fota_host);

	if (!ota_client.connect(server, port))
		// if (!ota_client.connect(fota_host, fota_port))
	{
		Serial.println(" fail");
#if FREE_RTOS_THREAD
		vTaskDelay(10000 / portTICK_PERIOD_MS);
#else
		delay(10000);
#endif
		return;
	}
	Serial.println(" success");
	// int sketch_md5=ESP.getSketchMD5();
	// Serial.println("sketch_md5 =" + String(sketch_md5));
	//  Make a HTTP GET request:

	ota_client.print(String("POST ") + resource + " HTTP/1.1\r\n");
	ota_client.print(String("Host: ") + server + "\r\n");
	// ota_client.print(String("POST ") + fota_path + " HTTP/1.1\r\n");
	// ota_client.print(String("Host: ") + fota_host + "\r\n");
	ota_client.print(String("x-ESP32-STA-MAC: ") + String(WiFi.macAddress()) + "\r\n");
	ota_client.print(String("x-ESP32-AP-MAC: ") + String(WiFi.softAPmacAddress()) + "\r\n");
	ota_client.print(String("x-ESP32-sketch-md5: ") + String(ESP.getSketchMD5()) + "\r\n");
	ota_client.print(String("x-ESP32-STA-MAC: ") + String(ESP.getSdkVersion()) + "\r\n");
	ota_client.print(String("x-ESP32-STA-MAC: ") + String(ESP.getFreeSketchSpace()) + "\r\n");

	// ota_client.print(String("x-ESP32-firmware-version: ") + VERSION + "\r\n");
	// ota_client.print(String("x-ESP32-device-id: ") + "evse_001" + "\r\n");
	ota_client.print(String("x-ESP32-device-test-id: ") + CP_Id_m + "\r\n");
	ota_client.print(String("x-ESP32-firmware-version: ") + EVSE_CHARGE_POINT_FIRMWARE_OTA_VERSION + "\r\n");

	ota_client.print("Connection: close\r\n\r\n");

	// Let's see what the entire elapsed time is, from after we send the request.
	uint32_t timeElapsed = millis();
	Serial.println(F("Waiting for response header"));

	// While we are still looking for the end of the header (i.e. empty line
	// FOLLOWED by a newline), continue to read data into the buffer, parsing each
	// line (data FOLLOWED by a newline). If it takes too long to get data from
	// the ota_client, we need to exit.

	const uint32_t ota_clientReadTimeout = 600000;
	uint32_t ota_clientReadStartTime = millis();
	String headerBuffer;
	bool finishedHeader = false;
	uint32_t contentLength = 0;

	while (!finishedHeader)
	{
		int nlPos;

		if (ota_client.available())
		{
			ota_clientReadStartTime = millis();
			while (ota_client.available())
			{
				char c = ota_client.read();
				headerBuffer += c;

				// Uncomment the lines below to see the data coming into the buffer
				if (c < 16)
					Serial.print('0');
				Serial.print(c, HEX);
				Serial.print(' ');
				/*if (isprint(c))
					Serial.print(reinterpret_cast<char> c);
					else
					Serial.print('*');*/
				Serial.print(' ');

				// Let's exit and process if we find a new line
				if (headerBuffer.indexOf(F("\r\n")) >= 0)
					break;
			}
		}
		else
		{
			if (millis() - ota_clientReadStartTime > ota_clientReadTimeout)
			{
				// Time-out waiting for data from ota_client
				Serial.println(F(">>> ota_client Timeout !"));
				break;
			}
		}
		// See if we have a new line.
		nlPos = headerBuffer.indexOf(F("\r\n"));

		if (nlPos > 0)
		{
			headerBuffer.toLowerCase();
			// Check if line contains content-length
			if (headerBuffer.startsWith(F("content-length:")))
			{
				contentLength = headerBuffer.substring(headerBuffer.indexOf(':') + 1).toInt();
				Serial.print("Got Content Length: "); // uncomment for
				Serial.println(contentLength);		  // confirmation
				// if(contentLength <= 0)
				// {
				//   // Serial.print(F("Got Content Length: "));  // comment at 06092023_170900
				//   // Serial.println(contentLength);
				//   contentLength = 1;
				// }
			}

			headerBuffer.remove(0, nlPos + 2); // remove the line
		}
		else if (nlPos == 0)
		{
			// if the new line is empty (i.e. "\r\n" is at the beginning of the line),
			// we are done with the header.
			finishedHeader = true;
		}
	}

	// The two cases which are not managed properly are as follows:
	// 1. The ota_client doesn't provide data quickly enough to keep up with this
	// loop.
	// 2. If the ota_client data is segmented in the middle of the 'Content-Length: '
	// header,
	//    then that header may be missed/damaged.
	//

	uint32_t readLength = 0;
	CRC32 crc;

#if 1
	File fs = FFat.open("/update.bin", FILE_APPEND);
	if (FFat.remove("/update.bin"))
	{
		Serial.println("- file deleted");
	}
	else
	{
		Serial.println("- delete failed");
	}
#endif

	pinMode(WATCH_DOG_PIN, OUTPUT);
	gu32reset_watch_dog_timer_count = 0;

	File file = FFat.open("/update.bin", FILE_APPEND);
	// if (finishedHeader && contentLength == knownFileSize) {
	if (finishedHeader)
	{

		Serial.println(F("Reading response data"));
		ota_clientReadStartTime = millis();

		OTA_4G_printPercent(readLength, contentLength);
		while (readLength < contentLength && ota_client.connected() &&
			millis() - ota_clientReadStartTime < ota_clientReadTimeout)
		{
			ota_clientReadStartTime = millis();
			while (ota_client.available())
			{
				uint8_t c = ota_client.read();
				// char c = ota_client.read();
				// Serial.print(c);
				if (!file.write(c))
				{
					Serial.println("not Appending file");
				}

				// Serial.print(reinterpret_cast<char>c);  // Uncomment this to show
				// data
				crc.update(c);
				readLength++;
				if (readLength % (contentLength / 100) == 0)
				{
					OTA_4G_printPercent(readLength, contentLength);
					Serial.print("  ");
					Serial.printf("[OTA] ESP32 heap size  %d \r\n", ESP.getHeapSize());
					Serial.println("  ");
				}

				gu32reset_watch_dog_timer_count++;
				if (gu32reset_watch_dog_timer_count > WATCH_DOG_RESET_COUNT + WATCH_DOG_RESET_INTERVAL)
				{
					gu32reset_watch_dog_timer_count = 0;
					// ptr_watch_dog->evse_watch_dog_off();
					digitalWrite(WATCH_DOG_PIN, LOW);
				}
				else if (gu32reset_watch_dog_timer_count > WATCH_DOG_RESET_COUNT)
				{
					// gu32reset_watch_dog_timer_count=0;
					// ptr_watch_dog->evse_watch_dog_on();
					digitalWrite(WATCH_DOG_PIN, HIGH);
				}
			}
			Serial.println("ota_client disconnected....");
		}
		OTA_4G_printPercent(readLength, contentLength);
	}

	timeElapsed = millis() - timeElapsed;
	Serial.println();
	file.close();

	// Shutdown

	ota_client.stop();
	Serial.println(F("Server disconnected"));

#if TINY_GSM_USE_WIFI
	ota_modem.networkDisconnect();
	Serial.println(F("WiFi disconnected"));
#endif
#if TINY_GSM_USE_GPRS
	ota_modem.gprsDisconnect();
	Serial.println(F("GPRS disconnected"));
#endif

	float duration = float(timeElapsed) / 1000;

	Serial.println();
	Serial.print("Content-Length: ");
	Serial.println(contentLength);
	Serial.print("Actually read:  ");
	Serial.println(readLength);
	Serial.print("Calc. CRC32:    0x");
	Serial.println(crc.finalize(), HEX);
	Serial.print("Known CRC32:    0x");
	Serial.println(knownCRC32, HEX);
	Serial.print("Duration:       ");
	Serial.print(duration);
	Serial.println("s");
	// Do nothing forevermore
	// pu();
	// OTA_4G_pu(FFat, "/update.bin");
	OTA_4G_pu("/update.bin");
}

/////////////////////SETUP 4G////////////////////////////////////////////

#define GSM_RXD2 16
#define GSM_TXD2 17

#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 115200

void OTA_4G_setup4G(void)
{
	SerialAT.begin(115200, SERIAL_8N1, GSM_RXD2, GSM_TXD2);
	Serial.println(F("[CustomSIM7672] Starting 4G Setup"));
	TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
	SerialAT.println("AT+CMGD=1,4");
	Serial.println("AT+CMGD=1,4");
	OTA_4G_waitForResp(20);
	Serial.println(F("[CustomSIM7672] ota_modem restart..."));
	ota_modem.restart();
	delay(200);
	Serial.println(F("[CustomSIM7672] Initializing ota_modem..."));

	String ota_modemInfo = ota_modem.getModemInfo();
	Serial.print("[CustomSIM7672] ota_modem Info: ");
	Serial.println(ota_modemInfo);
	ota_modem.gprsConnect(APN.c_str(), gprsUser, gprsPass);
	Serial.println(F("[CustomSIM7672] Waiting for network..."));
	if (!ota_modem.waitForNetwork())
	{
		Serial.println(F("[CustomSIM7672] fail"));
		// delay(200);
#if FREE_RTOS_THREAD
		vTaskDelay(200 / portTICK_PERIOD_MS);
#else
		delay(200);
#endif
		return;
	}
	Serial.println("[CustomSIM7672] success");
	if (ota_modem.isNetworkConnected())
	{
		Serial.println(F("[CustomSIM7672] Network connected"));
	}
	Serial.print("[CustomSIM7672] Connecting to APN :  ");
	Serial.print(APN.c_str());
	if (!ota_modem.gprsConnect(APN.c_str(), gprsUser, gprsPass))
	{
		Serial.println(F("[CustomSIM7672] fail"));
		// delay(200);
#if FREE_RTOS_THREAD
		vTaskDelay(200 / portTICK_PERIOD_MS);
#else
		delay(200);
#endif
		// gsm_net = false;
		return;
	}
	Serial.println(F("[CustomSIM7672] success"));
	if (ota_modem.isGprsConnected())
	{
		Serial.println(F("[CustomSIM7672] 4G connected"));
		// gsm_net = true;
	}
	int csq = ota_modem.getSignalQuality();
	Serial.println("Signal quality: " + String(csq));
	// delay(1);
#if FREE_RTOS_THREAD
	vTaskDelay(1 / portTICK_PERIOD_MS);
#else
	delay(1);
#endif
}
/////////////////////SETUP 4G////////////////////////////////////////////

/*************************fatfs***********/

/*waitForResp*/

uint8_t OTA_4G_waitForResp(uint8_t timeout)
{
	const char* crtResp = "+HTTPACTION: 1,200"; // Success
	const char* Resp400 = "+HTTPACTION: 1,4";	// Conf Error
	const char* Resp500 = "+HTTPACTION: 1,5";	// Internal Server Error
	const char* Resp700 = "+HTTPACTION: 1,7";	// Internal Server Error
	const char* errResp = "+HTTP_NONET_EVENT";
	const char* errResp1 = "ERROR";
	const char* okResp = "OK";
	const char* DOWNResp = "DOWNLOAD";
	const char* clkResp = "+CCLK: ";	  // Success
	const char* pdndeactResp = "+CGEV: "; // pdn deact
	const char* mqttconnectResp = "+CMQTTCONNECT: 0,0";
	const char* mqttcheckconnResp = "+CMQTTCONNECT: 0,\"tcp://34.134.133.145:1883\",60,1";
	const char* mqttdisconnResp = "+CMQTTDISC: 0,0";

	uint8_t len1 = strlen(crtResp);
	uint8_t len2 = strlen(Resp400);
	uint8_t len3 = strlen(errResp);
	uint8_t len4 = strlen(Resp700);
	uint8_t len5 = strlen(errResp1);
	uint8_t len6 = strlen(okResp);
	uint8_t len7 = strlen(DOWNResp);
	uint8_t len8 = strlen(clkResp);
	uint8_t len11 = strlen(pdndeactResp);
	uint8_t len12 = strlen(mqttconnectResp);
	uint8_t len13 = strlen(mqttcheckconnResp);
	uint8_t len14 = strlen(mqttdisconnResp);

	uint8_t sum1 = 0;
	uint8_t sum2 = 0;
	uint8_t sum3 = 0;
	uint8_t sum4 = 0;
	uint8_t sum5 = 0;
	uint8_t sum6 = 0;
	uint8_t sum7 = 0;
	uint8_t sum8 = 0;
	uint8_t sum11 = 0;
	uint8_t sum12 = 0;
	uint8_t sum13 = 0;
	uint8_t sum14 = 0;
	unsigned long timerStart, timerEnd;
	timerStart = millis();
	while (1)
	{
		timerEnd = millis();
		if (timerEnd - timerStart > 1000 * timeout)
		{
			// gsm_net = false;
			return -1; // Timed out
		}
		if (SerialAT.available())
		{
			char c = SerialAT.read();
			Serial.print(c);
			sum1 = (c == crtResp[sum1]) ? sum1 + 1 : 0;
			sum2 = (c == Resp400[sum2]) ? sum2 + 1 : 0;
			sum3 = (c == errResp[sum3]) ? sum3 + 1 : 0;
			sum4 = (c == Resp700[sum4]) ? sum4 + 1 : 0;
			sum5 = (c == errResp1[sum5]) ? sum5 + 1 : 0;
			sum6 = (c == okResp[sum6]) ? sum6 + 1 : 0;
			sum7 = (c == DOWNResp[sum7]) ? sum7 + 1 : 0;
			sum8 = (c == clkResp[sum8]) ? sum8 + 1 : 0;
			sum11 = (c == pdndeactResp[sum11]) ? sum11 + 1 : 0;
			sum12 = (c == mqttconnectResp[sum12]) ? sum12 + 1 : 0;
			sum13 = (c == mqttcheckconnResp[sum13]) ? sum13 + 1 : 0;
			sum14 = (c == mqttdisconnResp[sum14]) ? sum14 + 1 : 0;

			if (sum1 == len1)
			{
				// gsm_net = true;
				Serial.println(F("Success!"));
				return 1;
			}
			else if (sum2 == len2)
			{
				// gsm_net = true;
				Serial.println("400 Error!");
				return 0;
			}
			else if (sum3 == len3)
			{
				Serial.println(F("NONET Error!"));
				// gsm_net = false;
				return 0;
			}
			else if (sum4 == len4)
			{
				Serial.println(F("700 No internet!"));
				// gsm_net = false;
				return 0;
			}
			else if (sum5 == len5)
			{
				return 0;
			}
			else if (sum6 == len6)
			{
				Serial.println(F("AT_OK"));
				return 0;
			}
			else if (sum7 == len7)
			{
				return 0;
			}
			else if (sum8 == len8)
			{
				Serial.println(F("OK"));
#if 0
				Serial.print(F("Timestamp : "));
				//Serial.println(timestamp);
				for (int i = 0; i < strlen(timestamp); i++)
				{
					Serial.print(i);
					Serial.print(":");
					Serial.println(timestamp[i]);
				}
				//timeEpoch();
#endif
				return 0;
			}
			else if (sum11 == len11)
			{
				/*
					A7600 Series_ AT Command Manual

					+CGEV: ME PDN DEACT 1
					 The mobile termination has deactivated a context.
					 The context represents a PDN connection in LTE or a Primary PDP context in GSM/UMTS.
					 The <cid> for this context is provided to the TE.
					 The format of the parameter <cid> NOT E is found in command +CGDCONT.
				*/
				Serial.println(F("PDN DEACTED ...!"));
				// gsm_net = false;
				return 0;
			}
			else if (sum12 == len12)
			{
				Serial.println(F("Device connected to server using mqtt"));
				return 0;
			}
			else if (sum13 == len13)
			{
				Serial.println(F("Already connected"));
				return 0;
			}
			else if (sum14 == len14)
			{
				Serial.println(F("mqtt disconnected"));
				return 0;
			}
		}
	}
	while (SerialAT.available())
	{
		SerialAT.read();
	}
	return 0;
}
/*waitForResp*/

/*print percentage */

void OTA_4G_printPercent(uint32_t readLength, uint32_t contentLength)
{
	// If we know the total length
	if (contentLength != (uint32_t)-1)
	{
		Serial.print("\r ");
		Serial.print((100.0 * readLength) / contentLength);
		Serial.print('%');
	}
	else
	{
		Serial.println(readLength);
	}
}
/*print percentage */

void OTA_4G_pu(const char* path)
{
	// FS fs;
	Serial.println(F("***Starting OTA update***"));
	File updateBin = FFat.open(path);

	if (updateBin)
	{
		if (updateBin.isDirectory())
		{
			Serial.println("Directory error");
			updateBin.close();
			return;
		}

		size_t updateSize = updateBin.size();

		if (Update.begin(updateSize))
		{
			size_t written = Update.writeStream(updateBin);
			if (written == updateSize)
			{
				Serial.println("Writes : " + String(written) + " successfully");
			}
			else
			{
				Serial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
			}
			if (Update.end())
			{
				Serial.println("OTA finished!");
				if (Update.isFinished())
				{
					Serial.println("Restart ESP device!");
					// esp_deep_sleep(1000 * 1000);

					/* Watch dog timer is added */
					gu32reset_watch_dog_timer_count = 0;
					gu32reset_watch_dog_timer_count++;
					digitalWrite(WATCH_DOG_PIN, LOW);
					while (gu32reset_watch_dog_timer_count <= WATCH_DOG_RESET_COUNT)
					{
						gu32reset_watch_dog_timer_count++;
					}
					digitalWrite(WATCH_DOG_PIN, HIGH);

					evse_preferences_ble.begin("fota_url", false);
					evse_preferences_ble.putString("fota_uri", "");
					evse_preferences_ble.putUInt("fota_retries", 0);
					evse_preferences_ble.putString("fota_date", "");
					evse_preferences_ble.putBool("fota_avial", false);
					evse_preferences_ble.end();

					ESP.restart();
				}
				else
				{
					Serial.println("OTA not finished yet");
				}
			}
			else
			{
				Serial.println("Error occured #: " + String(Update.getError()));
				// esp_deep_sleep(1000 * 1000);

				/* Watch dog timer is added */
				gu32reset_watch_dog_timer_count = 0;
				gu32reset_watch_dog_timer_count++;
				digitalWrite(WATCH_DOG_PIN, LOW);
				while (gu32reset_watch_dog_timer_count <= WATCH_DOG_RESET_COUNT)
				{
					gu32reset_watch_dog_timer_count++;
				}
				digitalWrite(WATCH_DOG_PIN, HIGH);

				ESP.restart();
			}
		}
		else
		{
			Serial.println("Cannot begin update");
		}
	}
	updateBin.close();
}

#if EVSE_WEBSOCKET_PING_PONG_ENABLE
// short int counterPing = 0;
short int counterPing = 0;
void cloudConnectivityLed_Loop()
{
	if (gsm_connect == true)
	{
		if (counterPing++ >= 3 && !flag_ping_sent)
		{
			gu8_online_flag = 1;
			String p = "rockybhai";
			sendPingGsmStr(p);
			// sendFrame(WSop_ping,"HB",size_t(2),true,true);
			flag_ping_sent = true;
			counterPing = 0;
			// check for pong inside gsmOnEvent
		}
	}
}

#endif