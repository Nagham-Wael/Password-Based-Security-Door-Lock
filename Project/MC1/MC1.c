/********************************************************************************
 * File Name: MC1.c
 *
 * Author: Nagham
 *
 * Created on: Nov 23, 2020
 *
 * Description: Main file of MC1 of Final Project
 *******************************************************************************/

/*******************************************************************************
 *                                   INCLUDES                                  *
 *******************************************************************************/

#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include "timers.h"

/*******************************************************************************
 *                                  Definitions                                *
 *******************************************************************************/

#define M1_READY 0x10 /*MC1 send to MC2 when it is ready*/
#define M2_READY 0x20 /*MC2 send to MC1 when it is ready*/
#define Error_pass 0x00 /*MC2 send to MC1 while checking,indicates that the entered password is wrong*/
#define INITIALIZED_FLAG 0x01 /*Flag is saved once the one run code is done*/
#define SAVE 0x02 /*MC1 send to MC2 to begin saving the password*/
#define END_SAVING 0x03 /*MC2 send to MC1 to inform that saving password has been finished*/
#define CHECK_PASS 0x04 /*MC1 send to MC2 to begin checking the password*/
#define UNLOCK_DOOR 0x06 /*MC1 send to MC2 to begin opening the gate*/

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

uint16 g_tick = 0; /*Global variable indicates number of overflows*/
uint8 g_calc_sec_flag=0; /*Global variable indicates number of seconds*/
uint8 counter_error =0; /*Global variable indicates number of times the entered password was wrong*/
uint8 return_flag=0; /*Global variable used to know if the password has been entered for 3 times wrong or not*/

/*******************************************************************************
 *                            FUNCTIONS DEFINITIONS                            *
 *******************************************************************************/

/*Function responsible for calculating seconds*/
void Calculate_time(void)
{
	g_tick ++; /*Every overflow increase ticks*/

	/*Once ticks become 30(according to the used prescalar),increase seconds flag and tick =0*/
	if(g_tick == 30)
	{
		g_calc_sec_flag ++; /*Every 30 tick, increase seconds flag*/
		g_tick =0; /*Put tick=0 to start counting new second*/
	}
}

/*
 *Function responsible for taking the new password and the reentered password and compare them
 *If they are equal, it passes the password to MC2 to save it
 *If they aren't equal, it is repeated until they are equal and send it to be saved
 */
void First_run()
{
	/*
	 * key: variable used to save the pressed key from KEYPAD
	 * pass: variable used to save the password digit and pass it to MC2 to check it
	 * count_pass: variable used as a counter to count the entered password digits
	 */
	uint8 key = 0,pass=0,count_pass;
	/*
	 * new_pass: variable used to save the entered new password
	 * re_pass: variable used to save the reentered password
	 */
	sint32 new_pass = 0, re_pass = 0;

	LCD_clearScreen(); /*Clear LCD*/
	LCD_displayStringRowColumn(0,0,"Enter new pass:"); /*Display this string at this row & col*/
	/*Here we take the 5 digits and save them*/
	for(count_pass=0; count_pass<5; count_pass++) /*This loop is executed for 5 times(Pass of 5 digits)*/
	{
		key = KeyPad_getPressedKey(); /*Get the pressed key from KEYPAD*/
		/*As long as the pressed key is not a number from 0 to 9, take the new pressed key*/
		while(!((key >= 0) && (key <= 9)))
		{
			key = KeyPad_getPressedKey();
		}
		LCD_displayStringRowColumn(1,count_pass,"*"); /*Display the pressed key as (*)*/
		new_pass= new_pass*10 + key; /*Save the 5 digits in this variable (new_pass)*/
		_delay_ms(500); /*Don't respond for a new key for 500 ms*/
	}

	LCD_clearScreen(); /*Clear LCD*/
	LCD_displayStringRowColumn(0,0,"Renter pass:"); /*Display this string at this row & col*/
	/*Here we take the 5 digits and save them*/
	for(count_pass=0; count_pass<5; count_pass++) /*This loop is executed for 5 times(Pass of 5 digits)*/
	{
		key = KeyPad_getPressedKey(); /*Get the pressed key from KEYPAD*/
		/*As long as the pressed key is not a number from 0 to 9, take the new pressed key*/
		while(!((key >= 0) && (key <= 9)))
		{
			key = KeyPad_getPressedKey();
		}
		LCD_displayStringRowColumn(1,count_pass,"*");/*Display the pressed key as (*)*/
		re_pass= re_pass*10 + key; /*Save the 5 digits in this variable (re_pass)*/
		_delay_ms(500); /*Don't respond for a new key for 500 ms*/
	}

	/*Then compare the new_pass with re_pass
	 * If they are equal, pass the password to MC2
	 * If they aren't equal, repeat this function
	 */
	if(re_pass == new_pass) /*They are equal*/
	{
		UART_sendByte(SAVE); /*MC1 sends to MC2 to begin saving password*/
		for(count_pass=0;count_pass<5;count_pass++) /*This loop is executed for 5 times(Pass of 5 digits)*/
		{
			pass = re_pass % 10; /*Get the last digit from right*/
			while(UART_receiveByte()!= M2_READY); /*Wait till MC2 become ready*/
			UART_sendByte(pass); /*MC1 sends to MC2 the digit to save it*/
			re_pass /= 10; /* Divide pass by 10 to get the next digit to right*/
		}
		while(UART_receiveByte()!= END_SAVING); /*Wait until MC2 send that it finished saving*/
	}
	else /*They aren't equal*/
	{
		LCD_clearScreen(); /*Clear LCD*/
		LCD_displayStringRowColumn(0,0,"Incorrect pass"); /*Display this string at this row & col*/
		_delay_ms(500);
		First_run(); /*Repeat the function till they are the same and MC2 save them*/
	}
}

/*
 * Function responsible for checking the entered password,
 * If it isn't correct for less than 3 times, it displays on LCD and gives another chance
 * Once 3 times are executed, it displays on LCD for 1 minute
 * It takes the configuration of the timer to handle the LCD
 */
void Check_pass(const Timer_ConfigType * Config_Ptr)
{
	sint32 pass=0; /*Variable used to save the entered 5 digits password*/

	/*
	 * count_pass: variable used as a counter to count the entered password digits
	 * key: variable used to save the pressed key from KEYPAD
	 * pass_digit: variable used to save the password digit and pass it to MC2 to check it
	 * error_pass: variable used to know if there is an error in any digit from the entered password or not
	 */
	uint8 count_pass,key=0,pass_digit=0,error_pass=0;

	LCD_clearScreen(); /*Clear LCD*/
	LCD_displayStringRowColumn(0,0,"Enter pass:"); /*Display this string at this row & col*/
	/*Here we take the 5 digits and save them*/
	for(count_pass=0; count_pass<5; count_pass++) /*This loop is executed for 5 times(Pass of 5 digits)*/
	{
		key = KeyPad_getPressedKey(); /*Get the pressed key from KEYPAD*/
		/*As long as the pressed key is not a number from 0 to 9, take the new pressed key*/
		while(!((key >= 0) && (key <= 9)))
		{
			key = KeyPad_getPressedKey();
		}
		LCD_displayStringRowColumn(1,count_pass,"*"); /*Display the pressed key as (*)*/
		pass= pass*10 + key; /*Save the 5 digits in this variable (pass)*/
		_delay_ms(500);
	}

	UART_sendByte(CHECK_PASS); /*MC1 send to MC2 to begin checking the password*/
	/*Here MC1 passes the digits to MC2 to check*/
	for(count_pass=0; count_pass<5; count_pass++) /*This loop is executed for 5 times(Pass of 5 digits)*/
	{
		pass_digit = pass % 10; /*Get the last digit from right*/
		UART_sendByte(pass_digit); /*MC1 sends to MC2 the digit to save it*/
		/*If MC2 sent to MC1 Error_pass, this means that the sent digit was wrong*/
		if(UART_receiveByte() == Error_pass)
		{
			error_pass ++; /*Increase error_pass every error we receive from MC2*/
		}
		pass /= 10; /* Divide pass by 10 to get the next digit to right*/
	}
	/*Check the error_pass, if it is zero, this means that the password is right
	 * If it is more than 1, we will increase counter_error and repeat the function
	 * If counter_error became 3, the LCD will display error message for 1 minute*/
	if(error_pass != 0) /*The password is wrong*/
	{
		counter_error ++; /*Increase the error counter*/
		UART_sendByte(counter_error); /*MC1 send the value of the error counter to MC2*/
		/*If the error counter is less than 3, it will display a message and repeat the check function*/
		if(counter_error < 3)
		{
			LCD_clearScreen(); /*Clear LCD*/
			LCD_displayStringRowColumn(0,0,"Wrong pass"); /*Display this string at this row & col*/
			_delay_ms(500);
			Check_pass(Config_Ptr); /*Repeat the check function*/
		}
		else /*the error counter became 3(user entered password wrong for 3 times)*/
		{
			LCD_clearScreen(); /*Clear LCD*/

			/*Initialize the timer and pass the required configurations to it*/
			Timer_init(Config_Ptr);
			g_tick =0; /*Initially make the tick equals zero, to start from the beginning*/
			g_calc_sec_flag =0; /*Initially make the sec_flag equals zero,to start from the beginning*/
			while (g_calc_sec_flag != 60) /*As long as it hasn't passed 1 minute,display on LCD this meassage*/
			{
				LCD_displayStringRowColumn(0,0,"Wrong pass 0 try");
			}

			counter_error = 0; /*Put error counter to 0, to be able to count correct the next time*/
			return_flag =1; /*Flag indicates that the user has entered the password wrong for 3 times*/
		}
	}
	else /*The password is right*/
	{
		UART_sendByte(4); /*MC1 should send number of errors, so 4 means that no error in the eneterd password*/
		counter_error = 0; /*Put error counter to 0, to be able to count correct the next time*/
	}
}

/*
 *Function responsible for opening the gate
 *It takes the configuration of the timer to handle the LCD
 */
void Open_gate(const Timer_ConfigType * Config_Ptr)
{
	UART_sendByte(UNLOCK_DOOR); /*MC1 sends to MC2 to unlock the gate*/

	LCD_clearScreen(); /*Clear LCD*/
	Timer_init(Config_Ptr); /*Initialize the timer and pass the required configurations to it*/
	g_tick =0; /*Initially make the tick equals zero, to start from the beginning*/
	g_calc_sec_flag =0; /*Initially make the sec_flag equals zero, to start from the beginning*/
	/*As long as it hasn't passed 15 seconds, display on LCD this message*/
	while(g_calc_sec_flag != 15)
	{
		LCD_displayStringRowColumn(0,0,"Unlocking door");
	}

	LCD_clearScreen();/*Clear LCD*/
	g_tick =0; /*Initially make the tick equals zero, to start from the beginning*/
	g_calc_sec_flag =0; /*Initially make the sec_flag equals zero, to start from the beginning*/
	/*As long as it hasn't passed 3 seconds, display on LCD this message*/
	while(g_calc_sec_flag != 3)
	{
		LCD_displayStringRowColumn(0,0,"Door is opened");
	}

	LCD_clearScreen(); /*Clear LCD*/
	g_tick =0; /*Initially make the tick equals zero, to start from the beginning*/
	g_calc_sec_flag =0; /*Initially make the sec_flag equals zero, to start from the beginning*/
	/*As long as it hasn't passed 15 seconds, display on LCD this message*/
	while(g_calc_sec_flag != 15)
	{
		LCD_displayStringRowColumn(0,0,"locking door");
	}
}

/*
 *Function responsible for checking the password and handle the action that the user need
 *whether changing the password or opening the gate.
 *It takes the configuration of the timer to handle the motor and the led
 */
void Main_options(const Timer_ConfigType * Config_Ptr)
{
	uint8 key=0; /*Variable used to save the pressed key from KEYPAD*/

	LCD_clearScreen(); /*Clear LCD*/
	LCD_displayStringRowColumn(0,0,"*:Change pass");
	LCD_displayStringRowColumn(1,0,"%:Open door");
	/*As long as the pressed key is not '*' or '%', take the new pressed key*/
	while((key != '*') && (key != '%'))
	{
		key = KeyPad_getPressedKey();
	}
	/*
	 *Call this function where it takes the entered password and passes it MC2
	 *to check it if it is right or not and it handle all the cases
	 */
	Check_pass(Config_Ptr);
	UART_sendByte(return_flag); /*MC1 sends this value to MC2*/

	/*This means that the user has entered the password correct before the 3 time chances finish*/
	if(return_flag == 0)
	{
		UART_sendByte(key); /*MC1 sends the pressed key to MC2*/
		if (key == '*') /*It means that the user wants to change the password*/
		{
			First_run(); /*Go to this function to get the new password and send it to MC2 to save it*/
		}
		else /*It means that the user wants to open the gate*/
		{
			Open_gate(Config_Ptr); /*It takes the configuration of the timer to handle the motor and the LCD*/
		}
	}
	else /*This means that the user has entered the password wrong for 3 times*/
	{
		return_flag=0; /*Return it to 0, to work correct the next time*/
	}

}

int main(void)
{
	/*
	 *Variable used to know whether the one run code is done before or not,
	 *MC2 read the address where the Initialized flag is declared (It is declared when the
	 *one run code is done) and put it in this flag and send it to MC1. If this is equal to the
	 *Initialized flag, then the one run code was done and we will go to the main options
	 */
	uint8 flag_init=0;

	/*
	 * We pass the input to the structure of timer
	 * 1. Timer_num: Timer0
	 * 2. Timer_mode: Overflow mode
	 * 3. Prescalar: clk/1024
	 * 4. Initial value: 0
	 * 5. Compare value: 0 as it is overflow mode
	 */
	Timer_ConfigType Timer_Config = {TIMER0,OVERFLOW,F_CPU_1024,0,0};

	/*
	 * Set the Call back function pointer in the timer driver
	 * Calculate_time: Function that calculate seconds
	 */
	Timer0_Ovf_setCallBack(Calculate_time);

	/*
	 * We pass the input to the structure of UART
	 * 1. Parity: Disabled (no parity)
	 * 2. Stop_Bit: BIT_1 (1 stop bit)
	 * 3. CharSize: BIT_8 (8-bits data)
	 * 4. Baudrate: 9600
	 */
	UART_ConfigType UART_Config = {DISABLED,BIT_1,BIT_8,9600};

	LCD_init(); /*Initialize LCD*/
	UART_init(& UART_Config); /*Pass the required UART configuration to UART initialization*/

	while(1)
	{
		UART_sendByte(M1_READY); /*MC1 sends to MC2 that it is ready to begin*/
		/*
		 *MC2 should read the address where the initialized flag should be saved
		 *and send it to MC1
		 */
		flag_init = UART_receiveByte();

		/*
		 *If the flag_init not equal to INITIALIZED_FLAG,
		 *it means that the one run code is not done yet
		 */
		if(flag_init != INITIALIZED_FLAG)
		{
			/*
			 *Go to this function where it takes the new password and send it to MC2 to save it and save the flag
			 *So, when it check again on the address, it finds that they have the same value
			 *So, it then go to the main options
			 */
			First_run();
		}
		/*
		 *If the flag_init equals to INITIALIZED_FLAG,
		 *it means that the one run code is done
		 */
		else
		{
			/*Go to the main options and take the configuration of the timer to handle the LCD*/
			Main_options(& Timer_Config);
		}
	}
	return 0;
}
