#include <Thermo.h>
#include <NvM.h>
#include <DIO.h>

#include "Thermo_Priv.h"

/* Global Variables */
const uint8 Thermo_Eeprom_Temp_Addr[BLYNKCLIENT_NB_SLAVES] = THERMO_EEPROM_TEMP_ADRESSES;
const uint8 Thermo_Eeprom_OnOff_Addr[BLYNKCLIENT_NB_SLAVES] = THERMO_EEPROM_ONOFF_ADRESSES;
const uint8 Thermo_Relay_Pins[BLYNKCLIENT_NB_SLAVES + BLYNKCLIENT_NB_MASTER] = THERMO_RELAY_PINS;

Thermo_Data_Type Thermo_Data;

Thermo_Slave_Data_Type Thermo_Slave_Data[BLYNKCLIENT_NB_SLAVES];
Thermo_Slave_Info_Type Thermo_Slave_Info[BLYNKCLIENT_NB_SLAVES];
Thermo_Heating_Info_Type Thermo_Heating_Info[BLYNKCLIENT_NB_SLAVES];

uint8 Thermo_OneTimeCounter = 1U;
String Thermo_SerialPrint = "";

/* Functions */
/********************************************************
* Thermo Init function. 
********************************************************/
void Thermo_Init(void)
{
#ifdef SERIAL_PRINT_THERMO
    Thermo_SerialPrint = "Thermo: Init Start...";
    Serial.print(Thermo_SerialPrint);
#endif

    Thermo_Data.u8State = THERMO_STATE_INIT;
    for (uint8 i = 0U; i < BLYNKCLIENT_NB_SLAVES; i++)
    {
        Thermo_Data.u8InitRetryCounter[i] = 0U;

        Thermo_Slave_Data[i].u16Temp = TEMPERATURE_INVALID_VALUE;
        Thermo_Slave_Data[i].u8Qf = TEMPERATURE_QF_INIT;
        Thermo_Slave_Data[i].u8NoDataTimer = 0U;

        Thermo_Slave_Info[i].u16SetedTemp = TEMPERATURE_INVALID_VALUE;
        Thermo_Slave_Info[i].u8OnOffStatus = THERMO_HEATING_OFF;

        Thermo_Heating_Info[i].u8HeatingDecision = THERMO_HEATING_OFF;
        Thermo_Heating_Info[i].u8WaitForTermalSwitch = 0U;
        Thermo_Heating_Info[i].u16MaxOperatingTime = 0U;
        Thermo_Heating_Info[i].u8WaitAfterOperating = 0U;
    }
#ifdef SERIAL_PRINT_THERMO
    Thermo_SerialPrint = "  ...Stop.";
    Serial.println(Thermo_SerialPrint);
#endif
}


/********************************************************
* Thermo Main function. 
********************************************************/
void Thermo_Main(void)
{
    if (0U < Thermo_OneTimeCounter)
    {
        Thermo_OneTimeCounter = 0U;
#ifdef SERIAL_PRINT_THERMO
        Thermo_SerialPrint = "Thermo: Loop Start.";
        Serial.println(Thermo_SerialPrint);
#endif
    }
    else
    {
        /* do nothing */
    }

    switch (Thermo_Data.u8State)
    {
        case THERMO_STATE_INIT:
        {
            /* Set Next State */
            Thermo_Data.u8State = THERMO_STATE_RUN;
            /* Read values from NvM */
            for (uint8 i = 0U; i < BLYNKCLIENT_NB_SLAVES; i++)
            {
                /* NvM temperature threshold has to be scaled! */
                Thermo_Slave_Info[i].u16SetedTemp = (uint16)(NvM_Read(Thermo_Eeprom_Temp_Addr[i]) * THERMO_TEMP_SCALE);
                Thermo_Slave_Info[i].u8OnOffStatus = NvM_Read(Thermo_Eeprom_OnOff_Addr[i]);
                /* Check if received value is valid*/
                if ((THERMO_TEMP_THRH_MIN > Thermo_Slave_Info[i].u16SetedTemp) ||
                    (THERMO_TEMP_THRH_MAX < Thermo_Slave_Info[i].u16SetedTemp) ||
                    ((THERMO_ONOFF_SW_OFF != Thermo_Slave_Info[i].u8OnOffStatus) && (THERMO_ONOFF_SW_ON != Thermo_Slave_Info[i].u8OnOffStatus)))
                {
                    Thermo_Data.u8InitRetryCounter[i]++;
                    if (THERMO_INIT_MAX_RETRY > Thermo_Data.u8InitRetryCounter[i])
                    {
                        /* If Any of the readed values are not valid keep init state */
                        Thermo_Data.u8State = THERMO_STATE_INIT;
                    }
                    else
                    {
#ifdef SERIAL_PRINT_THERMO
                        Thermo_SerialPrint = "Thermo: Call NvM write.";
                        Serial.println(Thermo_SerialPrint);
#endif

                        /* go to Run state  with basic configuration*/
                        Thermo_Slave_Info[i].u16SetedTemp = (THERMO_TEMP_THRH_DEFAULT * THERMO_TEMP_SCALE);
                        Thermo_Slave_Info[i].u8OnOffStatus = THERMO_ONOFF_DEFAULT;
                        /* Write default value to NvM*/
                        NvM_Write(Thermo_Eeprom_Temp_Addr[i], THERMO_TEMP_THRH_DEFAULT);
                        NvM_Write(Thermo_Eeprom_OnOff_Addr[i], THERMO_ONOFF_SW_OFF);

                    }
                }
                else
                {
                    /* Readed values are valid */
                }
                
            }
            break;
        }
        case THERMO_STATE_RUN:
        {
            /* Heating related functions */
            thermo_SyncTcpData();
            thermo_ComputeHeatingDecisions();
            thermo_ExecuteHeatingDecisions();
            break;
        }        
        default:
            break;
    }
}

/********************************************************
* Thermo sync temperature threshold data from Blynk server. 
********************************************************/
void Thermo_SyncTempThreshold(uint8 SlaveID, uint8 SetedTemp)
{
    /* Save the received temperature threshold */
    Thermo_Slave_Info[SlaveID].u16SetedTemp = (uint16)(SetedTemp * THERMO_TEMP_SCALE);
    /* Save value in EEPROM */
    NvM_Write(Thermo_Eeprom_Temp_Addr[SlaveID], SetedTemp);
}

/********************************************************
* Thermo sync OnOff switch status data from Blynk server. 
********************************************************/
void Thermo_SyncOnOffSwitch(uint8 SlaveID, uint8 SwitchStatus)
{
    /* Save the received temperature threshold */
    Thermo_Slave_Info[SlaveID].u8OnOffStatus = SwitchStatus;
    /* Save value in EEPROM */
    NvM_Write(Thermo_Eeprom_OnOff_Addr[SlaveID], SwitchStatus);
}

/********************************************************
* Thermo sync data from TCP server. 
********************************************************/
void thermo_SyncTcpData(void)
{

    for (uint8 i = 0U; i < BLYNKCLIENT_NB_SLAVES; i++)
    {
        if (FALSE != (Rte_IRead_Thermo_TcpServer_Reading[i].SyncFlag & (1 << RTE_TCPSERVER_SYNC_FLAG_THERMO)))
        {
            /* Reset sync flag */
            Rte_IRead_Thermo_TcpServer_Reading[i].SyncFlag &= ~(1 << RTE_TCPSERVER_SYNC_FLAG_THERMO);
            Thermo_Slave_Data[i].u16Temp = Rte_IRead_Thermo_TcpServer_Reading[i].Temperature;
            Thermo_Slave_Data[i].u8Qf = Rte_IRead_Thermo_TcpServer_Reading[i].Qf;
            /* Reset no data timer. */
            Thermo_Slave_Data[i].u8NoDataTimer = 0U;
        }
        else
        {
            /* Increment no data counter. */
            if(THERMO_NO_DATA_TIMER_THR > Thermo_Slave_Data[i].u8NoDataTimer)
            {
                Thermo_Slave_Data[i].u8NoDataTimer++;
            }
            else
            {
                /* Timer threshold reached */
            }
            
        }
    }
}

/********************************************************
* Thermo compute heating decisions for each slave. 
********************************************************/
void thermo_ComputeHeatingDecisions(void)
{
    for (uint8 i = 0U; i < BLYNKCLIENT_NB_SLAVES; i++)
    {
        /* Check if heating is turned on for the current slave */
        /* For safety reason check the wifi connection. In case of no wifi heating must turn OFF */
        if ((THERMO_HEATING_OFF != Thermo_Slave_Info[i].u8OnOffStatus) &&
            (WIFI_STATUS_CONNECTED == Rte_IRead_Thermo_WifiStatus))
        {
            if ((TEMPERATURE_QF_BAD !=  Thermo_Slave_Data[i].u8Qf) &&
                (TEMPERATURE_QF_INIT !=  Thermo_Slave_Data[i].u8Qf) &&
                (TEMPERATURE_INVALID_VALUE !=  Thermo_Slave_Data[i].u16Temp) &&
                (THERMO_NO_DATA_TIMER_THR > Thermo_Slave_Data[i].u8NoDataTimer))
            {
                if (Thermo_Slave_Data[i].u16Temp <= (Thermo_Slave_Info[i].u16SetedTemp - (uint16)TEMPERATURE_TOLERANCE))
                {
                    /* Turn ON heating */
                    Thermo_Heating_Info[i].u8HeatingDecision = (uint8)THERMO_HEATING_ON;
                }
                else if (Thermo_Slave_Data[i].u16Temp >= Thermo_Slave_Info[i].u16SetedTemp)
                {
                    /* Turn OFF heating */
                    Thermo_Heating_Info[i].u8HeatingDecision = (uint8)THERMO_HEATING_OFF;
                    /* Reset wait for thermal switch timer */
                    Thermo_Heating_Info[i].u8WaitForTermalSwitch = 0U;
                    Thermo_Heating_Info[i].u16MaxOperatingTime = 0U;
                    Thermo_Heating_Info[i].u8WaitAfterOperating = 0U;
                }
                else
                {
                    /* Between tolerance -> do nothing */
                }
            }
            else
            {
                /* Bad Qf -> turn off heating */
                Thermo_Heating_Info[i].u8HeatingDecision = (uint8)THERMO_HEATING_OFF;
                /* Reset wait for thermal switch timer */
                Thermo_Heating_Info[i].u8WaitForTermalSwitch = 0U;
                Thermo_Heating_Info[i].u16MaxOperatingTime = 0U;
                Thermo_Heating_Info[i].u8WaitAfterOperating = 0U;
            }
        }
        else
        {
            /* Heating is turned OFF or no WiFi connection */
            Thermo_Heating_Info[i].u8HeatingDecision = (uint8)THERMO_HEATING_OFF;
            /* Reset wait for thermal switch timer */
            Thermo_Heating_Info[i].u8WaitForTermalSwitch = 0U;
            Thermo_Heating_Info[i].u16MaxOperatingTime = 0U;
            Thermo_Heating_Info[i].u8WaitAfterOperating = 0U;
        }            
    }
}

/********************************************************
* Thermo Computes heating decision based on timeing intervals. 
********************************************************/
uint8 thermo_ComputeTimeBasedDecision(uint8 SlaveID)
{
    uint8 bTimedecision = FALSE;

    if(THERMO_TIMEOUT_THERMAL_SW > Thermo_Heating_Info[SlaveID].u8WaitForTermalSwitch)
    {
        Thermo_Heating_Info[SlaveID].u8WaitForTermalSwitch++;
        bTimedecision = FALSE;
    }
    else if (THERMO_TIMEOUT_MAX_OPERATING > Thermo_Heating_Info[SlaveID].u16MaxOperatingTime)
    {
        Thermo_Heating_Info[SlaveID].u16MaxOperatingTime++;
        bTimedecision = TRUE;
    }
    else if (THERMO_TIMEOUT_WAIT_AFTER_OP > Thermo_Heating_Info[SlaveID].u8WaitAfterOperating)
    {
        Thermo_Heating_Info[SlaveID].u8WaitAfterOperating++;
        /* Check if temp difference from threshold is smaller then max diff threshold */
        if (THERMO_MAX_TEMP_DIFF >= (Thermo_Slave_Info[SlaveID].u16SetedTemp - Thermo_Slave_Data[SlaveID].u16Temp))
        {
            /* Temp Difference is small -> Apply wait after operating time. */
            bTimedecision = FALSE;
        }
        else
         {
            /* Temp Difference is big -> Keep heating ON. */
            bTimedecision = TRUE;
        }

    }
    else
    {
        /* One cycle completed. Reset operating timer and wait timer. */
        Thermo_Heating_Info[SlaveID].u16MaxOperatingTime = 0U;
        Thermo_Heating_Info[SlaveID].u8WaitAfterOperating = 0U;
        /* Check if temp difference from threshold is smaller then max diff threshold */
        if (THERMO_MAX_TEMP_DIFF >= (Thermo_Slave_Info[SlaveID].u16SetedTemp - Thermo_Slave_Data[SlaveID].u16Temp))
        {
            /* Temp Difference is small -> Apply wait after operating time. */
            bTimedecision = FALSE;
        }
        else
         {
            /* Temp Difference is big -> Keep heating ON. */
            bTimedecision = TRUE;
        }
    }
    
    return bTimedecision;
}

/********************************************************
* Thermo execute heating decisions for each slave. 
********************************************************/
void thermo_ExecuteHeatingDecisions(void)
{
    uint8 bIsHeatingNeeded = FALSE;
    uint8 bTimeBaseDecisioin = FALSE;
    uint8 u8LocalMasterHeatingStatus = 0U;

    for (uint8 i = 0U; i < BLYNKCLIENT_NB_SLAVES; i++)
    {
        /* Check if heating is turned on for the current slave */
        if (THERMO_HEATING_OFF != Thermo_Heating_Info[i].u8HeatingDecision)
        {
            bTimeBaseDecisioin = thermo_ComputeTimeBasedDecision(i);
            /* Check if main heating has to be turned ON */
            if (FALSE != bTimeBaseDecisioin)
            {
                /* At least one slave needs heating -> turn main heating ON. */
                bIsHeatingNeeded = TRUE;
            }
            else
            {
                /* do nothing */
            }
            /* Turn Heating ON by seting the corresponding relay pin */
            DIO_SetRelayPinValue(Thermo_Relay_Pins[i], DIO_RELAY_PIN_ON);
            /* Set master status */
            u8LocalMasterHeatingStatus |= (1U << i);
        }
        else
        {
            /* Turn Heating OFF */
            DIO_SetRelayPinValue(Thermo_Relay_Pins[i], DIO_RELAY_PIN_OFF);
            /* Set master status */
            u8LocalMasterHeatingStatus &= ~(1U << i);
        }
    }
    /* Check if main heating is turned on */
    if (FALSE != bIsHeatingNeeded)
    {
        /* Turn main heating pin ON */
        DIO_SetRelayPinValue(Thermo_Relay_Pins[THERMO_MASTER_RELAY_PIN_INDEX], DIO_RELAY_PIN_ON);
        /* Check if status has been changed */
        if (Rte_IRead_Thermo_Master_Info.u8MasterHeatingStatus != u8LocalMasterHeatingStatus)
        {
            /* Set new value */
            Rte_IWrite_Thermo_Master_Info.u8MasterHeatingStatus = u8LocalMasterHeatingStatus;
            /* Set sync flag that the value changed */
            Rte_IWrite_Thermo_Master_Info.bMasterSyncFlag = TRUE;
        }
        else
        {
            /* do nothing status is correct */
        }
    }
    else
    {
        /* Turn main heating pin OFF */
        DIO_SetRelayPinValue(Thermo_Relay_Pins[THERMO_MASTER_RELAY_PIN_INDEX], DIO_RELAY_PIN_OFF);
        /* Check if status has been changed */
        if (Rte_IRead_Thermo_Master_Info.u8MasterHeatingStatus != u8LocalMasterHeatingStatus)
        {
            /* Set new value */
            Rte_IWrite_Thermo_Master_Info.u8MasterHeatingStatus = u8LocalMasterHeatingStatus;
            /* Set sync flag that the value changed */
            Rte_IWrite_Thermo_Master_Info.bMasterSyncFlag = TRUE;
        }
        else
        {
            /* do nothing status is correct */
        }

    }
}