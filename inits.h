//*****************************************************************************
//
// inits.h
//
// Support module for all system initialisations and resets
//
// Authors:  D White
//           J Joseph
//           R Sheridan
//
// Last modified:   02.06.2018
//*****************************************************************************

#ifndef INITS_H_
#define INITS_H_

//*****************************************************************************
#define SAMPLE_RATE_HZ 2000      // ADC sample rate (Hz
#define BAUD_RATE 9600           // UART baud rate (bits per second)
//*****************************************************************************
extern uint32_t g_ulSampCnt;     // Number of samples undergone ADC
//*****************************************************************************

// The interrupt handler for the for SysTick interrupt.
void SysTickIntHandler(void);

// Initialization functions for the clock (incl. SysTick), ADC, display
void initClock(void);

// ISR for when the soft reset button is pressed
void resetInt(void);

// Intialization function for the soft reset ISR
void initResetPin(void);

// Initialization function for the UART transmissions.  Note:  This code is taken from the uartdemo program that was shown in lectures.  It is not original code by us.
void initDisplay(void);

// Initialization for the slide switch used to control the flight status. LANDED -> CALIBRATE -> FLYING -> LANDING
void initSwitch(void);

// Resets all peripherals before program begins as a precaution to avoid erroneous behaviour
void resetPeriphs(void);

// The ADC0 peripheral must be enabled for configuration and use
void initADC(void);

#endif /* INITS_H_ */
