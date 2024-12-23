#ifndef _EVSE_RFID_H_
#define _EVSE_RFID_H_
//C:\Users\rakes\Documents\evse_single_phase_board_test2\src\libraries

#include <SPI.h>

//#include <string.h>

#define MFRC_RST    (23)
#define MFRC_SS     (15)


 
class RFID {//: public MFRC522{

    public:
    void rfid_hspi_init(void);
    void rfid_init(void);
    void rfid_scan(void);
   
};
 




#endif