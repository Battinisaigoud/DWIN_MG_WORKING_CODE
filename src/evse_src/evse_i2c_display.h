#ifndef _EVSE_I2C_DISPLAY_H_
#define _EVSE_I2C_DISPLAY_H_

#include "evse_config.h"
#include "../evse_lib/src/LCD_I2C/LCD_I2C.h"
// LCD_I2C lcd(0x27, 20, 4);

void evse_lcd_init();
void evse_lcd_boot_display();
void evse_charging_page();
void evse_avaliable_page();
void evse_bill_page();
void evse_internet_page();
void evse_lcd_rfid_tap();
void evse_lcd_meter_val();

void lcd_voltageA(double voltA);
void lcd_voltageB(double voltB);
void lcd_voltageC(double voltC);
void lcd_currentA(double ampA);
void lcd_currentB(double ampB);
void lcd_currentC(double ampC);

void evse_lcd_connectivity_wifi();

void evse_lcd_connectivity_4G();
void evse_lcd_connecting_gsm();
void evse_lcd_state_offline();
void evse_lcd_state_available();

void evse_lcd_state_preparing();

void evse_lcd_state_suspendedev();

void evse_lcd_state_authontication();

void evse_lcd_state_Unavailable();

void evse_lcd_fault_over_voltage();

void evse_lcd_fault_under_voltage();

void evse_lcd_fault_over_current();
void evse_lcd_fault_over_temp();

void evse_lcd_fault_gfci();

void evse_lcd_fault_earth_dis();

void evse_lcd_fault_emergency();

void evse_lcd_state_update();

void evse_lcd_power_loss();

void evse_lcd_state_finished();
void evse_lcd_state_authentication();
void evse_lcd_state_authentication_success();
void evse_lcd_state_authentication_denied();
void evse_lcd_charging(double ampA, double energy);
void evse_lcd_charging_state_with_fault(double energy);

void evse_lcd_stop_charging(double ampA, double energy, uint32_t time, char *buffer);
void evse_lcd_offline();
void evse_lcd_initializing();
void evse_lcd_charger_reserved();
void evse_lcd_unplug_ev();

void evse_lcd_connecting();
void evse_lcd_connection_status();

void evse_volt_amps_update();

void update_time(uint32_t time);
#endif