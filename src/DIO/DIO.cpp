#include <DIO.h>

#include "DIO_Priv.h"

/* Global Variables */
DIO_Data_Type DIO_Data;

uint8 DIO_OneTimeCounter = 1U;
String DIO_SerialPrint = "";

/* Functions */
/********************************************************
* DIO Init function. 
********************************************************/
void DIO_Init(void)
{
#ifdef SERIAL_PRINT_DIO
    DIO_SerialPrint = "DIO: Init Start...";
    Serial.print(DIO_SerialPrint);
#endif

    /* Set IO pin mode */
    pinMode(THERMO_MASTER_RELAY_PIN, OUTPUT);
    pinMode(THERMO_SLAVE1_RELAY_PIN, OUTPUT);
    pinMode(THERMO_SLAVE2_RELAY_PIN, OUTPUT);
    pinMode(THERMO_SLAVE3_RELAY_PIN, OUTPUT);

    /* Set IO pin initial value */
#ifdef RELAY_TYPE_ACTIVE_ON_LOW
    digitalWrite(THERMO_MASTER_RELAY_PIN, HIGH);
    digitalWrite(THERMO_SLAVE1_RELAY_PIN, HIGH);
    digitalWrite(THERMO_SLAVE2_RELAY_PIN, HIGH);
    digitalWrite(THERMO_SLAVE3_RELAY_PIN, HIGH);
#else
    digitalWrite(THERMO_MASTER_RELAY_PIN, LOW);
    digitalWrite(THERMO_SLAVE1_RELAY_PIN, LOW);
    digitalWrite(THERMO_SLAVE2_RELAY_PIN, LOW);
    digitalWrite(THERMO_SLAVE3_RELAY_PIN, LOW);
#endif

    DIO_Data.State = DIO_STATE_INIT;

#ifdef SERIAL_PRINT_DIO
    DIO_SerialPrint = "  ...Stop.";
    Serial.println(DIO_SerialPrint);
#endif
}


/********************************************************
* DIO Main function. 
********************************************************/
void DIO_Main(void)
{
    if (0U < DIO_OneTimeCounter)
    {
        DIO_OneTimeCounter = 0U;
#ifdef SERIAL_PRINT_DIO
        DIO_SerialPrint = "DIO: Loop Start.";
        Serial.println(DIO_SerialPrint);
#endif
    }
    else
    {
        /* do nothing */
    }

    switch (DIO_Data.State)
    {
        case DIO_STATE_INIT:
        {
            DIO_Data.State = DIO_STATE_RUN;
            break;
        }
        case DIO_STATE_RUN:
        {
            /* Do nothing */
            break;
        }        
        default:
            break;
    }
}

/********************************************************
* DIO Sets the given pin to the provided value. 
********************************************************/
void DIO_SetPinValue(uint8 Pin, uint8 Val)
{
    if(DIO_PIN_ON == Val)
    {
        digitalWrite(Pin, HIGH);
    }
    else
    {
        digitalWrite(Pin, LOW);
    }
}

/********************************************************
* DIO Sets the given Relay pin to the provided value. 
********************************************************/
void DIO_SetRelayPinValue(uint8 Pin, uint8 Val)
{
    if(DIO_RELAY_PIN_ON == Val)
    {
#ifdef RELAY_TYPE_ACTIVE_ON_LOW
        digitalWrite(Pin, LOW);
#else
        digitalWrite(Pin, HIGH);
#endif
    }
    else
    {
#ifdef RELAY_TYPE_ACTIVE_ON_LOW
        digitalWrite(Pin, HIGH);
#else
        digitalWrite(Pin, LOW);
#endif
    }
}
