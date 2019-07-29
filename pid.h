//*****************************************************************************
//
// pid.h
//
// Support module for PID control functions for the main and tail rotors
//
// Authors:  D White
//           J Joseph
//           R Sheridan
//
// Last modified:   02.06.2018
//*****************************************************************************

#ifndef PID_H_
#define PID_H_

//*****************************************************************************
extern int16_t CURRENT_ALT_PERCENT;     // Current helicopter altitude as a percentage
extern int16_t CURRENT_YAW_DEGREES;     // Quadrature reading converted to degrees
extern int16_t TARGET_YAW_DEGREES;      // Target yaw for PID control
extern int16_t TARGET_ALT_PERCENT;      // Target altitude percentage for PID control
extern int16_t MAIN_DUTY;               // Main rotor PWM duty cycle as percentage
extern int16_t TAIL_DUTY;               // Tail rotor PWM duty cycle as percentage
//*****************************************************************************

// PID for the main rotor
void heightPID(void);

// PID for the tail rotor
void tailPID(void);

#endif /* PID_H_ */
