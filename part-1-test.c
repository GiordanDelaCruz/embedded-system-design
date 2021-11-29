/*---------------------------------------------------------------------------*/
/* 					Thread for Final Project — Joinable Threads											 */
/*---------------------------------------------------------------------------*/

#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include<stdio.h>
#include "LPC17xx.h"                    // Device header
#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_ADC.h"                  // ::Board Support:A/D Converter
#include "Board_Joystick.h"							// ::Board Support:Joystick
/*----------------------------------------------------------------------------
 *      Declare Functions
 *---------------------------------------------------------------------------*/
void delay(int milli_seconds );
void callback(void const *param);
void P1 (void const *argument); 
void P2 (void const *argument);
void P3 (void const *argument); 

 /*----------------------------------------------------------------------------
 *      Declare Variables
 *---------------------------------------------------------------------------*/                          
int option = 0; 

osTimerDef(timer0_handle, callback);
osTimerDef(timer1_handle, callback);

/*----------------------------------------------*/
/*-- 		  Prepare to Create Threads						--*/
/*----------------------------------------------*/
// Define Thread IDs
osThreadId t_main, t_P1, t_P2, t_P3;

// Set Thread Priority
osThreadDef (P1, osPriorityNormal, 1, 0);                   
osThreadDef (P2, osPriorityNormal, 1, 0);                   
osThreadDef (P3, osPriorityNormal, 1, 0);          

/*----------------------------------------------*/
/*-- 		NOT IN USE: Initialize Threads				--*/    
/*----------------------------------------------*/
int Init_Thread (void) {
	
	// Create Threads
  t_P1 = osThreadCreate (osThread(P1), NULL);
	t_P2 = osThreadCreate (osThread(P2), NULL);
	t_P3 = osThreadCreate (osThread(P3), NULL);
	

	// Check if thread is valid
  if(!t_P1) return(-1);
  
  return(0);
}


void P1 (void const *argument) {
	 //for(;;) {
		
			// Dont do anything
			osThreadYield();
	
			printf("[+]Option Value = %d\n", option);
	
			// Determine which LEDS to flash
			if( option == 2 ){
				
					printf("[+]Inside P1, option = 2\n");
					LED_On(0);
					delay(200);
					LED_Off(0);
	
			}else if ( option == 3){
				
					printf("[+]Inside P1, option = 3\n");
					LED_On(1);
					delay(200);															
					LED_Off(1);
				
			}
			
			// Signal Main thread to run again
			printf("DEBUG: P1 --> Calling Main\n");
			osSignalSet(t_main,0x04);								// Call Main to finish the task
			
//			printf("DEBUG: P1 --> Blocked \n");
//			osSignalWait(0x01,osWaitForever);			// Block P1
			
				// Terminate P1
				printf("DEBUG: P1 --> Terminate \n");
				osThreadTerminate( t_P1 );
			
  //}                                          
}

void P2 (void const *argument) {
	  //for(;;) {
		
			// Set option to flash LEDs sequentially
			option = 2;
			
			// Signal thread 1 to run
			printf("DEBUG: P2 --> Calling P1\n");
			osSignalSet(t_P1,0x01);								// Call P1 to finish the task
	
//			printf("DEBUG: P2 --> Blocked \n");
//			osSignalWait(0x02,osWaitForever);			// Block P2
//			printf("[+]After signal P1\n");
	
			// Terminate P2
			printf("DEBUG: P2 --> Terminate \n");
			osThreadTerminate( t_P2 );

  //}
}

// *** mycode ***
void P3 (void const *argument) {
	  //for(;;) {
				
				// Set option to flash LEDs evenly 
				option = 3;
				
//				// Signal thread 1 to run
//				osSignalSet(t_P1,0x01);								// Call P1 to finish the task
//				osSignalWait(0x03,osWaitForever);			// Block P3
	
				// Terminate P3
				printf("DEBUG: P3 --> Terminate \n");
				osThreadTerminate( t_P3 );

  //}
}


int main(void)
{    
	osKernelInitialize (); 
	LED_Initialize();
	Joystick_Initialize();								 // Joystck Initialization
	
	LPC_SC -> PCONP			|= (1 << 15);			 // Power up Peripheral
	
	/* P1.20, P1.23..26 is GPIO (Joystick) */
	LPC_PINCON -> PINSEL3 &= ~( (3<< 8) | (3<< 14) | (3<<16) | (3<<18) | (3<< 20) );		// Specify operating mode
		
	/* P1.20, P1.23..26 is input */
	LPC_GPIO1 -> FIODIR				&= ~( (1<< 20) | (1<< 23) | (1<< 24) | (1<< 25) | (1<< 26) );		// Specify pin direction
	
	//Virtual timer create and start
	osTimerId timer_0 = osTimerCreate(osTimer(timer0_handle), osTimerPeriodic, (void *)0);	
	osTimerStart(timer_0, 100);	
	
	osTimerId timer_1 = osTimerCreate(osTimer(timer0_handle), osTimerPeriodic, (void *)0);	
	osTimerStart(timer_1, 200);	
	
	// Get Joystick input
	uint32_t joyStickValue =  Joystick_GetState();


	while(1){
			
			// Create Main thread
			t_main = osThreadGetId ();
			osThreadSetPriority(t_main,osPriorityHigh);
			
			// Create Sub-Threads
			t_P1 = osThreadCreate(osThread(P1), NULL);
			t_P2 = osThreadCreate(osThread(P2), NULL);
			t_P3 = osThreadCreate(osThread(P3), NULL);

			osKernelStart (); 
		
		
		
			joyStickValue =  Joystick_GetState();
		
		  	// Alternate between threads
			if( joyStickValue == JOYSTICK_UP ){
				
				// Signal thread 2 to run
				osSignalSet(t_P2,0x02);								// Call P2 to finish the task
				
			}else if ( joyStickValue == JOYSTICK_DOWN ){
				
				// Signal thread 3 to run
				osSignalSet(t_P3,0x03);								// Call P3 to finish the task
			
			}
			
			osSignalWait(0x04,osWaitForever);			// Block Main
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


// Toggle the LED associated with the timer
void callback(void const *param){
	switch( (uint32_t) param){
		case 0:		
			osSignalSet	(t_main,0x04);									// Main thread
		
			break;
		
		case 1:
			//osSignalSet(t_P1,0x01);								// Call P1 to finish the task
		
		  break;
	}
}

// Call lights to turn on

