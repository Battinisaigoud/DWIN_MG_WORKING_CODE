
#ifndef _EVSE_DWIN_H_
#define _EVSE_DWIN_H_

#include <Arduino.h>
#include <SoftwareSerial.h>

#define DWIN_TESTING (0)

#define EVSE_SET_ZERO (0x00)
#define EVSE_SET_ONE (0x01)
#define EVSE_SET_TWO (0x02)
#define EVSE_SET_THREE (0x03)
#define EVSE_SET_FOUR (0x04)
#define EVSE_SET_FIVE (0x05)
#define EVSE_SET_SIX (0x06)
#define EVSE_SET_SEVEN (0x07)
#define EVSE_SET_EIGHT (0x08)
#define EVSE_SET_NINE (0x09)
#define EVSE_SET_TEN (0x0A)
#define EVSE_SET_ELEVEN (0x0B)
#define EVSE_SET_TWELVE (0x0C)
#define EVSE_SET_THIRTEEN (0x0D)
#define EVSE_SET_FOURTEEN (0x0E)
#define EVSE_SET_FIFTEEN (0x0F)
#define EVSE_SET_SIXTEEN (0x10)
#define EVSE_SET_SEVENTEEN (0x11)
#define EVSE_SET_EIGHTEEN (0x12)
#define EVSE_SET_NINETEEN (0X13)
#define EVSE_SET_TWENTYONE (0x15)

#define EVSE_SET_TWENTY (0x14)
#define EVSE_SET_TWENTYTWO (0x16)
#define EVSE_SET_TWENTYFOUR (0x18)
#define EVSE_SET_TWENTYSIX (0x1A)
#define EVSE_SET_TWENTYEIGHT (0x1C)
#define EVSE_SET_TWENTYNINE (0x1D)
#define EVSE_SET_THIRTYONE (0x1F)  
#define EVSE_SET_THIRTYTWO (0x20)
#define EVSE_SET_THIRTYFOUR (0x22)
#define EVSE_SET_THIRTYSIX (0x24)
/**************************************************************************************8*/

#define EVSE_SET_PAGE_ZERO EVSE_SET_ZERO
#define EVSE_SET_PAGE_ONE EVSE_SET_ONE
#define EVSE_SET_PAGE_TWO EVSE_SET_TWO
#define EVSE_SET_PAGE_THREE EVSE_SET_THREE
#define EVSE_SET_PAGE_FOUR EVSE_SET_FOUR

#define EVSE_SET_PACKET_ONE EVSE_SET_ONE
#define EVSE_SET_PACKET_TWO EVSE_SET_TWO
#define EVSE_SET_PACKET_THREE EVSE_SET_THREE
#define EVSE_SET_PACKET_FOUR EVSE_SET_FOUR

/****************************4G display ****************************************/
#define EVSE_4G_CLEAR_LOGO EVSE_SET_ZERO
#define EVSE_4G_AVAILABLE_LOGO EVSE_SET_ONE
#define EVSE_4G_UNAVAILABLE_LOGO EVSE_SET_TWO

#define EVSE_4G_CLEAR_LOGO_ADDR_MSB EVSE_SET_SIXTEEN
#define EVSE_4G_AVAILABLE_LOGO_ADDR_LSB EVSE_SET_THREE

/*****************************WIFI**********************************************/
#define EVSE_WIFI_CLEAR_LOGO EVSE_SET_ZERO
#define EVSE_WIFI_AVAILABLE_LOGO EVSE_SET_ONE
#define EVSE_WIFI_UNAVAILABLE_LOGO EVSE_SET_TWO

#define EVSE_WIFI_CLEAR_LOGO_ADDR_MSB EVSE_SET_SIXTEEN
#define EVSE_WIFI_AVAILABLE_LOGO_ADDR_LSB EVSE_SET_SIX

/*******************************ocpp status************************************************/

#define EVSE_OCPP_STATUS_MSB EVSE_SET_SIXTEEN
#define EVSE_OCPP_STATUS_LSB EVSE_SET_SIXTEEN
#define EVSE_AVAILABLE_STATUS_LSB EVSE_SET_SIXTEEN
#define EVSE_PREP_UNAV_RES_STATUS_LSB EVSE_SET_TWENTY
#define EVSE_CHARGING_STATUS_LSB EVSE_SET_TWENTYTWO
#define EVSE_FAULTED_STATUS_LSB EVSE_SET_EIGHTEEN
#define EVSE_FAULT_TYPE_STATUS_LSB EVSE_SET_SIXTEEN

#define EVSE_AVAILABLE_STATUS EVSE_SET_ZERO
#define EVSE_PREPARING_STATUS EVSE_SET_ONE
#define EVSE_CHARGING_STATUS EVSE_SET_TWO
#define EVSE_SUSPENDEDEVSE_STATUS EVSE_SET_THREE
#define EVSE_SUSPENDEDEV_STATUS EVSE_SET_FOUR
#define EVSE_FINISHING_STATUS EVSE_SET_FIVE
#define EVSE_RESERVED_STATUS EVSE_SET_SIX
#define EVSE_UNAVAILABLE_STATUS EVSE_SET_SEVEN
#define EVSE_FAULTED_STATUS EVSE_SET_EIGHT

#define EVSE_ERROR_CLEAR EVSE_SET_ZERO
#define EVSE_EARTH_DISCONNECT EVSE_SET_ONE
#define EVSE_EMERGANCY_STOP EVSE_SET_TWO
#define EVSE_POWER_LOSS EVSE_SET_THREE
#define EVSE_OTHER_ERROR EVSE_SET_FOUR
#define EVSE_OVER_VOLTAGE EVSE_SET_FIVE
#define EVSE_UNDER_VOLTAGE EVSE_SET_SIX
#define EVSE_OVER_CURRENT EVSE_SET_SEVEN
#define EVSE_OVER_TEMP EVSE_SET_EIGHT

#define EVSE_FAULT_MSB EVSE_SET_SIXTEEN
#define EVSE_FAULT_LSB EVSE_SET_EIGHTEEN

#define EVSE_CLOUD_NULL EVSE_SET_ZERO
#define EVSE_CLOUD_ONLINE EVSE_SET_ONE
#define EVSE_CLOUD_OFFLINE EVSE_SET_TWO

#define EVSE_CLOUD_MSB EVSE_SET_SIXTEEN
#define EVSE_CLOUD_LSB EVSE_SET_FOURTEEN

#define EVSE_VOLTAGE_MSB EVSE_SET_SIXTEEN
#define EVSE_VOLTAGE_LSB EVSE_SET_ONE
#define EVSE_CURRENT_MSB EVSE_SET_SIXTEEN
#define EVSE_CURRENT_LSB EVSE_SET_EIGHTEEN

#define EVSE_ENERGY_MSB EVSE_SET_SIXTEEN
#define EVSE_ENERGY_LSB EVSE_SET_TWENTYFOUR

#define EVSE_POWER_LSB EVSE_SET_TWENTY

#define EVSE_TIME_MSB EVSE_SET_SIXTEEN
#define EVSE_TIME_HOURS_LSB EVSE_SET_EIGHTEEN
#define EVSE_TIME_MINUTES_LSB EVSE_SET_TWENTYTWO
#define EVSE_TIME_SECONDS_LSB EVSE_SET_TWENTYSIX

#define EVSE_USER_INFO_MSB EVSE_SET_SIXTEEN
#define EVSE_USER_INFO_LSB EVSE_SET_TWENTYFOUR
/********************************function protypes************************************************/
void evse_dwin_setup(void);
// void evse_send_cmd(bool checkResult);
// void evse_show_cmd();
void evse_check_return();

void evse_send_cmd(bool checkResult, uint8_t pkt_type);
void evse_dump_cmd(uint8_t pkt_type);

void evse_4g_avaliable(void);
void evse_4g_unavaliable(void);
void evse_4g_clear_logo(void);

void evse_page_change(uint8_t page_num);

void evse_page_change_zero(void);

void evse_page_change_one(void);

void evse_page_change_two(void);

void evse_page_change_three(void);

void evse_page_change_four(void);

void evse_wifi_available(void);
void evse_wifi_unavailable(void);

void evse_wifi_clear_logo(void);

void evse_available_status(uint8_t status);

void evse_prep_res_unav_status(uint8_t status);

void evse_charging_status(uint8_t status);

void evse_suspendedev_status(void);

void evse_suspendedevse_status(void);

void evse_finishing_status(void);

void evse_unavailable_status(void);
void evse_faulted_status(uint8_t status);
void evse_fault_type_status(uint8_t status);
void evse_earth_disconnect_status(void);

void evse_emergency_stop_status(void);
void evse_power_loss_status(void);
void evse_other_error_status(void);
void evse_clear_error_status(void);

void evse_over_volatge_status(void);

void evse_under_voltage_status(void);

void evse_over_temp_status(void);

void evse_over_current_status(void);

void evse_cloud_null_status(void);

void evse_cloud_online_status(void);

void evse_cloud_offline_status(void);

void evse_voltage_value(uint16_t voltage);
void evse_current_value(uint16_t current);

void evse_energy_value(float energy);
void evse_power_value(float energy);
void evse_hours_value(uint8_t hours);

void evse_minutes_value(uint8_t minutes);

void evse_seconds_value(uint8_t seconds);

void evse_update_fault(uint8_t fault);

void evse_update_status(uint8_t status);

void evse_update_wifi_logo(uint8_t logo);

void evse_update_4G_logo(uint8_t logo);

void connection_status_update(void);

void evse_update_time(uint32_t time);

void evse_update_user_info(uint8_t user_info);

void evse_update_charger_info(uint8_t charger_info);

void evse_dwin_state_update(void);

void evse_connecter_icon(uint8_t logo);

void convertStrToHex(const String &str, String &strH);

void evse_charger_initialization (void);

#endif
