#ifndef RTE_TYPES_H
#define RTE_TYPES_H 1
/**************** Includes ************/
#include <Arduino.h>

#include "CompilerSwitch.h"


#define RTE_TCPSERVER_SYNC_FLAG_THERMO              0U
#define RTE_TCPSERVER_SYNC_FLAG_BLYNK               1U

/**************** Data Types ************/
typedef struct Rte_Temperature_Reading_Type_Tag
{
    uint16      Data;
    uint8       Qf; 
}Rte_Temperature_Reading_Type;


typedef struct Rte_TcpServer_Reading_Type_Tag
{
    uint16      Temperature;
    uint8       Qf; 
    uint8       SyncFlag; 
}Rte_TcpServer_Reading_Type;

typedef struct Rte_Thermo_Master_Info_Type_Tag
{
    uint8       u8SlaveHeatingStaus[BLYNKCLIENT_NB_SLAVES];
    uint8       u8MasterHeatingStatus;
    uint8       bMasterSyncFlag;
}Rte_Thermo_Master_Info_Type;



/**************** Varables ************/



/**************** Functions ************/
void Rte_Init(void);
void Rte_DelayMs(unsigned int d);


#endif