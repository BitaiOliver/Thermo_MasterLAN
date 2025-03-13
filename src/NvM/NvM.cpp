#include <NvM.h>

#include "NvM_Priv.h"

/* Global Variables */
NvM_Data_Type NvM_Data;

NvM_Block_Type NvM_Block[EEPROM_NB_USED_BLOCKS];
NvM_Block_Type NvM_Ram_Block[EEPROM_NB_USED_BLOCKS];

uint8 NvM_OneTimeCounter = 1U;
String NvM_SerialPrint = "";

/* Functions */
/********************************************************
* NvM Init function. 
********************************************************/
void NvM_Init(void)
{
#ifdef SERIAL_PRINT_NVM
    NvM_SerialPrint = "NvM: Init Start...";
    Serial.print(NvM_SerialPrint);
#endif

    EEPROM.begin(NVM_EEPROM_SIZE);

    for (uint8 i = 0U; i < EEPROM_NB_USED_BLOCKS; i++)
    {
        NvM_Block[i].u8Value = 0U;
        NvM_Block[i].bSyncFlag = FALSE;

        NvM_Ram_Block[i].u8Value = 0U;
        NvM_Ram_Block[i].bSyncFlag = FALSE;
    }

    NvM_Data.State = NVM_STATE_INIT;
    NvM_Data.u8WriteWaitTimer = 0U;

#ifdef SERIAL_PRINT_NVM
    NvM_SerialPrint = "  ...Stop.";
    Serial.println(NvM_SerialPrint);
#endif
}


/********************************************************
* NvM Main function. 
********************************************************/
void NvM_Main(void)
{
    uint8 bWriteFlag = FALSE;

    if (0U < NvM_OneTimeCounter)
    {
        NvM_OneTimeCounter = 0U;
#ifdef SERIAL_PRINT_NVM
        NvM_SerialPrint = "NvM: Loop Start.";
        Serial.println(NvM_SerialPrint);
#endif
    }
    else
    {
        /* do nothing */
    }

    switch (NvM_Data.State)
    {
        case NVM_STATE_INIT:
        {
            for (uint8 i = 0U; i < EEPROM_NB_USED_BLOCKS; i++)
            {
                NvM_Block[i].u8Value = EEPROM.read(i);
                NvM_Ram_Block[i].u8Value =  NvM_Block[i].u8Value;
#ifdef SERIAL_PRINT_NVM
                //NvM_SerialPrint = "NvM: Init read Block[" + String(i) +"]= "+ String(NvM_Block[i].u8Value);
                //Serial.println(NvM_SerialPrint);
#endif

            }
            NvM_Data.State = NVM_STATE_RUN;
            break;
        }
        case NVM_STATE_RUN:
        {
            bWriteFlag = nvm_CheckSyncFlags();
            if (FALSE != bWriteFlag)
            {
                /* start writeing proess */
                nvm_WriteEeprom();
            }
            else
            {
                /* no new data to write to eeprom -> do nothing */
            }
            /* Do nothing */
            break;
        }        
        default:
            break;
    }
}

/********************************************************
* Read data from block ID. 
********************************************************/
uint8 NvM_Read(uint8 BlockId)
{
    return (NvM_Ram_Block[BlockId].u8Value);
}

/********************************************************
* Write data from block ID. 
********************************************************/
void NvM_Write(uint8 BlockId, uint8 u8Data)
{
#ifdef SERIAL_PRINT_NVM
                NvM_SerialPrint = "NvM: Write called for Block[" + String(BlockId) +"]= "+ String(u8Data);
                Serial.println(NvM_SerialPrint);
#endif

    NvM_Ram_Block[BlockId].u8Value = u8Data;
    NvM_Ram_Block[BlockId].bSyncFlag = TRUE;
}

/********************************************************
* Commits write action to EEPROM. 
********************************************************/
void NvM_Commit(void)
{
    EEPROM.commit();
}

/********************************************************
* Function to check periodically the sync flags. 
********************************************************/
uint8 nvm_CheckSyncFlags(void)
{
    uint8 bWriteNeeded = FALSE;
    for (uint8 i = 0U; i < EEPROM_NB_USED_BLOCKS; i++)
    {
        if(FALSE != NvM_Ram_Block[i].bSyncFlag)
        {
            bWriteNeeded = TRUE;
        }
        else
        {
            /* do nothing */
        }
    }

    return bWriteNeeded;
}

/********************************************************
* Function to write Eeprom. 
********************************************************/
void nvm_WriteEeprom(void)
{
    NvM_Data.u8WriteWaitTimer++;
    if (NVM_EEPROM_WAIT_TIME < NvM_Data.u8WriteWaitTimer)
    {
        /* Reset counter*/
        NvM_Data.u8WriteWaitTimer = 0U;

        for (uint8 i = 0U; i < EEPROM_NB_USED_BLOCKS; i++)
        {
            if(FALSE != NvM_Ram_Block[i].bSyncFlag)
            {
                if (NvM_Ram_Block[i].u8Value !=  NvM_Block[i].u8Value)
                {
                    /* Write to NvM */
                    EEPROM.write(i, NvM_Ram_Block[i].u8Value);
                    /* Copy value to local block */
                    NvM_Block[i].u8Value = NvM_Ram_Block[i].u8Value;
                }
                else
                {
                    /* code */
                }
                NvM_Ram_Block[i].bSyncFlag = FALSE;
            }
            else
            {
                /* do nothing */
            }
        }
        /* Write eeprom */
        NvM_Commit();
#ifdef SERIAL_PRINT_NVM
        NvM_SerialPrint = "NvM: -----------------------Eeprom writed.-----------------";
        Serial.println(NvM_SerialPrint);
#endif
    }
    else
    {
        /* wait till threshold met */
    }
}