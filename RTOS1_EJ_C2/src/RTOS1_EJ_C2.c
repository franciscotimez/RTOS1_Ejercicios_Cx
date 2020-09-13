/*=============================================================================
 * Copyright (c) 2020, Francisco Timez <franciscotimez@gmail.com>
 * All rights reserved.
 * License: mit (see LICENSE.txt)
 * Date: 2020/09/12
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "RTOS1_EJ_C2.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

#include "sapi.h"
#include "userTasks.h"

#include "macros.h"				//Definicion de macros
#include "fsm_debounce_RTOS.h"

#include "semphr.h"  			//Libreria de semaforo de freertos

#include <string.h>


/*==================[definiciones de datos internos]=========================*/

gpioMap_t teclas[] = {TEC1,TEC2,TEC3,TEC4};
gpioMap_t leds[]   = {LEDB,LED1,LED2,LED3};

#define N_TECLAS  sizeof(teclas)/sizeof(gpioMap_t)		// 4 * gpioMap_t / gpioMap_t = 4

led_tecla_t teclaLED[N_TECLAS];

// Prototipo de funcion de la tarea
void tarea_led( void* taskParmPtr );
void tarea_tecla( void* taskParmPtr );
void tecla_led_init(void);
void crear_tareas(TaskFunction_t tarea,const char * const nombre);

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.

int main( void ){
	// ---------- CONFIGURACIONES ------------------------------
	boardConfig();									// Inicializar y configurar la plataforma

	debugPrintConfigUart( UART_USB, 115200 );		// UART for debug messages
	printf( "Ejercicio C_1.\r\n" );

	tecla_led_init();
	printf( "Se inicio la estructura de las teclas y leds. \n\r" );


	// Create a task in freeRTOS with dynamic memory
	crear_tareas(tarea_tecla,"tarea_tecla_");
	printf( "Se crearon las tareas para las teclas. \n\r" );

	crear_tareas(tarea_led,"tarea_led_");
	printf( "Se crearon las tareas para los leds. \n\r" );

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

/*******************************************
 * 		tecla_led_init FUNCTION
 * 	Realiza el linkeo entre los leds y
 * 	las teclas y los semaforos
 *******************************************/
void tecla_led_init(void){
	uint16_t i;

	for(i = 0 ; i < N_TECLAS ; i++){
		teclaLED[i].led = leds[i];
		teclaLED[i].tecla.tecla_fsm = teclas[i];
		teclaLED[i].semaphore = xSemaphoreCreateBinary();

		// Gestion de errores de semaforos
		if( teclaLED[i].semaphore == pdFALSE){
			gpioWrite( LEDR, ON );
			printf( "Error al crear los semaforos.\r\n" );
			while(TRUE);						// VER ESTE LINK: https://pbs.twimg.com/media/BafQje7CcAAN5en.jpg
		}
	}
	return;
}

/*******************************************
 * 		crear_tareas FUNCTION
 * 	Crea el conjunto de tareas para las
 * 	teclas/leds
 *******************************************/
void crear_tareas(TaskFunction_t tarea,const char * const nombre){
	uint16_t i;

	char nombre_aux[STR_AUX];

	for(i = 0 ; i < N_TECLAS ; i++)
	{
		strcpy(nombre_aux, nombre);
		strcat(nombre_aux, (char)(i+48));
		//          tarea    ,nombre      ,SIZE*STACK,parametro           ,idle+1,0;
		crear_tarea(tarea,
				nombre_aux,
				SIZE,
				&teclaLED[i],
				PRIORITY,
				NULL);
	}
	return;
}


/*******************************************
 * 		crear_tarea FUNCTION
 * 	Crea la tarea RTOS y gestiona los errores
 *******************************************/
void crear_tarea(TaskFunction_t tarea,const char * const nombre,uint8_t stack,void * const parametros,uint8_t prioridad,TaskHandle_t * const puntero)
{
	// Crear tarea en freeRTOS
	BaseType_t res = xTaskCreate(tarea,
			nombre,
			configMINIMAL_STACK_SIZE*stack,
			parametros,
			tskIDLE_PRIORITY+prioridad,
			puntero);                         		// Puntero a la tarea creada en el sistema

	// Gestion de errores
	if(res == pdFAIL)
	{
		gpioWrite( LEDR, ON );
		printf( "Error al crear las tareas. \n\r" );
		while(TRUE);						// VER ESTE LINK: https://pbs.twimg.com/media/BafQje7CcAAN5en.jpg
	}
	return;
}

/*******************************************
 * 		tarea_tecla FUNCTION
 * 	Tarea que realiza la tecla
 *******************************************/
void tarea_tecla( void* taskParmPtr )
{
	led_tecla_t* tecla_led = (led_tecla_t*) taskParmPtr;

	fsmButtonInit(&tecla_led->tecla);

	while( TRUE )
	{
		fsmButtonUpdate( &tecla_led->tecla );

		if(tecla_led->tecla.tiempo_medido > 0)
			xSemaphoreGive( tecla_led->semaphore );

		vTaskDelay( BUTTON_RATE / portTICK_RATE_MS );

		//vTaskDelay ( pdMS_TO_TICKS(BUTTON_RATE) );
	}
}

/*******************************************
 * 		tarea_led FUNCTION
 * 	Tarea que realiza el led
 *******************************************/
void tarea_led( void* taskParmPtr )
{
	// ---------- CONFIGURACIONES ------------------------------
	led_tecla_t* tecla_led = (led_tecla_t*) taskParmPtr;

	// ---------- REPETIR POR SIEMPRE --------------------------
	while( TRUE )
	{
		xSemaphoreTake( tecla_led->semaphore , portMAX_DELAY );			// Esperamos tecla sin TimeOut

		gpioWrite( tecla_led->led , ON );
		vTaskDelay( tecla_led->tecla.tiempo_medido );
		gpioWrite( tecla_led->led , OFF );
		tecla_led->tecla.tiempo_medido = 0;
	}
}

/*==================[fin del archivo]========================================*/
