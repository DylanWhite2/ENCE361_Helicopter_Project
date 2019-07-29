//*****************************************************************************
//
// inputs.h
//
//  Support module for user input changing target parameters
//
// Authors:  D White
//           J Joseph
//           R Sheridan
//
// Last modified:   02.06.2018
//*****************************************************************************

#ifndef INPUTS_H_
#define INPUTS_H_

//*****************************************************************************
#define ALT_STEP 10                     // Change in target altitude percent on button press
#define YAW_STEP 15                     // CHange in target yaw degrees on button press
#define MAX_HEIGHT 100                  // Max target altitude percent for helicopter
#define MIN_HEIGHT 0                    // Min target altitude percent for helicopter
#define MAX_MAIN_DUTY 85                // Max duty cycle percent for main rotor
#define MIN_MAIN_DUTY 15                // Min duty cycle percent for main rotor
#define MAX_TAIL_DUTY 98                // Max duty cycle percent for tail rotor
#define MIN_TAIL_DUTY 2                 // Min duty cycle percent for tail rotor
//*****************************************************************************
extern int16_t CURRENT_ALT_PERCENT;     // Current helicopter altitude as a percentage
extern int16_t CURRENT_YAW_DEGREES;     // Quadrature reading converted to degrees
extern int16_t TARGET_YAW_DEGREES;      // Target yaw for PID control
extern int16_t TARGET_ALT_PERCENT;      // Target altitude percentage for PID control
extern int16_t MAIN_DUTY;               // Main rotor PWM duty cycle as percentage
extern int16_t TAIL_DUTY;               // Tail rotor PWM duty cycle as percentage
extern heliStatus status;               // Current flight mode of the helicopter
//*****************************************************************************

// Checks the user input altitude target is within the limits of the program (0-100%)
void checkTargets(void);

// Checks the main rotor duty cycle set by the PID does not exceed the limits (15-85%)
uint16_t checkMainDuty(uint16_t duty);

// Checks the tail rotor duty cycle set by the PID does not exceed the limits   (2-98%)
uint16_t checkTailDuty(uint16_t duty);

// Polls buttons for user input to change target altitude and yaw
void buttonInput(void);

#endif /* INPUTS_H_ */
