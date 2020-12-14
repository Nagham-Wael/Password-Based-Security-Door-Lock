/********************************************************************************
 * Module: UART
 *
 * File Name: uart.h
 *
 * Author: Nagham
 *
 * Created on: Nov 19, 2020
 *
 * Description: Header file of uart driver
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

/*******************************************************************************
 *                         		    INCLUDES                                   *
 *******************************************************************************/

#include "std_types.h"
#include "micro_config.h"
#include "common_macros.h"

/*******************************************************************************
 *                         	    TYPES DECLARATION                              *
 *******************************************************************************/

typedef enum
{
	DISABLED,RESERVED,EVEN,ODD
}UART_Parity;

typedef enum
{
	BIT_1,BIT_2
}UART_StopBit;

typedef enum
{
	BIT_5,BIT_6,BIT_7,BIT_8,RESERVED_1,RESERVED_2,RESERVED_3,BIT_9
}UART_CharSize;

typedef struct
{
	UART_Parity parity;
	UART_StopBit stop;
	UART_CharSize size;
	uint32 s_baudrate; /*Take the required baudrate from the user*/
}UART_ConfigType;

/*******************************************************************************
 *                            FUNCTIONS DECLARATIONS                           *
 *******************************************************************************/

/*
 * Function that initialize the uart
 * 1. Set the required parity
 * 2. Set the required stop bits
 * 3. Set the required number of bits
 * 4. Set the required baudrate
 */
void UART_init(const UART_ConfigType * Config_Ptr);

/*Function responsible for sending the given data*/
void UART_sendByte(const uint16 data);

/*Function responsible for receiving the data*/
uint16 UART_receiveByte(void);

/*Function responsible for sending a string*/
void UART_sendString(const uint8 *Str);

/*Function responsible for receiving a string*/
void UART_receiveString(uint8 *Str); // Receive until #


#endif /* UART_H_ */
