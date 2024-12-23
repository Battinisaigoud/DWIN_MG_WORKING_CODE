
#include "ClearCache.h"
#include "OcppEngine.h"



ClearCache  Clear_Cache;

//extern bool ClearCache_status;
ClearCache::ClearCache()
{
}

const char *ClearCache::getOcppOperationType()
{
    return "ClearCache";
}

DynamicJsonDocument *ClearCache::createReq()
{
    DynamicJsonDocument *doc = new DynamicJsonDocument(0);
    JsonObject payload = doc->to<JsonObject>();
    /*
     * empty payload
     */
    return doc;
}

void ClearCache::processReq(JsonObject payload)
{
    Serial.println(F("[ClearCache]*******Received ClearCache Request|However, nothing to clear*******"));
    //accepted = true;
    
}

void ClearCache::processConf(JsonObject payload)
{
    String status = payload["status"] | "Invalid";

    if (status.equals("Accepted"))
    {
        if (DEBUG_OUT)
            Serial.print("[ClearCache] Request has been accepted!\n");
    }
    else
    {
        Serial.print("[ClearCache] Request has been denied!");
    }
}

DynamicJsonDocument *ClearCache::createConf()
{
    // As per OCPP 1.6J it is being given similar to GetConfiguration
    DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
    JsonObject payload = doc->to<JsonObject>();
    //if (accepted)
    #if 0
    if(ClearCache_status)	{
		payload["status"] = "Accepted";
	}
	else
	{
		payload["status"] = "Rejected";
	}
     #endif
	//accepted = false; // reset the flag
    return doc;
   
}
