/*---------------------------------------------------------------------------*/
/* 					Thread for Final Project — Joinable Threads											 */
/*---------------------------------------------------------------------------*/
 
 
/*******************************************************************/
/* Note: Change from using threadYield() to signel & wait */

/*******************************************************************/


#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include<stdio.h>
#include "Board_LED.h"                  // ::Board Support:LED

/*----------------------------------------------------------------------------
 *      Declare Functions
 *---------------------------------------------------------------------------*/
void delay(int milli_seconds );
void P1 (void const *argument); 
void P2 (void const *argument);
void P3 (void const *argument); 

 /*----------------------------------------------------------------------------
 *      Declare Variables
 *---------------------------------------------------------------------------*/
unsigned int counta=0;
unsigned int countb=1;
unsigned int countc=0;                            
int option = 0; 


/*----------------------------------------------*/
/*-- 		  Prepare to Create Threads						--*/
/*----------------------------------------------*/
// Define Thread IDs
osThreadId t_main, t_P1, t_P2, t_P3;

// Set Thread Priority
osThreadDef (P1, osPriorityAboveNormal, 1, 0);                   // thread object
osThreadDef (P2, osPriorityNormal, 1, 0);                   // thread object
osThreadDef (P3, osPriorityNormal, 1, 0);              // thread object

/*----------------------------------------------*/
/*-- 		  		Initialize Threads					  	--*/
/*----------------------------------------------*/
int Init_Thread (void) {
	
	// Main Thread
	t_main = osThreadGetId ();										
	osThreadSetPriority(t_main,osPriorityHigh);
	
	// Create Threads
  t_P1 = osThreadCreate (osThread(P1), NULL);
	t_P2 = osThreadCreate (osThread(P2), NULL);
	t_P3 = osThreadCreate (osThread(P3), NULL);
	

	// Check if thread is valid
  if(!t_P1) return(-1);
  
  return(0);
}


void P1 (void const *argument) {
	 for(;;) {
			
			// Determine which LEDS to flash
			if( option == 2 ){
					LED_On(0);
					delay(200);
					LED_Off(0);
					countb++;
			}else if ( option == 3){
					LED_On(1);
					delay(10000);															
					LED_Off(1);
					countc++;
			}
			
			// Alternate between threads
			if( countb == 10 ){
				// Signal thread 3 to run
				osSignalSet(t_P3,0x03);								// Call P3 to finish the task
				osSignalWait(0x01,osWaitForever);			// Block P1
				countb = 0;
				
			}else{
				// Signal thread 2 to run
				osSignalSet(t_P2,0x02);								// Call P3 to finish the task
				osSignalWait(0x01,osWaitForever);			// Block P1
				
			}
			
  }                                          
}

void P2 (void const *argument) {
	  for(;;) {
		
			// Set option to flash LEDs sequentially
			option = 2;
			
				// Signal thread 1 to run
				osSignalSet(t_P1,0x01);								// Call P1 to finish the task
				osSignalWait(0x02,osWaitForever);			// Block P2
  }
}

// *** mycode ***
void P3 (void const *argument) {
	  for(;;) {
				
				// Set option to flash LEDs evenly 
				option = 3;
				
				// Signal thread 1 to run
				osSignalSet(t_P1,0x01);								// Call P1 to finish the task
				osSignalWait(0x03,osWaitForever);			// Block P3
  }
}


// Delay Function
void delay(int milli_seconds ){
	int max;
	
	max = 1 * milli_seconds;
	
	for(int i = 0; i < max; i++){
		// do nothing
	}
}
