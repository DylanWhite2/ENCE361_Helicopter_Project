//*****************************************************************************
//
// adc.h
//
// Support module for all of the ADC altitude readings and conversions.
//
// Authors:  D White
//           J Joseph
//           R Sheridan
//
// Last modified:   02.06.2018
//
//*****************************************************************************

#ifndef ADC_H_
#define ADC_H_

//*****************************************************************************
#define BUF_SIZE 25             // Circular buffer size
#define MAX_DELTA_V 993         // 4-bit representation of 0.8V
#define MAX_ALT_PERCENT 100  // Maximum allowed altitude percentage
//*****************************************************************************
extern circBuf_t g_inBuffer;                   // Buffer of size BUF_SIZE integers (sample values)
volatile uint16_t CURRENT_ALT;          // Current raw altitude 12 bit. Volatile as updates with the ADC ISR
extern int16_t CURRENT_ALT_PERCENT;     // Altitude as a percentage
int16_t INIT_ALT;                       // Used to calibrate the 'zero' height. Updated at the start of the program
//*****************************************************************************

// The handler for the ADC conversion complete interrupt. Writes to the circular buffer.
void ADCIntHandler(void);

// Calculates the current altitude of the helicopter by averaging the buffer contents
void calculateAlt(void);

// Calibrates the initial altitude
void initAlt(void);

// Converts the raw ADC sensor data into a percentage
void calcAltPC(void);

// Fills the circular buffer at program start to find the landed ADC sensor reading
void fillBuffer(void);

#endif /* ADC_H_ */
