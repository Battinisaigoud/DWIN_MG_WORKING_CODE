/*******************************************************************************
 * 
 * program : evse_gfci.cpp
 * author  : krishna & rakesh
 * 
 * 
 * Description : 
 * 
 *
*******************************************************************************/




#include "evse_watch_dog.h"

WATCH_DOG evse_watch_dog;

void WATCH_DOG::evse_watch_dog_init(void)
{
  pinMode(WATCH_DOG_PIN, OUTPUT);
}

void WATCH_DOG::evse_watch_dog_on(void)
{
  digitalWrite(WATCH_DOG_PIN, HIGH);
}

void WATCH_DOG::evse_watch_dog_off(void)
{
  digitalWrite(WATCH_DOG_PIN, LOW);
}
