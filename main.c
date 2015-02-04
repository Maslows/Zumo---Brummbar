/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "bluetooth.h"
#include "sonar.h"
#include "servo.h"
#include "motors.h"

//osMailQId qid_SonarSample;
osMailQId qid_ProcessMessage;

/*
 * main: initialize and start the system
 */
int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS
  
  /* Initialize Mail queues */
  qid_ProcessMessage = osMailCreate(osMailQ(ProcessMessage),NULL);
  
  /* Initialize Timers */
  tid_DebounceTimer = osTimerCreate (osTimer(DebounceTimer), osTimerOnce,0);
  
  /* Initialize Peripherials */
  bt_init(9600u);
  Sonar_init(CONTINUOUS);
  Servo_init(MANUAL, SCAN_AND_LOCK);
  motors_init();
   
  
  Init_comms();
  
	osKernelStart ();                         // start thread execution 
}
