/*----------------------------------------------------------------------------
 * osObjects.h: CMSIS-RTOS global object definitions for an application
 *----------------------------------------------------------------------------
 *
 * This header file defines global RTOS objects used throughout a project
 *
 * #define osObjectsPublic indicates that objects are defined; without that
 * definition the objects are defined as external symbols.
 *
 *--------------------------------------------------------------------------*/


#ifndef __osObjects
#define __osObjects

#if (!defined (osObjectsPublic))
#define osObjectsExternal          // define RTOS objects with extern attribute
#endif

#include <cmsis_os.h>              // CMSIS RTOS header file

// global signals definitions
typedef enum {
  SIG_SONAR_MAIL_SENT = 0x01,
  SIG_UART_DATA_RECIEVED = 0x02,
  SIG_MOVE_COMPLETE = 0x4,
  SIG_ENEMY_LOCK_ON = 0x08,
  SIG_ENEMY_LOCK_LOST = 0x10,
  SIG_PROCMSG_MAIL_SENT = 0x20,
  SIG_START_ZUMOAI = 0x40,
  SIG_KILL_ZUMOAI = 0x80,
  SIG_START_DEBOUNCE_TIMER = 0x100,
  SIG_SWEEP_COMPLETE = 0x200
} Signal_t;

//typedef enum {
//} ZummoAISignals_t;
             

// global 'thread' functions ---------------------------------------------------
/* 
Example:
extern void sample_name (void const *argument);          // thread function

osThreadId tid_sample_name;                              // thread id
osThreadDef (sample_name, osPriorityNormal, 1, 0);       // thread object
*/

/* Comms */
void comms (void const *argument);                 // thread function
int Init_comms(void);                       // thread initialization function
extern osThreadId tid_comms;                       // thread id
osThreadDef(comms, osPriorityHigh, 1, 0);       // thread object

/* ZumoAI */
void zumoAI (void const *argument);                  // thread function
void Kill_ZumoAI(void);
int Init_ZumoAI(void);                       // thread initialization function
extern osThreadId tid_zumoAI;                       // thread id
osThreadDef(zumoAI, osPriorityNormal, 1, 0);       // thread object


// global 'semaphores' ----------------------------------------------------------
/* 
Example:
osSemaphoreId sid_sample_name;                           // semaphore id
osSemaphoreDef (sample_name);                            // semaphore object
*/


// global 'memory pools' --------------------------------------------------------
/* 
Example:
typedef struct sample_name type_sample_name;             // object data type

osPoolId mpid_sample_name;                               // memory pool id
osPoolDef (sample_name, 16, type_sample_name);           // memory pool object
*/


// global 'message queues' -------------------------------------------------------
/* 
Example:
typedef struct sample_name type_sample_name;             // object data type

osMessageQId mid_sample_name;                            // message queue id
osMessageQDef (sample_name, 16, type_sample_name);       // message queue object
*/


// global 'mail queues' ----------------------------------------------------------
/* 
Example:
typedef struct sample_name type_sample_name;             // object data type

osMailQId qid_sample_name;                               // mail queue id
osMailQDef (sample_name, 16, type_sample_name);          // mail queue object
*/


/* Process Messages */
typedef struct ProcessMessage{
  char msg[100];
} ProcessMessage_t;

extern osMailQId qid_ProcessMessage;
osMailQDef (ProcessMessage, 15, ProcessMessage_t);
extern void SendMessage(const char * fmt , ...);


/* Timers */
/*Debounce timer for user button */
void Debounce_Callback (void const *arg);                    // prototype for timer callback function
osTimerDef (DebounceTimer, Debounce_Callback);                        // define timer
extern osTimerId tid_DebounceTimer;
#endif  // __osObjects
