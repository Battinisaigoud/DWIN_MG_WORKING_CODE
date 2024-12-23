#ifndef EVSE_ESPNOW_MASTER_H
#define EVSE_ESPNOW_MASTER_H

// #include "Arduino.h"
#include <esp_now.h>
#include <WiFi.h>

/* These values are update from client side. These macros are update to pack_TYPE variable */
#define CAMERA_INFO         (0x10)
#define CARAVAIL            (0x20)
#define CARPALTENUM         (0x21)
#define CARNOTAVAIL         (0x22)

/*These ALPR Device IDs*/
#define DEVICE1             (0x01)
#define DEVICE2             (0x02)
#define DEVICE3             (0x03)

#define EXCTIMEDEBUG        (0)

void ask_Alpr_Data(uint8_t device_ID, uint8_t packTYPE);
void read_Data(void);
void Read_Response(int len);
void init_esp_now(void);

extern uint8_t alpr_device_id;
extern int chan; 
#endif
