#ifndef _EVRE_CONFIG_H_
#define _EVRE_CONFIG_H_
#include <Arduino.h>

#include <string.h>

#define EVSE_7S_RT_OST (0) // 7s online and offline session resume
#define EVSE_7_4_ (1)      // only online(you can configure if you want offine using BLE App)
#define EVSE_3S_CP_SR (0)  // 3s with type2 session resume(online and offline)

#define EVSE_SESSION_RESUME_ENEABLED (1)
#define EVSE_SESSION_RESUME_DISABLED (1)
#define EVSE_LCD_ENABLE (0)
#define EVSE_DWIN_ENABLE (1)

#if EVSE_7S_RT_OST
#define EVSE_CHARGE_POINT_FIRMWARE_OTA_VERSION String("002")
#define DEVICE_ID String("evse_001")
#define EVSE_CHARGE_POINT_FIRMWARE_VERSION String("7S_RT_OST_V_0.0.2")
#define EVSE_CHARGE_POINT_VENDOR String("EVRE")
#define EVSE_CHARGE_POINT_MODEL String("7S_RT_OST")
#define EVSE_CHARGE_POINT_VENDOR_SIZE strlen("EVRE")
#define EVSE_CHARGE_POINT_MODEL_SIZE strlen("7.4KW")
#endif

#if EVSE_7_4_
#define EVSE_CHARGE_POINT_FIRMWARE_OTA_VERSION String("001")
#define DEVICE_ID String("evse_001")
#define EVSE_CHARGE_POINT_FIRMWARE_VERSION String("EVSE_7.4_3.3KW_V_0.0.1")
#define EVSE_CHARGE_POINT_VENDOR String("EVRE")
#define EVSE_CHARGE_POINT_MODEL String("7.4_3.3KW")
#define EVSE_CHARGE_POINT_VENDOR_SIZE strlen("EVRE")
#define EVSE_CHARGE_POINT_MODEL_SIZE strlen("7.4_3.3KW")
#endif

#if EVSE_3S_CP_SR
#define EVSE_CHARGE_POINT_FIRMWARE_OTA_VERSION String("105")
#define DEVICE_ID String("evse_001")
#define EVSE_CHARGE_POINT_FIRMWARE_VERSION String("3S_CP_SR_V_0.0.7")
#define EVSE_CHARGE_POINT_VENDOR String("EVRE")
#define EVSE_CHARGE_POINT_MODEL String("HALO_CP_3S")
#define EVSE_CHARGE_POINT_VENDOR_SIZE strlen("EVRE")
#define EVSE_CHARGE_POINT_MODEL_SIZE strlen("3.3KW")
#endif

#define EVSE_CHARGE_BOX_SERIAL_NUMBER String("123456")
#define EVSE_CHARGE_BOX_SERIAL_NUMBER_SIZE strlen("123456")

#define TASK1_CONTROLPOILT (1)
#define TASK2_MANAGEMENT (1)
#define TASK3_RFID_ENERGY (1)

#if EVSE_LCD_ENABLE || EVSE_DWIN_ENABLE
#define TASK4_RTC_LCD (1)
#endif

#define TASK5_OCPP (1)

#define EVSE_OFFLINE_FUNCTIONALITY_ENABLE (1)
#define EVSE_ONLINE_FUNCTIONALITY_ENABLE (1)

#define EVSE_CONTROL_PILOT_SM_WINDOW_SIZE (2)
#define EVSE_CONTROL_PILOT_SM_SKIP_COUNT (4) // changed from 4 to 6

#define EVSE_READING_VA (0)
#define EVSE_READING_VB (1)
#define EVSE_READING_VC (2)
#define EVSE_READING_IA (3)
#define EVSE_READING_IB (4)
#define EVSE_READING_IC (5)
#define EVSE_READING_TEMP (6)

#if 1
#define EVSE_CHANGE_STATE_ZERO (0)
#define EVSE_CHANGE_STATE_ONE (1)

#define EVSE_BOOT (1)
#define EVSE_READ_RFID (2)
#define EVSE_AUTHENTICATION (3)
#define EVSE_START_TXN (4)
#define EVSE_CHARGING (5)
#define EVSE_STOP_TXN (6)
#define EVSE_UNAUTHENTICATION (7)
#define EVSE_REBOOT (8)
#define EVSE_RESERVE_NOW (9)
#define EVSE_CANCEL_RESERVATION (10)
#define EVSE_LOCAL_AUTHENTICATION (11)
#endif

#define EVSE_FOTA_ENABLE (1)
#define EVSE_RFID_ENABLE (1)
#define EVSE_ENERGY_METER_ENABLE (1)
#define EVSE_RTC_ENABLE (1)

#define RFID_SCANNER_COUNT (2)
#define ENERGY_METER_COUNT (1)
#define RTC_READING_COUNT (3)
#define LCD_DISPLAY_COUNT (2)
#define I2C_RFID_COUNT (2)

#define TOTAL_NO_OF_RFID_TAGS (10)
#define RFID_SCANNER (0)
#define ENERGY_METER (1)

#define RTC_READING (0)
#define I2C_RFID_READ (2)
#define LCD_DISPLAY (1)

#define RFID_I2C (0)
#define RFID_SPI (1)

#define EVSE_CONNECTION_KEEP_ALIVE (0) // for server connection

#define EVSE_WEBSOCKET_PING_PONG_ENABLE (1)

#define EVSE_OVER_VOLTAGE_THRESHOLD (275)
#define EVSE_UNDER_VOLTAGE_LOW_THRESHOLD_1 (21)
#define EVSE_UNDER_VOLTAGE_LOW_THRESHOLD_2 (89)
#define EVSE_UNDER_VOLTAGE_HIGH_THRESHOLD_1 (20)
#define EVSE_UNDER_VOLTAGE_HIGH_THRESHOLD_2 (180) // 200 for 7s and 180 for 3s
#define EVSE_EARTH_DISCONNECT_THRESHOLD_1 (90)
#define EVSE_EARTH_DISCONNECT_THRESHOLD_2 (150)
#define EVSE_POWER_LOSS_THRESHOLD (20)

#if EVSE_3S_CP_SR
#define EVSE_OVER_CURRENT_THRESHOLD (16)
#endif

#if EVSE_7_4_ || EVSE_7S_RT_OST
#define EVSE_OVER_CURRENT_THRESHOLD (32)
#endif

#if EVSE_7_4_ || EVSE_7S_RT_OST
#define CHARGE_CURRENT_IN_AMPS (32)
#endif

#define CHARGE_CURRENT_IN_AMPS_3_3KW (16)

#if EVSE_3S_CP_SR
#define CHARGE_CURRENT_IN_AMPS (16)
#endif

#define EVSE_HIGH_TEMPERATURE_THRESHOLD (70)

#define CONNECT_WIFI_GSM_ETHERNET_DISABLED (0)
#define CONNECT_WIFI_ENABLED (1)
#define CONNECT_GSM_ENABLED (2)
#define CONNECT_ETHERNET_ENABLED (3)
#define CONNECT_WIFI_GSM_ENABLED (4)
#define CONNECT_WIFI_ETHERNET_ENABLED (5)
#define CONNECT_GSM_ETHERNET_ENABLED (6)
#define CONNECT_WIFI_GSM_ETHERNET_ENABLED (7)

#define EVSE_CMS_RESET_HARD (1)
#define EVSE_CMS_RESET_SOFT (2)

#define EVSE_CMS_CHANGE_AVAILABLITY_ZERO (0)
#define EVSE_CMS_CHANGE_AVAILABLITY_ONE (1)
#define EVSE_CMS_CHANGE_AVAILABLITY_TWO (2)

#define GT_CT (0)

#define GSM_ENABLED (1)

#define WIFI_ENABLED (1)

#define ETHERNET_ENABLED (0)

#define BLE_ENABLE (1)

#define CP_ACTIVE (0)

#define DWIN_ENABLED (0)

#define LCD_ENABLED (0)

#define EARTH_DISCONNECT (0)

#define THREE_PHASE_ENERGY (0)

#define SINGLE_PHASE_ENERGY (0)

#define RTC_ENEBLED (0)

#define RELAY1_ENABLED (0)

#define TEMP_SENSOR_ENABLED (0)

#define RFID_ENABLED (0)

#define ENERGY_10_KW (0)

#define ENERGY_14_kw (0) /*if we use 14kw charger then enable this */

#define THREE_PHASE (0) /* enable it, if we need 3 phase */

#define PREFERENCE_USED (1)

#define TIMEOUT_BLE (60000 * 5)

#define EVSE_OCPP_CONFIG_PREFERENCE (0)

#define EVSE_TRUE (1)
#define EVSE_FASLE (0)

#define EVSE_SUSPEND_EV_COUNT_TRESHOLD (60000 * 1) //  1000 milli seconds, 60 seconds, 1minutes

#define EVSE_SUS_COUNT (0)

#define EVSE_LOW_CURRENT (1)

#if EVSE_3S_CP_SR

#define EVSE_STATE_A_UPPER_THRESHOULD (4096)

#define EVSE_STATE_A_LOWER_THRESHOULD (3500)

#define EVSE_STATE_B_UPPER_THRESHOULD (3500)

#define EVSE_STATE_B_LOWER_THRESHOULD (2800)

#define EVSE_STATE_C_UPPER_THRESHOULD (720)

#define EVSE_STATE_C_LOWER_THRESHOULD (575)

#define EVSE_STATE_D_UPPER_THRESHOULD (575)

#define EVSE_STATE_D_LOWER_THRESHOULD (425)

#define EVSE_STATE_E_THRESHOULD (425)

#define EVSE_STATE_SUS_UPPER_THRESHOULD (840)

#define EVSE_STATE_SUS_LOWER_THRESHOULD (720)

#define EVSE_STATE_E2_UPPER_THRESHOULD (1400)

#define EVSE_STATE_E2_LOWER_THRESHOULD (1050)

#define EVSE_STATE_DIS_UPPER_THRESHOULD (1050)

#define EVSE_STATE_DIS_LOWER_THRESHOULD (840)
#endif

#if EVSE_7_4_ || EVSE_7S_RT_OST

#define EVSE_STATE_A_UPPER_THRESHOULD (4096)

#define EVSE_STATE_A_LOWER_THRESHOULD (3500)

#define EVSE_STATE_B_UPPER_THRESHOULD (3500)

#define EVSE_STATE_B_LOWER_THRESHOULD (2400) // previous 2800

#define EVSE_STATE_C_UPPER_THRESHOULD (1550)

#define EVSE_STATE_C_LOWER_THRESHOULD (1150)

#define EVSE_STATE_D_UPPER_THRESHOULD (1150)

#define EVSE_STATE_D_LOWER_THRESHOULD (850)

#define EVSE_STATE_E_THRESHOULD (850)

#define EVSE_STATE_SUS_UPPER_THRESHOULD (1799)

#define EVSE_STATE_SUS_LOWER_THRESHOULD (1550)

#define EVSE_STATE_E2_UPPER_THRESHOULD (2399) // previous 2800

#define EVSE_STATE_E2_LOWER_THRESHOULD (2100)

#define EVSE_STATE_DIS_UPPER_THRESHOULD (2099)

#define EVSE_STATE_DIS_LOWER_THRESHOULD (1800)

#endif

/* EVSE to OCPP message update state machine */
enum EvseDevStatuse
{

  flag_EVSE_is_Booted,
  flag_EVSE_Read_Id_Tag,
  falg_EVSE_Authentication,
  flag_EVSE_Start_Transaction,
  flag_EVSE_Request_Charge,
  flag_EVSE_Stop_Transaction,
  flag_EVSE_UnAutharized,
  flag_EVSE_Reboot_Request,
  flag_EVSE_Reserve_Now,
  flag_EVSE_Cancle_Reservation,
  flag_EVSE_Local_Authantication

};

enum EvseChargePointStatus
{
  EVSE_is_Booted,
  EVSE_Available,
  EVSE_Preparing,
  EVSE_Charging,
  EVSE_SuspendedEVSE,
  EVSE_SuspendedEV,
  EVSE_Finishing,
  EVSE_Reserved,
  EVSE_Unavailable,
  EVSE_Faulted,
};

enum evsetransactionstatus
{
  EVSE_NO_TRANSACTION,
  EVSE_START_TRANSACTION,
  EVSE_STOP_TRANSACTION,
};

extern enum evsetransactionstatus EVSE_transaction_status;

enum evseconnectivitystatus
{
  EVSE_NOCONNECTION,
  EVSE_CONNECTED,
  EVSE_DISCONNECTED,

};

extern enum evseconnectivitystatus EVSE_Connectivity_status;

enum evseconnectivity_offline_to_online
{
  EVSE_OFFLINE_TO_ONLINE_OCPP_MESSAGE_INITIATED,
  EVSE_OFFLINE_TO_ONLINE_OCPP_MESSAGE_SENT,
  EVSE_OFFLINE_TO_ONLINE_OCPP_MESSAGE_COMPLETED,
  EVSE_OFFLINE_TO_ONLINE_OCPP_MESSAGE_DEFAULT
};

extern enum evseconnectivity_offline_to_online evseconnectivity_offline_to_online_status;

enum EvseChargePointErrorCode
{
  EVSE_ConnectorLockFailure,
  EVSE_EVCommunicationError,
  EVSE_GroundFailure,
  EVSE_HighTemperature,
  EVSE_InternalError,
  EVSE_LocalListConflict,
  EVSE_NoError,
  EVSE_OtherError,
  EVSE_OverCurrentFailure,
  EVSE_OverVoltage,
  EVSE_PowerMeterFailure,
  EVSE_ReaderFailure,
  EVSE_ResetFailure,
  EVSE_UnderVoltage,
  EVSE_WeakSignal
};

enum EvseChargePointErrorCode_OtherError_t
{
  EVSE_Earth_Disconnect,
  EVSE_EmergencyStop,
  EVSE_PowerLoss,
  EVSE_NO_Other_Error,

};

extern enum EvseChargePointErrorCode_OtherError_t EvseChargePointErrorCode_OtherError;

/* status notification update state machine */
enum evse_status_notification_stat_t
{
  EVSE_STATUS_NOTIFICATION_INITIATED = 0,
  EVSE_STATUS_NOTIFICATION_SENT,
  EVSE_STATUS_NOTIFICATION_ACCEPTED,
  EVSE_STATUS_NOTIFICATION_REJECTED,
  EVSE_STATUS_NOTIFICATION_DEFAULT,
};
extern enum evse_status_notification_stat_t evse_status_notification_stat;

/* boot notification update state machine */
enum evse_boot_stat_t
{
  EVSE_BOOT_INITIATED = 0,
  EVSE_BOOT_SENT,
  EVSE_BOOT_ACCEPTED,
  EVSE_BOOT_REJECTED,
  EVSE_BOOT_DEFAULT,
};
extern enum evse_boot_stat_t evse_boot_state;

/* Authorize update state machine */
enum evse_authorize_stat_t
{
  EVSE_AUTHORIZE_INITIATED = 0,
  EVSE_AUTHORIZE_SENT,
  EVSE_AUTHORIZE_ACCEPTED,
  EVSE_AUTHORIZE_REJECTED,
  EVSE_AUTHORIZE_DEFAULT,
};
extern enum evse_authorize_stat_t evse_authorize_state;

/* Start Transaction update state machine */
enum evse_start_txn_stat_t
{
  EVSE_START_TXN_INITIATED = 0,
  EVSE_START_TXN_SENT,
  EVSE_START_TXN_ACCEPTED,
  EVSE_START_TXN_REJECTED,
  EVSE_START_TXN_DEFAULT,
};
extern enum evse_start_txn_stat_t evse_start_txn_state;

/* Stop Transaction update state machine */
enum evse_stop_txn_stat_t
{
  EVSE_STOP_TXN_INITIATED = 0,
  EVSE_STOP_TXN_SENT,
  EVSE_STOP_TXN_ACCEPTED,
  EVSE_STOP_TXN_REJECTED,
  EVSE_STOP_TXN_DEFAULT,
};
extern enum evse_stop_txn_stat_t evse_stop_txn_state;

enum evse_display_chargeing
{

  VOLTAGE_DISPLAY,
  CURRENT_DISPLAY,
  ENERGY_DISPLAY,
  POWER_DISPLAY,

  TIME_HOURS_DISPLAY,
  TIME_MINUTES_DISPLAY,
  TIME_SECONDS_DISPLAY,

};

/* States for EVSE state machine */
typedef enum
{
  STARTUP,
  GFCI_CHECK_START,
  GFCI_CHECK_EXPECTED,
  GFCI_PASSED,
  FAULT,
  STATE_A,   // 5
  STATE_B,   // 6
  STATE_C,   // 7
  STATE_D,   // 9
  STATE_E,   // 10
  STATE_SUS, // 11
  STATE_DIS  // 12
  // STATE_F
} EVSE_states_enum;

typedef enum
{
  GFCI_FAULT,
  GFCI_CHECK_FAULT,
  RELAY_FAULT,
  PILOT_FAULT,
  VOLTAGE_FAULT,
  GENERIC_FAULT
} fault_conditions;

typedef enum
{
  V_12,
  V_9,
  V_SUS,
  V_DIS,
  V_6,
  V_3,
  V_1,
  V_UNKNOWN,
  V_DEFAULT
} PILOT_readings_t;

extern EVSE_states_enum EVSE_state;

extern PILOT_readings_t PILOT_reading;

#endif