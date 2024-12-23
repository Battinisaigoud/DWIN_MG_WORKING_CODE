

#include "Variants.h"

#include "FOTA_OCPP.h"
#include "OcppEngine.h"
//#include "urlparse.h"

 #include <Preferences.h>

// #include "../Evse_Preferences/Evse_Preferences.h"

extern Preferences evse_preferences_ble;
extern Preferences evse_preferences_ws;

bool fota_available = false;
String uri;

FotaNotification  Fota_Notification;

/*
 * @breif: Instantiate an object FotaNotification
 */ 

FotaNotification::FotaNotification() {

}

/*
 * @breif: Method - getOcppOperationType => This method gives the type of Ocpp operation
 */ 

const char* FotaNotification::getOcppOperationType(){
	return "FotaNotification";
}

/*
 * @breif: Method - createReq => This method creates Fota request to be sent to the OCPP server
 * Field Name   | Field Type         | Card. | Description
 * location     | anyURI(char *)     | 1..1  | Required. This contains a string containing a URI pointing to a location from which to retrieve the firmware.
 * retries      | integer(uint_8t)   | 0..1  | Optional. This specifies how many times Charge Point must try to download the firmware before giving up. If this field is not present, it i left to Charge Point to decide how many time it wants to retry.
 * retrieveDate | dateTime(struct dt)| 1..1  | Required. This contains the date and time after which the Charge Point must retrieve the (new) firmware.
   getJsonDateStringFromSystemTime(currentTime, JSONDATE_LENGTH);
 * 
 * retryInterval| integer(uint_8t)   | 0..1  | Optional. The interval in seconds after which a retry may be attempted. If this field is not present, it is left to Charge Point to decide how long to wait between attempts.
 * customData OCPP2.0 | DataTransfer message OCPP1.6 https://www.youtube.com/watch?v=WKt5DI6qTbk
 * [User-Agent] => ESP32-OCPP-Update
[x-ESP32-STA-MAC] => 18:FE:AA:AA:AA:AA
[x-ESP32-AP-MAC] => 1A:FE:AA:AA:AA:AA
[x-ESP32-free-space] => 671744
[x-ESP32-sketch-size] => 373940
[x-ESP32-sketch-md5] => a56f8ef78a0bebd812f62067daf1408a
[x-ESP32-chip-size] => 4194304
[x-ESP32-sdk-version] => 1.3.0
[x-ESP32-version] => DOOR-7-g14f53a19
[x-ESP32-mode] => sketch
* Ex: "createReq": {
"properties": {
"customData": {
"$ref": "#/definitions/CustomDataType"
},
"location": {
"type": "char *"
},
"retries": {
"type": "number"
},
"retrieveDate": {
"type": "dateTime"
},
"phaseToUse": {
"type": "integer"
}
},
"required": [
"location",
"retrieveDate"
]
}
Modified bootnotification : 
[2,"531531531","BootNotification",{"chargePointVendor":"Agrawal","chargePointSerialNumber":"dummyLocal2","chargePointModel":"Pulkit"},"User-Agent":"ESP32-OCPP-Update","x-ESP32-STA-MAC":"18:FE:AA:AA:AA:AA","x-ESP32-AP-MAC":"1A:FE:AA:AA:AA:AA","x-ESP32-free-space":"671744","x-ESP32-sketch-size":"373940","x-ESP32-sketch-md5":"a56f8ef78a0bebd812f62067daf1408a","x-ESP32-chip-size":"4194304","x-ESP32-sdk-version":"1.3.0","x-ESP32-version":"POD_V2_4","x-ESP32-mode","sketch"]
 */ 

void FotaNotification::processReq(JsonObject payload) {

	// For now the object size is 2, but with custom data, it will increase
	//DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(3) + strlen(EVSE_getChargePointVendor()) + 1 + cpSerial.length() + 1 + strlen(EVSE_getChargePointModel()) + 1);
	/*int url_length = 0;
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(2) + (JSONDATE_LENGTH + 1) + url_length);
	JsonObject payload = doc->to<JsonObject>();
	payload["location"] = "https://url";
	char currentTime[JSONDATE_LENGTH + 1] = {'\0'};
	getJsonDateStringFromSystemTime(currentTime, JSONDATE_LENGTH);
	payload["retrieveDate"] = currentTime;*/
	//Need nested json for customData here
	
	 

	//const char *uri = payload["location"] | "Invalid";
	uri = String(payload["location"].as<String>());
	uint8_t retries = payload["retries"];
	const char *date = payload["retrieveDate"];
    String fota_str = uri;

    evse_preferences_ble.begin("fota_url", false);
    evse_preferences_ble.putString("fota_uri", uri);
    evse_preferences_ble.putUInt("fota_retries", retries);
    evse_preferences_ble.putString("fota_date", date);
    evse_preferences_ble.putBool("fota_avial", true);
    evse_preferences_ble.end();

    Serial.println(F("[FOTA_OCPP] got the uri to update FOTA"));
    Serial.println(uri);
#if 0
	//  String fota_str = uri; 
  // String str = "ws://34.100.138.28:8080/steve/websocket/CentralSystemService/ac001rdtest";
 
  if(fota_str.equals("") == true)
  {
	Serial.println(F("[FOTA_OCPP] Error while uri Processing..! "));
    
    return;
  }

  int n = fota_str.length() + 1 ;
  char fota_str_char[n];
  char *fota_token;
  char *fota_rest = fota_str_char;
  strcpy(fota_str_char, fota_str.c_str());
    
  char fota_protocol[10];
  fota_token = strtok_r(fota_rest, "/", &fota_rest);
  // token = strtok_r(rest, "/", &rest); // Added for eliminate "//"
  strcpy(fota_protocol, fota_token);
  Serial.println("protocol : " +String(fota_protocol));
//   fota_token = strtok_r(fota_rest, ":", &fota_rest);
  fota_token = strtok_r(fota_rest, "/", &fota_rest);
  char fota_host[24];
  strcpy(fota_host, fota_token);
  Serial.println("host : " +String(fota_host));

#if 0
  char fota_port[24];
  fota_token = strtok_r(fota_rest, "/", &fota_rest);
  strcpy(fota_port, fota_token);
  Serial.println("port : " +String(fota_port));
  String fota_port_str = String(80);
#endif

  char fota_path[64];
  strcpy(fota_path, fota_rest);
  Serial.println("path : " +String(fota_path));

  String fota_host_n = String(fota_host);
  String fota_path_n = "/" + String (fota_path);
  uint32_t fota_port_n =  80;

  Serial.println("fota_host_n : " +String(fota_host_n));
  Serial.println("fota_path_n : " +String(fota_path_n));
  Serial.println("fota_port_n : " +String(fota_port_n));

  
  evse_preferences_ws.begin("fota_url_parser",false);
  evse_preferences_ws.putString("fota_host",fota_host_n);
  evse_preferences_ws.putString("fota_path", fota_path_n);
  evse_preferences_ws.putUInt("fota_port", fota_port_n);
  evse_preferences_ws.end();

  String host_check = "";
  int port_check = 0;
  String path_check = "";

  evse_preferences_ws.begin("fota_url_parser",false);
  host_check = evse_preferences_ws.getString("fota_host","");
  path_check = evse_preferences_ws.getString("fota_path", "");
  port_check = evse_preferences_ws.getUInt("fota_port", 0);
  evse_preferences_ws.end();

  Serial.println("host_check : " +String(host_check));
  Serial.println("path_check : " +String(path_check));
  Serial.println("port_check : " +String(port_check));

#endif
	
	
}

void FotaNotification::processConf(JsonObject payload){
	/*
	 * OTA update should be processed.
	 */ 
}

DynamicJsonDocument*  FotaNotification::createReq(JsonObject payload){
	/*
	* Ignore Contents of this Req-message, because this is for debug purposes only
	*/
}

#if 1
/*
* @breif: Added by G. Raja Sumant on the lines of heartbeat which has
* No fields are defined.
*/

DynamicJsonDocument* FotaNotification::createConf(){
	//As per OCPP 1.6 no conf is being given similar to Heartbeat
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(5) + (JSONDATE_LENGTH + 1));
  JsonObject payload = doc->to<JsonObject>();
   payload["status"] = "Accepted";
  fota_available = true;
 // urlparser_fota(uri);
  /*
   * Empty payload
   */
  return doc;
}
#endif


#if 0
void FotaNotification::createConf(JsonObject payload){
	/*
	 * OTA update should be processed.
	 */ 
	
}
#endif 