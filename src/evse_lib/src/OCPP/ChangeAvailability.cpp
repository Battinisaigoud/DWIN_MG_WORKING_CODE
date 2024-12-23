// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\ChangeAvailability.cpp"
/*
 * ChangeAvailability.cpp
 *
 * Copyright 2022 raja <raja@raja-IdeaPad-Gaming-3-15IMH05>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#include "Variants.h"
#include "../../../evse_src/evse_config.h"
#include "ChangeAvailability.h"
#include "OcppEngine.h"



extern ChargePointStatus evse_ChargePointStatus;
extern EvseDevStatuse EvseDevStatus_connector_1;


ChangeAvailability Change_Availability;
uint8_t gu8_change_avaliable =0;

/*
 * @breif: Instantiate an object ChangeAvailability
 */

ChangeAvailability::ChangeAvailability()
{

}

/*
 * @breif: Method - getOcppOperationType => This method gives the type of Ocpp operation
 */

const char *ChangeAvailability::getOcppOperationType()
{
    return "ChangeAvailability";
}

void ChangeAvailability::processReq(JsonObject payload)
{

    // Required field
    connectorId = payload["connectorId"].as<int>();

    if (connectorId != 1)
    {
        accepted = false;
    }
    else
    {
        accepted = true;
    }
    // Required field
    const char *type = payload["type"] | "Invalid";
    if (!strcmp(type, "Inoperative"))
    {
        // First check the current status.
        if ((getChargePointStatusService()->inferenceStatus() == ChargePointStatus::Available || 
             getChargePointStatusService()->inferenceStatus() == ChargePointStatus::Charging))
        {
            // set the status to unavailable. Status notification will take care of the rest.
            // requestLed(BLINKYWHITE, START, 1);
            getChargePointStatusService()->setUnavailable(true);
            accepted = true;
            if(EvseDevStatus_connector_1 == flag_EVSE_Request_Charge )
            {
                EvseDevStatus_connector_1 = flag_EVSE_Stop_Transaction;
                gu8_change_avaliable = EVSE_CMS_CHANGE_AVAILABLITY_ONE;
                
            }
            else
            {
                evse_ChargePointStatus = Unavailable;
                gu8_change_avaliable = EVSE_CMS_CHANGE_AVAILABLITY_TWO;
            }
            // gu8_change_avaliable = true;
                     
        }
        else
        {
            accepted = false;
        }
    }
    else if (!strcmp(type, "Operative"))
    {
        // change the status if it is unavailable only.
        if ((getChargePointStatusService()->inferenceStatus() == ChargePointStatus::Unavailable))
        {
            // It now becomes available.
            getChargePointStatusService()->setUnavailable(false);
            accepted = true;
            evse_ChargePointStatus = Available;
            gu8_change_avaliable = EVSE_CMS_CHANGE_AVAILABLITY_ZERO;
            // Serial.print("Available6");
        }
        else
        {
            accepted = false;
        }
    }

    Serial.println(F("[ChangeAvailability] got the request to change availability"));
}

void ChangeAvailability::processConf(JsonObject payload)
{
    /*
     * Change availability update should be processed.
     */
}

DynamicJsonDocument *ChangeAvailability::createReq(JsonObject payload)
{
    /*
     * Ignore Contents of this Req-message, because this is for debug purposes only
     */
}

/*
 * @breif: Added by G. Raja Sumant on the lines of ChangeConfiguration which has
 * 1 field defined as Enum - Accepted/Rejected/Scheduled.
 */
DynamicJsonDocument *ChangeAvailability::createConf()
{
    // As per OCPP 1.6 it is being given similar to ChangeConfiguration
    DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
    JsonObject payload = doc->to<JsonObject>();
    if (accepted)
	{
		payload["status"] = "Accepted";
	}
	else
	{
		
		payload["status"] = "Rejected";
	}
	accepted = false; // reset the flag
    return doc;
}
