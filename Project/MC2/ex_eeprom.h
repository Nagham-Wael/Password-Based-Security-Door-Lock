/********************************************************************************
 * Module: EXTERNAL EEPROM
 *
 * File Name: ex_eeprom.h
 *
 * Author: Nagham
 *
 * Created on: Nov 22, 2020
 *
 * Description: Header file of eeprom driver
 *******************************************************************************/

#ifndef EX_EEPROM_H_
#define EX_EEPROM_H_

/*******************************************************************************
 *                                   INCLUDES                                  *
 *******************************************************************************/

#include "std_types.h"

/*******************************************************************************
 *                                 Definitions                                 *
 *******************************************************************************/

#define ERROR 0
#define SUCCESS 1

/*******************************************************************************
 *                            FUNCTIONS DECLARATIONS                           *
 *******************************************************************************/

/*
 * Function that initialize the EEPROM by initializing I2C.
 * According the required prescaler, devision factor and slave address,
 * the registers are initialized
 */
void EEPROM_init(const TWI_ConfigType * Config_Ptr);

/*Function responsible for writing the required data in the given address*/
uint8 EEPROM_writeByte(uint16 u16addr,uint8 u8data);

/*Function responsible for reading the data from the given address*/
uint8 EEPROM_readByte(uint16 u16addr,uint8 *u8data);

#endif /* EX_EEPROM_H_ */
