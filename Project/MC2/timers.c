/********************************************************************************
 * Module: Timer
 *
 * File Name: timers.c
 *
 * Author: Nagham
 *
 * Created on: Dec 5, 2020
 *
 * Description: source file of timer driver
 *******************************************************************************/

/*******************************************************************************
 *                                   INCLUDES                                  *
 *******************************************************************************/

#include "timers.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/*Global variables to hold the addresses of the call back functions in the application*/
static volatile void (*g_t_0_ovf_callBackPtr)(void) = NULL_PTR; /*Timer0,overflow mode*/
static volatile void (*g_t_0_ctc_callBackPtr)(void) = NULL_PTR; /*Timer0,compare mode*/
static volatile void (*g_t_1_ovf_callBackPtr)(void) = NULL_PTR; /*Timer1,overflow mode*/
static volatile void (*g_t_1_ctc_callBackPtr)(void) = NULL_PTR; /*Timer1,compare mode*/
static volatile void (*g_t_2_ovf_callBackPtr)(void) = NULL_PTR; /*Timer2,overflow mode*/
static volatile void (*g_t_2_ctc_callBackPtr)(void) = NULL_PTR; /*Timer2,compare mode*/

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/


ISR(TIMER0_OVF_vect)
{
	if(g_t_0_ovf_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after each overflow */
		(*g_t_0_ovf_callBackPtr)();
		/* another method to call the function using pointer to function t_0_ovf_callBackPtr(); */
	}
}
ISR(TIMER0_COMP_vect)
{
	if(g_t_0_ctc_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application
		 * after the timer reach the compare value
		 */
		(*g_t_0_ctc_callBackPtr)();
		/* another method to call the function using pointer to function t_0_ctc_callBackPtr(); */
	}
}
ISR(TIMER1_OVF_vect)
{
	if(g_t_1_ovf_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after each overflow */
		(*g_t_1_ovf_callBackPtr)();
		/* another method to call the function using pointer to function t_1_ovf_callBackPtr(); */
	}
}
ISR(TIMER1_COMPA_vect)
{
	if(g_t_1_ctc_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application
		 * after the timer reach the compare value
		 */
		(*g_t_1_ctc_callBackPtr)();
		/* another method to call the function using pointer to function t_1_ctc_callBackPtr(); */
	}
}
ISR(TIMER2_OVF_vect)
{
	if(g_t_2_ovf_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after each overflow */
		(*g_t_2_ovf_callBackPtr)();
		/* another method to call the function using pointer to function t_2_ovf_callBackPtr(); */
	}
}
ISR(TIMER2_COMP_vect)
{
	if(g_t_2_ctc_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application
		 * after the timer reach the compare value
		 */
		(*g_t_2_ctc_callBackPtr)();
		/* another method to call the function using pointer to function t_2_ctc_callBackPtr(); */
	}
}

/*******************************************************************************
 *                            FUNCTIONS DEFINITIONS                            *
 *******************************************************************************/

/*
 * Function that initialize the timer
 * Given the required timer and the mode, it initiate the registers
 * It also initiate the initial value and the compare value according to the required
 */
void Timer_init(const Timer_ConfigType * Config_Ptr)
{
	uint8 timer_num,mode,scalar;
	SREG &= ~(1<<7); /*Disable I-bit at the beginning*/
	/*The prescalar in Timer2 not lie Timer1 and Timer0.
	 * So, which is included in the enums is that of Timer2 and
	 * here we will map them to these of Timer0 and Timer1*/
	scalar = Config_Ptr->scalar; /*We will use this value (scalar) in case of Timer0 & Timer1*/
	switch(scalar)
	{
		case 4: /*If it is 4 (clk/64), it should be 3 in Timers 0 & 1*/
			scalar =3;
			break;
		case 6: /*If it is 6 (clk/256), it should be 4 in Timers 0 & 1*/
			scalar =4;
			break;
		case 7: /*If it is 4 (clk/1024), it should be 5 in Timers 0 & 1*/
			scalar =5;
			break;
	}
	mode = Config_Ptr->mode;
	timer_num = Config_Ptr->num;
	if(timer_num == 0) /*Timer0 is required*/
	{
		/*FOC0=1: Non_PWM mode
		 *WGM01:0=0: Normal mode (we will check later on the required mode but it is initially normal)
		 *COM01:0=0: No need for OC0
		 */
		TCCR0 = (1<<FOC0);
		if(mode == 1) /*If it is compare mode*/
		{
			TCCR0 |= (1<<WGM01); /*WGM01=1: compare mode*/
			OCR0 = Config_Ptr->s_comp; /*Put the required compare value in OCR0*/
		}
		TCCR0 = (TCCR0 & 0xF8) | (scalar); /*Put the prescalar in the first 3-bits*/
		TCNT0 = Config_Ptr->s_initial; /*Put the required initial value in TCNT0*/
		if(mode == 1) /*If it is compare mode*/
		{
			TIMSK = (1<<OCIE0); /*OCIE0=1: Enable output compare match interrupt*/
		}
		else /*It is overflow mode*/
		{
			TIMSK = (1<<TOIE0); /*TOIE0=1: Enable overflow interrupt*/
		}
	}
	else if(timer_num == 1) /*Timer1 is required*/
	{
		/*FOC1A,FOC1B=1: Non_PWM mode
		 *WGM10,WGM11=0: Whether it is normal mode or compare mode
		 *COM1A1:0,COM1B1:0=0: No need for OC1
		 */
		TCCR1A = (1<<FOC1A) | (1<<FOC1B);
		if(mode == 1) /*If it is compare mode*/
		{
			TCCR1B = (1<<WGM12); /*WGM12=1: Compare mode (Mode 12)*/
			OCR1A = Config_Ptr->s_comp; /*Put the required compare value in OCR1A*/
		}
		TCCR1B = (TCCR1B & 0xF8) | (scalar); /*Put the prescalar in the first 3-bits*/
		TCNT1 = Config_Ptr->s_initial; /*Put the required initial value in TCNT1*/
		if(mode == 1) /*If it is compare mode*/
		{
			TIMSK = (1<<OCIE1A); /*OCIE1A=1: Enable output compare A match interrupt*/
		}
		else /*It is overflow mode*/
		{
			TIMSK = (1<<TOIE1); /*TOIE1=1: Enable overflow interrupt*/
		}
	}
	else /*Timer2 is required*/
	{
		/*FOC2=1: Non_PWM mode
		 *WGM21:0=0: Normal mode (we will check later on the required mode but it is initially normal)
		 *COM1A1:0,COM1B1:0=0: No need for OC2
		 */
		TCCR2= (1<<FOC2);
		if(mode == 1) /*If it is compare mode*/
		{
			TCCR2 |= (1<<WGM21); /*WGM21=1: compare mode*/
			OCR2 = Config_Ptr->s_comp; /*Put the required compare value in OCR2*/
		}
		TCCR2 = (TCCR2 & 0xF8) | (Config_Ptr->scalar); /*Put the prescalar in the first 3-bits*/
		TCNT2 = Config_Ptr->s_initial; /*Put the required initial value in TCNT2*/
		if(mode == 1) /*If it is compare mode*/
		{
			TIMSK = (1<<OCIE2); /*OCIE2=1: Enable output compare match interrupt*/
		}
		else /*It is overflow mode*/
		{
			TIMSK = (1<<TOIE2); /*TOIE2=1: Enable overflow interrupt*/
		}
	}
	SREG |= (1<<7); /*Enable I-bit*/
}

/*Function to set the Call Back function address for Timer0, overflow mode*/
void Timer0_Ovf_setCallBack(void(*a_t0_ovf_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_t_0_ovf_callBackPtr = a_t0_ovf_ptr;
}

/*Function to set the Call Back function address for Timer0, compare mode*/
void Timer0_CTC_setCallBack(void(*a_t0_ctc_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_t_0_ctc_callBackPtr = a_t0_ctc_ptr;
}

/*Function to set the Call Back function address for Timer1, overflow mode*/
void Timer1_Ovf_setCallBack(void(*a_t1_ovf_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_t_1_ovf_callBackPtr = a_t1_ovf_ptr;
}

/*Function to set the Call Back function address for Timer1, compare mode*/
void Timer1_CTC_setCallBack(void(*a_t1_ctc_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_t_1_ctc_callBackPtr = a_t1_ctc_ptr;
}

/*Function to set the Call Back function address for Timer2, overflow mode*/
void Timer2_Ovf_setCallBack(void(*a_t2_ovf_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_t_2_ovf_callBackPtr = a_t2_ovf_ptr;
}

/*Function to set the Call Back function address for Timer2, compare mode*/
void Timer2_CTC_setCallBack(void(*a_t2_ctc_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_t_2_ctc_callBackPtr = a_t2_ctc_ptr;
}
