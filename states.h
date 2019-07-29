//*****************************************************************************
//
// states.h
//
// Support module for changing between flight modes and the main control logic
// for the helicopter
//
// Authors:  D White
//           J Joseph
//           R Sheridan
//
// Last modified:   02.06.2018
//
//*****************************************************************************

#ifndef STATES_H_
#define STATES_H_

//*****************************************************************************
#define MAIN_DUTY_INIT 13       // Calibration duty cycle for main rotor
#define TAIL_DUTY_INIT 15       // Calibration duty cycle for tail rotor
#define MAIN_STATIC 7           // Lowest PWM for main rotor to spin
#define TAIL_STATIC 2           // Lowest PWM for tail rotor to spin
#define MIN_YAW_THRESH -2       // Negative yaw threshold value for alitude reduction during landing
#define MAX_YAW_THRESH 2        // Positive yaw threshold value for altitude reduction during landing
#define MIN_YAW_LANDING -5      // Negative yaw threshold during landing for motors to turn off once altitude below ALT_LANDING_THRESH
#define MAX_YAW_LANDING 5       // Positive yaw threshold during landing for motors to turn off once altitude below ALT_LANDING_THRESH
#define YAW_STEP 15             // Change in yaw degrees
#define ALT_LANDING_THRESH 10   // Maximum altitude for the helicopter to land
#define LANDING_ALT_STEP 2      // Change in altitude percent for landing
#define PID_DELAY 10000         // Delay to prevent motors updating too fast
#define STATE_DELAY 3000000     // Delay to prevent helicopter going unstable after calibration
//*****************************************************************************
extern bool firstRun;                   // Bool checks if calibration need to be completed before going into FLYING state
extern int16_t QUAD;                    // Raw quadrature reading. Volatile as updates with ISR
extern int16_t CURRENT_ALT_PERCENT;     // Current helicopter altitude as a percentage
extern int16_t CURRENT_YAW_DEGREES;     // Quadrature reading converted to degrees
extern int16_t TARGET_YAW_DEGREES;      // Target yaw for PID control
extern int16_t TARGET_ALT_PERCENT;      // Target altitude percentage for PID control
extern int16_t MAIN_DUTY;               // Main rotor PWM duty cycle as percentage
extern int16_t TAIL_DUTY;               // Tail rotor PWM duty cycle as percentage
extern heliStatus status;               // Current flight mode of the helicopter
extern bool yawCalibrated;              // Bool that triggers on yaw reference interupt.  Moves heli into FLYING state once calibrated
//*****************************************************************************

// User input on SW1 changes the flight mode.
void flightSelector(void);

// Main logic control of the helicopter dependent on which state from flightSelector is active.  Calls the PID when flying and has landing sequence logic within.
void flightControl(uint16_t flightMode);

#endif /* STATES_H_ */
