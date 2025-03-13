#include <Arduino.h>
#include <Ticker.h>

#include <main.h>

Ticker timerISR;

void setup() {
  /* Initialize Global timer */
  GlobalTimer = 0U;

  Serial.begin(SERIAL_BAUDRATE);

  /* Call Task Manager Init function */
  TaskMgr_Init();

  /* Attach handler to timer interrupt */
  timerISR.attach(0.5,HandleTimerInterrupt);
}

void loop() {
  /* Execute task manager */
  TaskMgr_Main();
}

void HandleTimerInterrupt(void)
{
  /* Increment global timer. */
  GlobalTimer++;

  if (0U == (GlobalTimer % TIMER_1S))
  {
    /* Flag 1s Task as to be executed */
    Rte_IWrite_TaskMgr_Handler_Flags |= 1 << TIMER_1S_ID;
  }
  else
  {
    /* do nothing */
  }
  if (0U == (GlobalTimer % TIMER_5S))
  {
    /* Flag 5s Task as to be executed */
    Rte_IWrite_TaskMgr_Handler_Flags |= 1 << TIMER_5S_ID;
  }
  else
  {
    /* do nothing */
  }
  if (0U == (GlobalTimer % TIMER_10S))
  {
    /* Flag 10s Task as to be executed */
    Rte_IWrite_TaskMgr_Handler_Flags |= 1 << TIMER_10S_ID;
  }
  else
  {
    /* do nothing */
  }
  if (0U == (GlobalTimer % TIMER_30S))
  {
    /* Flag 30s Task as to be executed */
    Rte_IWrite_TaskMgr_Handler_Flags |= 1 << TIMER_30S_ID;
  }
  else
  {
    /* do nothing */
  }
  if (0U == (GlobalTimer % TIMER_1M))
  {
    /* Flag 1m Task as to be executed */
    Rte_IWrite_TaskMgr_Handler_Flags |= 1 << TIMER_1M_ID;
  }
  else
  {
    /* do nothing */
  }
  if (GlobalTimer == TIMER_1M)
  {
    /* Reset timer */
    GlobalTimer = 0;
  }
  else
  {
    /* do nothing */
  }
}