/********************************************************************************
 * Module: UART
 *
 * File Name: uart.c
 *
 * Author: Nagham
 *
 * Created on: Nov 19, 2020
 *
 * Description: Source file of uart driver
 *******************************************************************************/

/*******************************************************************************
 *                         		    INCLUDES                                   *
 *******************************************************************************/

#include "uart.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/*Flag used when 9-bits data are used, to handle the transmitting and receiving in this case*/
uint8 flag_9bits =0;

/*******************************************************************************
 *                            FUNCTIONS DEFINITIONS                            *
 *******************************************************************************/

/*
 * Function that initialize the uart
 * 1. Set the required parity
 * 2. Set the required stop bits
 * 3. Set the required number of bits
 * 4. Set the required baudrate
 */
void UART_init(const UART_ConfigType * Config_Ptr)
{
	uint32 baudrate; /*Used to put in it the required baudrate*/
	uint8 baud_prescale; /*Used to put in it the baud_prescale according to the equation*/
	UCSRA = (1<<U2X); /*Double transmission speed*/

	/* USART CONTROL AND STATUS REGISTER B
	 * RXCIE = 0: Disable RX complete interrupt enable
	 * TXCIE = 0: Disable TX complete interrupt enable
	 * UDRIE = 0: Disable USART data register empty interrupt enable
	 * RXEN = 1: Enable receiver enable
	 * TXEN = 1: Enable transmitter enable
	 */
	UCSRB = (1<<RXEN) | (1<<TXEN);

	if(Config_Ptr->size == 7) /*If it is 9-bits data*/
	{
		UCSRB |= (1<<UCSZ2); /*UCSZ2=1: 9-bits data*/
		flag_9bits =1; /*Put flag 1 to help us in receiving and transmitting*/
	}

	/* USART CONTROL AND STATUS REGISTER C
	 * URSEL = 1: Write in UCSRC
	 * UMSEL = 0: Asynchronous operation
	 * USBS = 0: 1-bit stop
	 * UCPOL = 0: Used with synchronous only
	 * UCSZ1:0 = 1: 8-bit mode
	 */
	UCSRC = (1<<URSEL);
	switch(Config_Ptr->parity)
	{
		case 2:
			UCSRC |= (1<<UPM1); /*UPM1=1: Even parity*/
			break;
		case 3:
			UCSRC |= (1<<UPM0) | (1<<UPM1); /*UPM0,UPM1=1: Odd parity*/
			break;
	}
	switch(Config_Ptr->stop)
	{
		case 1:
			UCSRC |= (1<<USBS); /*USBS=1: 2-bits stop*/
	}
	/*Put the required size in the 2nd,3rd and 4th bits in UCSRC*/
	UCSRC = (UCSRC & 0xF9) | ((Config_Ptr->size)<<1);

	/*First 8 bits are written in UBRRL and the last 4 bits are written in UBRRH*/
	baudrate = Config_Ptr->s_baudrate;
	baud_prescale= ((F_CPU / (baudrate * 8UL)) - 1);
	UBRRL = baud_prescale;
	UBRRH = baud_prescale>>8;
}

/*Function responsible for sending the given data*/
void UART_sendByte(const uint16 data)
{
	uint8 bit_9=0; /*Variable used to get the 9th bit of data*/
	/*UDRE=1, if UDR is empty and ready to be written in it*/
	while(BIT_IS_CLEAR(UCSRA,UDRE));
	/*Write data in the register, and the flag will be equal 0*/
	if(flag_9bits == 1)
	{
		bit_9= data & (1<<8); /*get the last bit(9th)from the data*/
		UCSRB = (UCSRB & 0xFE) | (bit_9); /*Put the 9th bit in TXB8*/
	}
	UDR = data;

	/************************* Another Method *************************
	if(flag_9bits == 1)
	{
		bit_9= data & (1<<8);
		UCSRB = (UCSRB & 0xFE) | (bit_9);
	}
	UDR = data;
	while(BIT_IS_CLEAR(UCSRA,TXC)){} // Wait until the transmission is complete TXC = 1
	SET_BIT(UCSRA,TXC); // Clear the TXC flag
	*******************************************************************/
}

/*Function responsible for receiving the data*/
uint16 UART_receiveByte(void)
{
	uint8 bit_9; /*Variable where we get the 9th bit*/
	/*RXC=1, when the receive buffer is completed*/
	while(BIT_IS_CLEAR(UCSRA,RXC));
	bit_9 = (UCSRB & 1); /*Read the 9th bit*/
	return (UDR | (bit_9<<8)); /*Send the 9-bit data (8-bit UDR+ 9th bit)*/
}

/*Function responsible for sending a string*/
void UART_sendString(const uint8 *Str)
{
	while(*Str != '\0')
	{
		UART_sendByte(*Str);
		Str++;
	}
}

/*Function responsible for receiving a string*/
void UART_receiveString(uint8 *Str)
{
	uint8 i = 0;
	Str[i] = UART_receiveByte();
	while(Str[i] != '#')
	{
		i++;
		Str[i] = UART_receiveByte();
	}
	Str[i] = '\0';
}
