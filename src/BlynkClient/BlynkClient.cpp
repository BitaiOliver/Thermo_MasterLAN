#include "CompilerSwitch.h"


#include <BlynkClient.h>
#include <Thermo.h>

#include "BlynkClient_Priv.h"


BlynkClient_Data_Type BlynkClient_Data;

BlynkClient_Slave_Data_Pins_Type BlynkClient_Slave_Data[BLYNKCLIENT_NB_SLAVES];

const uint8 BlynkClient_Temp_Pins[BLYNKCLIENT_NB_SLAVES] = BLYNKCLIENT_TEMP_PINS;
const uint8 BlynkClient_Qf_Pins[BLYNKCLIENT_NB_SLAVES] = BLYNKCLIENT_QF_PINS;

uint8 BlynkClient_OneTimeCounter = 1U;
String BlynkClient_SerialPrint = "";

/* this counter is decremented after startup */
uint8 BlynkClient_StartupCounter = 3U;


/* Functions */
/********************************************************
* BlynkClient Init function. 
********************************************************/
void BlynkClient_Init(void)
{
#ifdef SERIAL_PRINT_BLYNK
    BlynkClient_SerialPrint = "BlynkClient: Init Start...";
    Serial.print(BlynkClient_SerialPrint);
#endif

    BlynkClient_Data.State = BLYNKCLIENT_STATE_INIT;
    BlynkClient_Data.Notification = BLYNKCLIENT_NO_NOTIFICATION;
    BlynkClient_Data.SyncCounter = 0U;
    /* Init with max value to start connect at startup.*/
    BlynkClient_Data.ReconnectCounter = BLYNKCLIENT_RECONNECT_WAIT_TIME;
    BlynkClient_Data.u8DataSendingState = BLYNKCLIENT_DATA_SEND_ST1;

    for (uint8 i = 0U; i < BLYNKCLIENT_NB_SLAVES; i++)
    {
        BlynkClient_Slave_Data[i].TempPin = 0U;
        BlynkClient_Slave_Data[i].QfPin = TEMPERATURE_QF_INIT;
    }
 
    /* Blynk Configuration */
    Blynk.config(Blynk_Auth_Token, "46.101.217.214");
    //Blynk.begin(Blynk_Auth_Token);

    //Blynk.begin(auth, ssid, pass);
    // You can also specify server:
    //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
    //Blynk.begin(Blynk_Auth_Token, WiFi_Name, WiFi_Password, IPAddress(IP_1st,IP_2nd,IP_3rd,IP_4th),BLYNKCLIENT_PORT);
    //Blynk.begin(Blynk_Auth_Token, WiFi_Name, WiFi_Password);
#ifdef SERIAL_PRINT_BLYNK
    BlynkClient_SerialPrint = "  ...Stop.";
    Serial.println(BlynkClient_SerialPrint);
#endif

}
/********************************************************
* BlynkClient Main function. 
********************************************************/
void BlynkClient_Main(void)
{
    if (0U < BlynkClient_OneTimeCounter)
    {
        BlynkClient_OneTimeCounter = 0U;
#ifdef SERIAL_PRINT_BLYNK
        BlynkClient_SerialPrint = "BlynkClient: Loop Start.";
        Serial.println(BlynkClient_SerialPrint);
#endif
    }
    else
    {
        /* do nothing */
    }

    if(WIFI_STATUS_CONNECTED == Rte_IRead_BlynkClient_WifiStatus)
    {
        /* Blynk Configuration */
        //Blynk.config(Blynk_Auth_Token);

        blynkclient_CheckBlinkState();

        switch (BlynkClient_Data.State)
        {
            case BLYNKCLIENT_STATE_INIT:
            {
                /* Wait */
                break;
            }
            case BLYNKCLIENT_STATE_CONNECTED:
            {
                /* Run Blynk Handler */
                Blynk.run();

                /* Handle init phase by sync data from server */
                if (BLYNKCLIENT_SYNC_WAIT > BlynkClient_Data.SyncCounter)
                {
                    blynkclient_InitSyncDatas();
                    BlynkClient_Data.SyncCounter++;    
                }
                else
                {
                    /* Init phase done -> do nothing */
                }
                blynkclient_HandleDataSending();    
                blynkclient_HandleMasterHeatingState();        
                
                break;
            }
            case BLYNKCLIENT_STATE_NOT_CONNECTED:
            {
                if(0U < BlynkClient_StartupCounter)
                {
                    //Blynk.connect();
                    /* at sturtup run a few times the blynk client */
                    for (uint8 i = 0; i < 5; i++)
                    {
                        /* Run Blynk Handler */
                        Blynk.run();
                    }
                    BlynkClient_StartupCounter--;
                }
                /* Wait */
                if (BLYNKCLIENT_RECONNECT_WAIT_TIME < BlynkClient_Data.ReconnectCounter)
                {
                    /* Reset counter. */
                    BlynkClient_Data.ReconnectCounter = 0U;
                    //Blynk.disconnect();
                    //Blynk.config(Blynk_Auth_Token);
                    BlynkClient_SerialPrint = "Blynk: Test loop";
                    Serial.println(BlynkClient_SerialPrint);

                    for (uint8 i = 0; i < 5; i++)
                    {
                        /* Run Blynk Handler */
                        Blynk.run();
                    }
                    
                // Blynk.connect();
                }
                else
                {
                    /* increment counter */
                    BlynkClient_Data.ReconnectCounter++;
                }
                break;
            }
            default:
                break;
        }
    }
    else
    {
        /* wait for wifi */
    }
    
}

/********************************************************
* BlynkClient handles the master heating status sending to Blynk server. 
********************************************************/
void blynkclient_HandleMasterHeatingState(void)
{
    if (FALSE != Rte_IRead_BlynkClient_Thermo_Master_Info.bMasterSyncFlag)
    {
        Rte_IRead_BlynkClient_Thermo_Master_Info.bMasterSyncFlag = FALSE;
        /* Master operation notify */
        Blynk.virtualWrite(BLYNKCLIENT_MASTER_HEATING_STATUS_PIN, Rte_IRead_BlynkClient_Thermo_Master_Info.u8MasterHeatingStatus);
    }
    else
    {
        /* code */
    }
}
/********************************************************
* BlynkClient handles the sequenced data sending to Blynk server. 
********************************************************/
void blynkclient_HandleDataSending(void)
{
    uint8 u8LocalSlaveID = 55U; /* Init with a invalid slave ID*/

    /* This flag shall be set every 1m by TaskMgr */
    if (FALSE != (Rte_IRead_BlynkClient_ActionFlags & (1 << BLYNKCLIENT_START_DATA_SENDING_ID)))
    {
        switch (BlynkClient_Data.u8DataSendingState)
        {
            case BLYNKCLIENT_DATA_SEND_ST1:
            {
                /* Set Slave ID.*/
                u8LocalSlaveID = BLYNKCLIENT_SLAVE1_ID;
                /* Go to next State */
                BlynkClient_Data.u8DataSendingState = BLYNKCLIENT_DATA_SEND_ST2;
                break;
            }
            case BLYNKCLIENT_DATA_SEND_ST2:
            {
                /* Set Slave ID.*/
                u8LocalSlaveID = BLYNKCLIENT_SLAVE2_ID;
                 /* Go to next State */
                BlynkClient_Data.u8DataSendingState = BLYNKCLIENT_DATA_SEND_ST3;
                break;
            }
            case BLYNKCLIENT_DATA_SEND_ST3:
            {
                /* Set Slave ID.*/
                u8LocalSlaveID = BLYNKCLIENT_SLAVE3_ID;
                 /* Go to next State */
                BlynkClient_Data.u8DataSendingState = BLYNKCLIENT_DATA_SEND_ST4;
                break;
            }
            case BLYNKCLIENT_DATA_SEND_ST4:
            {
                /* Write phase is done -> go to first stage */
                BlynkClient_Data.u8DataSendingState = BLYNKCLIENT_DATA_SEND_ST1;
                /* Reset flag */
                Rte_IRead_BlynkClient_ActionFlags &= ~(1 << BLYNKCLIENT_START_DATA_SENDING_ID);

                /* Master operation notify */
                Blynk.virtualWrite(BLYNKCLIENT_MASTER_OPERATING_STATUS_PIN, BLYNKCLIENT_MASTER_STATUS_OK);
                Blynk.virtualWrite(BLYNKCLIENT_MASTER_HEATING_STATUS_PIN, Rte_IRead_BlynkClient_Thermo_Master_Info.u8MasterHeatingStatus);
                //BlynkClient_SerialPrint = "Blynk------: Status: " + String(BLYNKCLIENT_MASTER_STATUS_OK) + " Heating: " + String(Rte_IRead_BlynkClient_Thermo_Master_Info.u8MasterHeatingStatus);
                //Serial.println(BlynkClient_SerialPrint);

                break;
            }
            default:
                break;
        }
        /* check if slave ID is valid*/
        if (BLYNKCLIENT_NB_SLAVES > u8LocalSlaveID)
        {
            /* Check if a new data was received */
            if (FALSE != (Rte_IRead_BlynkClient_TcpServer_Reading[u8LocalSlaveID].SyncFlag & (1 << RTE_TCPSERVER_SYNC_FLAG_BLYNK)))
            {
                /* Reset sync flag */
                Rte_IRead_BlynkClient_TcpServer_Reading[u8LocalSlaveID].SyncFlag &= ~(1 << RTE_TCPSERVER_SYNC_FLAG_BLYNK);
                /* Read data from Rte*/
                BlynkClient_Slave_Data[u8LocalSlaveID].TempPin = (float)((float)Rte_IRead_BlynkClient_TcpServer_Reading[u8LocalSlaveID].Temperature / (float)10);
                BlynkClient_Slave_Data[u8LocalSlaveID].QfPin = Rte_IRead_BlynkClient_TcpServer_Reading[u8LocalSlaveID].Qf;
                /* Write data to Blynk server*/
                Blynk.virtualWrite(BlynkClient_Temp_Pins[u8LocalSlaveID], BlynkClient_Slave_Data[u8LocalSlaveID].TempPin);
                Blynk.virtualWrite(BlynkClient_Qf_Pins[u8LocalSlaveID], BlynkClient_Slave_Data[u8LocalSlaveID].QfPin);
                BlynkClient_SerialPrint = "Blynk------: Slave:" + String(u8LocalSlaveID) + "Temp: " + String(BlynkClient_Slave_Data[u8LocalSlaveID].TempPin);
                Serial.println(BlynkClient_SerialPrint);

            }
            else
            {
                /* do nothing. no new data */
            }
        }
        else 
        {
            /* do nothing */
        }
        
        
    }
    else
    {
        /* code */
    }
}
/********************************************************
* BlynkClient sync data at power on. 
********************************************************/
void blynkclient_InitSyncDatas(void)
{
    switch (BlynkClient_Data.SyncCounter)
    {
        case 1U:
        {
            Blynk.syncVirtual(BLYNKCLIENT_SLAVE1_TEMP_THRES_PIN, BLYNKCLIENT_SLAVE1_ONOFF_PIN);
            break;
        }
        case 2U:
        {
            Blynk.syncVirtual(BLYNKCLIENT_SLAVE2_TEMP_THRES_PIN, BLYNKCLIENT_SLAVE2_ONOFF_PIN);
            break;
        }
        case 3U:
        {
            Blynk.syncVirtual(BLYNKCLIENT_SLAVE3_TEMP_THRES_PIN, BLYNKCLIENT_SLAVE3_ONOFF_PIN);
            break;
        }
        default:
            break;
    }
}

/********************************************************
* BlynkClient chech connection status to the server function. 
********************************************************/
void blynkclient_CheckBlinkState(void)
{
   if(Blynk.connected())
   {
        Rte_IWrite_BlynkClient_ServerConnStatus = BLYNK_SERVER_STATUS_CONNECTED;
        BlynkClient_Data.State = BLYNKCLIENT_STATE_CONNECTED;
        /* Reset counter. */
        BlynkClient_Data.ReconnectCounter = 0U;
#ifdef SERIAL_PRINT_BLYNK
            BlynkClient_SerialPrint = "Blynk: Connected ";
            //Serial.println(BlynkClient_SerialPrint);
#endif
   }
   else
   {
        Rte_IWrite_BlynkClient_ServerConnStatus = BLYNK_SERVER_STATUS_NOT_CONNECTED;
        BlynkClient_Data.State = BLYNKCLIENT_STATE_NOT_CONNECTED;
#ifdef SERIAL_PRINT_BLYNK
            BlynkClient_SerialPrint = "Blynk: Not Conn ";
            Serial.println(BlynkClient_SerialPrint);
#endif

   }

}


/*********************** Handlers for Slave 1 ***********************/
BLYNK_WRITE(BLYNKCLIENT_SLAVE1_TEMP_THRES_PIN)
{
    int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
    /* Notify Thermo about the recepion of new data */
    Thermo_SyncTempThreshold(BLYNKCLIENT_SLAVE1_ID, (uint8)pinValue);

    //BlynkClient_SerialPrint = "Blynk: Callbecak Slave 1 temp thresh: " + String(pinValue);
    //Serial.println(BlynkClient_SerialPrint);

}
BLYNK_WRITE(BLYNKCLIENT_SLAVE1_ONOFF_PIN)
{
    int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
    /* Notify Thermo about the recepion of new data */
    if((THERMO_ONOFF_SW_ON != (uint8)pinValue) &&
        (THERMO_ONOFF_SW_OFF != (uint8)pinValue))
    {
        /* do nothing the received value is not valid */
    }
    else
    {
        Thermo_SyncOnOffSwitch(BLYNKCLIENT_SLAVE1_ID, (uint8)pinValue);
    }
    //BlynkClient_SerialPrint = "Blynk: Callbecak Slave 1 onoff: " + String(pinValue);
    //Serial.println(BlynkClient_SerialPrint);

}
/********************************************************************/

/*********************** Handlers for Slave 2 ***********************/
BLYNK_WRITE(BLYNKCLIENT_SLAVE2_TEMP_THRES_PIN)
{
    int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
    /* Notify Thermo about the recepion of new data */
    Thermo_SyncTempThreshold(BLYNKCLIENT_SLAVE2_ID, (uint8)pinValue);
    //BlynkClient_SerialPrint = "Blynk: Callbecak Slave 2 temp thresh: " + String(pinValue);
    //Serial.println(BlynkClient_SerialPrint);

}
BLYNK_WRITE(BLYNKCLIENT_SLAVE2_ONOFF_PIN)
{
    int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
    /* Notify Thermo about the recepion of new data */
    if((THERMO_ONOFF_SW_ON != (uint8)pinValue) &&
    (THERMO_ONOFF_SW_OFF != (uint8)pinValue))
    {
        /* do nothing the received value is not valid */
    }
    else
    {
        Thermo_SyncOnOffSwitch(BLYNKCLIENT_SLAVE2_ID, (uint8)pinValue);
    }
    //BlynkClient_SerialPrint = "Blynk: Callbecak Slave 2 onoff: " + String(pinValue);
    //Serial.println(BlynkClient_SerialPrint);

}
/********************************************************************/

/*********************** Handlers for Slave 3 ***********************/
BLYNK_WRITE(BLYNKCLIENT_SLAVE3_TEMP_THRES_PIN)
{
    int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
    /* Notify Thermo about the recepion of new data */
    Thermo_SyncTempThreshold(BLYNKCLIENT_SLAVE3_ID, (uint8)pinValue);
    //BlynkClient_SerialPrint = "Blynk: Callbecak Slave 3 temp thresh: " + String(pinValue);
    //Serial.println(BlynkClient_SerialPrint);

}
BLYNK_WRITE(BLYNKCLIENT_SLAVE3_ONOFF_PIN)
{
    int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
    /* Notify Thermo about the recepion of new data */
    if((THERMO_ONOFF_SW_ON != (uint8)pinValue) &&
    (THERMO_ONOFF_SW_OFF != (uint8)pinValue))
    {
        /* do nothing the received value is not valid */
    }
    else
    {
        Thermo_SyncOnOffSwitch(BLYNKCLIENT_SLAVE3_ID, (uint8)pinValue);
    }
    //BlynkClient_SerialPrint = "Blynk: Callbecak Slave 3 on off: " + String(pinValue);
    //Serial.println(BlynkClient_SerialPrint);

}
/********************************************************************/
