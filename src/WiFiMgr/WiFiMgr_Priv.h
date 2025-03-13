#ifndef WIFIMGR_PRIV_H
#define WIFIMGR_PRIV_H 1
/**************** Includes ************/
#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "CompilerSwitch.h"


/**************** Defines ************/
#define WIFIMGR_STATE_INIT                      1U
#define WIFIMGR_STATE_RUN                       2U

#define WIFIMGR_RESET_TIMER_THRESHOLD           600U
/**************** Varables ************/
typedef struct WiFiMgr_Data_Type_Tag
{
    uint8       State; 
    uint8       WiFiState; 
    uint16       u16ResetCounter; 
}WiFiMgr_Data_Type;

extern uint8 Rte_WiFiMgr_WiFiStatus;
#define Rte_IWrite_WiFiMgr_WifiStatus Rte_WiFiMgr_WiFiStatus

/// TCP ///
//------//

/// Blynk ///
//--------//

/**************** Functions ************/
void wifimgr_CheckWiFiState(void);
void wifimgr_NoWiFiResetHandling(void);



#endif