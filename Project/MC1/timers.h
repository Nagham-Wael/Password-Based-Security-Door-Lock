/********************************************************************************
 * Module: Timer
 *
 * File Name: timers.h
 *
 * Author: Nagham
 *
 * Created on: Dec 5, 2020
 *
 * Description: header file of timer driver
 *******************************************************************************/

#ifndef TIMERS_H_
#define TIMERS_H_

/*******************************************************************************
 *                                   INCLUDES                                  *
 *******************************************************************************/

#include "std_types.h"
#include "micro_config.h"
#include "common_macros.h"

/*******************************************************************************
 *                              Types Declaration                              *
 *******************************************************************************/

typedef enum
{
	TIMER0,TIMER1,TIMER2
}Timer_Num;
typedef enum
{
	OVERFLOW,COMPARE
}Timer_Mode;
typedef enum
{
	NO_CLOCK,F_CPU_CLOCK,F_CPU_8,F_CPU_32,F_CPU_64,F_CPU_128,F_CPU_256,F_CPU_1024
}Timer_Prescalar;

typedef struct
{
	Timer_Num num;
	Timer_Mode mode;
	Timer_Prescalar scalar;
	uint16 s_initial; /*Take the initial value from the user*/
	uint16 s_comp; /*Take the compare value from user, put 0 if using overflow mode*/
}Timer_ConfigType;

/*******************************************************************************
 *                            FUNCTIONS DECLARATIONS                           *
 *******************************************************************************/

/*
 * Function that initialize the timer
 * Given the required timer and the mode, it initiate the registers
 * It also initiate the initial value and the compare value according to the required
 */
void Timer_init(const Timer_ConfigType * Config_Ptr);

/*Function to set the Call Back function address for Timer0, overflow mode*/
void Timer0_Ovf_setCallBack(void(*a_t0_ovf_ptr)(void));

/*Function to set the Call Back function address for Timer0, compare mode*/
void Timer0_CTC_setCallBack(void(*a_t0_ctc_ptr)(void));

/*Function to set the Call Back function address for Timer1, overflow mode*/
void Timer1_Ovf_setCallBack(void(*a_t1_ovf_ptr)(void));

/*Function to set the Call Back function address for Timer1, compare mode*/
void Timer1_CTC_setCallBack(void(*a_t1_ctc_ptr)(void));

/*Function to set the Call Back function address for Timer2, overflow mode*/
void Timer2_Ovf_setCallBack(void(*a_t2_ovf_ptr)(void));

/*Function to set the Call Back function address for Timer2, compare mode*/
void Timer2_CTC_setCallBack(void(*a_t2_ctc_ptr)(void));


#endif /* TIMERS_H_ */
