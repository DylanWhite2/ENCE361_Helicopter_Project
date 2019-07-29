//*****************************************************************************
//
// pid.c - Helicopter Project.
//
// Support module for PID control functions for the main and tail rotors
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

void heightPID(void)
{
    int32_t error = 0;              // Error calculated as the TARGET - CURRENT to give positive result - aggressiveness of changes (pole locations)
    int32_t derivative = 0;         // The change in error between samples - reduces overshoot
    static int32_t integral = 0;    // The accumulated error that is integrated - reduces steady state error
    static int32_t lastError = 0;   // Used to caluclate the derivative 
    int16_t control = 0;            // PWM duty cycle output to the motors

    float kp = 0.4;                 // Low proportional gain due to variances in helicopters 
    float kd = 2.0;                 // Hard to tell how much effect this has with low frame rate
    float ki = 0.00005;             // Very small integral gain due to the high sampling rate

    error = (TARGET_ALT_PERCENT - CURRENT_ALT_PERCENT);
    integral += error;
    derivative = (error - lastError);
    lastError = error;

    control = ((kp * error) + (kd * derivative) + (ki * integral)); // PID

    MAIN_DUTY = checkMainDuty(control); // Check the duty cycle is within the limits

    setMainPWM(MAIN_DUTY);

}

void tailPID(void)
{
    int32_t error = 0;              // Error calculated as the TARGET - CURRENT to give positive result - aggressiveness of changes (pole locations)
    int32_t derivative = 0;         // The change in error between samples - reduces overshoot
    static int32_t integral = 0;    // The accumulated error that is integrated - reduces steady state error
    static int32_t lastError = 0;   // Used to caluclate the derivative 
    int16_t control = 0;            // PWM duty cycle output to the motors

    float kp = 1.4;                 // Low proportional gain due to variances in helicopters 
    float kd = 5.0;                 // Hard to tell how much effect this has with low frame rate
    float ki = 0.0001;              // Very small integral gain due to the high sampling rate

    error = (TARGET_YAW_DEGREES - CURRENT_YAW_DEGREES);
    integral = integral + error;
    derivative = (error - lastError);
    lastError = error;

    control = ((kp * error) + (kd * derivative) + (ki * integral)); // PID

    TAIL_DUTY = checkTailDuty(control); // Check the duty cycle is within the limits

    setTailPWM(TAIL_DUTY);

}
