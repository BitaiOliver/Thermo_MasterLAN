#ifndef NVM_H
#define NVM_H 1
/**************** Includes ************/
#include <Arduino.h>



/**************** Defines ************/

/**************** Varables ************/



/**************** Functions ************/
void NvM_Init(void);
void NvM_Main(void);

uint8 NvM_Read(uint8 BlockId);
void NvM_Write(uint8 BlockId, uint8 u8Data);
void NvM_Commit(void);


#endif