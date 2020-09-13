/*
 * fsm_debounce_RTOS.c
 *
 *  Created on: 12 sep. 2020
 *      Author: francisco
 */

#include "fsm_debounce_RTOS.h"

/*****************************************
 * 			Implementaciones
 *****************************************/
void fsmButtonInit( TeclaRTOSfsm_t* button ){
	button->contFalling = 0;
	button->contRising = 0;
	button->fsmButtonState = FSM_BUTTON_UP;  // Set initial state
	return;
}

// FSM Update Sate Function
void fsmButtonUpdate( TeclaRTOSfsm_t* button ){
	switch( button->fsmButtonState ){

	case FSM_BUTTON_UP:

		/* CHECK TRANSITION CONDITIONS */
		if( !gpioRead(button->tecla_fsm) ) {
			button->fsmButtonState = FSM_BUTTON_FALLING;
		}
		break;

	case FSM_BUTTON_DOWN:

		/* CHECK TRANSITION CONDITIONS */
		if( gpioRead(button->tecla_fsm) ) button->fsmButtonState = FSM_BUTTON_RISING;
		break;

	case FSM_BUTTON_FALLING:
		/* ENTRY */

		/* CHECK TRANSITION CONDITIONS */
		if( button->contFalling >= DEBOUNCE_TIME){
			if( !gpioRead(button->tecla_fsm) ){
				button->fsmButtonState = FSM_BUTTON_DOWN;
				// Acciones estandar
				button->tiempo_down = xTaskGetTickCount();
			}
			else button->fsmButtonState = FSM_BUTTON_UP;

			/* LEAVE */
			button->contFalling = 0;
		}
		button->contFalling++;
		break;

	case FSM_BUTTON_RISING:
		/* ENTRY */

		/* CHECK TRANSITION CONDITIONS */
		if(button->contRising >= DEBOUNCE_TIME){
			if( gpioRead(button->tecla_fsm) ){
				button->fsmButtonState = FSM_BUTTON_UP;
				// Acciones estandar para boton release
				button->tiempo_up = xTaskGetTickCount();
				button->tiempo_medido = button->tiempo_up - button->tiempo_down;
			}
			else button->fsmButtonState = FSM_BUTTON_DOWN;

			button->contRising = 0;
		}
		button->contRising++;
		break;

	default:
		//fsmButtonError();
		break;
	}
	return;
}
