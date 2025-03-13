#include <TaskMgr.h>
#include <BlynkClient.h>
#include <TcpServer.h>
#include <DIO.h>
#include <NvM.h>
#include <WiFiMgr.h>
#include <Thermo.h>

#include "TaskMgr_Priv.h"
#include "CompilerSwitch.h"

/* Global shared variables */
uint8 TaskMgr_InRunPrintCounter = 0U;
uint8 TaskMgr_OneTimeCounter = 1U;
String TaskMgr_SerialPrint = "";

uint8 TaskMgr_BlynkWaitTimeCounter = 0U;

void TaskMgr_Init(void)
{
#ifdef SERIAL_PRINT_TASKMGR
  TaskMgr_SerialPrint = "TaskMgr: Init Start....";
  Serial.print(TaskMgr_SerialPrint);
#endif

    /// OS ///
    /* Initialize timer handlers flag */
    Rte_IRead_TaskMgr_Handler_Flags = 0U;
    /* Call Rte_Init() */
    Rte_Init();
    //------//

    /// DIO ///
    DIO_Init();
    //-------//

    /// NvM ///
    NvM_Init();
    //-------//

    /// WifiMgr ///
    WiFiMgr_Init();
    //----------//

    /// TcpServer ///
    TcpServer_Init();
    //-------------//

    /// Thermo ///
    Thermo_Init();
    //----------//

    /* Blynk init lasts much time so keep it in the last position. */
    /// Blynk ///
    BlynkClient_Init();
    //---------//

#ifdef SERIAL_PRINT_TASKMGR
  TaskMgr_SerialPrint = "Stop.";
  Serial.println(TaskMgr_SerialPrint);
#endif

}

void TaskMgr_Main(void)
{
    if (0U < TaskMgr_OneTimeCounter)
    {
        TaskMgr_OneTimeCounter = 0U;
#ifdef SERIAL_PRINT_TASKMGR
        TaskMgr_SerialPrint = "TaskMgr: Loop Start.";
        Serial.println(TaskMgr_SerialPrint);
#endif
    }
    else
    {
        /* do nothing */
    }

    /* The order of the tasks are decreasing because rearly executed tasks set flags 
        for often executed tasks */
    if(FALSE != (Rte_IRead_TaskMgr_Handler_Flags & (1 << TIMER_1M_ID)))
    {
        /* Reset flag */
        Rte_IRead_TaskMgr_Handler_Flags &= ~(1 << TIMER_1M_ID);
        /* Call handler */
        Handle_1m_Task();
    }
    else
    {
        /* code */
    }
    if(FALSE != (Rte_IRead_TaskMgr_Handler_Flags & (1 << TIMER_30S_ID)))
    {
        /* Reset flag */
        Rte_IRead_TaskMgr_Handler_Flags &= ~(1 << TIMER_30S_ID);
        /* Call handler */
        Handle_30s_Task();
    }
    else
    {
        /* code */
    }
    if(FALSE != (Rte_IRead_TaskMgr_Handler_Flags & (1 << TIMER_10S_ID)))
    {
        /* Reset flag */
        Rte_IRead_TaskMgr_Handler_Flags &= ~(1 << TIMER_10S_ID);
        /* Call handler */
        Handle_10s_Task();
    }
    else
    {
        /* code */
    }
    if(FALSE != (Rte_IRead_TaskMgr_Handler_Flags & (1 << TIMER_5S_ID)))
    {
        /* Reset flag */
        Rte_IRead_TaskMgr_Handler_Flags &= ~(1 << TIMER_5S_ID);
        /* Call handler */
        Handle_5s_Task();
    }
    else
    {
        /* code */
    }
    if(FALSE != (Rte_IRead_TaskMgr_Handler_Flags & (1 << TIMER_1S_ID)))
    {
        /* Reset flag */
        Rte_IRead_TaskMgr_Handler_Flags &= ~(1 << TIMER_1S_ID);
        /* Call handler */
        Handle_1s_Task();
    }
    else
    {
        /* code */
    }
}

void TaskMgr_InRunPrint(void)
{
#ifdef SERIAL_PRINT_TASKMGR
    switch (TaskMgr_InRunPrintCounter)
    {
        case 0:
        {
            TaskMgr_InRunPrintCounter = 1U;
            TaskMgr_SerialPrint = "TaskMgr: Running 1s.";
            Serial.println(TaskMgr_SerialPrint);
            break;
        }
        case 1:
        {
            TaskMgr_InRunPrintCounter = 2U;
            //TaskMgr_SerialPrint = "TaskMgr: Run: ";
            //Serial.print(TaskMgr_SerialPrint);
            break;
        }
        case 2:
        {
            TaskMgr_InRunPrintCounter = 3U;
            //TaskMgr_SerialPrint = " .";
            //TaskMgr_SerialPrint = " W: " + String(Rte_IRead_TaskMgr_WifiStatus) + " B: " + String(Rte_IRead_TaskMgr_ServerConnStatus);
            //Serial.print(TaskMgr_SerialPrint);
            break;
        }
        case 3:
        {
            TaskMgr_InRunPrintCounter = 1U;
            TaskMgr_SerialPrint = "TaskMgr: Run:  W: " + String(Rte_IRead_TaskMgr_WifiStatus) + " B: " + String(Rte_IRead_TaskMgr_ServerConnStatus);
            Serial.println(TaskMgr_SerialPrint);
            break;
        }
        default:
            break;
    }
#endif
}

void Handle_1s_Task(void)
{
    DIO_Main();
    NvM_Main();
    WiFiMgr_Main();

    //TcpServer_Main();
    Thermo_Main();  /* Thermo has to be after Tcp server in order to have its inputs */
    if (0U < TaskMgr_BlynkWaitTimeCounter)
    {
        TaskMgr_BlynkWaitTimeCounter--;
#ifdef SERIAL_PRINT_TASKMGR
        //TaskMgr_SerialPrint = "TaskMgr:----Blynk wait----.";
        //Serial.println(TaskMgr_SerialPrint);
#endif
    }
    else
    {
        BlynkClient_Main();     /* No more waiting start blynk */
    }

    //BlynkClient_Main();     /* Has to be after thermo in order to has its inputs */
    TcpServer_Main();
    /* Print function for debugging */
    TaskMgr_InRunPrint();
}
void Handle_5s_Task(void)
{
    /* do nothing */
}
void Handle_10s_Task(void)
{
    /* do nothing */
}
void Handle_30s_Task(void)
{  
    /* do nothing */
}
void Handle_1m_Task(void)
{
    /* Set flag for Blynk to send data to server */
    Rte_IWrite_TaskMgr_BlynkClient_ActionFlags |= (1 << BLYNKCLIENT_START_DATA_SENDING_ID);
}

/*
void ICACHE_RAM_ATTR Twi::busywait(unsigned int v)
{
    unsigned int i;
    for (i = 0; i < v; i++)  // loop time is 5 machine cycles: 31.25ns @ 160MHz, 62.5ns @ 80MHz
    {
        asm("nop"); // minimum element to keep GCC from optimizing this function out.
    }
}
*/