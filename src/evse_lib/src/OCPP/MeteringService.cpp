
#include "MeteringService.h"
#include "OcppOperation.h"
#include "MeterValues.h"
#include "OcppEngine.h"
#include "SimpleOcppOperationFactory.h"
// #include "display.h"
#include <EEPROM.h>
#include <Preferences.h>

#include "../../../evse_src/evse_config.h"
// #include "../Evse_Preferences/Evse_Preferences.h"

bool flag_MeteringIsInitialised = false;
unsigned int meterSampleInterval = 30;

extern uint8_t gu8_online_flag;

unsigned long evse_stop_time;

// float online_charging_Enargy_A;

uint32_t g32currentenergy = 0;
float f32lastenergy = 0;
uint32_t last_unix_epoch = 0;

extern volatile double ipc_voltage_B;
extern volatile double ipc_voltage_A;
extern volatile double ipc_voltage_C;
extern volatile double ipc_current_A;
extern volatile double ipc_current_B;
extern volatile double ipc_current_C;
extern volatile double ipc_temp;

Preferences energymeter;

volatile float LastPresentEnergy_A = 0;
volatile float discurrEnergy_A = 0;
volatile float current_energy_A = 0;

volatile unsigned long int gs32_st_time = 0;

volatile int hr;	  // Number of seconds in an hour
volatile int mins; // Remove the number of hours and calculate the minutes.
volatile int sec;

// #if EVSE_DWIN_DISPLAY_ENABLED
float current_energy = 0;
float voltage_display = 0;
float current_display = 0;

// MeteringService Metering_Service;

MeteringService::MeteringService(WebSocketsClient *webSocket)
	: webSocket(webSocket)
{

	sampleTimeA = LinkedList<time_t>();
	sampleTimeB = LinkedList<time_t>();
	sampleTimeC = LinkedList<time_t>();
	power = LinkedList<float>();
	energyA = LinkedList<float>();
	energyB = LinkedList<float>();
	energyC = LinkedList<float>();
	voltageA = LinkedList<float>();
	voltageB = LinkedList<float>();
	voltageC = LinkedList<float>();
	currentA = LinkedList<float>();
	currentB = LinkedList<float>();
	currentC = LinkedList<float>();
	temperature = LinkedList<float>();

	setMeteringSerivce(this); // make MeteringService available through Ocpp Engine
}

void MeteringService::addDataPoint(time_t currentTime, float currentPower, float currentEnergy, float currentVoltage, float currentCurrent, float currentTemperature)
{

	if (getChargePointStatusService() != NULL && getChargePointStatusService()->getTransactionId() == -1)
	{

		lastSampleTimeA = currentTime;
	}
	else if (getChargePointStatusService() != NULL && getChargePointStatusService()->getConnectorId() == 1)
	{
		sampleTimeA.add(currentTime);
		power.add(currentPower);
		energyA.add(currentEnergy);
		voltageA.add(currentVoltage);
		currentA.add(currentCurrent);
		temperature.add(currentTemperature);

		lastSampleTimeA = currentTime;

		/*
		 * Check if to send all the meter values to the server
		 */
		if (power.size() >= METER_VALUES_SAMPLED_DATA_MAX_LENGTH && energyA.size() >= METER_VALUES_SAMPLED_DATA_MAX_LENGTH && voltageA.size() >= METER_VALUES_SAMPLED_DATA_MAX_LENGTH && currentA.size() >= METER_VALUES_SAMPLED_DATA_MAX_LENGTH && temperature.size() >= METER_VALUES_SAMPLED_DATA_MAX_LENGTH)
		{
			flushLinkedListValues();
		}
	}
#if 0
	else if (getChargePointStatusService() != NULL && getChargePointStatusService()->getConnectorId() == 2)
	{
		sampleTimeB.add(currentTime);
		power.add(currentPower);
		energyB.add(currentEnergy);
		voltageB.add(currentVoltage);
		currentB.add(currentCurrent);
		temperature.add(currentTemperature);

		lastSampleTimeB = currentTime;
		// lastPower = currentPower;

		/*
		 * Check if to send all the meter values to the server
		 */
		if (power.size() >= METER_VALUES_SAMPLED_DATA_MAX_LENGTH && energyB.size() >= METER_VALUES_SAMPLED_DATA_MAX_LENGTH && voltageB.size() >= METER_VALUES_SAMPLED_DATA_MAX_LENGTH && currentB.size() >= METER_VALUES_SAMPLED_DATA_MAX_LENGTH && temperature.size() >= METER_VALUES_SAMPLED_DATA_MAX_LENGTH)
		{
			flushLinkedListValues();
		}
	}
	else if (getChargePointStatusService() != NULL && getChargePointStatusService()->getConnectorId() == 3)
	{
		sampleTimeC.add(currentTime);
		power.add(currentPower);
		energyC.add(currentEnergy);
		voltageC.add(currentVoltage);
		currentC.add(currentCurrent);
		temperature.add(currentTemperature);

		lastSampleTimeC = currentTime;
		// lastPower = currentPower;

		/*
		 * Check if to send all the meter values to the server
		 */
		if (power.size() >= METER_VALUES_SAMPLED_DATA_MAX_LENGTH && energyC.size() >= METER_VALUES_SAMPLED_DATA_MAX_LENGTH && voltageC.size() >= METER_VALUES_SAMPLED_DATA_MAX_LENGTH && currentC.size() >= METER_VALUES_SAMPLED_DATA_MAX_LENGTH && temperature.size() >= METER_VALUES_SAMPLED_DATA_MAX_LENGTH)
		{
			flushLinkedListValues();
		}
	}
#endif
	// if (DEBUG_OUT)
	// 	Serial.println("It is NULL");
}

void MeteringService::flushLinkedListValues()
{
	if (getChargePointStatusService() != NULL)
	{
		if (getChargePointStatusService()->getTransactionId() == -1)
		{
			if (power.size() == 0 && energyA.size() == 0 && voltageA.size() == 0 && currentA.size() == 0 && temperature.size() == 0)
				return; // Nothing to report

#if 1

			if (gu8_online_flag == 1)
			{
				OcppOperation *meterValues = makeOcppOperation(webSocket,
															   new MeterValues(&sampleTimeA, &power, &energyA, &voltageA, &currentA, &temperature));
				initiateOcppOperation(meterValues);
			}
#endif

			sampleTimeA.clear();
			power.clear();
			energyA.clear();
			voltageA.clear();
			currentA.clear();
			temperature.clear();
		}
		else if (getChargePointStatusService()->getConnectorId() == 1)
		{
			if (power.size() == 0 && energyA.size() == 0 && voltageA.size() == 0 && currentA.size() == 0 && temperature.size() == 0)
				return; // Nothing to report

			if (getChargePointStatusService()->getTransactionId() < 0)
			{
				sampleTimeA.clear();
				power.clear();
				energyA.clear();
				voltageA.clear();
				currentA.clear();
				temperature.clear();
				return;
			}
#if 1

			if (gu8_online_flag == 1)
			{
				OcppOperation *meterValues = makeOcppOperation(webSocket,
															   new MeterValues(&sampleTimeA, &power, &energyA, &voltageA, &currentA, &temperature));
				initiateOcppOperation(meterValues);
			}
#endif

			sampleTimeA.clear();
			power.clear();
			energyA.clear();
			voltageA.clear();
			currentA.clear();
			temperature.clear();
		}
#if 0
		else if (getChargePointStatusService()->getConnectorId() == 2)
		{
			if (power.size() == 0 && energyB.size() == 0 && voltageB.size() == 0 && currentB.size() == 0 && temperature.size() == 0)
				return; // Nothing to report
			if (getChargePointStatusService()->getTransactionId() < 0)
			{
				sampleTimeB.clear();
				power.clear();
				energyB.clear();
				voltageB.clear();
				currentB.clear();
				temperature.clear();
				return;
			}
#if 1

			if(gu8_online_flag == 1)
			{
			OcppOperation *meterValues = makeOcppOperation(webSocket,
														   new MeterValues(&sampleTimeB, &power, &energyB, &voltageB, &currentB, &temperature));
			initiateOcppOperation(meterValues);
			}
#endif
			sampleTimeB.clear();
			power.clear();
			energyB.clear();
			voltageB.clear();
			currentB.clear();
			temperature.clear();
		}
		else if (getChargePointStatusService()->getConnectorId() == 3)
		{
			if (power.size() == 0 && energyC.size() == 0 && voltageC.size() == 0 && currentC.size() == 0 && temperature.size() == 0)
				return; // Nothing to report
			if (getChargePointStatusService()->getTransactionId() < 0)
			{
				sampleTimeC.clear();
				power.clear();
				energyC.clear();
				voltageC.clear();
				currentC.clear();
				temperature.clear();
				return;
			}
#if 1

			if(gu8_online_flag == 1)
			{
			OcppOperation *meterValues = makeOcppOperation(webSocket,
														   new MeterValues(&sampleTimeC, &power, &energyC, &voltageC, &currentC, &temperature));
			initiateOcppOperation(meterValues);
			}

#endif
			sampleTimeC.clear();
			power.clear();
			energyC.clear();
			voltageC.clear();
			currentC.clear();
			temperature.clear();
		}
#endif
	}
}

void MeteringService::loop()
{

	/*
	 * Calculate energy consumption which finally should be reportet to the Central Station in a MeterValues.req.
	 * This code uses the EVSE's own energy register, if available (i.e. if energySampler is set). Otherwise it
	 * uses the power sampler.
	 * If no powerSampler is available, estimate the energy consumption taking the Charging Schedule and CP Status
	 * into account.
	 */

#if 0
	//@bug: fix it for multiple connectors.
	if (DEBUG_OUT_M)
		Serial.println("The last sample time is: ");
	if (DEBUG_OUT_M)
		Serial.println(lastSampleTimeA + /*METER_VALUE_SAMPLE_INTERVAL*/ meterSampleInterval);
	if (DEBUG_OUT_M)
		Serial.println("The now() is: ");
	if (DEBUG_OUT_M)
		Serial.println(now());
#endif

	if ((now() >= (time_t) /*METER_VALUE_SAMPLE_INTERVAL*/ meterSampleInterval + lastSampleTimeA))
	{
		if (energyASampler != NULL && powerASampler != NULL && voltageASampler != NULL && currentASampler != NULL && temperatureSampler != NULL)
		{

			time_t sampledTimeA = now();
			time_t deltaA = sampledTimeA - lastSampleTimeA;
			float sampledVoltage = voltageASampler();
			float sampledCurrent = currentASampler();
			float sampledPower = powerASampler(sampledVoltage, sampledCurrent);
			// float sampledEnergyA = energyASampler(sampledVoltage, sampledCurrent, deltaA);  // Commented on 01122023_1250
			float sampledEnergyA = setPresentEnergyASampler(sampledVoltage, sampledCurrent, deltaA); // need to test
			float sampledTemperature = temperatureSampler();

			// float sampledVoltage = read_voltageASampler();
			// float sampledCurrent = read_currentASampler();
			// float sampledPower = read_powerASampler();
			// float sampledEnergyA = read_energyASampler();
			// float sampledTemperature = read_temperatureSampler();

			// if (DEBUG_OUT_M)
			// 	Serial.println("Inside if cond.");

#if 0

			if(gu8_online_flag == 1)
			{
				addDataPoint(sampledTimeA, sampledPower, sampledEnergyA, sampledVoltage, sampledCurrent, sampledTemperature);
            
			}
#else

			addDataPoint(sampledTimeA, sampledPower, sampledEnergyA, sampledVoltage, sampledCurrent, sampledTemperature);

			Serial.println("sampledEnergyA" + String(sampledEnergyA));
#endif
			// addDataPoint(sampledTimeA, 0, 0,sampledVoltage, sampledCurrent, sampledTemperature);
		}
	}
}

void MeteringService::init(MeteringService *meteringService)
{

	flag_MeteringIsInitialised = true;

	meteringService->lastSampleTimeA = now(); // 0 means not charging right now
	// meteringService->lastSampleTimeB = now(); // 0 means not charging right now
	// meteringService->lastSampleTimeC = now(); // 0 means not charging right now

	if (meterSampleInterval <= 0)
	{
		meterSampleInterval = 30;
	}
	Serial.println("meterValueSampleInterval: " + String(meterSampleInterval));

	/*
	 * initialize EEPROM
	 */
#if 1
	int isInitialized;
	float currEnergy_A;
	//   float currEnergy_B;
	//   float currEnergy_C;

	uint32_t time_now = now();

	// Replacing EEROM implementation with preferences
	energymeter.begin("MeterData", false); // opening nmespace with R/W access
	isInitialized = energymeter.getInt("isInitialized", 0);
	Serial.println("[Metering init] isInitialized: " + String(isInitialized));

	currEnergy_A = energymeter.getFloat("currEnergy_A", 0);
	Serial.println("[Metering init] currEnergy_A: " + String(currEnergy_A));

	last_unix_epoch = energymeter.getULong("lastunix_epoch", time_now);

	g32currentenergy = currEnergy_A;
	Serial.println("[Metering init] g32currentenergy: " + String(g32currentenergy));
	//   currEnergy_B = energymeter.getFloat("currEnergy_B",0);
	//   Serial.println("[Metering init] currEnergy_B: "+ String(currEnergy_B));

	//   currEnergy_C = energymeter.getFloat("currEnergy_C",0);
	//   Serial.println("[Metering init] currEnergy_C: " + String(currEnergy_C));

	if (isInitialized == 22121996)
	{
		if (currEnergy_A > 200100000)
		{ // 20100000
			energymeter.putFloat("currEnergy_A", 0);
			energymeter.putULong("lastunix_epoch", time_now);
			Serial.println("[Metering init] Reinitialized currEnergy_A");
		}
	}
	else if (isInitialized != 22121996)
	{

		energymeter.putInt("isInitialized", 22121996);
		energymeter.putFloat("currEnergy_A", 0);
		energymeter.putULong("lastunix_epoch", time_now);
		// energymeter.putFloat("currEnergy_B",0);
		// energymeter.putFloat("currEnergy_C",0);

		Serial.println("[Metering init] Initialized Meter Data");
	}
#endif

	Serial.println("[Mertering init] isInitialized " + String(isInitialized));
	Serial.println("[Mertering init] current energy: " + String(currEnergy_A));

	meteringService->setVoltageASampler([]()
										{ return roundingFloat((float)ipc_voltage_A); });
	meteringService->setVoltageBSampler([]()
										{ return roundingFloat((float)ipc_voltage_B); });
	meteringService->setVoltageCSampler([]()
										{ return roundingFloat((float)ipc_voltage_C); });
	meteringService->setCurrentASampler([]()
										{ return roundingFloat((float)ipc_current_A); });
	meteringService->setCurrentBSampler([]()
										{ return roundingFloat((float)ipc_current_B); });
	meteringService->setCurrentCSampler([]()
										{ return roundingFloat((float)ipc_current_C); });
	meteringService->setEnergyASampler([](float volt_A, float current_A, time_t delta_A)
									   {
#if 0
	EEPROM.begin(sizeof(EEPROM_Data));
	float lastEnergy;
	EEPROM.get(4,lastEnergy);
	float finalEnergy = lastEnergy + ((float) (volt*current*((float)delta)))/3600;
	EEPROM.put(4, finalEnergy);
	EEPROM.commit();
	EEPROM.end();
	if (DEBUG_OUT) Serial.println("EEPROM Energy Register Value: ");
	if (DEBUG_OUT) Serial.println(finalEnergy);
#endif
	
	Serial.println("delta_A : " +String(delta_A));
    
    if(delta_A >= meterSampleInterval )
	{
		delta_A = meterSampleInterval;
	}

#if 0
	if(gu8_online_flag == 1)
	{
	if(delta_A >= 30 )
	{
		delta_A = 30;
	}
	}
	else
	{
		if(delta_A > 100)
		{
           delta_A = 40; 
		} 
		
	}
#endif



	float lastEnergy_A = energymeter.getFloat("currEnergy_A",0);
  	float finalEnergy_A = lastEnergy_A + ((float)(volt_A * current_A * ((float)delta_A)))/3600; //Whr

  	//placing energy value back in EEPROM
  	energymeter.putFloat("currEnergy_A",finalEnergy_A);
  	Serial.println("[EnergyASampler] currEnergy_A: "+ String(finalEnergy_A));
	// energymeter.end();  //add by rakesh
	return roundingFloat(finalEnergy_A); });

	meteringService->setPowerASampler([](float volt, float current)
									  { return roundingFloat(volt * current / 1000); });
	meteringService->setEnergyBSampler([]()
									   {
	//@bug: change this to energy if it is 3 phase. Ignoring it since it is 3.3KW Single phase
    return (float) ipc_current_B; });
	meteringService->setEnergyCSampler([]()
									   {
	//@bug: change this to energy if it is 3 phase. Ignoring it since it is 3.3KW Single phase
    return (float) ipc_current_C; });
	meteringService->setTemperatureSampler([]()
										   {
	//example values. Put your own power meter in heres
    return (float) ipc_temp; });

#if 0
	meteringService->setVoltageASampler([]()
										{ return roundingFloat((float)eic.GetLineVoltageA()); });
	meteringService->setVoltageBSampler([]()
										{ return roundingFloat((float)eic.GetLineVoltageB()); });
	meteringService->setVoltageCSampler([]()
										{ return roundingFloat((float)eic.GetLineVoltageC()); });
	meteringService->setCurrentASampler([]()
										{ return roundingFloat((float)eic.GetLineCurrentA()); });
	meteringService->setCurrentBSampler([]()
										{ return roundingFloat((float)eic.GetLineCurrentB()); });
	meteringService->setCurrentCSampler([]()
										{ return roundingFloat((float)eic.GetLineCurrentC()); });
	meteringService->setEnergyASampler([](float volt_A, float current_A, time_t delta_A)
									   {
#if 0
	EEPROM.begin(sizeof(EEPROM_Data));
	float lastEnergy;
	EEPROM.get(4,lastEnergy);
	float finalEnergy = lastEnergy + ((float) (volt*current*((float)delta)))/3600;
	EEPROM.put(4, finalEnergy);
	EEPROM.commit();
	EEPROM.end();
	if (DEBUG_OUT) Serial.println("EEPROM Energy Register Value: ");
	if (DEBUG_OUT) Serial.println(finalEnergy);
#endif
	
	Serial.println("delta_A : " +String(delta_A));

	if(delta_A >= 30 )
	{
		delta_A = 30;
	}

	float lastEnergy_A = energymeter.getFloat("currEnergy_A",0);
  	float finalEnergy_A = lastEnergy_A + ((float)(volt_A * current_A * ((float)delta_A)))/3600; //Whr

  	//placing energy value back in EEPROM
  	energymeter.putFloat("currEnergy_A",finalEnergy_A);
  	Serial.println("[EnergyASampler] currEnergy_A: "+ String(finalEnergy_A));
	return roundingFloat(finalEnergy_A); });

	meteringService->setPowerASampler([](float volt, float current)
									  { return roundingFloat(volt * current / 1000); });
	meteringService->setEnergyBSampler([]()
									   {
	//@bug: change this to energy if it is 3 phase. Ignoring it since it is 3.3KW Single phase
    return (float) eic.GetLineCurrentC(); });
	meteringService->setEnergyCSampler([]()
									   {
	//@bug: change this to energy if it is 3 phase. Ignoring it since it is 3.3KW Single phase
    return (float) eic.GetLineCurrentC(); });
	meteringService->setTemperatureSampler([]()
										   {
	//example values. Put your own power meter in heres
    return (float) eic.GetTemperature(); });
#endif

	timer_init = false;
	energymeter.end(); // add by rakesh
}

float MeteringService::currentEnergy()
{
#if 0 
	EEPROM.begin(sizeof(EEPROM_Data));
	float lastEnergy;
	EEPROM.get(4, lastEnergy);
	float volt = voltageASampler();
	float curr = currentASampler();
	time_t delta = now() - lastSampleTimeA;
	float finalEnergy = energyASampler(volt, curr, delta);
	EEPROM.put(4, finalEnergy);
	EEPROM.commit();
	EEPROM.end();
#endif

	energymeter.begin("MeterData", false); // add by rakesh

	float lastEnergy_A = energymeter.getFloat("currEnergy_A", 0);
	f32lastenergy = lastEnergy_A;

	float volt_A = voltageASampler();
	float curr_A = currentASampler();

	time_t delta_A = now() - lastSampleTimeA;
	float finalEnergy_A = energyASampler(volt_A, curr_A, delta_A);

	energymeter.putFloat("currEnergy_A", finalEnergy_A);
	energymeter.end(); // add by rakesh

	if (DEBUG_OUT)
		Serial.println("The last Energy_A is: " + String(lastEnergy_A));
	if (DEBUG_OUT)
		Serial.println("The current Energy_A is: " + String(finalEnergy_A));

	return finalEnergy_A;

#if 0
	if (DEBUG_OUT_M)
		Serial.println("The last Energy is is: ");
	if (DEBUG_OUT_M)
		Serial.println(lastEnergy);
	if (DEBUG_OUT_M)
		Serial.println("The Final Energy is is: ");
	if (DEBUG_OUT_M)
		Serial.println(finalEnergy);

	return finalEnergy;
#endif

} // wamique, can you flash this one?

/*void MeteringService::setPowerSampler(float (*ps)()){
  this->powerSampler = ps;
}*/

void MeteringService::setEnergyASampler(float (*es)(float volt, float current, time_t delta))
{
	this->energyASampler = es;
}

void MeteringService::setPowerASampler(float (*ps)(float volt, float current))
{
	this->powerASampler = ps;
}

void MeteringService::setEnergyBSampler(float (*es)())
{
	this->energyBSampler = es;
}

void MeteringService::setEnergyCSampler(float (*es)())
{
	this->energyCSampler = es;
}

void MeteringService::setVoltageASampler(float (*vs)())
{
	this->voltageASampler = vs;
}

void MeteringService::setVoltageBSampler(float (*vs)())
{
	this->voltageBSampler = vs;
}

void MeteringService::setVoltageCSampler(float (*vs)())
{
	this->voltageCSampler = vs;
}

void MeteringService::setCurrentASampler(float (*cs)())
{
	this->currentASampler = cs;
}

void MeteringService::setCurrentBSampler(float (*cs)())
{
	this->currentBSampler = cs;
}

void MeteringService::setCurrentCSampler(float (*cs)())
{
	this->currentCSampler = cs;
}

void MeteringService::setTemperatureSampler(float (*ts)())
{
	this->temperatureSampler = ts;
}

float roundingFloat(float var) // rounding to 2 decimal point
{
	// 37.66666 * 100 =3766.66
	// 3766.66 + .5 =3767.16    for rounding off value
	// then type cast to int so value is 3767
	// then divided by 100 so the value converted into 37.67
	float value = (int)(var * 100 + 0.5);
	return (float)value / 100;
}

float MeteringService::setPresentEnergyASampler(float volt, float current, time_t delta)
{
	// if(delta >= 30 )
	// {
	// 	delta = 30;
	// }

	Serial.println("delta : " + String(delta));

#if 0 
	if (delta >= meterSampleInterval)
	{
		delta = meterSampleInterval;
	}
#endif

	if (delta >= (3 * meterSampleInterval))
	{
		delta = meterSampleInterval;
	}

	// int connector_id = getConnectorId();
	// Serial.println("[EnergySampler] connector_id: " + String(connector_id));
	energymeter.begin("MeterData", false); // opening nmespace with R/W access
	float lastEnergy_A = energymeter.getFloat("currEnergy_A", 0);
	float finalEnergy_A = lastEnergy_A + ((float)(volt * current * ((float)delta))) / 3600; // Whr

	// placing energy value back in EEPROM
	energymeter.putFloat("currEnergy_A", finalEnergy_A);
	energymeter.end();

#if EVSE_DWIN_ENABLE
	voltage_display = volt;
	current_display = current;
	current_energy = current_energy + ((float)(volt * current * ((float)delta))) / 3600;
	Serial.println(" currEnergy>>>>>>>>>>>>>>>>>>>> " + String(current_energy));

	Serial.println("[EnergyASampler] currEnergy_A: " + String(finalEnergy_A));
	// unsigned long lu_stop_time = millis();
	// unsigned long lu_seconds = ((lu_stop_time - gs32_st_time) / 1000);

	// hr = lu_seconds / 3600;					// Number of seconds in an hour
	// mins = ((lu_seconds - hr * 3600) / 60); // Remove the number of hours and calculate the minutes.
	// sec = ((((lu_seconds - hr) * 3600) - mins) * 60);
#endif

#if EVSE_LCD_ENABLE // for i2c display
	current_energy_A = current_energy_A + ((float)(volt * current * ((float)delta))) / 3600;
	discurrEnergy_A = (current_energy_A - LastPresentEnergy_A) / 1000;
	evse_stop_time = millis();
#endif
#if EVSE_DWIN_ENABLE
	current_energy_A = current_energy_A + ((float)(volt * current * ((float)delta))) / 3600;
	// discurrEnergy_A = (current_energy_A - LastPresentEnergy_A) / 1000;
	// evse_stop_time = millis();
#endif
	Serial.println("[EnergyASampler] currEnergy_A: " + String(finalEnergy_A));
	return roundingFloat(finalEnergy_A);
}

float MeteringService::read_voltageASampler(void)
{
	return float(ipc_voltage_A);
}
float MeteringService::read_currentASampler(void)
{
	return float(ipc_current_A);
}
float MeteringService::read_powerASampler(void)
{
	return float(ipc_voltage_A * ipc_current_A);
}
float MeteringService::read_energyASampler(void)
{
	return float(energymeter.getFloat("currEnergy_A", 0));
}
float MeteringService::read_temperatureSampler(void)
{
	return float(ipc_temp);
}
void MeteringService::update_metervalues(void)
{

	time_t sampledTimeA = now();
	time_t deltaA = sampledTimeA - lastSampleTimeA;
	float sampledVoltage = voltageASampler();
	float sampledCurrent = currentASampler();
	float sampledPower = powerASampler(sampledVoltage, sampledCurrent);
	float sampledEnergyA = energyASampler(sampledVoltage, sampledCurrent, deltaA);
	float sampledTemperature = temperatureSampler();
}