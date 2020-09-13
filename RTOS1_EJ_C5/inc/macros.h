/*
 * macros.h
 *
 *  Created on: 12 sep. 2020
 *      Author: francisco
 */

#ifndef MISPROGRAMAS_RTOS1_EJ_B5_INC_MACROS_H_
#define MISPROGRAMAS_RTOS1_EJ_B5_INC_MACROS_H_

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "sapi.h"

#include "semphr.h"

/*=====[Definicion de macros]==============================*/

#define RATE 40
#define LED_RATE pdMS_TO_TICKS(RATE)
#define BUTTON_RATE 1

#define STR_AUX 20

#define PRIORITY 1
#define SIZE	2

#define MAX_SEMPH 		3
#define INICIO_SEMPH 	0

#define LED_TIME pdMS_TO_TICKS(500)

#define DEBOUNCE_TIME 40

DEBUG_PRINT_ENABLE;

/*****************************************
 * 			Data Types
 *****************************************/

typedef enum{
	FSM_BUTTON_UP,
	FSM_BUTTON_DOWN,
	FSM_BUTTON_FALLING,
	FSM_BUTTON_RISING
} fsmButtonState_t;

typedef struct{
	gpioMap_t tecla_fsm;

	// Variables
	TickType_t tiempo_medido;
	fsmButtonState_t fsmButtonState;
	TickType_t tiempo_down;
	TickType_t tiempo_up;

	uint8_t contFalling;
	uint8_t contRising;
} TeclaRTOSfsm_t;


typedef struct{
	TeclaRTOSfsm_t tecla;
	gpioMap_t		led;
	SemaphoreHandle_t semaphore;
} led_tecla_t;
#endif /* MISPROGRAMAS_RTOS1_EJ_B5_INC_MACROS_H_ */
