//*****************************************************************************
//
// display.h
//
// Support module for interfacing the UART serial communications
//
// Authors:  D White
//           J Joseph
//           R Sheridan
//
// Last modified:   02.06.2018
//
//*****************************************************************************

#ifndef DISPLAY_H_
#define DISPLAY_H_

//*****************************************************************************
uint32_t g_ulSampCnt;
typedef enum {LANDED = 0, CALIBRATE, FLYING, LANDING} heliStatus;  //the different flight states
//*****************************************************************************
extern int16_t CURRENT_ALT_PERCENT;     // Current helicopter altitude as a percentage
extern int16_t CURRENT_YAW_DEGREES;     // Quadrature reading converted to degrees
extern int16_t TARGET_YAW_DEGREES;      // Target yaw for PID control
extern int16_t TARGET_ALT_PERCENT;      // Target altitude percentage for PID control
extern int16_t MAIN_DUTY;               // Main rotor PWM duty cycle as percentage
extern int16_t TAIL_DUTY;               // Tail rotor PWM duty cycle as percentage
extern heliStatus status;               // Current flight mode of the helicopter
//*****************************************************************************

// Writes to the UART using FIFO queue.  Note: This function was taken from the uartdemo code given in lectures and not original code by us
void UARTSend (char *pucBuffer);

// Function that controls what is sent over the UART and how often.  Makes use of the systick timer to trigger sends
void display(void);

#endif /* DISPLAY_H_ */
