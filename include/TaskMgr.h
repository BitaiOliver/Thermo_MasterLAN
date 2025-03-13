#ifndef TASKMGR_H
#define TASKMGR_H 1
/**************** Includes ************/
#include <Arduino.h>



/**************** Defines ************/
#define TIMER_1S                2U
#define TIMER_5S                10U
#define TIMER_10S               20U
#define TIMER_30S               60U
#define TIMER_1M                120U /* put back to 120 */


#define TIMER_1S_ID             0U
#define TIMER_5S_ID             1U
#define TIMER_10S_ID            2U
#define TIMER_30S_ID            3U
#define TIMER_1M_ID             4U

#define NUMBER_OF_TIMERS        5U


/**************** Varables ************/


/**************** Functions ************/

void TaskMgr_Init(void);
void TaskMgr_Main(void);



#endif