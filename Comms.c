#include "osObjects.h"                         // RTOS object definitions
#include "bluetooth.h"
#include <string.h>
#include "motors.h"
#include "sonar.h"
#include "servo.h"
#include <stdlib.h>
#include <stdio.h>

/*----------------------------------------------------------------------------
 *      Thread 1 'Comms'
 *---------------------------------------------------------------------------*/
 
 typedef enum {
   STOP, 
   MOVE_FORWARD,
   MOVE_BACKWARD,
   TURN_LEFT,
   TURN_RIGHT,
   TURN_BY_DEGREE,
   SPEED_UP,
   SPEED_DOWN,
   SPEED_SET,
   SONAR_MODE,
   SONAR_SINGLE_POOL,
   SONAR_SINGLE_INT,
   SONAR_SET_LOCK_RANGE,
   COMMAND_PARSE_FAILED
 }cmdName_t;
 
 typedef struct command{
	cmdName_t name;
	int arg1;
	int arg2;
	int arg3;
} cmd_t; 
 
cmd_t parse_command(char * command_string){
	cmd_t command;
	char status;
	
	status = sscanf(command_string, "@%i#%d#%d#%d$", (int *)&command.name, &command.arg1, &command.arg2, &command.arg3);
	
	if (status != 4){
		command.name = COMMAND_PARSE_FAILED;
	}

	return command;
}

osThreadId tid_comms;  // thread id

int Init_comms (void) {

  tid_comms = osThreadCreate (osThread(comms), NULL);
  if(!tid_comms) return(-1);
  
  return(0);
}

char InputString[BUFF_SIZE];

__inline void Command_STOP(void){
   driveStop();
}
__inline void Command_MOVE_FORWARD(void){
    driveForward(speed);
}
__inline void Command_MOVE_BACKWARD(void){
     driveReverse(speed);
}
__inline void Command_TURN_LEFT(void){
     turnLeft();
}
__inline void Command_TURN_RIGHT(void){
     turnRight();  
}
__inline void Command_TURN_BY_DEGREE(int angle){
     rotate(angle);
}
__inline void Command_SPEED_UP(void){
  if (speed<=90){
    speed+=10;
  };
}
__inline void Command_SPEED_DOWN(void){
  if (speed>=10){
    speed-=10;
   };
}
__inline void Command_SPEED_SET(int new_speed){
 if (new_speed >= 0 && new_speed <= 100){
  speed = new_speed;
 };
}
__inline void Command_SONAR_MODE(int NewMode){
  switch (NewMode){
            case 0:
              ServoChangeMode(MANUAL);
              ServoMoveByDegree(0);
              break;
            case 1:
               ServoChangeSweepMode(SCAN_AND_GO);
               ServoChangeMode(SWEEP); 
               break;
            case 2:
               ServoChangeSweepMode(SCAN_AND_LOCK);
               ServoChangeMode(SWEEP);
               break;
            default:
              break;
            }        
}
__inline void Command_SONAR_SINGLE_POOL(int new_deg){
  char buffor[12];
  sprintf(buffor, "%04d,%04hu\n",new_deg,SonarGetDistance(new_deg));
  bt_sendStr(buffor);
}
__inline void Command_SONAR_SINGLE_INT(int new_deg){
  SonarStartMeas(new_deg);
}
__inline void Command_SONAR_SET_LOCK_RANGE(int new_range){
  ServoChangeLockRange(new_range);
}
__inline void Command_COMMAND_PARSE_FAILED(void){
  bt_sendStr("Unknown command \n");
}

__inline void ExecuteCommand(cmd_t * command){
  switch (command->name){
    case STOP:
      Command_STOP();
      break;
    
    case MOVE_FORWARD:
      Command_MOVE_FORWARD();
      break;
    
    case MOVE_BACKWARD:
      Command_MOVE_BACKWARD();
      break;
    
    case TURN_LEFT:
      Command_TURN_LEFT();
      break;
    
    case TURN_RIGHT:
      Command_TURN_RIGHT();
      break;
    
    case TURN_BY_DEGREE:
      Command_TURN_BY_DEGREE(command->arg1);
      break;
    
    case SPEED_UP:
      Command_SPEED_UP();
      break;
    
    case SPEED_DOWN:
      Command_SPEED_DOWN();
      break;
    
    case SPEED_SET:
       Command_SPEED_SET(command->arg1);
       break;
    
    case SONAR_MODE:
       Command_SONAR_MODE(command->arg1);
       break; 
    
    case SONAR_SINGLE_POOL:
       Command_SONAR_SINGLE_POOL(command->arg1);
       break;
    
    case SONAR_SINGLE_INT:
       Command_SONAR_SINGLE_INT(command->arg1);
       break;
    
    case SONAR_SET_LOCK_RANGE:
       Command_SONAR_SET_LOCK_RANGE(command->arg1);    
       break;
    
    case COMMAND_PARSE_FAILED:
    default:
       Command_COMMAND_PARSE_FAILED();
  }
}
void comms (void const *argument) {
  osEvent SigEvt,MailEvt;
  cmd_t NewCommand;
  SonarSample_t * SonarSample;
  
  while (1) {
    SigEvt = osSignalWait(0,osWaitForever);
    
    if (SigEvt.status == osEventSignal){
      
      if(SigEvt.value.signals | SIG_SONAR_MAIL_SENT){
        while(1){
          MailEvt = osMailGet(qid_SonarSample,0);
          if (MailEvt.status == osEventMail){
            char buffor[12];
            SonarSample = MailEvt.value.p;
            sprintf(buffor, "%04d,%04hu\n",SonarSample->angle,SonarSample->distance);
            bt_sendStr(buffor);
            osMailFree(qid_SonarSample, SonarSample);
          } else {
            break;
          }
        }    
      }
      
      if(SigEvt.value.signals | SIG_UART_DATA_RECIEVED){
        while(bt_getStr( InputString )){						
          //bt_sendStr( InputString );						// ...send it back.
          NewCommand = parse_command(InputString);
          ExecuteCommand(&NewCommand);
        }
      } 
    }
  }
}
