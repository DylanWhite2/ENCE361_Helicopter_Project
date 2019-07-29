//*****************************************************************************
//
// adc.c - Helicopter Project.
//
// Support module for all of the ADC altitude readings and conversions.
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

void ADCIntHandler(void)
{
    uint32_t ulValue;

    ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);     // Get the single sample from ADC0.

    writeCircBuf(&g_inBuffer, ulValue);    // Place it in the circular buffer (advancing write index)

    ADCIntClear(ADC0_BASE, 3);      // Clean up, clearing the interrupt
}

void calculateAlt(void)
{
    uint16_t BUFFER_INDEX;
    int32_t BUFFER_SUM;

    BUFFER_SUM = 0; // Reset the buffer sum to 0 each iteration to get an updated mean value

    for (BUFFER_INDEX = 0; BUFFER_INDEX < BUF_SIZE; BUFFER_INDEX++)  // Calculating mean altitude in the buffer
        BUFFER_SUM = BUFFER_SUM + readCircBuf(&g_inBuffer);

    CURRENT_ALT = (2 * BUFFER_SUM + BUF_SIZE) / 2 / BUF_SIZE;
}

void initAlt(void)
{
    uint16_t sum = 0;
    uint16_t i = 0;
    INIT_ALT = 0;

    ADCIntDisable(ADC0_BASE, 3);    // Disable interrupts while calculating to not change the buffer contents

    for (i = 0; i < BUF_SIZE; i++)
        sum = sum + readCircBuf (&g_inBuffer);
    INIT_ALT = ((2 * sum + BUF_SIZE) / 2 / BUF_SIZE);   // Calculate the mean value in the buffer and set global

    ADCIntEnable(ADC0_BASE, 3);     // Re-enable interrupts cause that's obvious
}

void calcAltPC(void)
{
    volatile uint16_t maxAlt = INIT_ALT - MAX_DELTA_V;
    CURRENT_ALT_PERCENT = (MAX_ALT_PERCENT - (MAX_ALT_PERCENT * (CURRENT_ALT - maxAlt) / (INIT_ALT - maxAlt)));
}

void fillBuffer(void)
{
    while (g_ulSampCnt < 2000)    // Fill up the buffer before calibrating landed voltage
    {
        continue;
    }
}


