#ifndef NVM_PRIV_H
#define NVM_PRIV_H 1
/**************** Includes ************/
#include <Arduino.h>
#include <EEPROM.h>


#include "CompilerSwitch.h"


/**************** Defines ************/
#define NVM_STATE_INIT                      1U
#define NVM_STATE_RUN                       2U


#define NVM_EEPROM_SIZE                     512U

#define NVM_EEPROM_WAIT_TIME                 30U     /* Wait time to write eeprom after first change occured: 30s */


/**************** Varables ************/
typedef struct NvM_Data_Type_Tag
{
    uint8       State;
    uint8       u8WriteWaitTimer;    
}NvM_Data_Type;

typedef struct NvM_Block_Type_Tag
{
    uint8       u8Value; 
    uint8       bSyncFlag; 
}NvM_Block_Type;

/// TCP ///
//------//

/// Blynk ///
//--------//

/**************** Functions ************/
uint8 nvm_CheckSyncFlags(void);
void nvm_WriteEeprom(void);
#endif