/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "bluetooth.h"
#include "sonar.h"
#include "servo.h"
#include "motorDriver.h"

osMailQId qid_SonarSample;

/*
 * main: initialize and start the system
 */
int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS
  
  /* Initialize Mail queues */
  qid_SonarSample = osMailCreate(osMailQ(SonarSample),NULL);
  
  /* Initialize Peripherials */
  bt_init(9600u);
  Sonar_init(CONTINUOUS);
  Servo_init(MANUAL, SCAN_AND_LOCK);
  motorDriverInit();
 
  
  
  
  Init_comms();
  
	osKernelStart ();                         // start thread execution 
}
