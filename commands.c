#include "osObjects.h"
#include "commands.h"
#include "sonar.h"

#include "Comms.h"
#include <stdio.h>

#include "servo.h"
#include "motors.h"

/**
  @brief Parse command to common format 
 */
cmd_t parse_command(char * command_string){
	cmd_t command;
	char status;
	
	status = sscanf(command_string, "@%i#%d#%d#%d$", (int *)&command.name, &command.arg1, &command.arg2, &command.arg3);
	
	if (status != 4){
		command.name = COMMAND_PARSE_FAILED;
	}

	return command;
}

void Command_STOP(void){
   driveStop();
   if(tid_zumoAI){
     Kill_ZumoAI();
   }
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
  SendMessage("%04d,%04hu\n",new_deg,SonarGetDistance(new_deg)); 
}
void Command_SONAR_SINGLE_INT(int new_deg){
  SonarStartMeas(new_deg);
}
void Command_SONAR_SET_LOCK_RANGE(int new_range){
  ServoChangeLockRange(new_range);
}
void Command_COMMAND_PARSE_FAILED(void){
  SendMessage("Unknown command.\n");
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
