/********************************************************************************
 * Module: I2C
 *
 * File Name: i2c.h
 *
 * Author: Nagham
 *
 * Created on: Nov 22, 2020
 *
 * Description: Header file of i2c driver
 *******************************************************************************/

#ifndef I2C_H_
#define I2C_H_

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
	F_CPU_CLOCK,F_CPU_CLOCK_4,F_CPU_CLOCK_16,F_CPU_CLOCK_64
}TWI_Prescaler;

typedef struct
{
	TWI_Prescaler prescaler;
	uint8 s_devision_factor; /*Take TWBR from user*/
	uint8 s_slave_address; /*Take the address as it should be known if it became slave*/
}TWI_ConfigType;

/*******************************************************************************
 *                                 Definitions                                 *
 *******************************************************************************/

/* I2C Status Bits in the TWSR Register */
#define TW_START         0x08 // start has been sent
#define TW_REP_START     0x10 // repeated start
#define TW_MT_SLA_W_ACK  0x18 // Master transmit ( slave address + Write request ) to slave + Ack received from slave
#define TW_MT_SLA_R_ACK  0x40 // Master transmit ( slave address + Read request ) to slave + Ack received from slave
#define TW_MT_DATA_ACK   0x28 // Master transmit data and ACK has been received from Slave.
#define TW_MR_DATA_ACK   0x50 // Master received data and send ACK to slave
#define TW_MR_DATA_NACK  0x58 // Master received data but doesn't send ACK to slave

/*******************************************************************************
 *                            FUNCTIONS DECLARATIONS                           *
 *******************************************************************************/

/*
 * Function that initialize the I2C
 * According the required prescaler, devision factor and slave address,
 * the registers are initialized
 */
void TWI_init(const TWI_ConfigType * Config_Ptr);

/*Function responsible for sending the start bit*/
void TWI_start(void);

/*Function responsible for sending the stop bit*/
void TWI_stop(void);

/*Function responsible for sending (writing) data in the slave address sent before*/
void TWI_write(uint8 data);

/*Function responsible for reading data from the address that was sent before and reply by ack*/
uint8 TWI_readWithACK(void);

/*Function responsible for reading data from the address
 * that was sent before without replying by ack
 */
uint8 TWI_readWithNACK(void);

/*Function responsible for getting the status of TWI*/
uint8 TWI_getStatus(void);

#endif /* I2C_H_ */
