//*****************************************************************************
//
// pwm.h
//
// Support module for all PWM intitalisation and motor controls
//
// Authors:  D White
//           J Joseph
//           R Sheridan
//
// Last modified:   02.06.2018
//*****************************************************************************

#ifndef PWM_H_
#define PWM_H_

//*****************************************************************************
#define PWM_START_RATE_HZ  250
#define PWM_DIVIDER_CODE   SYSCTL_PWMDIV_1
#define PWM_DIVIDER        1
#define DUTY_MAX           98
#define DUTY_MAIN_MIN      15
#define DUTY_TAIL_MIN      2

//  PWM Hardware Details M0PWM7 (gen 3)
//  ---Main PWM: PC5, J4-05
#define PWM_MAIN_BASE        PWM0_BASE
#define PWM_MAIN_GEN         PWM_GEN_3
#define PWM_MAIN_OUTNUM      PWM_OUT_7
#define PWM_MAIN_OUTBIT      PWM_OUT_7_BIT
#define PWM_MAIN_PERIPH_PWM  SYSCTL_PERIPH_PWM0
#define PWM_MAIN_PERIPH_GPIO SYSCTL_PERIPH_GPIOC
#define PWM_MAIN_GPIO_BASE   GPIO_PORTC_BASE
#define PWM_MAIN_GPIO_CONFIG 0x00021404
#define PWM_MAIN_GPIO_PIN    GPIO_PIN_5

//  PWM Hardware Details M1PWM5 (gen 3)
//  ---Tail PWM: PF1, J3-10
#define PWM_TAIL_BASE        PWM1_BASE
#define PWM_TAIL_GEN         PWM_GEN_2
#define PWM_TAIL_OUTNUM      PWM_OUT_5
#define PWM_TAIL_OUTBIT      PWM_OUT_5_BIT
#define PWM_TAIL_PERIPH_PWM  SYSCTL_PERIPH_PWM1
#define PWM_TAIL_PERIPH_GPIO SYSCTL_PERIPH_GPIOF
#define PWM_TAIL_GPIO_BASE   GPIO_PORTF_BASE
#define PWM_TAIL_GPIO_CONFIG 0x00050405
#define PWM_TAIL_GPIO_PIN    GPIO_PIN_1
//*****************************************************************************

// Resets the PWM peripherals as a precaution
void resetPWM(void);

// Sets the main rotor duty cycle from the PID
void setMainPWM (uint32_t mainDuty);

// Sets the tail rotor duty cycle from the PID
void setTailPWM (uint32_t tailDuty);

// Intialization of PWM parameters
void initPWM (void);

// Turns the PWM output on
void motorsOn(void);

// Turns the PWM output off
void motorsOff(void);

#endif /* PWM_H_ */
