/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "bluetooth.h"
#include "sonar.h"
#include "servo.h"
#include "motorDriver.h"

osMailQId qid_SonarPacket;

/*
 * main: initialize and start the system
 */
int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS
  bt_init(9600u);
  Sonar_init(CONTINUOUS);
  Servo_init(MANUAL, SCAN_AND_LOCK);
  motorDriverInit();
  // initialize peripherals here
	
  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
  
  //tid_comms = osThreadCreate (osThread(comms), NULL);
  Init_comms();
  
	osKernelStart ();                         // start thread execution 
}
