/*---------------------------------------------------------------------------*/
/* 						Main for Final Project — Joinable Threads											 */
/*---------------------------------------------------------------------------*/
 
 
/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "Board_LED.h"                  // ::Board Support:LED

extern int Init_Thread (void);							// Initialize Threads found in "Threads.c"
 

/*
 * main: initialize and start the system
 */
int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS
	LED_Initialize();													// Initialize LEDs
	
 	Init_Thread ();																// Create Main thread & 3 other threads
  osKernelStart ();                         // start thread execution 
	osDelay(osWaitForever);
}
