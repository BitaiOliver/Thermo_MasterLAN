#ifndef THERMO_PRIV_H
#define THERMO_PRIV_H 1
/**************** Includes ************/
#include <Arduino.h>
#include <Rte_Types.h>

#include "CompilerSwitch.h"


/**************** Defines ************/
#define THERMO_STATE_INIT                      1U
#define THERMO_STATE_RUN                       2U

#define THERMO_INIT_MAX_RETRY                  5U


#define THERMO_HEATING_OFF                      0U
#define THERMO_HEATING_ON                       1U

#define THERMO_RELAY_STATUS_OFF                 0U /* Value for relay to turn OFF. */
#define THERMO_RELAY_STATUS_S1_ON               1U /* Value for Slave 1 relay to turn ON. */
#define THERMO_RELAY_STATUS_S2_ON               2U /* Value for Slave 2 relay to turn ON. */
#define THERMO_RELAY_STATUS_S3_ON               4U /* Value for Slave 3 relay to turn ON. */



#define THERMO_TEMP_SCALE                       10U     /* Temperature scaling factor */

#define THERMO_TEMP_THRH_MIN                    100U     /* Temp threshold min value */
#define THERMO_TEMP_THRH_MAX                    300U     /* Temp threshold max value */

#define THERMO_TEMP_THRH_DEFAULT                20U     /* Default Temperature value: 20C */
#define THERMO_ONOFF_DEFAULT                    0U     /* Default OnOFF Switch value: OFF */


/* Timeouts for each slave */
#define THERMO_TIMEOUT_THERMAL_SW               120U /* Time to wait for the thermal switch to open: 120s */
#define THERMO_TIMEOUT_MAX_OPERATING            330U /* Max time to operate without stopping: 330s. */
#define THERMO_TIMEOUT_WAIT_AFTER_OP            200U /* Time to wait after max operation time has expired 150s. */

#define THERMO_MAX_TEMP_DIFF                    (uint8)12U /* Max temp difference (1.2C) for which the operation timeout functionality is working. */

#define THERMO_HEATING_ST1                      1U /* Heating process stage 1: Wait for thermal switch to open. */
#define THERMO_HEATING_ST2                      2U /* Heating process stage 2: Heating is ON. */
#define THERMO_HEATING_ST3                      3U /* Heating process stage 3: Wait after heating was ON for max operating time. */



#define THERMO_NO_DATA_TIMER_THR                250U    /* No data timer threshold: 250s */

#define THERMO_EEPROM_TEMP_ADRESSES {EEPROM_ADDR_S1_TEMP_THRESH, EEPROM_ADDR_S2_TEMP_THRESH, EEPROM_ADDR_S3_TEMP_THRESH}
#define THERMO_EEPROM_ONOFF_ADRESSES {EEPROM_ADDR_S1_ONOFF, EEPROM_ADDR_S2_ONOFF, EEPROM_ADDR_S3_ONOFF}

#define THERMO_MASTER_RELAY_PIN_INDEX           3U
#define THERMO_RELAY_PINS {THERMO_SLAVE1_RELAY_PIN, THERMO_SLAVE2_RELAY_PIN, THERMO_SLAVE3_RELAY_PIN, THERMO_MASTER_RELAY_PIN}



/**************** Varables ************/
typedef struct Thermo_Data_Type_Tag
{
    uint8       u8State;
    uint8       u8InitRetryCounter[BLYNKCLIENT_NB_SLAVES];
}Thermo_Data_Type;


typedef struct Thermo_Slave_Data_Type_Tag{
    uint16      u16Temp;
    uint8       u8Qf;
    uint8       u8NoDataTimer;
}Thermo_Slave_Data_Type;

typedef struct Thermo_Slave_Info_Type_Tag{
    uint16      u16SetedTemp;
    uint8       u8OnOffStatus;
}Thermo_Slave_Info_Type;

typedef struct Thermo_Heating_Info_Type_Tag{
    uint8       u8HeatingDecision;
    uint8       u8WaitForTermalSwitch;
    uint16      u16MaxOperatingTime;
    uint8       u8WaitAfterOperating;
}Thermo_Heating_Info_Type;



extern Rte_Thermo_Master_Info_Type Rte_Thermo_Master_Info;
#define Rte_IWrite_Thermo_Master_Info   Rte_Thermo_Master_Info
#define Rte_IRead_Thermo_Master_Info   Rte_Thermo_Master_Info


/// TCP ///
extern Rte_TcpServer_Reading_Type Rte_TcpServer_Reading[BLYNKCLIENT_NB_SLAVES];
#define Rte_IRead_Thermo_TcpServer_Reading    Rte_TcpServer_Reading
//------//

/// Blynk ///
//--------//

/// WiFiMgr ///
extern uint8 Rte_WiFiMgr_WiFiStatus;
#define Rte_IRead_Thermo_WifiStatus Rte_WiFiMgr_WiFiStatus
//----------//


/**************** Functions ************/
void thermo_SyncTcpData(void);
void thermo_CheckEepromWriteFlag(void);
void thermo_ComputeHeatingDecisions(void);
uint8 thermo_ComputeTimeBasedDecision(uint8 SlaveID);
void thermo_ExecuteHeatingDecisions(void);





#endif