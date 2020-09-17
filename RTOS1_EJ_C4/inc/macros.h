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

/*=====[Definicion de macros]==============================*/
#define LED_SEM_ERROR 	LED3		// Led de error al crear semaforos
#define LED_TASK_ERROR 	LED3		// Led de error al crear tareas
#define LED_VERDE		LEDG
#define LED_ROJO		LEDR

#define RATE 40
#define LED_RATE pdMS_TO_TICKS(RATE)
#define BUTTON_RATE 1

#define	PERIOD_LED_MS 1000

#define LEDG_HIGH_TIME_MS	200
#define LEDR_HIGH_TIME_MS	200

#define PERIOD_LED_TICKS pdMS_TO_TICKS(PERIOD_LED_MS)	//Periodo de espera del semaforo 1s
#define LEDG_HIGH_TIME_TICKS pdMS_TO_TICKS(LEDG_HIGH_TIME_MS)
#define LEDR_HIGH_TIME_TICKS pdMS_TO_TICKS(LEDR_HIGH_TIME_MS)

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
} led_tecla_t;
#endif /* MISPROGRAMAS_RTOS1_EJ_B5_INC_MACROS_H_ */
