#include <TcpServer.h>
#include <BlynkClient.h>

#include "TcpServer_Priv.h"


/* Global Variables */
TcpServer_Data_Type TcpServer_Data;

WiFiServer Serv(TCPSERVER_SERVER_PORT);

uint8 TcpServer_OneTimeCounter = 1U;
String TcpServer_SerialPrint = "";


/* Functions */
/********************************************************
* TcpServer Init function. 
********************************************************/
void TcpServer_Init(void)
{
#ifdef SERIAL_PRINT_TCP
    TcpServer_SerialPrint = "TcpServer: Init Start....";
    Serial.print(TcpServer_SerialPrint);
#endif

    TcpServer_Data.State = TCPSERVER_STATE_INIT;
    /* Start the server */
    Serv.begin();

#ifdef SERIAL_PRINT_TCP
    TcpServer_SerialPrint = "Stop";
    Serial.println(TcpServer_SerialPrint);
#endif

}

/********************************************************
* TcpServer Main function. 
********************************************************/
void TcpServer_Main(void)
{
    uint8 SlaveID = 99U;
    uint16 SlaveTemperature = 99U;
    uint16 SlaveQf = 99U;
    String ReadVal = "";
    String temp = "";

    WiFiClient client;


    if (0U < TcpServer_OneTimeCounter)
    {
        TcpServer_OneTimeCounter = 0U;
#ifdef SERIAL_PRINT_TCP
        TcpServer_SerialPrint = "TcpServer: Loop Start.";
        Serial.println(TcpServer_SerialPrint);
#endif
    }
    else
    {
        /* do nothing */
    }

    switch (TcpServer_Data.State)
    {
        case TCPSERVER_STATE_INIT:
        {
            /* do nothing */
            TcpServer_Data.State = TCPSERVER_STATE_RUN;
            break;
        }
        case TCPSERVER_STATE_RUN:
        {
            if (WIFI_STATUS_CONNECTED == Rte_IRead_WiFiMgr_WifiStatus)
            {
                /* check server */
                client = Serv.available();

                if (client)
                {
    #ifdef SERIAL_PRINT_TCP
                    TcpServer_SerialPrint = "TcpServer: Client connected.";
                    Serial.println(TcpServer_SerialPrint);
    #endif

                    if (client.connected())
                    {
                        /* Read client data */
                        ReadVal = client.readStringUntil('\n');
    #ifdef SERIAL_PRINT_TCP
                        TcpServer_SerialPrint = "TcpServer: Recieved data: " + ReadVal;
                        Serial.println(TcpServer_SerialPrint);
    #endif
                        /* Process received data */
                        temp = ReadVal.substring(1, 2);
                        //Serial.print("SlaveID:" + temp);
                        SlaveID = (uint8)temp.toInt();

                        temp = ReadVal.substring(3, 6);
                        //Serial.print("  Temp:" + temp);
                        SlaveTemperature = (uint16)temp.toInt();

                        temp = ReadVal.substring(7, 8);
                        //Serial.print("  QF:" + temp);
                        SlaveQf = (uint8)temp.toInt();

    #ifdef SERIAL_PRINT_TCP
                        TcpServer_SerialPrint = "TcpServer: SlaveID: " + String(SlaveID) + " Temperature: " + String(SlaveTemperature) + " Qf:" + String(SlaveQf);
                        Serial.println(TcpServer_SerialPrint);
    #endif
                        /* Process received data */
                        tcpserver_ProcessReceivedData(SlaveID, SlaveTemperature, SlaveQf);
                        /* Send response */
                        client.println(String(SlaveTemperature + TCPSERVER_POSITIVE_RESP_OFFSET));
                    }
                    else
                    {
    #ifdef SERIAL_PRINT_TCP
                        TcpServer_SerialPrint = "TcpServer: Client NOT connected well.";
                        Serial.println(TcpServer_SerialPrint);
    #endif
                    }
                    /* Stop current client */
                    client.stop();
                }
                else
                {
                    /* wait for clients */
                }
            }
            else
            {
                /* Wifi not connected wait. */
            }

            break;
        }
        case TCPSERVER_STATE_NOT_CONNECTED:
        {
            /* do nothing */
            break;
        }
        default:
            break;
    }
}

/********************************************************
* TcpServer Main function. 
********************************************************/
void tcpserver_ProcessReceivedData(uint8 Id, uint16 Temperature, uint8 Qf)
{
    if ((0U < Id) && (BLYNKCLIENT_NB_SLAVES >= Id))
    {
        if ((0U < Temperature) && (800U >= Temperature))
        {
            if ((TEMPERATURE_QF_INIT < Qf) && (TEMPERATURE_QF_BAD >= Qf))
            {
                Rte_IWrite_TcpServer_Reading[(Id - 1U)].Temperature = Temperature;
                Rte_IWrite_TcpServer_Reading[(Id - 1U)].Qf = Qf;
                Rte_IWrite_TcpServer_Reading[(Id - 1U)].SyncFlag |= (1U << RTE_TCPSERVER_SYNC_FLAG_THERMO);
                Rte_IWrite_TcpServer_Reading[(Id - 1U)].SyncFlag |= (1U << RTE_TCPSERVER_SYNC_FLAG_BLYNK);

            }
            else
            {
                /* Invalid  Qf */
            }
        }
        else
        {
            /* Invalid slave Temperature */
        }
    }
    else
    {
        /* Invalid slave ID */
    }
}
