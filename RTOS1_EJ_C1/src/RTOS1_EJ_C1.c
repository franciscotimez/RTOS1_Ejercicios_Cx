/*=============================================================================
 * Copyright (c) 2020, Francisco Timez <franciscotimez@gmail.com>
 * All rights reserved.
 * License: mit (see LICENSE.txt)
 * Date: 2020/09/10
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "RTOS1_EJ_C1.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

#include "sapi.h"
#include "userTasks.h"

#include "macros.h"				//Definicion de macros
#include "fsm_debounce_RTOS.h"

#include "semphr.h"  			//Libreria de semaforo de freertos


/*==================[definiciones de datos internos]=========================*/

SemaphoreHandle_t sem_tec_pulsada;

TeclaRTOSfsm_t tecla1;

// Prototipo de funcion de la tarea
void tarea_led( void* taskParmPtr );
void tarea_tecla( void* taskParmPtr );

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.

int main( void )
{
    // ---------- CONFIGURACIONES ------------------------------
	boardConfig();									// Inicializar y configurar la plataforma

	debugPrintConfigUart( UART_USB, 115200 );		// UART for debug messages
	printf( "Ejercicio C_1.\r\n" );

	//gpioWrite( LED3, ON );							// Led para dar señal de vida

    // Crear tarea en freeRTOS
    BaseType_t res_A =
    xTaskCreate(
        tarea_led,                     // Funcion de la tarea a ejecutar
        ( const char * )"tarea_led",   // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
        0,                          // Parametros de tarea
        tskIDLE_PRIORITY+1,         // Prioridad de la tarea
        0                           // Puntero a la tarea creada en el sistema
    );


    BaseType_t res_B =
    xTaskCreate(
    	tarea_tecla,                     // Funcion de la tarea a ejecutar
        ( const char * )"tarea_tecla",   // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE*2, 	// Cantidad de stack de la tarea
        0,                          	// Parametros de tarea
        tskIDLE_PRIORITY+1,         	// Prioridad de la tarea
        0                           	// Puntero a la tarea creada en el sistema
    );

    // Gestion de errores de tareas
	if((res_A || res_B) == pdFAIL)
	{
		gpioWrite( LEDR, ON );
		printf( "Error al crear las tareas.\r\n" );
		while(TRUE);						// VER ESTE LINK: https://pbs.twimg.com/media/BafQje7CcAAN5en.jpg
	}

	sem_tec_pulsada = xSemaphoreCreateBinary();

	// Gestion de errores de semaforos
	if( sem_tec_pulsada == pdFALSE)
	{
		gpioWrite( LEDR, ON );
		printf( "Error al crear los semaforos.\r\n" );
		while(TRUE);						// VER ESTE LINK: https://pbs.twimg.com/media/BafQje7CcAAN5en.jpg
	}

    // Iniciar scheduler
    vTaskStartScheduler();					// Enciende tick | Crea idle y pone en ready | Evalua las tareas creadas | Prioridad mas alta pasa a running

    // ---------- REPETIR POR SIEMPRE --------------------------
    while( TRUE )
    {
        // Si cae en este while 1 significa que no pudo iniciar el scheduler
    }

    // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
    // directamenteno sobre un microcontroladore y no es llamado por ningun
    // Sistema Operativo, como en el caso de un programa para PC.
    return 0;
}

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

void tarea_tecla( void* taskParmPtr )
{
	tecla1.tecla_fsm = TEC1;

	fsmButtonInit(&tecla1);

	while( TRUE )
	{
		fsmButtonUpdate( &tecla1 );

		if(tecla1.tiempo_medido > 0)
			xSemaphoreGive( sem_tec_pulsada );

		vTaskDelay( BUTTON_RATE / portTICK_RATE_MS );

	 	//vTaskDelay ( pdMS_TO_TICKS(BUTTON_RATE) );
	}
}

// Implementacion de funcion de la tarea
void tarea_led( void* taskParmPtr )
{
    // ---------- CONFIGURACIONES ------------------------------

    // ---------- REPETIR POR SIEMPRE --------------------------
    while( TRUE )
    {
    	xSemaphoreTake( sem_tec_pulsada , portMAX_DELAY );			// Esperamos tecla sin TimeOut

    	gpioWrite( LEDB , ON );
		vTaskDelay( tecla1.tiempo_medido );
		gpioWrite( LEDB , OFF );
		tecla1.tiempo_medido = 0;
    }
}

/*==================[fin del archivo]========================================*/
