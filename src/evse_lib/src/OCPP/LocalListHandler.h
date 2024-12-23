


#ifndef LOCALLISTHANDLER_H
#define LOCALLISTHANDLER_H

#include <ArduinoJson.h>
#include <Arduino.h>
class IdTagInfoClass {
private:
	String idTag;
	String idTagStatus;
	String idExpDate;
	
public:
	void setIdTagInfo (String idTag, String idTagStatus, String idExpDate);
	void getIdTagInfo();
	void processJson(char *json);

};






#endif