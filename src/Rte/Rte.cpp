
/**************** Includes ************/
#include <Arduino.h>
#include <Rte_Types.h>

#include "CompilerSwitch.h"

/**************** Defines ************/
#define RTE_DELAY_MS_FACTOR         16000U

/**************** Varables ************/
/// OS ///
uint8 Rte_TaskMgr_Handler_Flags;
//------//


/// Blynk ///
uint8 Rte_BlynkClient_ActionFlags;
uint8 Rte_BlynkClient_ServerConnStatus;
//---------//

/// WiFiMgr ///
uint8 Rte_WiFiMgr_WiFiStatus;
//----------//

/// TcpServer ///
Rte_TcpServer_Reading_Type Rte_TcpServer_Reading[BLYNKCLIENT_NB_SLAVES];

Rte_Thermo_Master_Info_Type Rte_Thermo_Master_Info;

/**************** Functions ************/
void Rte_Init(void)
{
    /// OS ///
    Rte_TaskMgr_Handler_Flags = 0U;
    //------//
    
    /// Blynk ///
    Rte_BlynkClient_ActionFlags = 0U;
    Rte_BlynkClient_ServerConnStatus = 0U;
    //---------//
    /// WiFiMgr ///
    Rte_WiFiMgr_WiFiStatus = WIFI_STATUS_INIT;
    //-----------//


    /// Thermo ///
    Rte_Thermo_Master_Info.u8MasterHeatingStatus = 0U;
    Rte_Thermo_Master_Info.bMasterSyncFlag = 0U;


    for (uint8 i = 0; i < BLYNKCLIENT_NB_SLAVES; i++)
    {
        Rte_TcpServer_Reading[i].Temperature = 0U;
        Rte_TcpServer_Reading[i].Qf = TEMPERATURE_QF_INIT;
        Rte_TcpServer_Reading[i].SyncFlag = 0U;

        Rte_Thermo_Master_Info.u8SlaveHeatingStaus[i] = 0U;
    }
    
}

void Rte_DelayMs(unsigned int d)
{
    unsigned int i;
    d = d * RTE_DELAY_MS_FACTOR;
    for (i = 0; i < d; i++)  // loop time is 5 machine cycles: 31.25ns @ 160MHz, 62.5ns @ 80MHz
    {
        asm("nop"); // minimum element to keep GCC from optimizing this function out.
    }

}