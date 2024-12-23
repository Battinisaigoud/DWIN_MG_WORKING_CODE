#include "ChangeConfiguration.h"
#include "OcppEngine.h"
// #include <nvs_flash.h>
#include <Preferences.h>
// #include "../Evse_Preferences/Evse_Preferences.h"
#include "../../../evse_src/evse_cfg_file.h"

#include <EEPROM.h>

// #include <FreeRTOS.h>

extern unsigned int meterSampleInterval;
extern unsigned int heartbeatInterval;

Preferences change_config;
#define EEPROM_SIZE 1
Preferences evse_preferences_ocpp;

extern uint8_t gu8_change_conf_flag;

#if 1

// Local_Authorization_List_t Local_Authorization_List;
Change_configuration_tag_t Change_configuration_instance;

// Create a Linklist for
LinkedList<Change_configuration_tag_t> LL_Change_configuration_tag = LinkedList<Change_configuration_tag_t>();

Change_configuration_tag_t get_Change_configuration_instance;

#endif

// extern evse_changeconfig evse_change_config;
#if 0
bool evse_ocpp_config_AuthorizeRemoteTxRequests;
uint32_t evse_ocpp_config_ClockAlignedDataInterval;
uint32_t evse_ocpp_config_ConnectionTimeOut;
uint32_t evse_ocpp_config_GetConfigurationMaxKeys;

bool evse_ocpp_config_LocalAuthorizeOffline;
bool evse_ocpp_config_LocalPreAuthorize;
String evse_ocpp_config_MeterValuesAlignedData;
String evse_ocpp_config_MeterValuesSampledData;
uint32_t evse_ocpp_config_MeterValueSampleInterval;
uint32_t evse_ocpp_config_ResetRetries;
String evse_ocpp_config_ConnectorPhaseRotation;
bool evse_ocpp_config_StopTransactionOnEVSideDisconnect;
String evse_ocpp_config_StopTxnAlignedData;
String evse_ocpp_config_StopTxnSampledData;
uint32_t evse_ocpp_config_TransactionMessageAttempts;
uint32_t evse_ocpp_config_TransactionMessageRetryInterval;
uint32_t evse_ocpp_config_UnlockConnectorOnEVSideDisconnect;
uint32_t evse_ocpp_config_LocalAuthListEnabled;
bool evse_ocpp_config_StopTransactionOnInvalidId;
#endif

extern portMUX_TYPE upload__mux;

// uint32_t evse_ocpp_config_HeartbeatInterval;
// uint32_t evse_ocpp_config_HeartbeatInterval_org;
// uint32_t evse_ocpp_config_HeartbeatInterval;

// uint32_t value_1;
// uint32_t value_2;

extern SemaphoreHandle_t preferencesMutex;
/*****************************************OCPP CONFIGURATION KEY****************************************************************/
extern OCPPconfigkeyStorage *ptrOCPPconfigKeys;
extern ocpp_config_keys_t gOCPP_config_keys_write;
extern ocpp_config_keys_t gOCPP_config_keys_read;

/*****************************************OCPP CONFIGURATION KEY****************************************************************/

ChangeConfiguration Change_Configuration;

ChangeConfiguration::ChangeConfiguration()
{
}

const char *ChangeConfiguration::getOcppOperationType()
{
	return "ChangeConfiguration";
}

/*
 * @brief : ChangeConfiguration implemented by
 * G. Raja Sumant 08/07/2022
 * It can either accept , reject or say not supported.
 */

#if 0
void ChangeConfiguration::processReq(JsonObject payload)
{
	String key = String(payload["key"].as<String>());
	// const char *key = payload["key"];
	unsigned int value = 0;
    
LL_Change_configuration_tag.clear();
	//  if (!strcmp(key, "Heartbeat"))
	

	if (key.equals("HeartbeatInterval"))
	{
		Serial.println("im in heartbeat>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
		value = String(payload["value"].as<String>()).toInt();
		if (value == 0)
		{

			Serial.println(F("[ChangeConfig] Rejecting the change as invalid value has been received!"));
			acceptance = false;
			rejected = true;
		}
		else
		{
			Serial.println("[ChangeConfig] Setting heartbeatInterval: " + String(value));
			// heartbeatInterval = value;
			// evse_ocpp_config_HeartbeatInterval_org = value;
               gu8_change_conf_flag = 1;
				get_Change_configuration_instance.ocppkey = value;
				Serial.print("listVersion inString : ");
				Serial.println(	get_Change_configuration_instance.ocppkey);
			// value_1 = value;
			acceptance = true;
			rejected = false;

			// EEPROM.begin(EEPROM_SIZE);
			// EEPROM.write(1, evse_ocpp_config_HeartbeatInterval_org);
			// EEPROM.commit();
			//  Serial.printf("HeartbeatInterval: %d \n", evse_ocpp_config_HeartbeatInterval_org);
			// evse_preferences_ws.begin("ws_cred", false);
			// evse_preferences_ws.putUInt("Heartbeat_Interval", evse_ocpp_config_HeartbeatInterval_org);
			// uint32_t evse_ocpp_config_HeartbeatInterval = evse_preferences_ws.getUInt("Heartbeat_Interval", evse_ocpp_config_HeartbeatInterval_org);
			// Serial.printf("HeartbeatInterval: %d \n", evse_ocpp_config_HeartbeatInterval);
			// evse_preferences_ws.end();

#if EVSE_OCPP_CONFIG_PREFERENCE == EVSE_TRUE
			// preferencesMutex = xSemaphoreCreateMutex();
			// //  Preferences evse_preferences_ocpp;

			// evse_preferences_ocpp.begin("HeartbeatInterval", false);
			// portENTER_CRITICAL(&upload__mux);
			// evse_preferences_ocpp.putUInt("Heartbeat_Interval", value_1);
			// portEXIT_CRITICAL(&upload__mux);
			// Serial.printf("HeartbeatInterval1: %d \n", value_1);
			// evse_preferences_ocpp.end();

#endif
		}
	}
}
#endif
#if 0
void ChangeConfiguration::processReq(JsonObject payload)
{
	String key = String(payload["key"].as<String>());

	unsigned int value = 0;

	if (key.equals("AuthorizeRemoteTxRequests"))
	{
		String val = payload["value"];
		Serial.println("[ChangeConfiguration] AuthorizeRemoteTxRequests : " + String(val));
		if (val == "true" || val == "True" || val == "TRUE" || val.toInt() == 1)
		{
			Serial.println(F("[ChangeConfiguration] AuthorizeRemoteTxRequests set to true"));
			acceptance = true;
			rejected = false;
			// flag_AuthorizeRemoteTxRequests = true;
			evse_ocpp_config_AuthorizeRemoteTxRequests = true;

#if EVSE_OCPP_CONFIG_PREFERENCE == EVSE_TRUE
			evse_preferences_ocpp.begin("ocpp_config", false);
			evse_preferences_ocpp.putBool("AuthorizeRemote_TxRequests", true);
			evse_preferences_ocpp.end();
			Serial.println(F("[ChangeConfiguration] In EVSE_OCPP_CONFIG_PREFERENCE"));
#endif
			// change_config.begin("configurations", false);
			// change_config.putBool("authRemoteStart", true);
			// change_config.end();
		}
		else if (val == "false" || val == "False" || val == "FALSE" || val.toInt() == 0)
		{
			Serial.println(F("[ChangeConfiguration] AuthorizeRemoteTxRequests set to false"));
			acceptance = true;
			rejected = false;
			// change_config.begin("configurations", false);
			// change_config.putBool("authRemoteStart", false);
			// change_config.end();
			// flag_AuthorizeRemoteTxRequests = false;
			evse_ocpp_config_AuthorizeRemoteTxRequests = false;

			// #if EVSE_OCPP_CONFIG_PREFERENCE == EVSE_TRUE
			// evse_preferences_ocpp.begin("ocpp_config", false);
			// evse_preferences_ocpp.putBool("AuthorizeRemote_TxRequests", false);
			// evse_preferences_ocpp.end();
			// #endif
		}
		else
		{
			Serial.println(F("[ChangeConfiguration] AuthorizeRemoteTxRequests Rejected the change config!"));
			Serial.println(String(payload["value"].as<String>()));
			acceptance = false;
			rejected = true;
		}

		// evse_preferences_ocpp.begin("ocpp_config", false);
		// evse_ocpp_config_AuthorizeRemoteTxRequests = evse_preferences_ocpp.getBool("AuthorizeRemote_TxRequests", false);
		// Serial.printf("AuthorizeRemoteTxRequests: %d \n", evse_ocpp_config_AuthorizeRemoteTxRequests);
		// evse_preferences_ocpp.end();

#if EVSE_OCPP_CONFIG_PREFERENCE
		ptrOCPPconfigKeys->OCPP_config_keys_write_param(AUTHORIZEREMOTETXREQUESTS, READ_WRITE, TYPE_BOOL_ONE, value, AUTHORIZEREMOTETXREQUESTS);
#endif
	}
	else if (key.equals("ClockAlignedDataInterval"))
	{
		value = String(payload["value"].as<String>()).toInt();
		if (value == 0)
		{
			// Serial.println("Setting MeterValueSampleInterval to Max");
			// meterSampleInterval = 43200;
			// acceptance = true;
			Serial.println(F("[ChangeConfig] Rejecting the change as invalid value has been received!"));
			acceptance = false;
			rejected = true;
		}
		else
		{
			Serial.println("[ChangeConfig] Setting ClockAlignedDataInterval: " + String(meterSampleInterval));
			// meterSampleInterval = value;
			evse_ocpp_config_ClockAlignedDataInterval = value;
			acceptance = true;
			rejected = false;
// change_config.begin("configurations", false);
// change_config.putInt("meterSampleInterval", value);
// change_config.end();
#if EVSE_OCPP_CONFIG_PREFERENCE == EVSE_TRUE
			evse_preferences_ocpp.begin("ClockAlignedDataInterval", false);
			evse_preferences_ocpp.putUInt("ClockAligned_DataInterval", evse_ocpp_config_ClockAlignedDataInterval);
			Serial.printf("ClockAlignedDataInterval : %d \n", evse_ocpp_config_ClockAlignedDataInterval);
			evse_preferences_ocpp.end();
#endif
		}
#if EVSE_OCPP_CONFIG_PREFERENCE
		ptrOCPPconfigKeys->OCPP_config_keys_write_param(CLOCKALIGNEDDATAINTERVAL, READ_WRITE, TYPE_BOOL_ONE, value, CLOCKALIGNEDDATAINTERVAL);
#endif
	}
	else if (key.equals("ConnectionTimeOut"))
	{
		value = String(payload["value"].as<String>()).toInt();
		if (value == 0)
		{
			// Serial.println("Setting MeterValueSampleInterval to Max");
			// meterSampleInterval = 43200;
			// acceptance = true;
			Serial.println(F("[ChangeConfig] Rejecting the change as invalid value has been received!"));
			acceptance = false;
			rejected = true;
		}
		else
		{
			Serial.println("[ChangeConfig] Setting ConnectionTimeOut: " + String(meterSampleInterval));
			// meterSampleInterval = value;
			evse_ocpp_config_ConnectionTimeOut = value;
			acceptance = true;
			rejected = false;
			// change_config.begin("configurations", false);
			// change_config.putInt("meterSampleInterval", value);
			// change_config.end();
#if EVSE_OCPP_CONFIG_PREFERENCE == EVSE_TRUE
			evse_preferences_ocpp.begin("ConnectionTimeOut", false);
			evse_preferences_ocpp.putUInt("Connection_TimeOut", evse_ocpp_config_ConnectionTimeOut);
			Serial.printf("ConnectionTimeOut: %d \n", evse_ocpp_config_ConnectionTimeOut);
			evse_preferences_ocpp.end();
#endif
		}
#if EVSE_OCPP_CONFIG_PREFERENCE
		ptrOCPPconfigKeys->OCPP_config_keys_write_param(CONNECTIONTIMEOUT, READ_WRITE, TYPE_BOOL_ONE, value, CONNECTIONTIMEOUT);
#endif
	}
	else if (key.equals("GetConfigurationMaxKeys"))
	{
		value = String(payload["value"].as<String>()).toInt();
		if (value == 0)
		{
			// Serial.println("Setting MeterValueSampleInterval to Max");
			// meterSampleInterval = 43200;
			// acceptance = true;
			Serial.println(F("[ChangeConfig] Rejecting the change as invalid value has been received!"));
			acceptance = false;
			rejected = true;
		}
		else
		{
			Serial.println("[ChangeConfig] Setting GetConfigurationMaxKeys: " + String(meterSampleInterval));
			// meterSampleInterval = value;
			evse_ocpp_config_GetConfigurationMaxKeys = value;
			acceptance = true;
			rejected = false;
			// change_config.begin("configurations", false);
			// change_config.putInt("meterSampleInterval", value);
			// change_config.end();
#if EVSE_OCPP_CONFIG_PREFERENCE == EVSE_TRUE
			evse_preferences_ocpp.begin("GetConfigurationMaxKeys", false);
			evse_preferences_ocpp.putUInt("GetConfiguration_MaxKeys", evse_ocpp_config_GetConfigurationMaxKeys);
			Serial.printf("GetConfigurationMaxKeys: %d \n", evse_ocpp_config_GetConfigurationMaxKeys);
			evse_preferences_ocpp.end();
#endif
		}
#if EVSE_OCPP_CONFIG_PREFERENCE
		ptrOCPPconfigKeys->OCPP_config_keys_write_param(GETCONFIGURATIONMAXKEYS, READ_WRITE, TYPE_BOOL_ONE, value, GETCONFIGURATIONMAXKEYS);
#endif
	}
	else if (key.equals("HeartbeatInterval"))
	{
		value = String(payload["value"].as<String>()).toInt();
		if (value == 0)
		{
			// Serial.println("Setting MeterValueSampleInterval to Max");
			// meterSampleInterval = 43200;
			// acceptance = true;
			Serial.println(F("[ChangeConfig] Rejecting the change as invalid value has been received!"));
			acceptance = false;
			rejected = true;
		}
		else
		{
			Serial.println("[ChangeConfig] Setting heartbeatInterval: " + String(value));
			// heartbeatInterval = value;
			evse_ocpp_config_HeartbeatInterval = value;
			acceptance = true;
			rejected = false;
			// change_config.begin("configurations", false);
			// change_config.putInt("heartbeatInterval", value);
			// change_config.end();

#if EVSE_OCPP_CONFIG_PREFERENCE == EVSE_TRUE
			evse_preferences_ocpp.begin("HeartbeatInterval", false);
			evse_preferences_ocpp.putUInt("Heartbeat_Interval", evse_ocpp_config_HeartbeatInterval);
			Serial.printf("HeartbeatInterval: %d \n", evse_ocpp_config_HeartbeatInterval);
			evse_preferences_ocpp.end();
#endif
		}
#if EVSE_OCPP_CONFIG_PREFERENCE
		ptrOCPPconfigKeys->OCPP_config_keys_write_param(HEARTBEATINTERVAL, READ_WRITE, TYPE_BOOL_ONE, value, HEARTBEATINTERVAL);
#endif
	}
	else if (key.equals("LocalAuthorizeOffline"))
	{
		value = String(payload["value"].as<String>()).toInt();
		if (value == 0)
		{
			// Serial.println("Setting MeterValueSampleInterval to Max");
			// meterSampleInterval = 43200;
			// acceptance = true;
			Serial.println(F("[ChangeConfig] Rejecting the change as invalid value has been received!"));
			acceptance = false;
			rejected = true;
		}
		else
		{
			Serial.println("[ChangeConfig] Setting LocalAuthorizeOffline: " + String(value));
			// meterSampleInterval = value;
			evse_ocpp_config_LocalAuthorizeOffline = value;
			acceptance = true;
			rejected = false;
			// change_config.begin("configurations", false);
			// change_config.putInt("meterSampleInterval", value);
			// change_config.end();
#if EVSE_OCPP_CONFIG_PREFERENCE == EVSE_TRUE
			evse_preferences_ocpp.begin("LocalAuthorizeOffline", false);
			evse_preferences_ocpp.putBool("LocalAuthorize_Offline", evse_ocpp_config_LocalAuthorizeOffline);
			Serial.printf("LocalAuthorizeOfflin: %d \n", evse_ocpp_config_LocalAuthorizeOffline);
			evse_preferences_ocpp.end();
#endif
		}
#if EVSE_OCPP_CONFIG_PREFERENCE
		ptrOCPPconfigKeys->OCPP_config_keys_write_param(LOCALAUTHORIZEOFFLINE, READ_WRITE, TYPE_BOOL_ONE, value, LOCALAUTHORIZEOFFLINE);
#endif
	}
	else if (key.equals("LocalPreAuthorize"))
	{
		value = String(payload["value"].as<String>()).toInt();
		if (value == 0)
		{
			// Serial.println("Setting MeterValueSampleInterval to Max");
			// meterSampleInterval = 43200;
			// acceptance = true;
			Serial.println(F("[ChangeConfig] Rejecting the change as invalid value has been received!"));
			acceptance = false;
			rejected = true;
		}
		else
		{
			Serial.println("[ChangeConfig] Setting LocalPreAuthorize: " + String(value));
			// meterSampleInterval = value;
			evse_ocpp_config_LocalPreAuthorize = value;
			acceptance = true;
			rejected = false;
			// change_config.begin("configurations", false);
			// change_config.putInt("meterSampleInterval", value);
			// change_config.end();
#if EVSE_OCPP_CONFIG_PREFERENCE == EVSE_TRUE
			evse_preferences_ocpp.begin("LocalPreAuthorize", false);
			evse_preferences_ocpp.putBool("LocalPre_Authorize", evse_ocpp_config_LocalPreAuthorize);
			Serial.printf("LocalPreAuthorizet: %d \n", evse_ocpp_config_LocalPreAuthorize);
			evse_preferences_ocpp.end();
#endif
		}
#if EVSE_OCPP_CONFIG_PREFERENCE
		ptrOCPPconfigKeys->OCPP_config_keys_write_param(LOCALPREAUTHORIZE, READ_WRITE, TYPE_BOOL_ONE, value, LOCALPREAUTHORIZE);
#endif
	}
	else if (key.equals("MeterValuesAlignedData"))
	{
		value = String(payload["value"].as<String>()).toInt();
		if (value == 0)
		{
			// Serial.println("Setting MeterValueSampleInterval to Max");
			// meterSampleInterval = 43200;
			// acceptance = true;
			Serial.println(F("[ChangeConfig] Rejecting the change as invalid value has been received!"));
			acceptance = false;
			rejected = true;
		}
		else
		{
			Serial.println("[ChangeConfig] Setting MeterValuesAlignedData: " + String(value));
			// meterSampleInterval = value;
			evse_ocpp_config_MeterValuesAlignedData = value;
			acceptance = true;
			rejected = false;
			// change_config.begin("configurations", false);
			// change_config.putInt("meterSampleInterval", value);
			// change_config.end();
#if EVSE_OCPP_CONFIG_PREFERENCE == EVSE_TRUE
			evse_preferences_ocpp.begin("MeterValuesAlignedData", false);
			evse_preferences_ocpp.putString("MeterValues_AlignedData", evse_ocpp_config_MeterValuesAlignedData);
			Serial.printf("MeterValuesAlignedData: %d \n", evse_ocpp_config_MeterValuesAlignedData);
			evse_preferences_ocpp.end();
#endif
		}
#if EVSE_OCPP_CONFIG_PREFERENCE
		ptrOCPPconfigKeys->OCPP_config_keys_write_param(METERVALUESALIGNEDDATA, READ_WRITE, TYPE_BOOL_ONE, value, METERVALUESALIGNEDDATA);
#endif
	}
	else if (key.equals("MeterValuesSampledData"))
	{
		value = String(payload["value"].as<String>()).toInt();
		if (value == 0)
		{
			// Serial.println("Setting MeterValueSampleInterval to Max");
			// meterSampleInterval = 43200;
			// acceptance = true;
			Serial.println(F("[ChangeConfig] Rejecting the change as invalid value has been received!"));
			acceptance = false;
			rejected = true;
		}
		else
		{
			Serial.println("[ChangeConfig] Setting MeterValuesSampledData: " + String(value));
			// meterSampleInterval = value;
			evse_ocpp_config_MeterValuesSampledData = value;
			acceptance = true;
			rejected = false;
			// change_config.begin("configurations", false);
			// change_config.putInt("meterSampleInterval", value);
			// change_config.end();
#if EVSE_OCPP_CONFIG_PREFERENCE == EVSE_TRUE
			evse_preferences_ocpp.begin("MeterValuesSampledData", false);
			evse_preferences_ocpp.putString("MeterValues_SampledData", evse_ocpp_config_MeterValuesSampledData);
			Serial.printf("MeterValuesSampledData: %d \n", evse_ocpp_config_MeterValuesSampledData);
			evse_preferences_ocpp.end();
#endif
		}
#if EVSE_OCPP_CONFIG_PREFERENCE
		ptrOCPPconfigKeys->OCPP_config_keys_write_param(METERVALUESSAMPLEDDATA, READ_WRITE, TYPE_BOOL_ONE, value, METERVALUESSAMPLEDDATA);
#endif
	}
	else if (key.equals("MeterValueSampleInterval"))
	{
		value = String(payload["value"].as<String>()).toInt();
		if (value == 0)
		{
			// Serial.println("Setting MeterValueSampleInterval to Max");
			// meterSampleInterval = 43200;
			// acceptance = true;
			Serial.println(F("[ChangeConfig] Rejecting the change as invalid value has been received!"));
			acceptance = false;
			rejected = true;
		}
		else
		{
			Serial.println("[ChangeConfig] Setting MeterValueSampleInterval: " + String(value));
			// meterSampleInterval = value;
			evse_ocpp_config_MeterValueSampleInterval = value;
			;
			acceptance = true;
			rejected = false;
			// change_config.begin("configurations", false);
			// change_config.putInt("meterSampleInterval", value);
			// change_config.end();
#if EVSE_OCPP_CONFIG_PREFERENCE == EVSE_TRUE
			evse_preferences_ocpp.begin("MeterValueSampleInterval", false);
			evse_preferences_ocpp.putUInt("MeterValueSample_Interval", evse_ocpp_config_MeterValueSampleInterval);
			Serial.printf("MeterValueSampleInterval: %d \n", evse_ocpp_config_MeterValueSampleInterval);
			evse_preferences_ocpp.end();
#endif
		}
#if EVSE_OCPP_CONFIG_PREFERENCE
		ptrOCPPconfigKeys->OCPP_config_keys_write_param(METERVALUESAMPLEINTERVAL, READ_WRITE, TYPE_BOOL_ONE, value, METERVALUESAMPLEINTERVAL);
#endif
	}
	else if (key.equals("ResetRetries"))
	{
		value = String(payload["value"].as<String>()).toInt();
		if (value == 0)
		{
			// Serial.println("Setting MeterValueSampleInterval to Max");
			// meterSampleInterval = 43200;
			// acceptance = true;
			Serial.println(F("[ChangeConfig] Rejecting the change as invalid value has been received!"));
			acceptance = false;
			rejected = true;
		}
		else
		{
			Serial.println("[ChangeConfig] Setting ResetRetries: " + String(value));
			// meterSampleInterval = value;
			evse_ocpp_config_ResetRetries = value;
			acceptance = true;
			rejected = false;
			// change_config.begin("configurations", false);
			// change_config.putInt("meterSampleInterval", value);
			// change_config.end();
#if EVSE_OCPP_CONFIG_PREFERENCE == EVSE_TRUE
			evse_preferences_ocpp.begin("ResetRetries", false);
			evse_preferences_ocpp.putUInt("Reset_Retries", evse_ocpp_config_ResetRetries);
			Serial.printf("ResetRetries: %d \n", evse_ocpp_config_ResetRetries);
			evse_preferences_ocpp.end();
#endif
		}
#if EVSE_OCPP_CONFIG_PREFERENCE
		Serial.println("OCPP_config_keys_write_param");
		ptrOCPPconfigKeys->OCPP_config_keys_write_param(RESETRETRIES, READ_WRITE, TYPE_BOOL_ONE, value, RESETRETRIES);
#endif
	}
	else if (key.equals("ConnectorPhaseRotation"))
	{
		value = String(payload["value"].as<String>()).toInt();
		if (value == 0)
		{
			// Serial.println("Setting MeterValueSampleInterval to Max");
			// meterSampleInterval = 43200;
			// acceptance = true;
			Serial.println(F("[ChangeConfig] Rejecting the change as invalid value has been received!"));
			acceptance = false;
			rejected = true;
		}
		else
		{
			Serial.println("[ChangeConfig] Setting ConnectorPhaseRotation: " + String(value));
			evse_ocpp_config_ConnectorPhaseRotation = value;
			acceptance = true;
			rejected = false;
			// change_config.begin("configurations", false);
			// change_config.putInt("meterSampleInterval", value);
			// change_config.end();
#if EVSE_OCPP_CONFIG_PREFERENCE == EVSE_TRUE
			evse_preferences_ocpp.begin("ConnectorPhaseRotation", false);
			evse_preferences_ocpp.putString("ConnectorPhase_Rotation", evse_ocpp_config_ConnectorPhaseRotation);
			Serial.printf("ConnectorPhaseRotation: %d \n", evse_ocpp_config_ConnectorPhaseRotation);
			evse_preferences_ocpp.end();
#endif
		}
#if EVSE_OCPP_CONFIG_PREFERENCE
		ptrOCPPconfigKeys->OCPP_config_keys_write_param(CONNECTORPHASEROTATION, READ_WRITE, TYPE_BOOL_ONE, value, CONNECTORPHASEROTATION);
#endif
	}
	else if (key.equals("StopTransactionOnEVSideDisconnect"))
	{
		value = String(payload["value"].as<String>()).toInt();
		if (value == 0)
		{
			// Serial.println("Setting MeterValueSampleInterval to Max");
			// meterSampleInterval = 43200;
			// acceptance = true;
			Serial.println(F("[ChangeConfig] Rejecting the change as invalid value has been received!"));
			acceptance = false;
			rejected = true;
		}
		else
		{
			Serial.println("[ChangeConfig] Setting StopTransactionOnEVSideDisconnect: " + String(value));
			// meterSampleInterval = value;
			evse_ocpp_config_StopTransactionOnEVSideDisconnect = value;
			acceptance = true;
			rejected = false;
			// change_config.begin("configurations", false);
			// change_config.putInt("meterSampleInterval", value);
			// change_config.end();
#if EVSE_OCPP_CONFIG_PREFERENCE == EVSE_TRUE
			evse_preferences_ocpp.begin("StopTransactionOnEVSideDisconnect", false);
			evse_preferences_ocpp.putBool("StopTransactionOn_EVSideDisconnect", evse_ocpp_config_StopTransactionOnEVSideDisconnect);
			Serial.printf("StopTransactionOnEVSideDisconnect: %d \n", evse_ocpp_config_StopTransactionOnEVSideDisconnect);
			evse_preferences_ocpp.end();
#endif
		}
#if EVSE_OCPP_CONFIG_PREFERENCE
		ptrOCPPconfigKeys->OCPP_config_keys_write_param(STOPTRANSACTIONONEVSIDEDISCONNECT, READ_WRITE, TYPE_BOOL_ONE, value, STOPTRANSACTIONONEVSIDEDISCONNECT);
#endif
	}
	else if (key.equals("StopTransactionOnInvalidId"))
	{
		value = String(payload["value"].as<String>()).toInt();
		if (value == 0)
		{
			// Serial.println("Setting MeterValueSampleInterval to Max");
			// meterSampleInterval = 43200;
			// acceptance = true;
			Serial.println(F("[ChangeConfig] Rejecting the change as invalid value has been received!"));
			acceptance = false;
			rejected = true;
		}
		else
		{
			Serial.println("[ChangeConfig] Setting StopTransactionOnInvalidId: " + String(value));
			// meterSampleInterval = value;
			evse_ocpp_config_StopTransactionOnInvalidId = value;
			acceptance = true;
			rejected = false;
			// change_config.begin("configurations", false);
			// change_config.putInt("meterSampleInterval", value);
			// change_config.end();
#if EVSE_OCPP_CONFIG_PREFERENCE == EVSE_TRUE
			evse_preferences_ocpp.begin("StopTransactionOnInvalidId", false);
			evse_preferences_ocpp.putBool("StopTransactionOn_EVSideDisconnect", evse_ocpp_config_StopTransactionOnInvalidId);
			Serial.printf("StopTransactionOnInvalidId : %d \n", evse_ocpp_config_StopTransactionOnInvalidId);
			evse_preferences_ocpp.end();
#endif
		}
#if EVSE_OCPP_CONFIG_PREFERENCE
		ptrOCPPconfigKeys->OCPP_config_keys_write_param(STOPTRANSACTIONONINVALIDID, READ_WRITE, TYPE_BOOL_ONE, value, STOPTRANSACTIONONINVALIDID);
#endif
	}
	else if (key.equals("StopTxnAlignedData"))
	{
		value = String(payload["value"].as<String>()).toInt();
		if (value == 0)
		{
			// Serial.println("Setting MeterValueSampleInterval to Max");
			// meterSampleInterval = 43200;
			// acceptance = true;
			Serial.println(F("[ChangeConfig] Rejecting the change as invalid value has been received!"));
			acceptance = false;
			rejected = true;
		}
		else
		{
			Serial.println("[ChangeConfig] Setting StopTxnAlignedData: " + String(value));
			// meterSampleInterval = value;
			evse_ocpp_config_StopTxnAlignedData = value;
			acceptance = true;
			rejected = false;
			// change_config.begin("configurations", false);
			// change_config.putInt("meterSampleInterval", value);
			// change_config.end();
#if EVSE_OCPP_CONFIG_PREFERENCE == EVSE_TRUE
			evse_preferences_ocpp.begin("StopTxnAlignedData", false);
			evse_preferences_ocpp.putString("StopTxn_AlignedData", evse_ocpp_config_StopTxnAlignedData);
			Serial.printf("StopTxnSampledData: %d \n", evse_ocpp_config_StopTxnAlignedData);
			evse_preferences_ocpp.end();
#endif
		}
#if EVSE_OCPP_CONFIG_PREFERENCE
		ptrOCPPconfigKeys->OCPP_config_keys_write_param(STOPTXNALIGNEDDATA, READ_WRITE, TYPE_BOOL_ONE, value, STOPTXNALIGNEDDATA);
#endif
	}
	else if (key.equals("StopTxnSampledData"))
	{
		value = String(payload["value"].as<String>()).toInt();
		if (value == 0)
		{
			// Serial.println("Setting MeterValueSampleInterval to Max");
			// meterSampleInterval = 43200;
			// acceptance = true;
			Serial.println(F("[ChangeConfig] Rejecting the change as invalid value has been received!"));
			acceptance = false;
			rejected = true;
		}
		else
		{
			Serial.println("[ChangeConfig] Setting StopTxnSampledData: " + String(value));
			evse_ocpp_config_StopTxnSampledData = value;
			acceptance = true;
			rejected = false;
			// change_config.begin("configurations", false);
			// change_config.putInt("meterSampleInterval", value);
			// change_config.end();
#if EVSE_OCPP_CONFIG_PREFERENCE == EVSE_TRUE
			evse_preferences_ocpp.begin("StopTxnSampledData", false);
			evse_preferences_ocpp.putString("StopTxn_SampledData", evse_ocpp_config_StopTxnSampledData);
			Serial.printf("StopTxnSampledData: %d \n", evse_ocpp_config_StopTxnSampledData);
			evse_preferences_ocpp.end();
#endif
		}
#if EVSE_OCPP_CONFIG_PREFERENCE
		ptrOCPPconfigKeys->OCPP_config_keys_write_param(STOPTXNSAMPLEDDATA, READ_WRITE, TYPE_BOOL_ONE, value, STOPTXNSAMPLEDDATA);
#endif
	}
	else if (key.equals("TransactionMessageAttempts"))
	{
		value = String(payload["value"].as<String>()).toInt();
		if (value == 0)
		{
			// Serial.println("Setting MeterValueSampleInterval to Max");
			// meterSampleInterval = 43200;
			// acceptance = true;
			Serial.println(F("[ChangeConfig] Rejecting the change as invalid value has been received!"));
			acceptance = false;
			rejected = true;
		}
		else
		{
			Serial.println("[ChangeConfig] Setting TransactionMessageAttempts: " + String(value));
			evse_ocpp_config_TransactionMessageAttempts = value;
			acceptance = true;
			rejected = false;
			// change_config.begin("configurations", false);
			// change_config.putInt("meterSampleInterval", value);
			// change_config.end();
#if EVSE_OCPP_CONFIG_PREFERENCE == EVSE_TRUE
			evse_preferences_ocpp.begin("TransactionMessageAttempts", false);
			evse_preferences_ocpp.putUInt("Transaction_MessageAttempts", evse_ocpp_config_TransactionMessageAttempts);
			Serial.printf("TransactionMessageAttempts: %d \n", evse_ocpp_config_TransactionMessageAttempts);
			evse_preferences_ocpp.end();
#endif
		}
#if EVSE_OCPP_CONFIG_PREFERENCE
		ptrOCPPconfigKeys->OCPP_config_keys_write_param(TRANSACTIONMESSAGEATTEMPTS, READ_WRITE, TYPE_BOOL_ONE, value, TRANSACTIONMESSAGEATTEMPTS);
#endif
	}
	else if (key.equals("TransactionMessageRetryInterval"))
	{
		value = String(payload["value"].as<String>()).toInt();
		if (value == 0)
		{
			// Serial.println("Setting MeterValueSampleInterval to Max");
			// meterSampleInterval = 43200;
			// acceptance = true;
			Serial.println(F("[ChangeConfig] Rejecting the change as invalid value has been received!"));
			acceptance = false;
			rejected = true;
		}
		else
		{
			Serial.println("[ChangeConfig] Setting TransactionMessageRetryInterval: " + String(value));
			evse_ocpp_config_TransactionMessageRetryInterval = value;
			acceptance = true;
			rejected = false;
			// change_config.begin("configurations", false);
			// change_config.putInt("meterSampleInterval", value);
			// change_config.end();
#if EVSE_OCPP_CONFIG_PREFERENCE == EVSE_TRUE
			evse_preferences_ocpp.begin("TransactionMessageRetryInterval", false);
			evse_preferences_ocpp.putUInt("TransactionMessage_RetryInterval", evse_ocpp_config_TransactionMessageRetryInterval);
			Serial.printf("TransactionMessageRetryInterval: %d \n", evse_ocpp_config_TransactionMessageRetryInterval);
			evse_preferences_ocpp.end();
#endif
		}
#if EVSE_OCPP_CONFIG_PREFERENCE
		ptrOCPPconfigKeys->OCPP_config_keys_write_param(TRANSACTIONMESSAGERETRYINTERVAL, READ_WRITE, TYPE_BOOL_ONE, value, TRANSACTIONMESSAGERETRYINTERVAL);
#endif
	}
	else if (key.equals("UnlockConnectorOnEVSideDisconnect"))
	{
		value = String(payload["value"].as<String>()).toInt();
		if (value == 0)
		{
			// Serial.println("Setting MeterValueSampleInterval to Max");
			// meterSampleInterval = 43200;
			// acceptance = true;
			Serial.println(F("[ChangeConfig] Rejecting the change as invalid value has been received!"));
			acceptance = false;
			rejected = true;
		}
		else
		{
			Serial.println("[ChangeConfig] Setting UnlockConnectorOnEVSideDisconnect: " + String(value));
			evse_ocpp_config_UnlockConnectorOnEVSideDisconnect = value;
			acceptance = true;
			rejected = false;
			// change_config.begin("configurations", false);
			// change_config.putInt("meterSampleInterval", value);
			// change_config.end();
#if EVSE_OCPP_CONFIG_PREFERENCE == EVSE_TRUE
			evse_preferences_ocpp.begin("UnlockConnectorOnEVSideDisconnect", false);
			evse_preferences_ocpp.putBool("UnlockConnectorOn_EVSideDisconnect", evse_ocpp_config_UnlockConnectorOnEVSideDisconnect);
			Serial.printf("UnlockConnectorOnEVSideDisconnect: %d \n", evse_ocpp_config_UnlockConnectorOnEVSideDisconnect);
			evse_preferences_ocpp.end();
#endif
		}
#if EVSE_OCPP_CONFIG_PREFERENCE
		ptrOCPPconfigKeys->OCPP_config_keys_write_param(UNLOCKCONNECTORONEVSIDEDISCONNECT, READ_WRITE, TYPE_BOOL_ONE, value, UNLOCKCONNECTORONEVSIDEDISCONNECT);
#endif
	}
	else if (key.equals("LocalAuthListEnabled"))
	{
		value = String(payload["value"].as<String>()).toInt();
		if (value == 0)
		{
			// Serial.println("Setting MeterValueSampleInterval to Max");
			// meterSampleInterval = 43200;
			// acceptance = true;
			Serial.println(F("[ChangeConfig] Rejecting the change as invalid value has been received!"));
			acceptance = false;
			rejected = true;
		}
		else
		{
			Serial.println("[ChangeConfig] Setting LocalAuthListEnabled: " + String(value));
			evse_ocpp_config_LocalAuthListEnabled = value;
			acceptance = true;
			rejected = false;
			// change_config.begin("configurations", false);
			// change_config.putInt("meterSampleInterval", value);
			// change_config.end();
#if EVSE_OCPP_CONFIG_PREFERENCE == EVSE_TRUE
			evse_preferences_ocpp.begin("LocalAuthListEnabled", false);
			evse_preferences_ocpp.putBool("LocalAuthList_Enabled", evse_ocpp_config_LocalAuthListEnabled);
			Serial.printf("Local Auth List Enabled: %d \n", evse_ocpp_config_LocalAuthListEnabled);
			evse_preferences_ocpp.end();
#endif
		}
#if EVSE_OCPP_CONFIG_PREFERENCE
		ptrOCPPconfigKeys->OCPP_config_keys_write_param(LOCALAUTHLISTENABLED, READ_WRITE, TYPE_BOOL_ONE, value, LOCALAUTHLISTENABLED);
#endif
	}
}
#endif
#if 1
void ChangeConfiguration::processReq(JsonObject payload)
{
	String key = String(payload["key"].as<String>());
	unsigned int value = 0;

	if (key.equals("MeterValueSampleInterval"))
	{
		value = String(payload["value"].as<String>()).toInt();
		if (value == 0)
		{
			// Serial.println("Setting MeterValueSampleInterval to Max");
			// meterSampleInterval = 43200;
			// acceptance = true;
			Serial.println(F("[ChangeConfig] Rejecting the change as invalid value has been received!"));
			acceptance = false;
			rejected = true;
		}
		else
		{
			Serial.println("[ChangeConfig] Setting MeterValueSampleInterval: " + String(meterSampleInterval));
			meterSampleInterval = value;
			acceptance = true;
			rejected = false;
			change_config.begin("configurations", false);
			change_config.putInt("meterSampleInterval", value);
			change_config.end();
		}
	}
	else if (key.equals("HeartbeatInterval"))
	{
		value = String(payload["value"].as<String>()).toInt();
		if (value == 0)
		{
			// Serial.println("Setting MeterValueSampleInterval to Max");
			// meterSampleInterval = 43200;
			// acceptance = true;
			Serial.println(F("[ChangeConfig] Rejecting the change as invalid value has been received!"));
			acceptance = false;
			rejected = true;
		}
		else
		{
			Serial.println("[ChangeConfig] Setting heartbeatInterval: " + String(value));
			heartbeatInterval = value;
			acceptance = true;
			rejected = false;
			change_config.begin("configurations", false);
			change_config.putInt("heartbeatInterval", value);
			change_config.end();
		}
	}
	else if (key.equals("AuthorizeRemoteTxRequests"))
	{
		// value = String(payload["value"].as<String>()).toInt();
		String val = payload["value"];
		/*if (val == "true")
		{
			// Serial.println("Setting MeterValueSampleInterval to Max");
			// meterSampleInterval = 43200;
			// acceptance = true;
			Serial.println(F("[ChangeConfig] Rejecting the change as invalid value has been received!"));
			acceptance = false;
			rejected = true;
		}
		else
		{
			Serial.println("[ChangeConfig] Setting auth remote start: " + String(val));
			acceptance = true;
			rejected = false;
			change_config.begin("configurations", false);
			change_config.putBool("authRemoteStart", val);
			change_config.end();
		}*/
		if (val == "true" || val == "True" || val == "TRUE")
		{
			Serial.println(F("[ChangeConfiguration] AuthorizeRemoteTxRequests set to true"));
			acceptance = true;
			rejected = false;
			change_config.begin("configurations", false);
			change_config.putBool("authRemoteStart", true);
			change_config.end();
			// flag_AuthorizeRemoteTxRequests = true;
		}
		else if (val == "false" || val == "False" || val == "FALSE")
		{
			Serial.println(F("[ChangeConfiguration] AuthorizeRemoteTxRequests set to false"));
			acceptance = true;
			rejected = false;
			change_config.begin("configurations", false);
			change_config.putBool("authRemoteStart", false);
			change_config.end();
			// flag_AuthorizeRemoteTxRequests = false;
		}
		else
		{
			Serial.println(F("[ChangeConfiguration] AuthorizeRemoteTxRequests Rejected the change config!"));
			Serial.println(String(payload["value"].as<String>()));
			acceptance = false;
			rejected = true;
		}
	}
	else
	{
		// to give scope for not supported.
		rejected = false;
		acceptance = false;
	}
}
#endif

DynamicJsonDocument *ChangeConfiguration::createConf()
{

	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();
	if (acceptance)
	{
		payload["status"] = "Accepted";
	}
	else
	{
		if (rejected)
			payload["status"] = "Rejected";
		else
			payload["status"] = "NotSupported";
	}
	acceptance = false;
	return doc;
}

DynamicJsonDocument *ChangeConfiguration::createReq()
{

	/****Can be implemented for testing purpose****/
}

void ChangeConfiguration::processConf(JsonObject payload)
{

	/****Can be implemented for testing purpose****/
}

