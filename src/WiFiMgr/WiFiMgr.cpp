#include <WiFiMgr.h>

#include "WiFiMgr_Priv.h"

/* Global Variables */
WiFiMgr_Data_Type WiFiMgr_Data;



IPAddress Esp_ip(192, 168, 1, 185);
IPAddress gateway_ip(192, 168, 1, 1);
IPAddress subnet_mask(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   
IPAddress secondaryDNS(8, 8, 4, 4);

uint8 WiFiMgr_OneTimeCounter = 1U;
String WiFiMgr_SerialPrint = "";

/* Functions */
/********************************************************
* NvM Init function. 
********************************************************/
void WiFiMgr_Init(void)
{
#ifdef SERIAL_PRINT_WIFIMGR
    WiFiMgr_SerialPrint = "WiFiMgr: Init Start...";
    Serial.print(WiFiMgr_SerialPrint);
#endif

    /********** Static IP **********/
    /* WiFi Configuration */
    WiFi.mode(WIFI_STA);
    
    //if(!WiFi.config(Esp_ip, gateway_ip, subnet_mask, primaryDNS, secondaryDNS))
    if(!WiFi.config(Esp_ip, gateway_ip, subnet_mask))
    {
#ifdef SERIAL_PRINT_WIFIMGR
        WiFiMgr_SerialPrint = " !!!Failed WiFi config!!! ";
        Serial.print(WiFiMgr_SerialPrint);
#endif
    }
    else
    {
        /* do nothing */
    }
    
    // WiFi.softAP(WiFi_Name, WiFi_Password);
    WiFi.begin(WiFi_Name, WiFi_Password);

    WiFiMgr_Data.State = WIFIMGR_STATE_INIT;
    WiFiMgr_Data.WiFiState = WIFI_STATUS_NOT_CONNECTED;
    WiFiMgr_Data.u16ResetCounter = 0U;

#ifdef SERIAL_PRINT_WIFIMGR
    WiFiMgr_SerialPrint = "  ...Stop.";
    Serial.println(WiFiMgr_SerialPrint);
#endif
}


/********************************************************
* NvM Main function. 
********************************************************/
void WiFiMgr_Main(void)
{
    if (0U < WiFiMgr_OneTimeCounter)
    {
        WiFiMgr_OneTimeCounter = 0U;
#ifdef SERIAL_PRINT_WIFIMGR
        WiFiMgr_SerialPrint = "WiFiMgr: Loop Start.";
        Serial.println(WiFiMgr_SerialPrint);
#endif
    }
    else
    {
        /* do nothing */
    }

    switch (WiFiMgr_Data.State)
    {
        case WIFIMGR_STATE_INIT:
        {
            if(WiFi.status() != WL_CONNECTED)
            {
                WiFiMgr_Data.WiFiState = WIFI_STATUS_NOT_CONNECTED;
                Rte_IWrite_WiFiMgr_WifiStatus = WIFI_STATUS_NOT_CONNECTED;
            }
            else
            {
                WiFiMgr_Data.WiFiState = WIFI_STATUS_CONNECTED;
                Rte_IWrite_WiFiMgr_WifiStatus = WIFI_STATUS_CONNECTED;
            }

            WiFiMgr_Data.State = WIFIMGR_STATE_RUN;
            break;
        }
        case WIFIMGR_STATE_RUN:
        {
            wifimgr_CheckWiFiState();
            wifimgr_NoWiFiResetHandling();
            break;
        }        
        default:
            break;
    }
}
/********************************************************
* WiFiMagr chech connection status to the WiFI. 
********************************************************/
void wifimgr_CheckWiFiState(void)
{
    if(WiFi.status() != WL_CONNECTED)
   {
       if (WIFI_STATUS_NOT_CONNECTED != WiFiMgr_Data.WiFiState)
       {
            WiFiMgr_Data.WiFiState = WIFI_STATUS_NOT_CONNECTED;
            Rte_IWrite_WiFiMgr_WifiStatus = WIFI_STATUS_NOT_CONNECTED;
       }
       else
       {
           /* do nothing */
       }
#ifdef SERIAL_PRINT_WIFIMGR
        WiFiMgr_SerialPrint = "WiFiMgr: WiFI Not connected ";
        //Serial.println(WiFiMgr_SerialPrint);
#endif
   }
   else
   {
        if (WIFI_STATUS_CONNECTED != WiFiMgr_Data.WiFiState)
       {
            WiFiMgr_Data.WiFiState = WIFI_STATUS_CONNECTED;
            Rte_IWrite_WiFiMgr_WifiStatus = WIFI_STATUS_CONNECTED;
       }
       else
       {
           /* do nothing */
       }

#ifdef SERIAL_PRINT_WIFIMGR
        WiFiMgr_SerialPrint = "WiFiMgr: WiFi Connected ";
        //Serial.println(WiFiMgr_SerialPrint);
#endif

   }

}

void wifimgr_NoWiFiResetHandling(void)
{
    if (WIFI_STATUS_CONNECTED != WiFiMgr_Data.WiFiState)
    {
        if (WIFIMGR_RESET_TIMER_THRESHOLD > WiFiMgr_Data.u16ResetCounter)
        {
            WiFiMgr_Data.u16ResetCounter++;
        }
        else
        {
#ifdef SERIAL_PRINT_WIFIMGR
            WiFiMgr_SerialPrint = "WiFiMgr: Reset...";
            Serial.println(WiFiMgr_SerialPrint);
#endif
            WiFiMgr_Data.u16ResetCounter = 0U;
            /* do reset */
            ESP.restart();
        }
        
    }
    else
    {
        WiFiMgr_Data.u16ResetCounter = 0U;
    }
#ifdef SERIAL_PRINT_WIFIMGR
        WiFiMgr_SerialPrint = "WiFiMgr: Counter at second: " + String(WiFiMgr_Data.u16ResetCounter);
        //Serial.println(WiFiMgr_SerialPrint);
#endif
    
}
