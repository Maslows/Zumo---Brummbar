#ifndef COMMANDS_H
#define COMMANDS_H

/**
  @brief Define Command Names
*/
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

/**
  @brief Define Command structure.
*/
typedef struct command{
	cmdName_t name;
	int arg1;
	int arg2;
	int arg3;
} cmd_t; 

cmd_t parse_command(char * command_string);
void ExecuteCommand(cmd_t * command);


#endif
