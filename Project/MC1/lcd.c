/********************************************************************************
 * Module: LCD
 *
 * File Name: lcd.c
 *
 * Author: Nagham
 *
 * Created on: Nov 2, 2020
 *
 * Description: source file of lcd driver
 *******************************************************************************/

/*******************************************************************************
 *                                   INCLUDES                                  *
 *******************************************************************************/

#include "lcd.h"

/*******************************************************************************
 *                            FUNCTIONS DEFINITIONS                            *
 *******************************************************************************/

/*Function responsible for initiating the LCD*/
void LCD_init(void)
{
	LCD_CTRL_PORT_DIR |= (1<< RS) | (1<<RW) | (1<<E); /*Configure E,RW,RS as output pins*/
	#if(DATA_BITS_MODE == 4)
		#ifdef UPPER_PORT_PINS
			LCD_DATA_PORT_DIR |= 0xF0; /* Configure the last 4 bits of the data port as output pins */
		#else
			LCD_DATA_PORT_DIR |= 0x0F; /* Configure the first 4 bits of the data port as output pins */
		#endif
		LCD_sendCommand(FOUR_BITS_DATA_MODE); /* initialize LCD in 4-bit mode */
		LCD_sendCommand(TWO_LINE_LCD_Four_BIT_MODE); /* use 2-line lcd + 4-bit Data Mode*/
	#else
		LCD_DATA_PORT_DIR = 0xFF; /*Define data port as output port*/
		LCD_sendCommand(TWO_LINE_LCD_Eight_BIT_MODE);/*Use 2-line lcd + 8-bit Data Mode*/
	#endif
	LCD_sendCommand(CURSOR_OFF); /*Cursor off*/
	LCD_sendCommand(CLEAR_COMMAND); /*Clear LCD at the begining*/
}

/*Function responsible for sending commands to LCD*/
void LCD_sendCommand(uint8 command)
{
	CLEAR_BIT(LCD_CTRL_PORT,RS); /*Reset=0 as it is command*/
	CLEAR_BIT(LCD_CTRL_PORT,RW); /*R/W=0 as we write on LCD*/
	_delay_ms(1); /*Delay for Tas = 50 ns*/
	SET_BIT(LCD_CTRL_PORT,E); /*E=1 to enable the LCD display*/
	_delay_ms(1); /*Delay for Tpw - Tdsw = 190 ns*/
	#if (DATA_BITS_MODE == 4)
		/* out the highest 4 bits of the required command to the data bus D4 --> D7 */
		#ifdef UPPER_PORT_PINS
			LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (command & 0xF0);
		#else
			LCD_DATA_PORT = (LCD_DATA_PORT & 0xF0) | ((command & 0xF0) >> 4);
		#endif
		_delay_ms(1); /*Delay for Tdsw = 100 ns*/
		CLEAR_BIT(LCD_CTRL_PORT,E); /*E=0 to disable LCD*/
		_delay_ms(1); /*Delay for Th= 13ns*/
		SET_BIT(LCD_CTRL_PORT,E); /*E=1 to enable the LCD display*/
		_delay_ms(1); /*Delay for Tpw - Tdsw = 190 ns*/
		/* out the least 4 bits of the required command to the data bus D4 --> D7 */
		#ifdef UPPER_PORT_PINS
			LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | ((command & 0x0F)<<4);
		#else
			LCD_DATA_PORT = (LCD_DATA_PORT & 0xF0) | (command & 0x0F);
		#endif
		_delay_ms(1); /*Delay for Tdsw = 100 ns*/
		CLEAR_BIT(LCD_CTRL_PORT,E); /*E=0 to disable LCD*/
		_delay_ms(1); /*Delay for Th= 13ns*/
	#else
		LCD_DATA_PORT = command;
		_delay_ms(1); /*Delay for Tdsw = 100 ns*/
		CLEAR_BIT(LCD_CTRL_PORT,E); /*E=0 to disable LCD*/
		_delay_ms(1); /*Delay for Th= 13ns*/
	#endif
}

/*Function responsible for displaying the required char data*/
void LCD_displayCharacter(uint8 data)
{
	SET_BIT(LCD_CTRL_PORT,RS); /*Reset=1 as it is data*/
	CLEAR_BIT(LCD_CTRL_PORT,RW); /*R/W=0 as we write on LCD*/
	_delay_ms(1); /*Delay for Tas = 50 ns*/
	SET_BIT(LCD_CTRL_PORT,E); /*E=1 to enable the LCD display*/
	_delay_ms(1); /*Delay for Tpw - Tdsw = 190 ns*/
	#if (DATA_BITS_MODE == 4)
		/* out the highest 4 bits of the required command to the data bus D4 --> D7 */
		#ifdef UPPER_PORT_PINS
			LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (data & 0xF0);
		#else
			LCD_DATA_PORT = (LCD_DATA_PORT & 0xF0) | ((data & 0xF0) >> 4);
		#endif
		_delay_ms(1); /*Delay for Tdsw = 100 ns*/
		CLEAR_BIT(LCD_CTRL_PORT,E); /*E=0 to disable LCD*/
		_delay_ms(1); /*Delay for Th= 13ns*/
		SET_BIT(LCD_CTRL_PORT,E); /*E=1 to enable the LCD display*/
		_delay_ms(1); /*Delay for Tpw - Tdsw = 190 ns*/
		/* out the least 4 bits of the required data to the data bus D4 --> D7 */
		#ifdef UPPER_PORT_PINS
			LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | ((data & 0x0F)<<4);
		#else
			LCD_DATA_PORT = (LCD_DATA_PORT & 0xF0) | (data & 0x0F);
		#endif
		_delay_ms(1); /*Delay for Tdsw = 100 ns*/
		CLEAR_BIT(LCD_CTRL_PORT,E); /*E=0 to disable LCD*/
		_delay_ms(1); /*Delay for Th= 13ns*/
	#else
		LCD_DATA_PORT = data;
		_delay_ms(1); /*Delay for Tdsw = 100 ns*/
		CLEAR_BIT(LCD_CTRL_PORT,E); /*E=0 to disable LCD*/
		_delay_ms(1); /*Delay for Th= 13ns*/
	#endif
}

/*Function responsible for displaying the required string*/
void LCD_displayString(const char *Str)
{
	while((*Str) != '\0')
	{
		LCD_displayCharacter(*Str);
		Str++;
	}
}

/*Function responsible for moving the cursor of the LCD
 * to the required row and column (address)
 */
void LCD_goToRowColumn(uint8 row,uint8 col)
{
	uint8 address;
	switch(row)
	{
		case 0:
				address=col;
				break;
		case 1:
				address=col+0x40;
				break;
		case 2:
				address=col+0x10;
				break;
		case 3:
				address=col+0x50;
				break;
	}
	/* to write to a specific address in the LCD
	 * we need to apply the corresponding command 0b10000000+Address */
	LCD_sendCommand(SET_CURSOR_LOCATION | address);
}

/*Function responsible for displaying the required string at the required address on LCD*/
void LCD_displayStringRowColumn(uint8 row,uint8 col,const char *Str)
{
	LCD_goToRowColumn(row,col);
	LCD_displayString(Str);
}

/*Function responsible for clearing the LCD*/
void LCD_clearScreen(void)
{
	LCD_sendCommand(CLEAR_COMMAND);
}

/*Function responsible for converting the given integer data into string and displaying it*/
void LCD_intgerToString(int data)
{
	char buff[16]; /* String to hold the ascii result */
	itoa(data,buff,10); /* 10 for decimal */
    LCD_displayString(buff);
}


