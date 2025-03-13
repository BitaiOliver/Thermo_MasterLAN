#ifndef TASKMGR_PRIV_H
#define TASKMGR_PRIV_H 1
/**************** Includes ************/
#include <Arduino.h>
#include <Rte_Types.h>


/**************** Defines ************/


/**************** Varables ************/
/// OS ///
extern uint8 Rte_TaskMgr_Handler_Flags;
#define Rte_IRead_TaskMgr_Handler_Flags    Rte_TaskMgr_Handler_Flags
//-----//

/// Blynk ///
extern uint8 Rte_BlynkClient_ActionFlags;
#define Rte_IWrite_TaskMgr_BlynkClient_ActionFlags    Rte_BlynkClient_ActionFlags

extern uint8 Rte_BlynkClient_ServerConnStatus;
#define Rte_IRead_TaskMgr_ServerConnStatus Rte_BlynkClient_ServerConnStatus
//---------//

/// WiFiMgr ///
extern uint8 Rte_WiFiMgr_WiFiStatus;
#define Rte_IRead_TaskMgr_WifiStatus Rte_WiFiMgr_WiFiStatus
//----------//



/**************** Functions ************/
void Handle_1s_Task(void);
void Handle_5s_Task(void);
void Handle_10s_Task(void);
void Handle_30s_Task(void);
void Handle_1m_Task(void);

void TaskMgr_InRunPrint(void);



#endif