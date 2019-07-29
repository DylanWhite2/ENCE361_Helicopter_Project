//*****************************************************************************
//
// pwm.c - Helicopter Project.
//
// Support module for all PWM intitalisation and motor controls
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

void resetPWM(void)
{
    SysCtlPeripheralReset(PWM_MAIN_PERIPH_GPIO);
    SysCtlPeripheralReset(PWM_MAIN_PERIPH_PWM);
    SysCtlPeripheralReset(PWM_TAIL_PERIPH_GPIO);
    SysCtlPeripheralReset(PWM_TAIL_PERIPH_PWM);
}

void setMainPWM (uint32_t mainDuty)
{
    uint32_t ui32Period = SysCtlClockGet() / PWM_DIVIDER / PWM_START_RATE_HZ;           // Period for PWM
    PWMGenPeriodSet(PWM_MAIN_BASE, PWM_MAIN_GEN, ui32Period);
    PWMPulseWidthSet(PWM_MAIN_BASE, PWM_MAIN_OUTNUM, (ui32Period * mainDuty / 100));    // Set the duty cycle
}

void setTailPWM (uint32_t tailDuty)
{
    uint32_t ui32Period = SysCtlClockGet() / PWM_DIVIDER / PWM_START_RATE_HZ;           // Period for PWM
    PWMGenPeriodSet(PWM_TAIL_BASE, PWM_TAIL_GEN, ui32Period);
    PWMPulseWidthSet(PWM_TAIL_BASE, PWM_TAIL_OUTNUM, (ui32Period * tailDuty / 100));    // Set the duty cycle
}

void initPWM (void)
{
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_PWM);        // Setup the PWM generation
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_GPIO);

    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_GPIO);

    GPIOPinConfigure(PWM_MAIN_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_MAIN_GPIO_BASE, PWM_MAIN_GPIO_PIN);

    GPIOPinConfigure(PWM_TAIL_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_TAIL_GPIO_BASE, PWM_TAIL_GPIO_PIN);

    PWMGenConfigure(PWM_MAIN_BASE, PWM_MAIN_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMGenConfigure(PWM_TAIL_BASE, PWM_TAIL_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMGenEnable(PWM_MAIN_BASE, PWM_MAIN_GEN);
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, false);  // Don't ouput anything once setup

    PWMGenEnable(PWM_TAIL_BASE, PWM_TAIL_GEN);
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, false);  // Don't output anything once setup
}

void motorsOn(void)
{
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, true);   // Enable the output - motors on
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, true);
}

void motorsOff(void)
{
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, false);  // Disable the output - motors off
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, false);
}
