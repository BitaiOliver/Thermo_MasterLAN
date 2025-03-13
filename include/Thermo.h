#ifndef THERMO_H
#define THERMO_H 1
/**************** Includes ************/
#include <Arduino.h>



/**************** Defines ************/
#define THERMO_ONOFF_SW_OFF                     0U     /* OnOFF Switch OFF value */
#define THERMO_ONOFF_SW_ON                      1U     /* OnOFF Switch ON value */

/**************** Varables ************/



/**************** Functions ************/
void Thermo_Init(void);
void Thermo_Main(void);
void Thermo_SyncTempThreshold(uint8 SlaveID, uint8 SetedTemp);
void Thermo_SyncOnOffSwitch(uint8 SlaveID, uint8 SwitchStatus);

#endif