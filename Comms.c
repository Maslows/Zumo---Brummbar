#include "osObjects.h"                         // RTOS object definitions
#include "bluetooth.h"
#include <string.h>
#include "motors.h"
#include "sonar.h"
#include "servo.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>


/*----------------------------------------------------------------------------
 *      Thread 1 'Comms'
 *---------------------------------------------------------------------------*/
 osThreadId tid_comms;  // thread id
 
void SendMessage(const char * fmt , ...){
  
 if(tid_comms){
  ProcessMessage_t * ProcessMessage;
  va_list vl;
   
  ProcessMessage = osMailAlloc(qid_ProcessMessage, 0);       // Allocate memory
  if (ProcessMessage != 0){
    va_start(vl, fmt);
    vsnprintf( ProcessMessage->msg, 99, fmt, vl);
    va_end( vl);
    
    osMailPut(qid_ProcessMessage, ProcessMessage);    // Send Mail
    osSignalSet(tid_comms, SIG_PROCMSG_MAIL_SENT);    // Send signal to Comms
  }
 }
}
 
typedef enum {
   STOP, 
   MOVE,
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



int Init_comms (void) {

  tid_comms = osThreadCreate (osThread(comms), NULL);
  if(!tid_comms) return(-1);
  
  return(0);
}

void Command_STOP(void){
   driveStop();
}
void Command_MOVE(direction_t direction, int32_t distance_cm){
    drive(speed,direction,distance_cm);
}

void Command_TURN_LEFT(void){
     turnLeft();
}
void Command_TURN_RIGHT(void){
     turnRight();  
}
void Command_TURN_BY_DEGREE(int angle){
     rotate(angle);
}
void Command_SPEED_UP(void){
  if (speed<=90){
    speed+=10;
  };
}
void Command_SPEED_DOWN(void){
  if (speed>=10){
    speed-=10;
   };
}
void Command_SPEED_SET(int new_speed){
 if (new_speed >= 0 && new_speed <= 100){
  speed = new_speed;
 };
}
void Command_SONAR_MODE(int NewMode){
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
void Command_SONAR_SINGLE_POOL(int new_deg){
  char buffor[12];
  sprintf(buffor, "%04d,%04hu\n",new_deg,SonarGetDistance(new_deg));
  bt_sendStr(buffor);
}
void Command_SONAR_SINGLE_INT(int new_deg){
  SonarStartMeas(new_deg);
}
void Command_SONAR_SET_LOCK_RANGE(int new_range){
  ServoChangeLockRange(new_range);
}
void Command_COMMAND_PARSE_FAILED(void){
  bt_sendStr("Unknown command \n");
}

void TransmitMessages(void){
  osEvent MailEvt;
  ProcessMessage_t * ProcessMessage;

  while(1){
    MailEvt = osMailGet(qid_ProcessMessage,0);
    if (MailEvt.status == osEventMail){
        ProcessMessage = MailEvt.value.p;
        bt_sendStr(ProcessMessage->msg);
        osMailFree(qid_ProcessMessage,ProcessMessage);     
    } else {
      break;
    }
  }  
}

void ExecuteCommand(cmd_t * command){
  switch (command->name){
    case STOP:
      Command_STOP();
      break;
    
    case MOVE:
      Command_MOVE((direction_t)command->arg1,command->arg2);
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

void ParseAndExecute(void){
  char InputString[100];
  cmd_t NewCommand;
  while(bt_getStr( InputString )){            
    //bt_sendStr( InputString );            // ...send it back.
    NewCommand = parse_command(InputString);
    ExecuteCommand(&NewCommand);
  }  
}
void comms (void const *argument) {
  osEvent SigEvt;
  
  
  while (1) {
    SigEvt = osSignalWait(0,osWaitForever);
         
    if(SigEvt.value.signals & SIG_PROCMSG_MAIL_SENT){
      TransmitMessages();
    }   
    if(SigEvt.value.signals & SIG_UART_DATA_RECIEVED){
      ParseAndExecute();
    }
    if(SigEvt.value.signals & SIG_START_ZUMOAI){
      Init_ZumoAI();
    }
    if(SigEvt.value.signals & SIG_KILL_ZUMOAI){
      Kill_ZumoAI();
    }
    if(SigEvt.value.signals & SIG_START_DEBOUNCE_TIMER){
      osTimerStart(tid_DebounceTimer,400);
    }
    
  }
}
