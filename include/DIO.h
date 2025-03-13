#ifndef DIO_H
#define DIO_H 1
/**************** Includes ************/
#include <Arduino.h>



/**************** Defines ************/
#define DIO_PIN_OFF                     0U
#define DIO_PIN_ON                      1U

#define DIO_RELAY_PIN_OFF               0U
#define DIO_RELAY_PIN_ON                1U

/**************** Varables ************/



/**************** Functions ************/
void DIO_Init(void);
void DIO_Main(void);

void DIO_SetPinValue(uint8 Pin, uint8 Val);
void DIO_SetRelayPinValue(uint8 Pin, uint8 Val);

#endif