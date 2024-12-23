/*******************************************************************************
 *
 * program : evse_gfci.cpp
 * author  : krishna ,rakesh & pream
 *
 *
 * Description :
 *
 *
 *******************************************************************************/

#include "evse_rfid.h"
#include <SPI.h>
#include "../evse_lib/src/MFRC522/MFRC522.h"
#include "evse_config.h"
#include "evse_addr_led.h"
#include "../evse_lib/src/OCPP/ChargePointStatusService.h"

extern enum EvseDevStatuse EvseDevStatus_connector_1;
extern uint8_t gu8_evse_change_state;
extern String evse_idTagData;
extern uint8_t gu8_online_flag;
extern ADDR_LEDs *led_rgb;

extern EVSE_states_enum EVSE_states;

extern enum evse_authorize_stat_t evse_authorize_state;

uint8_t rfid_read_flag = 0;

MFRC522 mfrc522(MFRC_SS, MFRC_RST);

RFID evse_rfid;

uint8_t flag_rfid = 0;

// char rfid_tag_val[10] ={'\0'};
extern char rfid_tag_val[16];
char temp[16];
extern String rfid_buf;
extern uint8_t gu8_rfid_flag;
extern uint8_t reasonForStop;
/*including mutex from main file*/
extern portMUX_TYPE rfid_tag_mux;

void RFID::rfid_init(void)
{

  mfrc522.PCD_Init(); // Init mfrc_rfid
}

void RFID ::rfid_hspi_init()
{

  /* 14 - sck,  12 - miso, 13 - mosi, 15(MFRC_SS) - ss   */
  SPI.begin(14, 12, 13, MFRC_SS); // sck miso mosi ss
}

void RFID ::rfid_scan()
{

  if (mfrc522.PICC_IsNewCardPresent())
  { // new tag is available
    if (mfrc522.PICC_ReadCardSerial())
    { // NUID has been readed
      MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
      rfid_read_flag = 1;
      Serial.print("RFID/NFC Tag Type: ");
      Serial.println(mfrc522.PICC_GetTypeName(piccType));

      // print UID in Serial Monitor in the hex format
      for (int i = 0; i < mfrc522.uid.size; i++)
      {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
        Serial.println(mfrc522.uid.uidByte[i], HEX);
        sprintf(temp, "%.2X", mfrc522.uid.uidByte[i]);
        strcat(rfid_tag_val, temp);
      }
#if 1
      /*brief using mutex to update values*/
      portENTER_CRITICAL(&rfid_tag_mux);
      rfid_buf = rfid_tag_val;
      portEXIT_CRITICAL(&rfid_tag_mux);
      Serial.printf("printing from rfid task : %s \r\n", rfid_buf);
      /*memset used to clear values in array*/
      memset(rfid_tag_val, '\0', sizeof(rfid_tag_val));
#endif
      gu8_rfid_flag = 1;
#if 0
      if(EvseDevStatus_connector_1 != flag_EVSE_is_Booted)
      {
        EvseDevStatus_connector_1 = falg_EVSE_Authentication;
      }
#endif
      led_rgb->led_on_off(Blinky_Blue);
      led_rgb->led_on_off(Blinky_Blue);

      switch (EvseDevStatus_connector_1)
      {
      case flag_EVSE_is_Booted:
        // EvseDevStatus_connector_1 = flag_EVSE_is_Booted;
        gu8_evse_change_state = EVSE_READ_RFID;
        Serial.println("Boot- rfid");
        break;

      case flag_EVSE_Read_Id_Tag:

        if (rfid_buf.equals("") != true)
        {
#if 0
          if (evse_ChargePointStatus != Faulted)
          {
            if (gu8_online_flag == 1)
            {
              EvseDevStatus_connector_1 = falg_EVSE_Authentication;
              gu8_evse_change_state = EVSE_AUTHENTICATION;
              Serial.println("Read_Id_Tag- rfid");
            }
          }
#endif

          if ((evse_ChargePointStatus != Faulted)) //&& ((EVSE_states == STATE_A) || (EVSE_states == STATE_B)))
          {
            EvseDevStatus_connector_1 = falg_EVSE_Authentication;
            gu8_evse_change_state = EVSE_AUTHENTICATION;
            evse_authorize_state = EVSE_AUTHORIZE_INITIATED;
            Serial.println("Read_Id_Tag- rfid");

            flag_rfid = 1;

            // gu8_evse_change_state = EVSE_READ_RFID;
          }
          else
          {
            Serial.println("Read_Id_Tag- rfid_ofline>>>>>>>>>");
          }
        }
        break;

      case flag_EVSE_Request_Charge:

        if (rfid_buf.equals(evse_idTagData) == true)
        {

          reasonForStop = 3;
          EvseDevStatus_connector_1 = flag_EVSE_Stop_Transaction;

          evse_stop_txn_state = EVSE_STOP_TXN_INITIATED;
          gu8_evse_change_state = EVSE_STOP_TXN;
          Serial.println("Request_Charge - rfid");
        }
        break;

      default:
        if (evse_ChargePointStatus == Finishing)
        {
          rfid_buf = ""; // it is commented for to maintain rfid tag data for authorization pkt
          Serial.println("default - rfid Cleared");
        }
        else
        {
          Serial.println("default - rfid");
        }

        break;
      }

      mfrc522.PICC_HaltA();      // halt PICC
      mfrc522.PCD_StopCrypto1(); // stop encryption on PCD
      // Serial.println(rfid_tag_val);
    }
  }
}
