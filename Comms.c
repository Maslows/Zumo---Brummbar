
//#include <cmsis_os.h>                        // CMSIS RTOS header file
#include "osObjects.h"                         // RTOS object definitions
#include "bluetooth.h"
#include <string.h>
#include "motorDriver.h"
#include "sonar.h"
#include "servo.h"
#include <stdlib.h>
#include <stdio.h>

/*----------------------------------------------------------------------------
 *      Thread 1 'Comms': Sample thread
 *---------------------------------------------------------------------------*/

osThreadId tid_comms;  // thread id

int Init_comms (void) {

  tid_comms = osThreadCreate (osThread(comms), NULL);
  if(!tid_comms) return(-1);
  
  return(0);
}

char tab[BUFF_SIZE];
// KrowaKrowa456 10/3
// Muka-23 4/3
int ParseIntNumber(char * string, int begining, int length){
	int i;
	int result = 0;
	int exponent = 1;
	for ( i = begining + length - 1; i != begining -1; i--){
		if (string[i] != '-'){
			string[i] -= 48;
			string[i] *= exponent;
			result += string[i];
			exponent *= 10;
		}
		else {
			result *= -1;
			break;
		}
	}
	return result;
}

void comms (void const *argument) {

  while (1) {
    bt_getStr( tab );								// Get string from buffer
		if(strlen( tab )){							// If isn't empty...
			//bt_sendStr( tab );						// ...send it back.
			if ( strcmp(tab, "w") == 0 ) {
				driveForward(speed);
			}
			else if ( strcmp(tab, "s") == 0) {
				driveReverse(speed);
			}
			else if ( strcmp(tab, " ") == 0) {
				driveStop();
			} 
			else if ( strcmp(tab, "a") == 0) {
        setTracksDir(REVERSE, FORWARD);
        setTracksSpeed(DEFAULT_TURNING_SPEED,DEFAULT_TURNING_SPEED);
			}
			else if ( strcmp(tab, "d") == 0) {
				setTracksDir(FORWARD, REVERSE);
        setTracksSpeed(DEFAULT_TURNING_SPEED,DEFAULT_TURNING_SPEED);
			}
			else if ( strcmp(tab, "ServoScanAndLock") == 0) {
				ServoChangeMode(SWEEP);
				ServoChangeSweepMode(SCAN_AND_LOCK);
			}
			else if ( strcmp(tab, "ServoScanAndGo") == 0) {
				ServoChangeMode(SWEEP);
				ServoChangeSweepMode(SCAN_AND_GO);
			}
			else if ( strcmp(tab, "ServoCenter") == 0) {
				ServoChangeMode(MANUAL);
				ServoMoveByDegree(0);
			}
			else if ( strncmp(tab, "SonarStartMeas",14) == 0) {
				int new_deg = ParseIntNumber(tab,14,3);
				SonarStartMeas(new_deg);
			}
			else if ( strncmp(tab, "SonarGetDistance",16) == 0) {
				char buffor[12];
				int new_deg = ParseIntNumber(tab,16,3);
				sprintf(buffor, "%04d,%04hu\n",new_deg,SonarGetDistance(new_deg));
				bt_sendStr(buffor);
			}
			else if ( strcmp(tab, "e") == 0) {
				if (speed<=90){
					speed+=10;
				}
			}
			else if ( strcmp(tab, "q") == 0) {
				if (speed>=10){
					speed-=10;
				}
			}
			else if (strncmp(tab, "speed",5) == 0){
				int new_speed = ParseIntNumber(tab,5,3);
				if (new_speed >= 0 && new_speed <= 100){
					speed = new_speed;
				}
			}
			else if ( strncmp(tab, "SonarLockRange",14) == 0) {
				int new_range = ParseIntNumber(tab,14,3);
				ServoChangeLockRange(new_range);
			}
		}
    osThreadYield();                                            // suspend thread
  }
}


