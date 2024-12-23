#ifndef BLE_COMMISSIONING_H_INCLUDED_
#define BLE_COMMISSIONING_H_INCLUDED_

#define BLE_OK (1)
#define BLE_FAIL (0)

#define BLE_GOTCONN (1)
#define BLE_CONN (2)
#define BLE_DISC (3)

#define ConfigFaultThresholds               (1)
#define WiFiConfig                          (2)
#define ethernetR                           (3)
#define OCPP_parameters                     (4)
//#define Offline_Functionality             (5)
#define ConnectivityConfig                  (5) 
#define StoreRFID                           (6)
#define DeleteRFID                          (7)
#define GetRFID                             (8)
#define FactoryReset                        (9)
#define ChargerSerialNum                    (10)
#define ChargePointVendorName               (11)
#define CommissioningDate                   (12)
#define CommissioningBy                     (13)
#define restoreSession                      (14)
#define TariffAmount                        (15)
#define DateTimeSet                         (16)
#define About                               (17)
#define LastMonthCumulative                 (20)
#define ThisMonthCumulative                 (21)
#define TotalCumulative                     (22)
#define TotalAmount                         (23)
#define DateTimeGet                         (24)
#define DataLogs                            (25)
#define CP_SERIAL                           (31)
#define readReq                             (13) // This message will be sent from phone to read defaults.
#define readTypeOfCharger                   (14) // 1 connector, 2 connector, 3 connector
#define ESP32RESET                          (30)
/*BLE*/
#define OVERVOLTAGE                          (41)
#define UNDERVOLTAGE                         (42)
#define OVERCURRENT                          (43)
#define UNDERCURRENT                         (44)
#define TERMINATIONCURRENT                   (45)
#define TERMINATIONTIME                      (46)
#define OVERTEMPERATURE                      (37)
#define PLUG_AND_CHARGE                      (33)
#define SESSIONRESUME                        (34)
#define CP_CURRENT_LIMIT                      (32)


#define DEBUG_EN true

void BLE_setup();
static uint8_t BLE_connectionCheck();
void BLE_loop();
void BLE_deinit();
uint8_t state_read();
uint8_t setCFT();
uint8_t setETH();
uint8_t setOCPP();
uint8_t Store_RFID();
uint8_t Delete_RFID();
uint8_t Get_RFID();
uint8_t factoryResetD();
uint8_t restoreFromFault();
uint8_t setConnConfg();
uint8_t setDOC();
uint8_t setCSSN();
uint8_t setCSN();
uint8_t setWiFi();
uint8_t sendRes(uint8_t, uint8_t);
uint8_t setChargingStationCommissionBy();
uint8_t setDateOfCommissioning();
uint8_t setDate_Time();
// uint8_t syncDateOfCommissioning();
uint8_t setChargingStationVendorName();
uint8_t setChargingStationSerialNumber();
uint8_t Store_Tariff_Amount();
uint8_t About_charger();
uint8_t Last_Month_Cumulative_Energy();
uint8_t This_Month_Cumulative_Energy();
uint8_t Total_Cumulative_Energy();
uint8_t ChargepointSerialNumber();

/*BLE*/
uint8_t setOverVoltage();
uint8_t setunderVoltage();
uint8_t setovercurrent();
uint8_t setundercurrent();
uint8_t setterminationcurrent();
uint8_t setterminationtime();
uint8_t setovertemparature();
uint8_t setPlugAndCharge();

uint8_t setSessionResume();
uint8_t setCPCurrentLimit();
#endif // HEADER_H_INCLUDED
