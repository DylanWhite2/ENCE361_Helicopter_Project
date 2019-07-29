//*****************************************************************************
//
// inputs.c - Helicopter Project.
//
//  Support module for user input changing target parameters
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

void checkTargets(void)
{
    if (TARGET_ALT_PERCENT < MIN_HEIGHT)     // Don't let target alitude be below 0%
    {
        TARGET_ALT_PERCENT = MIN_HEIGHT;
    }
    if (TARGET_ALT_PERCENT > MAX_HEIGHT)   // Don't let target alitude be above 100%
    {
        TARGET_ALT_PERCENT = MAX_HEIGHT;
    }
}

uint16_t checkMainDuty(uint16_t duty)
{
    if(duty > MAX_MAIN_DUTY)               // Maximum PWM of 85% to prevent instability
    {
        duty = MAX_MAIN_DUTY;              
    } else if(duty < MIN_MAIN_DUTY)        // Minimum PWM of 15% is to reduce integrator windup
    {
        duty = MIN_MAIN_DUTY;
    }

    return duty;
}

uint16_t checkTailDuty(uint16_t duty)
{
    if(duty > MAX_TAIL_DUTY)               // Given PWM limits of 2 - 98% for tail rotor
    {
        duty = MAX_TAIL_DUTY;
    } else if(duty < MIN_TAIL_DUTY)
    {
        duty = MIN_TAIL_DUTY;
    }
    return duty;
}

void buttonInput(void)
{
   if (status == FLYING)  //disables button presses during landed, calibrate and landing sequence
    {
        if (checkButton(UP) == PUSHED)      // Updates target altitude
        {
            TARGET_ALT_PERCENT += ALT_STEP;
            checkTargets();                 // Checks the target is legal
        }
        if (checkButton(DOWN) == PUSHED)    // Updates target altitude
        {
            TARGET_ALT_PERCENT -= ALT_STEP;
            checkTargets();                 // Checks the target is legal
        }
        if (checkButton(RIGHT) == PUSHED)   // Updates target yaw
        {
            TARGET_YAW_DEGREES += YAW_STEP;
        }
        if (checkButton(LEFT) == PUSHED)    // Updates target yaw
        {
            TARGET_YAW_DEGREES -= YAW_STEP;
        }
    }
}
