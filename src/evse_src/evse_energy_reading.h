#ifndef _EVSE_ENERGY_READING_H_
#define _EVSE_ENERGY_READING_H_

#include "evse_config.h"
#include"../evse_lib/src/ATM90E36/ATM90E36.h"

// #define chip_ss_pin (19)   //Hardware Board Version 2 

#define chip_ss_pin (5)    // Hardware Board Version 3



class ENERGY1 : public ATM90E36{
    public:
  // double voltageA,freq,voltageB,voltageC,currentA,currentB,currentC,power,pf,new_current,new_power;

   //void energy_meter_voltageA();
   void energy_meter_init();
// void energy_spi_init();
   void energy_meter_voltageA();
   void energy_meter_voltageB();
   void energy_meter_voltageC();
   void energy_meter_currentA();
   void energy_meter_currentB();
   void energy_meter_currentC();
   void energy_meter_pf();
   void energy_meter_frq();
   void energy_meter_power();
   void energy_meter_temp();
   // void energy_register_config();
};



#endif