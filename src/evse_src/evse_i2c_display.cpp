
#include "evse_i2c_display.h"
#include "../evse_lib/src/LCD_I2C/LCD_I2C.h"
#include "../evse_lib/src/OCPP/ChargePointStatusService.h"

#include "evse_config.h"
#include <Arduino.h>

LCD_I2C lcd(0x27, 20, 4);

extern enum EvseChargePointErrorCode_OtherError_t EvseChargePointErrorCode_OtherError;
extern enum EvseChargePointErrorCode EvseChargePoint_ErrorCode;
extern enum evse_authorize_stat_t evse_authorize_state;

enum ChargePointStatus lenum_evse_ChargePointStatus = NOT_SET;
enum EvseChargePointErrorCode_OtherError_t dip_EvseChargePointErrorCode_OtherError = EVSE_NO_Other_Error;

extern enum EvseDevStatuse EvseDevStatus_connector_1;
extern enum EvseDevStatuse currentEvseDevStatus_connector_1;

extern enum evse_boot_stat_t evse_boot_state;
// LCD evse_lcd;

extern unsigned long evse_st_time;

extern unsigned long evse_stop_time;

extern uint8_t dis_connection_flag;

extern volatile double ipc_voltage_A;
extern volatile double ipc_current_A;

extern volatile float discurrEnergy_A;

extern volatile float LastPresentEnergy_A;

extern uint8_t gu8_online_flag;

int flag = 1;

unsigned long charge_start_time = 0;

unsigned long storing_time = 0;

extern bool wifi_connect;
extern bool gsm_connect;

String hrMinSec = "";

uint8_t current_intial = 0;
uint8_t pre_intial = -1;

uint8_t gu8_charging_started = 0;

uint8_t gu8_flag_i2c_intialised = 0;
uint8_t gu8_flag_i2c_intialised_avl = 0;

void evse_lcd_init()
{

  lcd.begin();
  // LCD_I2C lcd(0x27, 20, 4);

  lcd.backlight();
}

void evse_lcd_boot_display()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("***EVRE-new board***");
  lcd.setCursor(0, 1);
  lcd.print("#########################");
  lcd.setCursor(4, 2);
  lcd.print("**7.4 KW**");
  lcd.setCursor(4, 3);
  lcd.print("**TYPE 2***");
}

void evse_lcd_initializing()
{
  // if (current_intial != pre_intial)
  // {
  lcd.clear();
  evse_lcd_connection_status();

  lcd.setCursor(0, 1);
  lcd.print("CHARGER INITIALIZING");
  current_intial = pre_intial;
  // }
}
void evse_lcd_connecting()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CONNECTING TO ");
  lcd.setCursor(0, 1);
  lcd.print("Wi-Fi");
  flag = 1;
}
void evse_lcd_connecting_gsm()
{
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("CONNECTING TO GSM");
  // lcd.setCursor(0, 1);
  // lcd.print("GSM");
  flag = 1;
}

void evse_lcd_state_Unavailable()
{
  lcd.clear();

  lcd.setCursor(0, 2); // Or setting the cursor in the desired position.
  lcd.print("CHARGER UNAVAILABLE");
}

void evse_lcd_state_available()
{

  lcd.clear();
  evse_lcd_connection_status();

  lcd.setCursor(0, 1);
  lcd.print("CHARGER AVAILABLE");
  lcd.setCursor(0, 2);
  lcd.print("TAP RFID/SCAN QR");
  lcd.setCursor(0, 3);
  lcd.print("TO START");
}

void evse_lcd_state_preparing()
{
  lcd.clear();
  evse_lcd_connection_status();
  lcd.setCursor(0, 1);
  lcd.print("EV PLUGGED IN    ");
  lcd.setCursor(0, 2);
  lcd.print("TAP RFID/SCAN QR");
  lcd.setCursor(0, 3);
  lcd.print("TO START");
}
void evse_lcd_state_suspendedev()
{
  lcd.clear();
  evse_lcd_connection_status();
  lcd.setCursor(0, 1);
  // lcd.print("EV CHARGER SUSPENDED");
  lcd.print("WAITING FOR");
  lcd.setCursor(0, 2);
  lcd.print("EV RESPONSE");
  flag = 1;
}
void evse_lcd_rfid_tap()
{
  lcd.clear();
  evse_lcd_connection_status();
  lcd.setCursor(0, 1);
  lcd.print("RFID TAPPED");
  lcd.setCursor(0, 2);
  lcd.print("AUTHENTICATING");
}

void evse_lcd_state_authentication_success()
{
  lcd.clear();
  evse_lcd_connection_status();

  // lcd.clear();
  // lcd.print("PREPARING");
  lcd.setCursor(0, 1);
  lcd.print("AUTHENTICATION   ");
  lcd.setCursor(0, 2);
  lcd.print("SUCCESSFUL      ");
  lcd.setCursor(0, 3);
  lcd.print("             ");
}

void evse_lcd_charging(double ampA, double energy)
{

  // if (flag)
  // {

  //   lcd.clear();
  //   flag = 0;
  // }
  lcd.clear();

  lcd.setCursor(0, 1);
  lcd.print("CHARGING @");
  lcd.setCursor(11, 1); // Or setting the cursor in the desired position.
  lcd.print(String(ampA));
  lcd.setCursor(16, 1);
  lcd.print("kW");
  lcd.setCursor(0, 2);
  lcd.print("ENERGY:");
  lcd.setCursor(7, 2);
  lcd.print(String(energy));
  lcd.setCursor(12, 2);
  lcd.print("kWh");
  lcd.setCursor(0, 3);
  lcd.print("TIME  : ");
}

void evse_lcd_charging_state_with_fault(double energy)
{
  evse_lcd_connection_status();
  lcd.setCursor(0, 1);
  lcd.print("THANKS FOR CHARGING");
  lcd.setCursor(0, 2);
  lcd.print("ENERGY:");
  lcd.setCursor(7, 2);
  lcd.print(String(energy));
  lcd.setCursor(12, 2);
  lcd.print("kWh");
  lcd.setCursor(0, 3);
  lcd.print("TIME  : ");
  lcd.setCursor(8, 3);
  // unsigned long evse_stop_time = millis(); // Or setting the cursor in the desired position.
  unsigned long seconds = (evse_stop_time - evse_st_time) / 1000;
  int hr = seconds / 3600;                                                 // Number of seconds in an hour
  int mins = (seconds - hr * 3600) / 60;                                   // Remove the number of hours and calculate the minutes.
  int sec = seconds - hr * 3600 - mins * 60;                               // Remove the number of hours and minutes, leaving only seconds.
  String hrMinSec = (String(hr) + ":" + String(mins) + ":" + String(sec)); // Converts to HH:MM:SS string. This can be returned to the calling function.
  lcd.print(String(hrMinSec));
}

void evse_lcd_state_authentication()
{
  // lcd.clear();
  //  lcd.print("PREPARING");
  lcd.clear();
  evse_lcd_connection_status();
  lcd.setCursor(0, 1);
  lcd.print("RFID TAPPED     ");
  lcd.setCursor(0, 2);
  lcd.print("AUTHENTICATING  ");
  lcd.setCursor(0, 3);
  lcd.print("                ");
}

void evse_lcd_state_authentication_denied()
{
  lcd.clear();
  evse_lcd_connection_status();
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("AUTHENTICATION");
  lcd.setCursor(0, 2);
  lcd.print("DENIED");
}

void evse_lcd_stop_charging(double ampA, double energy, uint32_t time, char *buffer)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("THANKS FOR CHARGING");
  lcd.setCursor(0, 1);
  lcd.print("ENERGY:");
  lcd.setCursor(7, 1);
  lcd.print(String(energy));
  lcd.setCursor(12, 1);
  lcd.print("kWh");
  lcd.setCursor(0, 2);
  lcd.print("TIME: ");
  lcd.setCursor(6, 2);
  uint8_t hours = time / 3600;
  uint8_t minutes = (time % 3600) / 60;
  uint8_t seconds = time % 60;

  sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);
  lcd.print(String(buffer)); // Or setting the cursor in the desired position.
}

void update_time(uint32_t time, char *buffer)
{
  int hours = time / 3600;
  int minutes = (time % 3600) / 60;
  int seconds = time % 60;

  sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);
  lcd.setCursor(8, 3);
  // hrMinSec = (String(hours) + ":" + String(minutes) + ":" + String(seconds));
  lcd.print(String(buffer));
}

void evse_lcd_connectivity_wifi()
{

  lcd.setCursor(13, 0);
  lcd.print("  Wi-Fi");
}

void evse_lcd_connectivity_4G()
{

  lcd.setCursor(13, 0);
  lcd.print("    GSM");
}

void evse_lcd_offline()
{

  lcd.setCursor(13, 0);
  lcd.print("OFFLINE");
}

void evse_lcd_fault_over_voltage()
{
  lcd.clear();
  evse_lcd_connection_status();
  lcd.setCursor(0, 1);
  lcd.print("CHARGER FAULTED ");
  lcd.setCursor(0, 2);
  lcd.print("OVER VOLTAGE");
}

void evse_lcd_fault_under_voltage()
{
  lcd.clear();
  evse_lcd_connection_status();

  lcd.setCursor(0, 1);
  lcd.print("CHARGER FAULTED");
  lcd.setCursor(0, 2);
  lcd.print("UNDER VOLTAGE");
}

void evse_lcd_fault_over_current()
{

  lcd.clear();
  evse_lcd_connection_status();

  lcd.setCursor(0, 1);
  lcd.print("CHARGER FAULTED");
  lcd.setCursor(0, 2);
  lcd.print("OVER CURRENT");
}

void evse_lcd_fault_over_temp()
{
  lcd.clear();
  evse_lcd_connection_status();
  lcd.setCursor(0, 1);
  lcd.print("CHARGER FAULTED");
  lcd.setCursor(0, 2);
  lcd.print("OVER TEMPERATURE");
}
// lcd.print("A: UNDER TEMPERATURE");

void evse_lcd_fault_gfci()
{
  lcd.clear();
  evse_lcd_connection_status();
  lcd.setCursor(0, 1);
  lcd.print("CHARGER FAULTED");
  lcd.setCursor(0, 2);
  lcd.print("GFCI");
}

void evse_lcd_fault_earth_dis()
{
  lcd.clear();
  evse_lcd_connection_status();
  lcd.setCursor(0, 1);
  lcd.print("CHARGER FAULTED");
  lcd.setCursor(0, 2);
  lcd.print("EARTH DISCONNECT");
}

void evse_lcd_fault_emergency()

{
  lcd.clear();
  evse_lcd_connection_status();
  lcd.setCursor(0, 1);
  lcd.print("CHARGER FAULTED");
  lcd.setCursor(0, 2);
  lcd.print("EMERGENCY      ");
  lcd.setCursor(0, 3);
  lcd.print("             ");
}

void evse_lcd_power_loss()
{
  lcd.clear();
  evse_lcd_connection_status();
  lcd.setCursor(0, 1);
  lcd.print("CHARGER FAULTED");
  lcd.setCursor(0, 2);
  lcd.print("POWER LOSS");
}

void evse_lcd_charger_reserved()
{
  lcd.clear();
  evse_lcd_connection_status();
  lcd.setCursor(0, 1);
  lcd.print("CHARGER RESERVED");
}

void evse_lcd_unplug_ev()
{

  lcd.clear();
  evse_lcd_connection_status();
  lcd.setCursor(0, 1);
  lcd.print("UNPLUG EV  ");
  flag = 1;
  LastPresentEnergy_A = (discurrEnergy_A) * 1000 + LastPresentEnergy_A;
  hrMinSec = "";
}

void evse_lcd_connection_status()
{

  if (gu8_online_flag == 1)
  {
    if (wifi_connect == 1)
    {
      evse_lcd_connectivity_wifi();
    }
    else if (gsm_connect == 1)
    {
      evse_lcd_connectivity_4G();
    }
  }
  else
  {
    evse_lcd_offline();
  }
}

void evse_volt_amps_update()
{
  lcd.clear();
  evse_lcd_connection_status();

  lcd.setCursor(0, 1);
  lcd.print("CHARGING");
  lcd.setCursor(0, 2);
  lcd.print("Voltage:");
  lcd.print(ipc_voltage_A, 1);
  lcd.setCursor(0, 3);
  lcd.print("current:");
  lcd.print(ipc_current_A, 1);
}

void evse_lcd_state_update()
{
  // static ChargePointStatus lenum_evse_ChargePointStatus = NOT_SET;
  // static enum EvseChargePointErrorCode_OtherError_t dip_EvseChargePointErrorCode_OtherError = EVSE_NO_Other_Error;
  if (EvseDevStatus_connector_1 != currentEvseDevStatus_connector_1)
  {
    switch (EvseDevStatus_connector_1)
    {
    case falg_EVSE_Authentication:
    {
      switch (evse_authorize_state)
      {
      case EVSE_AUTHORIZE_INITIATED:
      {
        // evse_lcd_state_authentication();
        vTaskDelay(100 / portTICK_PERIOD_MS);
      }
      break;
      case EVSE_AUTHORIZE_SENT:
      {
      }
      break;
      case EVSE_AUTHORIZE_ACCEPTED:
      {
        // evse_lcd_state_authentication_success();
        vTaskDelay(100 / portTICK_PERIOD_MS);
      }
      break;
      case EVSE_AUTHORIZE_REJECTED:
      {
        evse_lcd_state_authentication_denied();
        vTaskDelay(100 / portTICK_PERIOD_MS);
      }
      break;
      }
    }
    default:
      break;
    }
    currentEvseDevStatus_connector_1 = EvseDevStatus_connector_1;
  }
  if ((gu8_flag_i2c_intialised == 1) && (gu8_flag_i2c_intialised_avl == 1))
  {
    evse_lcd_init();
    Serial.println("I2C intialised");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("POWER COMES BACK");
    gu8_flag_i2c_intialised = 0;
    gu8_flag_i2c_intialised_avl = 0;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  evse_lcd_connection_status();

  if (lenum_evse_ChargePointStatus != evse_ChargePointStatus)
  {

    switch (evse_ChargePointStatus)
    {

    case Available:

    {
      if (gu8_charging_started == 1)
      {
        gu8_charging_started = 0;
        float evse_kw_finishing_dis = ((ipc_current_A * ipc_voltage_A) / 1000);
        // evse_lcd_remote_stop(evse_kw_finishing, discurrEnergy_A);
        char timeBuffer_val_dis[10];
        memset(timeBuffer_val_dis, '\0', sizeof(timeBuffer_val_dis));
        evse_lcd_stop_charging(evse_kw_finishing_dis, discurrEnergy_A, storing_time, timeBuffer_val_dis);
        charge_start_time = 0;
        vTaskDelay(5000 / portTICK_PERIOD_MS);
      }
      gu8_flag_i2c_intialised_avl = 1;
      evse_lcd_state_available();
      charge_start_time = 0;
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    break;
    case Preparing:
    {
      charge_start_time = 0;
      evse_lcd_state_preparing();
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    break;
    case Charging:
    {
    }
    break;

    case SuspendedEVSE:
    {
    }
    break;

    case SuspendedEV:
    {
      evse_lcd_state_suspendedev();
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    break;
    case Finishing: // not supported by this client
    {
      float evse_kw_finishing = ((ipc_current_A * ipc_voltage_A) / 1000);
      // evse_lcd_remote_stop(evse_kw_finishing, discurrEnergy_A);
      char timeBuffer_val[10];
      memset(timeBuffer_val, '\0', sizeof(timeBuffer_val));
      evse_lcd_stop_charging(evse_kw_finishing, discurrEnergy_A, storing_time, timeBuffer_val);
      charge_start_time = 0;
      vTaskDelay(5000 / portTICK_PERIOD_MS);
      evse_lcd_unplug_ev();
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    break;
    case Reserved: // Implemented reserve now
    {
      evse_lcd_charger_reserved();
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    break;
    case Unavailable: // Implemented Change Availability
    {

      evse_lcd_state_Unavailable();
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    break;
    case Faulted: // Implemented Faulted.
    {
      // if (dip_EvseChargePointErrorCode_OtherError != EvseChargePointErrorCode_OtherError)
      // {
      charge_start_time = 0;
      switch (EvseChargePoint_ErrorCode)
      {
      case EVSE_ConnectorLockFailure:
        break;

      case EVSE_EVCommunicationError:
        break;

      case EVSE_GroundFailure:
        evse_lcd_fault_gfci();
        break;

      case EVSE_HighTemperature:
        evse_lcd_fault_over_temp();

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
        {
          gu8_flag_i2c_intialised = 1;
          evse_lcd_fault_earth_dis();
          vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        break;

        case EVSE_EmergencyStop:
        {
          evse_lcd_fault_emergency();
          vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        break;

        case EVSE_PowerLoss:
        {

          evse_lcd_power_loss();
          vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        break;
        }
      }

      break;

      case EVSE_OverCurrentFailure:
        evse_lcd_fault_over_current();
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        break;

      case EVSE_OverVoltage:
        evse_lcd_fault_over_voltage();
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        break;

      case EVSE_PowerMeterFailure:

        break;

      case EVSE_ReaderFailure:

        break;

      case EVSE_ResetFailure:

        break;

      case EVSE_UnderVoltage:
        evse_lcd_fault_under_voltage();
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        break;

      case EVSE_WeakSignal:

        break;

      default:
        break;
      }
      // dip_EvseChargePointErrorCode_OtherError = EvseChargePointErrorCode_OtherError;
      // }
    }
    break;
    default:

      break;
    }
    lenum_evse_ChargePointStatus = evse_ChargePointStatus;
  }

  switch (evse_ChargePointStatus)
  {
  case Charging:
  {
    storing_time = charge_start_time++;
    evse_volt_amps_update();
    vTaskDelay(740 / portTICK_PERIOD_MS);
    storing_time = charge_start_time++;
    float evse_kw = ((ipc_current_A * ipc_voltage_A) / 1000);
    evse_lcd_charging(evse_kw, discurrEnergy_A);
    char timeBuffer[10];
    memset(timeBuffer, '\0', sizeof(timeBuffer));
    update_time(storing_time, timeBuffer);
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    evse_lcd_connection_status();
    gu8_charging_started = 1;
    vTaskDelay(500 / portTICK_PERIOD_MS);
    break;
  }
  }
}
