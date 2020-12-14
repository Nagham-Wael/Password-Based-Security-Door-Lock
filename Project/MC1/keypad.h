/********************************************************************************
 * Module: KEYPAD
 *
 * File Name: keypad.h
 *
 * Author: Nagham
 *
 * Created on: Nov 2, 2020
 *
 * Description: Header file of keypad driver
 *******************************************************************************/

#ifndef KEYPAD_H_
#define KEYPAD_H_

/*******************************************************************************
 *                                   INCLUDES                                  *
 *******************************************************************************/

#include "std_types.h"
#include "micro_config.h"
#include "common_macros.h"

/*******************************************************************************
 *                         Definitions & Configurations                        *
 *******************************************************************************/

#define N_col 4
#define N_row 4

#define KEYPAD_PORT_OUT PORTA
#define KEYPAD_PORT_IN  PINA
#define KEYPAD_PORT_DIR DDRA

/*******************************************************************************
 *                            FUNCTIONS DECLARATIONS                           *
 *******************************************************************************/

/*
 * Function responsible for getting the pressed key
 */
uint8 KeyPad_getPressedKey(void);

#endif /* KEYPAD_H_ */
