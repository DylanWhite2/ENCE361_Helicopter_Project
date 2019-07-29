//*****************************************************************************
//
// states.c - Helicopter Project.
//
// Support module for changing between flight modes and the main control logic
// for the helicopter 
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

void flightSelector(void)
{
    static bool switchStatus = 0;           // Current and previous switch status reads are compared to detect a change in switch state.
    static bool prevSwitchStatus = 1;       // Previous switch status needed incase program is started with SW1 in the 'UP' position. 
    static bool switchReset = 1;            // Bool to ensure switch state is LANDED on reset

    if (switchReset)    //ensure that the default startup mode is LANDED
    {
        status = LANDED;
        switchReset = 0;
    }

    if (status != LANDING)  //disable switch changes during landing sequence
    {
        if (status == LANDED)
        {
            switchStatus = GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_7);   

            if (switchStatus && !prevSwitchStatus && firstRun == 1)     // If it is the first run, do a yaw calibration
            {
                status = CALIBRATE;
            }
            if (switchStatus && !prevSwitchStatus && firstRun == 0)     // If it is not the first run, don't do the calibration.
            {
                TARGET_ALT_PERCENT = 0;
                TARGET_YAW_DEGREES = 0;
                status = FLYING;
            }
        }

        if (status == FLYING)
        {
            switchStatus = GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_7); 

            if (!switchStatus && prevSwitchStatus)                      // Begin the landing sequence if a SW1 is changed to 'DOWN'
            {
                status = LANDING;
            }
        }
    }

    prevSwitchStatus = switchStatus;    // Update the previous switch status for the next compare
}

void flightControl(uint16_t flightMode)
{
    if (flightMode == LANDED)
    {
        motorsOff();                // Turn the motors off if landed
    }

    if (flightMode == CALIBRATE)
    {

        uint32_t counter = 0;
        uint32_t quadCounter = 0;
        motorsOn();               // Turn the motors on to begin the calibration

        if (!yawCalibrated)
        {
            MAIN_DUTY = MAIN_DUTY_INIT;  // Values to make the heli spin on the spot to find the yaw reference
            TAIL_DUTY = TAIL_DUTY_INIT;
            setMainPWM(MAIN_DUTY);
            setTailPWM(TAIL_DUTY);

        } else {

            QUAD = 0;                   // Reset the QUAD to 0 once the calibration ISR is triggered
            CURRENT_YAW_DEGREES = 0;    // Reset the degrees as well as a precaution
            firstRun = 0;               // No longer needs to calibrate if landed then takes off again

            while (quadCounter < PID_DELAY)    // Give time for the yaw to set itself to zero before changing to the FLYING state
            {
                quadCounter++;
            }

            MAIN_DUTY = MAIN_STATIC;    // Lowest PWM duty cycle to keep motors spinning to reduce integrator windup
            TAIL_DUTY = TAIL_STATIC;
            setMainPWM(MAIN_DUTY); 
            setTailPWM(TAIL_DUTY); 

            while (counter < STATE_DELAY) // Wait for a while so the heli doesn't spin itself up too fast and go unstable
            {
                counter++;
            }

            TARGET_ALT_PERCENT = 0;     // Set the targets as 0 for PID
            TARGET_YAW_DEGREES = 0;     
            status = FLYING;
        }
    }

    if (flightMode == FLYING)
    {
       motorsOn();
       heightPID();     // Caluclate the duty cycles to keep heli stable and move to the target altitude & yaw
       tailPID();
    }

    if (flightMode == LANDING)
    {
        uint32_t delay = 0;
        uint32_t coolDown = 0;

        if (MIN_YAW_THRESH <= CURRENT_YAW_DEGREES <= MAX_YAW_THRESH)  // Slowly land the heli if it is within a the yaw threshold
        {
            if (TARGET_ALT_PERCENT > 0)
            {
                TARGET_ALT_PERCENT -= LANDING_ALT_STEP;
            }

        }

            if (CURRENT_YAW_DEGREES > MAX_YAW_THRESH)   // Update the target yaw if the heli isn't within the landing yaw threshold
            {
                if (CURRENT_YAW_DEGREES == TARGET_YAW_DEGREES)  // Only updates the target yaw once the current yaw matches the target (requires no steady state error in PID)
                {
                    TARGET_YAW_DEGREES -= YAW_STEP;
                }
            }

            if (CURRENT_YAW_DEGREES < MIN_YAW_THRESH)   // Same as above
            {
                if (CURRENT_YAW_DEGREES == TARGET_YAW_DEGREES)
                {
                    TARGET_YAW_DEGREES += YAW_STEP;
                }
            }

            if (CURRENT_ALT_PERCENT <= ALT_LANDING_THRESH && (MIN_YAW_LANDING <= CURRENT_YAW_DEGREES <= MAX_YAW_LANDING))   // If heli is within the thresholds for altitude and yaw, wind down the motors then turn them off
            {
                MAIN_DUTY = MAIN_STATIC;    //Wind down the motors to the minimum PWM
                TAIL_DUTY = TAIL_STATIC;
                setMainPWM(MAIN_DUTY);
                setTailPWM(TAIL_DUTY);

                while (coolDown < PID_DELAY)  // Wait while motors are winding down or the intertia difference between main and tail will jerk the
                                              // helicopter around before the motors switch off
                {
                    coolDown++;
                }

                coolDown = 0;

                status = LANDED;
            }


        tailPID();      // Compute duty cycle with PID with new targets
        heightPID();

        while (delay < PID_DELAY)   // Short wait so the the motors don't try and update the PWM signal too quickly.
        {
            delay++;
        }
        delay = 0;
    }
}
