
#include "urlparse.h"
#include <Arduino.h>
#include <Preferences.h>

// #include "../Evse_Preferences/Evse_Preferences.h"

extern Preferences evse_preferences_ble;
// extern Preferences preferences;

extern Preferences evse_preferences_ws;

#if 0
extern String host_m;
extern int port_m;
//extern String protocol_m;
extern String path_m;
#endif

#if 0
char host[128];
char port[24];
#endif

extern String host_m;
extern int port_m;
String CP_Id_m;
// extern String protocol_m;
extern String path_m;

char host[128];
char port[24];
char CP_Id[128];

#if 0
void urlparser()
{
  String str = "";
  // str = "";
  preferences.begin("credentials", false);
  str = preferences.getString("ws_url_prefix", "");
  // String str = "ws://34.100.138.28:8080/steve/websocket/CentralSystemService/ac001rdtest";
  if (str.equals("") == true)
  {
    return;
  }
  int n = str.length() + 1;
  char str_char[n];
  char *token;
  char *rest = str_char;
  strcpy(str_char, str.c_str());
  char protocol[10];
  token = strtok_r(rest, "/", &rest);
  strcpy(protocol, token);   // ws:
  Serial.println("protocol : " + String(protocol));
  char port_sample[5] = "80";

  if (strchr(rest, ':') || strchr(rest, '/'))
  {
    if (strchr(rest, ':'))
    {
      token = strtok_r(rest, ":", &rest);
      strcpy(host, token + 1);  //34.100.138.28
      Serial.println("host : " + String(host));
      token = strtok_r(rest, "/", &rest);
      strcpy(port, token);      //8080
      Serial.println("port : " + String(port));
    }
    else
    {
      token = strtok_r(rest, "/", &rest);
      strcpy(host, token);
      Serial.println("host : " + String(host));
      strcpy(port, port_sample);

      Serial.println("port : " + String(port));
      Serial.println("port : " + String(port));
    }
  }
  String port_str = port;

  char path[128];
  strcpy(path, rest);
  Serial.println("path : " + String(path)); //steve/websocket/CentralSystemService/ac001rdtest
  // ..................................

token = strrchr(rest,'/');
strcpy(CP_Id, token + 1);
Serial.println("CP_Id : " + String(CP_Id));

  // ...................................
  host_m = String(host);
  path_m = "/" + String(path);
  port_m = port_str.toInt();
  CP_Id_m = String(CP_Id);
  Serial.println("host_m : " + String(host_m));
  Serial.println("path_m : " + String(path_m));
  Serial.println("port_m : " + String(port_m));
  Serial.println("CP_Id_m : " + String(CP_Id_m));


}

#endif

#if 0
void urlparser()
{
  String str = "";
  // str = "";
  preferences.begin("credentials", false);
  str = preferences.getString("ws_url_prefix", "");
  preferences.end();
  // String str = "ws://34.100.138.28:8080/steve/websocket/CentralSystemService/ac001rdtest";
  if (str.equals("") == true)
  {
    return;
  }
  int n = str.length() + 1;
  char str_char[n];
  char *token;
  char *rest = str_char;
  strcpy(str_char, str.c_str());
  char protocol[10];
  token = strtok_r(rest, "/", &rest);
  // token = strtok_r(rest, "/", &rest); // Added for eliminate "//"
  strcpy(protocol, token);
  Serial.println("protocol : " + String(protocol));
  token = strtok_r(rest, "/", &rest);
  char host_port[160];
  strcpy(host_port, token + 1);
  Serial.println("host_port : " + String(host_port)); 
  
  char path[128];
  strcpy(path, rest);
  Serial.println("path : " + String(path));
  
  uint8_t slength =  strlen(host_port); 
  uint8_t luflag = 0;
  char host[128];
  char port[24] = "80";
  uint8_t jidx = 0;
  
  Serial.println("host_port length : " + String(slength));

  for(uint8_t idx = 0;  idx < slength; idx++)
  {
    if(luflag == 0)
    {
      host[idx] = host_port[idx];
      Serial.println("host is Updating...! ");
    }
    if(host_port[idx] == ':')
    {
      Serial.println("':' is Found ");
      luflag = 1;
    }
    
    if(luflag == 1)
    {
      port[jidx] = host_port[idx];
      jidx++;
      Serial.println("Port is Updating...! ");
    }
  }

  if(luflag == 0)
  {
    port[0] = '8';
    port[1] = '0';
  }
  String port_str = port;
  
  host_m = String(host);
  path_m = "/" + String(path);
  port_m = port_str.toInt();

  Serial.println("host_m : " + String(host_m));
  Serial.println("path_m : " + String(path_m));
  Serial.println("port_m : " + String(port_m));
  preferences.begin("ws_cred", false);
  preferences.putString("host_m", host_m);
  preferences.putString("path_m", path_m);
  preferences.putUInt("port_m", port_m);
  preferences.putString("protocol_m", "ocpp1.6");
  preferences.end();
  String host_n = "";
  int port_n = 0;
  String protocol_n = "";
  String path_n = "";
  preferences.begin("ws_cred", false);
  host_n = preferences.getString("host_m", "");
  path_n = preferences.getString("path_m", "");
  port_n = preferences.getUInt("port_m", 0);
  protocol_n = preferences.getString("protocol_m", "");
  preferences.end();
  Serial.println("host_n : " + String(host_n));
  Serial.println("path_n : " + String(path_n));
  Serial.println("port_n : " + String(port_n));
  Serial.println("protocol_n : " + String(protocol_n));
}
#endif
#if 0
#if 0
void urlparser(){
  
  String str = "";
  // str = "";
  
  evse_preferences_ble.begin("wifi_cred",false);
  str = evse_preferences_ble.getString("ws_url_prefix","");
  evse_preferences_ble.end();
  // String str = "ws://34.100.138.28:8080/steve/websocket/CentralSystemService/ac001rdtest";
 
  if(str.equals("") == true)
  {
    return;
  }

  int n = str.length() + 1 ;
  char str_char[n];
  char *token;
  char *rest = str_char;
  strcpy(str_char, str.c_str());
    
  char protocol[10];
  token = strtok_r(rest, "/", &rest);
  // token = strtok_r(rest, "/", &rest); // Added for eliminate "//"
  strcpy(protocol, token);
  Serial.println("protocol : " +String(protocol));
  token = strtok_r(rest, ":", &rest);
  
  char host_port[160];
  strcpy(host_port, token + 1);
  Serial.println("host_port : " + String(host_port)); 
  
  char path[128];
  strcpy(path, rest);
  Serial.println("path : " + String(path));
  
  uint8_t slength =  strlen(host_port); 
  uint8_t luflag = 0;
  char host[128];
  char port[24] = "80";
  uint8_t jidx = 0;
  
  Serial.println("host_port length : " + String(slength));

  for(uint8_t idx = 0;  idx < slength; idx++)
  {
    if(luflag == 0)
    {
      host[idx] = host_port[idx];
      Serial.println("host is Updating...! ");
    }
    if(host_port[idx] == ':')
    {
      Serial.println("':' is Found ");
      luflag = 1;
    }
    
    if(luflag == 1)
    {
      port[jidx] = host_port[idx];
      jidx++;
      Serial.println("Port is Updating...! ");
    }
  }

  if(luflag == 0)
  {
    port[0] = '8';
    port[1] = '0';
  }
  String port_str = port;
  
  host_m = String(host);
  path_m = "/" + String(path);
  port_m = port_str.toInt();
#endif

#if 0
  char host[24];
  strcpy(host, token+1 );
  Serial.println("host : " +String(host));

  char port[24];
  token = strtok_r(rest, "/", &rest);
  strcpy(port, token);
  Serial.println("port : " +String(port));
  String port_str = port;

  char path[64];
  strcpy(path, rest);
  Serial.println("path : " +String(path));

  host_m = String(host);
  path_m = "/" + String (path);
  port_m =  port_str.toInt();
#endif
  // strcpy(host_m, host);
  // strcpy(path_m, "/");
  // strcat(path_m, path);

  Serial.println("host_m : " +String(host_m));
  Serial.println("path_m : " +String(path_m));
  Serial.println("port_m : " +String(port_m));

  
  evse_preferences_ws.begin("ws_cred",false);
  evse_preferences_ws.putString("host_m",host_m);
  evse_preferences_ws.putString("path_m", path_m);
  evse_preferences_ws.putUInt("port_m", port_m);
  evse_preferences_ws.putString("protocol_m", "ocpp1.6");
  evse_preferences_ws.end();

  String host_n = "";
  int port_n = 0;
  String protocol_n = "";
  String path_n = "";

  evse_preferences_ws.begin("ws_cred",false);
  host_n = evse_preferences_ws.getString("host_m","");
  path_n = evse_preferences_ws.getString("path_m", "");
  port_n = evse_preferences_ws.getUInt("port_m", 0);
  protocol_n = evse_preferences_ws.getString("protocol_m", "");
  evse_preferences_ws.end();

  Serial.println("host_n : " +String(host_n));
  Serial.println("path_n : " +String(path_n));
  Serial.println("port_n : " +String(port_n));
  Serial.println("protocol_n : " +String(protocol_n));

}

#endif

#if 1
void urlparser()
{

  String str = "";
  // str = "";

  evse_preferences_ble.begin("wifi_cred", false);
  str = evse_preferences_ble.getString("ws_url_prefix", "");
  evse_preferences_ble.end();
  // String str = "ws://34.100.138.28:8080/steve/websocket/CentralSystemService/ac001rdtest";

  if (str.equals("") == true)
  {
    return;
  }

  int n = str.length() + 1;
  char str_char[n];
  char *token;
  char *rest = str_char;
  strcpy(str_char, str.c_str());

  char protocol[10];
  token = strtok_r(rest, "/", &rest);
  strcpy(protocol, token);
  Serial.println("protocol : " + String(protocol));

  //@brief implementaed by abhigina for port...
  char port_sample[5] = "80";
  if (strchr(rest, ':') || strchr(rest, '/'))
  {
    if (strchr(rest, ':'))
    {
      token = strtok_r(rest, ":", &rest);
      strcpy(host, token + 1);
      Serial.println("host : " + String(host));
      token = strtok_r(rest, "/", &rest);
      strcpy(port, token);
      Serial.println("port : " + String(port));
    }
    else
    {
      token = strtok_r(rest, "/", &rest);
      strcpy(host, token);
      Serial.println("host : " + String(host));
      strcpy(port, port_sample);
      Serial.println("port : " + String(port));
      Serial.println("port : " + String(port));
    }
  }
  String port_str = port;
  char path[128];
  strcpy(path, rest);
  Serial.println("path : " + String(path));
  token = strrchr(rest,'/');
  if (token == NULL)
  {
    Serial.println("token is null : ");
    Serial.println("rest : " + String(rest));
    strcpy(CP_Id, rest);
  }
  else
  {
    Serial.println("token : " + String(token));
    strcpy(CP_Id, token + 1);
  }
  Serial.println("CP_Id : " + String(CP_Id));

  // token = strrchr(rest, '/');
  // strcpy(CP_Id, token + 1);
  // Serial.println("CP_Id : " + String(CP_Id));

  host_m = String(host);
  path_m = "/" + String(path);
  port_m = port_str.toInt();
  CP_Id_m = String(CP_Id);

  Serial.println("host_m : " + String(host_m));
  Serial.println("path_m : " + String(path_m));
  Serial.println("port_m : " + String(port_m));

  evse_preferences_ws.begin("ws_cred", false);
  evse_preferences_ws.putString("host_m", host_m);
  evse_preferences_ws.putString("path_m", path_m);
  evse_preferences_ws.putUInt("port_m", port_m);
  evse_preferences_ws.putString("protocol_m", "ocpp1.6");
  evse_preferences_ws.end();

  String host_n = "";
  int port_n = 0;
  String protocol_n = "";
  String path_n = "";

  evse_preferences_ws.begin("ws_cred", false);
  host_n = evse_preferences_ws.getString("host_m", "");
  path_n = evse_preferences_ws.getString("path_m", "");
  port_n = evse_preferences_ws.getUInt("port_m", 0);
  protocol_n = evse_preferences_ws.getString("protocol_m", "");
  evse_preferences_ws.end();

  Serial.println("host_n : " + String(host_n));
  Serial.println("path_n : " + String(path_n));
  Serial.println("port_n : " + String(port_n));
  Serial.println("protocol_n : " + String(protocol_n));
  Serial.println("CP_Id_m : " + String(CP_Id_m));
}
#endif
