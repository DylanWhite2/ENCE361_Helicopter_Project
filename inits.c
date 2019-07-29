//*****************************************************************************
//
// inits.c - Helicopter Project.
//
// Support module for all system initialisations and resets
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

void SysTickIntHandler(void)
{
    ADCProcessorTrigger(ADC0_BASE, 3);  //initiate a conversion
    g_ulSampCnt++;  // Cheekily used as a counter for the UART as well :)
}

void initClock (void)
{
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);     // Set the clock rate to 20 MHz

    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);     // Set up the period for the SysTick timer.  The SysTick timer period is
                                                            // set as a function of the system clock.

    SysTickIntRegister(SysTickIntHandler);   // Register the interrupt handler

    SysTickIntEnable();     // Enable interrupt and device
    SysTickEnable();
}

void ResetInt(void)
{
    GPIOIntClear(GPIO_PORTA_BASE, GPIO_PIN_6);  // Clear the interrupt
    SysCtlReset();
}

void initResetPin(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); // Init GPIOA for soft reset
    GPIOIntRegister(GPIO_PORTA_BASE, ResetInt);  // Setup the pin change interrupt

    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_6);  // Setting which pins to trigger the pin change ISR
    GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);  // From the documentation

    GPIOIntTypeSet(GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_FALLING_EDGE); // Sets which events to trigger the ISR. Active LOW

    GPIOIntEnable(GPIO_PORTA_BASE, GPIO_PIN_6); // Enable the interrupts for the soft reset
    IntEnable(INT_GPIOA);
}

void initDisplay(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);                        // Enable GPIO port A which is used for UART0 pins.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);          // Select the alternate (UART) function for these pins.

    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), BAUD_RATE,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |    // Setting serial params
                         UART_CONFIG_PAR_NONE));

    UARTFIFOEnable(UART0_BASE);                                         // Enable the UART
    UARTEnable(UART0_BASE);
}

void initSwitch(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);                                         // Init GPIOA for soft reset
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_7);                                   // Setting which pins 
    GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD); // From the documentation
}

void resetPeriphs(void)
{
    SysCtlPeripheralReset(UP_BUT_PERIPH);           // Reset all the peripherals to clear anything before the program starts
    SysCtlPeripheralReset(DOWN_BUT_PERIPH);
    SysCtlPeripheralReset(LEFT_BUT_PERIPH);
    SysCtlPeripheralReset(RIGHT_BUT_PERIPH);
    SysCtlPeripheralReset(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralReset(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralReset(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralReset(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralReset(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralReset(SYSCTL_PERIPH_GPIOF);
}

void initADC (void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);     // The ADC0 peripheral must be enabled for configuration and use.

    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);       // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
                                                                        // will do a single sample when the processor sends a signal to start the
                                                                        // conversion.

    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE |  // Configure step 0 on sequence 3.  Sample channel 9 which is PE4 input
                             ADC_CTL_END);

    ADCSequenceEnable(ADC0_BASE, 3);        // Since sample sequence 3 is now configured, it must be enabled.

    ADCIntRegister (ADC0_BASE, 3, ADCIntHandler);       // Register the interrupt handler

    ADCIntEnable(ADC0_BASE, 3);         // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
}
