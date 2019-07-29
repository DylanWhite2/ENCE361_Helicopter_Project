//*****************************************************************************
//
// yaw.c - Helicopter Project.
//
// Support module for quadrature decoding, yaw conversions and yaw calibration
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

void pinChangeISR(void)
{
    int16_t pinAState;                      // State of pin A from the interrupt
    int16_t pinBState;                      // State of pin B from the interrupt
    static int16_t pinBPrevState;           // Previous A pin state to compare with new state.
    static int16_t pinAPrevState;           // Previous B pin state to compare with new state
    int16_t dir;                            // Clockwise/counter-clockwise rotation

    GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1); //clears the current state before the read to be compared to the previous state

    pinAState = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0);   //read the states of the pins
    pinBState = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_1);


    if (pinAState == pinAPrevState) //counter-clockwise rotation as B is leading A
    {
        dir = CCW;
    }

    if (pinBState == pinBPrevState) //clockwise rotation as A is leading B
    {
        dir = CW;
    }

    //logic to calculate to increase to decrease quadrature. 
    if (!pinAState && !pinBState) // State 00
    {
        if (dir == CW)
        {
            QUAD++;
        }
        if (dir == CCW)
        {
            QUAD--;
        }
    }

    if (!pinAState && pinBState)  //State 01
    {
        if (dir == CW)
        {
            QUAD--;
        }
        if (dir == CCW)
        {
            QUAD++;
        }
    }

    if (pinAState && pinBState)   //State 11
    {
        if (dir == CW)
        {
            QUAD++;
        }
        if (dir == CCW)
        {
            QUAD--;
        }
    }

    if (pinAState && !pinBState)  //State 10
    {
        if (dir == CW)
        {
            QUAD--;
        }
        if (dir == CCW)
        {
            QUAD++;
        }
    }

    pinAPrevState = pinAState; //once logic has completed, set the previous state to the current state so next interrupt can compare to find direction
    pinBPrevState = pinBState;
}

void yawCalibrate(void)
{
    GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_4);
    yawCalibrated = 1; //Updates the reference over the 0 point
}

void initYaw(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);    // Init GPIOB for quadrature readings

    GPIOIntRegister(GPIO_PORTB_BASE, pinChangeISR); // Setup the pin change interrupt

    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1); // Setting which pins to trigger the pin change ISR
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);   // From the documentation

    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_BOTH_EDGES);  // Sets which events to trigger the ISR. Want both rising and falling for this implementation

    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);    // Enable the interrupts for the quadrature reading.
    IntEnable(INT_GPIOB);   //INT_GPIOB from inc/hw_ints.h

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);    // All that follows is for the yaw reference pin
    GPIOIntRegister(GPIO_PORTC_BASE, yawCalibrate);

    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);  // Sets the event for when it goes low

    GPIOIntEnable(GPIO_PORTC_BASE, GPIO_PIN_4);
    IntEnable(INT_GPIOC);
}

void convertToDegrees(void)
{
    CURRENT_YAW_DEGREES = (((DEGREES * QUAD) + (YAW_SLOTS * YAW_STATES)) / (YAW_SLOTS * YAW_STATES) - 1);   // Convert the quad reading to degrees. This formula allows for +ve and -ve                                                                                                // values. Similar to the altitude percentage conversion equation.
}
