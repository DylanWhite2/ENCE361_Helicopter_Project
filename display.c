//*****************************************************************************
//
// display.c - Helicopter Project.
//
// Support module for interfacing the UART serial communications  
//
// Authors:  D White
//           J Joseph
//           R Sheridan
//
// Last modified:   02.06.2018
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"
#include "inc/hw_ints.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "circBufT.h"
#include "buttons4.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "OrbitOLED/lib_OrbitOled/OrbitOled.h"
#include "utils/uartstdio.h"
#include "driverlib/uart.h"
#include "stdio.h"
#include "stdlib.h"

#include "inits.h"
#include "yaw.h"
#include "pwm.h"
#include "display.h"
#include "pid.h"
#include "inputs.h"
#include "adc.h"
#include "states.h"
//*****************************************************************************
void UARTSend (char *pucBuffer)
{
    while(*pucBuffer)   // Loop while there are more characters to send.
    {
        UARTCharPut(UART0_BASE, *pucBuffer);   // Write the next character to the UART Tx FIFO.
        pucBuffer++;
    }
}

void display(void)
{
    char string[50];    // Set aside some memory for the strings to be sent over the UART

    if ((g_ulSampCnt % 500) == 0){
        sprintf(string, "Altitude: %d%% [%d]\n\r", CURRENT_ALT_PERCENT, TARGET_ALT_PERCENT);
        UARTSend (string);
        sprintf(string, "Yaw: %d [%d]\n\r", CURRENT_YAW_DEGREES, TARGET_YAW_DEGREES);
        UARTSend (string);
        sprintf(string, "Main: %d Tail: %d\n\r", MAIN_DUTY, TAIL_DUTY);
        UARTSend (string);

        if (status == LANDED)
        {
            sprintf(string, "Status: LANDED\n\r");
        }
        if (status == CALIBRATE)
        {
            sprintf(string, "Status: CALIBRATE\n\r");
        }
        if (status == FLYING)
        {
            sprintf(string, "Status: FLYING\n\r");
        }
        if (status == LANDING)
        {
            sprintf(string, "Status: LANDING\n\r");
        }
        UARTSend(string);

        sprintf(string, "---------------\n\r");
        UARTSend(string);
    }
}
