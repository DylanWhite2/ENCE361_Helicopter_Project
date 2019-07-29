//*****************************************************************************
//
// helicopter.c - Helicopter Project
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
// Constants
//*****************************************************************************
#define BUF_SIZE 25             // Circular buffer size
//*****************************************************************************
// Global variables
//*****************************************************************************
uint32_t g_ulSampCnt;            // Counter for the interrupts for ADC conversion and UART send rate.
circBuf_t g_inBuffer;            // ADC alitude sensor values are stored in circular buffer for averaging
int16_t QUAD;                    // Raw quadrature reading that gets converted to real degrees
int16_t CURRENT_ALT_PERCENT;     // Altitude as a percentage
int16_t CURRENT_YAW_DEGREES;     // Quadrature reading converted to degrees
int16_t TARGET_YAW_DEGREES;      // Target yaw for PID control
int16_t TARGET_ALT_PERCENT;      // Target altitude percentage for PID control
int16_t MAIN_DUTY;               // Duty cycle of main rotor as a percent
int16_t TAIL_DUTY;               // Duty cycle of tail rotor as a percent
heliStatus status = LANDED;      // Heli status for control logic
bool yawCalibrated = 0;          // Bool to check if the reference 0 point has been found for the yaw to be set to 0
bool firstRun = 1;               // Bool checks if calibration need to be completed before going into FLYING state
//*****************************************************************************
int main(void)
{
    resetPeriphs();                         // Resets GPIO for inputs
    resetPWM();                             // Resets GPIO for PWM
    initClock();                            // Setup the clock rates
    initPWM();                              // Setup PWM output parameters
    initCircBuf(&g_inBuffer, BUF_SIZE);     // Setup circular buffer to hold ADC values
    initADC();                              // Setup the ADC for altitude measurment
    initButtons();                          // Initialise the buttons for user input changing targets
    initYaw();                              // Initialise yaw pins for quadrature decoding interupts
    initSwitch();                           // Initialise the switch for user input chaning heli status
    initDisplay();                          // Initialise the UART
    IntMasterEnable();                      // Enable all interrupts to the processor
    fillBuffer();                           // Fill up the circ buffer with ADC readings before caluclating initial altitude
    initAlt();                              // Calculate the initial altitude as a percentage after interrupts enabled.

    while (1)                   //Simple kernel (gadfly loop) that loops forever until reset
    {
        updateButtons();        // Button polling
        buttonInput();          // Check for updates on buttons for yaw and altitude target changes
        convertToDegrees();     // Convert raw quadrature reading into degrees
        calculateAlt();         // Calculate the heli altitude from ADC voltage
        calcAltPC();            // Convert raw altitude into percentage
        flightSelector();       // Check for update on switch.  Switches between LANDED -> CALIBRATE -> FLYING -> LANDING
        flightControl(status);  // Set what the heli should be doing based on switch position
        display();              // Send all the info over the UART
    }
}
