#ifndef TCPSERVER_PRIV_H
#define TCPSERVER_PRIV_H 1
/**************** Includes ************/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Rte_Types.h>

#include "CompilerSwitch.h"


/**************** Defines ************/
#define TCPSERVER_STATE_INIT                    1U
#define TCPSERVER_STATE_RUN                     2U
#define TCPSERVER_STATE_NOT_CONNECTED           3U

#define TCPSERVER_SERVER_PORT                   1995U

#define TCPSERVER_POSITIVE_RESP_OFFSET          10U




/**************** Varables ************/
typedef struct TcpServer_Data_Type_Tag
{
    uint8       State; 
}TcpServer_Data_Type;

/// TCP ///
extern Rte_TcpServer_Reading_Type Rte_TcpServer_Reading[BLYNKCLIENT_NB_SLAVES];
#define Rte_IWrite_TcpServer_Reading    Rte_TcpServer_Reading
//------//


extern uint8 Rte_WiFiMgr_WiFiStatus;
#define Rte_IRead_WiFiMgr_WifiStatus Rte_WiFiMgr_WiFiStatus

//--------//

/**************** Functions ************/
void tcpserver_ProcessReceivedData(uint8 Id, uint16 Temperature, uint8 Qf);

#endif