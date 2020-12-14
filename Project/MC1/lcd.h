/********************************************************************************
 * Module: LCD
 *
 * File Name: lcd.h
 *
 * Author: Nagham
 *
 * Created on: Nov 2, 2020
 *
 * Description: Header file of lcd driver
 *******************************************************************************/
#ifndef LCD_H_
#define LCD_H_

/*******************************************************************************
 *                                   INCLUDES                                  *
 *******************************************************************************/

#include "std_types.h"
#include "micro_config.h"
#include "common_macros.h"

/*******************************************************************************
 *                         Definitions & Configurations                        *
 *******************************************************************************/

/* LCD Data bits mode configuration */
#define DATA_BITS_MODE 8

/* Use higher 4 bits in the data port */
#if (DATA_BITS_MODE == 4)
#define UPPER_PORT_PINS
#endif

#undef UPPER_PORT_PINS /*We use the lower 4 pins*/

#define RS PD4
#define RW PD5
#define E  PD6

#define LCD_CTRL_PORT PORTD
#define LCD_CTRL_PORT_DIR DDRD

#define LCD_DATA_PORT PORTC
#define LCD_DATA_PORT_DIR DDRC

/* LCD Commands */
#define CLEAR_COMMAND 0x01
#define FOUR_BITS_DATA_MODE 0x02
#define TWO_LINE_LCD_Four_BIT_MODE 0x28
#define TWO_LINE_LCD_Eight_BIT_MODE 0x38
#define CURSOR_OFF 0x0C
#define CURSOR_ON 0x0E
#define SET_CURSOR_LOCATION 0x80
#define SET_CURSOR_LOCATION 0x80

/*******************************************************************************
 *                            FUNCTIONS DECLARATIONS                           *
 *******************************************************************************/

/*Function responsible for initiating the LCD*/
void LCD_init(void);

/*Function responsible for sending commands to LCD*/
void LCD_sendCommand(uint8 command);

/*Function responsible for displaying the required char data*/
void LCD_displayCharacter(uint8 data);

/*Function responsible for displaying the required string*/
void LCD_displayString(const char *Str);

/*Function responsible for moving the cursor of the LCD to the required row and column(address)*/
void LCD_goToRowColumn(uint8 row,uint8 col);

/*Function responsible for displaying the required string at the required address on LCD*/
void LCD_displayStringRowColumn(uint8 row,uint8 col,const char *Str);

/*Function responsible for clearing the LCD*/
void LCD_clearScreen(void);

/*Function responsible for converting the given integer data into string and displaying it*/
void LCD_intgerToString(int data);

#endif /* LCD_H_ */
