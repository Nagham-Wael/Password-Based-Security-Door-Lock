/********************************************************************************
 * Module: I2C
 *
 * File Name: i2c.c
 *
 * Author: Nagham
 *
 * Created on: Nov 22, 2020
 *
 * Description: Source file of i2c driver
 *******************************************************************************/

/*******************************************************************************
 *                                   INCLUDES                                  *
 *******************************************************************************/

#include "i2c.h"

/*******************************************************************************
 *                            FUNCTIONS DEFINITIONS                            *
 *******************************************************************************/

/*
 * Function that initialize the I2C
 * According the required prescaler, devision factor and slave address,
 * the registers are initialized
 */
void TWI_init(const TWI_ConfigType * Config_Ptr)
{
	/*In order to configure the frequency, get TWBR
	 *Put the whole bits of TWSR zeros except the first 2 bits take them from the structure
	 */
	TWBR = Config_Ptr->s_devision_factor;
	TWSR = (TWSR & 0xFC) | (Config_Ptr->prescaler);

	/*Get the address from the structure, but before putting it in the register, shift it 1 bit
	 * as the General Call Recognition is disabled
	 */
	TWAR = (TWAR & 0x00) | ((Config_Ptr->s_slave_address)<<1);
	TWCR = (1<<TWEN); /*Enable TWI*/
}

/*Function responsible for sending the start bit*/
void TWI_start(void)
{
	/*TWINT=1: Clear TWI Interrupt Flag before sending
	 *TWSTA=1: Send start bit
	 *TWEN=1: Enable TWI
	 */
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

	/*Wait till TWI finish the task, and TWINT=1*/
	while(BIT_IS_CLEAR(TWCR,TWINT));
}

/*Function responsible for sending the stop bit*/
void TWI_stop(void)
{
	/*TWINT=1: Clear TWI Interrupt Flag before sending
	 *TWSTO=1: Send stop bit
	 *TWEN=1: Enable TWI
	 */
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

/*Function responsible for sending (writing) data in the slave address sent before*/
void TWI_write(uint8 data)
{
	TWDR = data; /*Put data in TWI Data Register*/
	/*TWINT=1: Clear TWI Interrupt Flag before sending data
	 *TWEN=1: Enable TWI
	 */
	TWCR = (1 << TWINT) | (1 << TWEN);
	/*Wait till TWI finish the task, and TWINT=1*/
	while(BIT_IS_CLEAR(TWCR,TWINT));
}

/*Function responsible for reading data from the address that was sent before and reply by ack*/
uint8 TWI_readWithACK(void)
{
	/*TWINT=1: Clear TWI Interrupt Flag before sending data
	 *TWEN=1: Enable TWI
	 *TWEA=1: Enable TWI Ack bit
	 */
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	/*Wait till TWI finish the task, and TWINT=1*/
	while(BIT_IS_CLEAR(TWCR,TWINT));
	return TWDR; /*Read Data*/
}

/*Function responsible for reading data from the address
 * that was sent before without replying by ack
 */
uint8 TWI_readWithNACK(void)
{
	/*TWINT=1: Clear TWI Interrupt Flag before sending data
	 *TWEN=1: Enable TWI
	 */
	TWCR = (1 << TWINT) | (1 << TWEN);
	/*Wait till TWI finish the task, and TWINT=1*/
	while(BIT_IS_CLEAR(TWCR,TWINT));
	return TWDR; /*Read Data*/
}

/*Function responsible for getting the status of TWI*/
uint8 TWI_getStatus(void)
{
	uint8 status;
	/*Get the last 5 bits in TWI Status Register*/
	status = TWSR & 0xF8;
	return status;
}
