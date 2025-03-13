#ifndef BLYNKCLIENT_PRIV_H
#define BLYNKCLIENT_PRIV_H 1
/**************** Includes ************/
#include <Arduino.h>
#include <BlynkSimpleEsp8266.h>
#include <Rte_Types.h>

#include "CompilerSwitch.h"



/**************** Defines ************/
/*
#define BLYNKCLIENT_PORT    8080U
#define IP_1st              192U
#define IP_2nd              168U
#define IP_3rd              0U
#define IP_4th              123U
//#define IP_4th              131U
*/

#define BLYNKCLIENT_STATE_INIT              1U
#define BLYNKCLIENT_STATE_CONNECTED         2U
#define BLYNKCLIENT_STATE_NOT_CONNECTED     3U

#define BLYNKCLIENT_DATA_SEND_ST1           0U
#define BLYNKCLIENT_DATA_SEND_ST2           1U
#define BLYNKCLIENT_DATA_SEND_ST3           2U
#define BLYNKCLIENT_DATA_SEND_ST4           3U


#define BLYNKCLIENT_RECONNECT_WAIT_TIME     3U

#define BLYNKCLIENT_NO_NOTIFICATION         0U

#define BLYNKCLIENT_SYNC_WAIT               5U

#define BLYNKCLIENT_MASTER_STATUS_OK        1U /* Value sent every 1m to make sure master status is OK. */

#define BLYNKCLIENT_TEMP_PINS               {BLYNKCLIENT_SLAVE1_TEMP_PIN, BLYNKCLIENT_SLAVE2_TEMP_PIN, BLYNKCLIENT_SLAVE3_TEMP_PIN} /* Value sent every 1m to make sure master status is OK. */
#define BLYNKCLIENT_QF_PINS                 {BLYNKCLIENT_SLAVE1_QF_PIN, BLYNKCLIENT_SLAVE2_QF_PIN, BLYNKCLIENT_SLAVE3_QF_PIN} /* Value sent every 1m to make sure master status is OK. */



/**************** Varables ************/

typedef struct BlynkClient_Data_Type_Tag
{
    uint8       State; 
    uint8       Notification;
    uint8       SyncCounter;
    uint8       ReconnectCounter;
    uint8       u8DataSendingState;
}BlynkClient_Data_Type;

typedef struct BlynkClient_Slave_Data_Pins_Tag{
    float TempPin;
    uint8 QfPin;
}BlynkClient_Slave_Data_Pins_Type;


extern uint8 Rte_BlynkClient_ActionFlags;
#define Rte_IRead_BlynkClient_ActionFlags Rte_BlynkClient_ActionFlags

extern uint8 Rte_BlynkClient_ServerConnStatus;
#define Rte_IWrite_BlynkClient_ServerConnStatus Rte_BlynkClient_ServerConnStatus

extern Rte_TcpServer_Reading_Type Rte_TcpServer_Reading[BLYNKCLIENT_NB_SLAVES];
#define Rte_IRead_BlynkClient_TcpServer_Reading    Rte_TcpServer_Reading

extern Rte_Thermo_Master_Info_Type Rte_Thermo_Master_Info;
#define Rte_IRead_BlynkClient_Thermo_Master_Info   Rte_Thermo_Master_Info

/// WiFiMgr ///
extern uint8 Rte_WiFiMgr_WiFiStatus;
#define Rte_IRead_BlynkClient_WifiStatus Rte_WiFiMgr_WiFiStatus
//----------//



/**************** Functions ************/
void blynkclient_CheckBlinkState(void);
void blynkclient_InitSyncDatas(void);
void blynkclient_HandleDataSending(void);
void blynkclient_HandleMasterHeatingState(void);
#endif