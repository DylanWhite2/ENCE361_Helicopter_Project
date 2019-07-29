//*****************************************************************************
//
// yaw.h
//
// Support module for quadrature decoding, yaw conversions and yaw calibration
//
// Authors:  D White
//           J Joseph
//           R Sheridan
//
// Last modified:   02.06.2018
//
//*****************************************************************************

#ifndef YAW_H_
#define YAW_H_

//*****************************************************************************
#define YAW_SLOTS 112                   // Number of slots on the encoder wheel
#define YAW_STATES  4                   // Number of possible states for yaw control
#define DEGREES 360                     // Degrees in a circle
//*****************************************************************************
typedef enum {CW = 0, CCW} yawRotation; // Yaw rotation modes
//*****************************************************************************
extern int16_t QUAD;                    // Raw quadrature reading
extern int16_t CURRENT_YAW_DEGREES;     // Quadrature reading converted to degrees
extern bool yawCalibrated;              // Bool to check if the reference 0 point has been found for the yaw to be set to 0
//*****************************************************************************

// Interrupt service routine for pin change interrupts measuring quadrature changes.
void pinChangeISR(void);

//Inits the GPIO pins for quadrature reading
void initYaw(void);

// ISR for the yaw calibration.  Triggers when reference pin goes low. Sets a bool to true.
void yawCalibrate(void);

// Converts the raw quadrature reading into degrees
void convertToDegrees(void);

#endif /* YAW_H_ */
