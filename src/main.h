#ifndef MAIN_H
#define MAIN_H 1
/**************** Includes ************/
#include <Arduino.h>
#include <TaskMgr.h>
#include "CompilerSwitch.h"



/**************** Defines ************/
#define SERIAL_BAUDRATE         9600U

/**************** Varables ************/
uint8 GlobalTimer;
extern uint8 Rte_TaskMgr_Handler_Flags;
#define Rte_IWrite_TaskMgr_Handler_Flags    Rte_TaskMgr_Handler_Flags

/**************** Functions ************/
void HandleTimerInterrupt(void);


#endif