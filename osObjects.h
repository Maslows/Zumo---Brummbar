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

/* Define RTOS objects with extern attribute */
#if (!defined (osObjectsPublic))
#define osObjectsExternal          
#endif

/* CMSIS RTOS header file */
#include <cmsis_os.h>              

/* Global signals definitions */
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



/*************************** Threads **********************************/
/* Comms */
void comms (void const *argument);                 // thread function
int Init_comms(void);                              // thread initialization function
extern osThreadId tid_comms;                       // thread id
osThreadDef(comms, osPriorityHigh, 1, 0);          // thread object

/* ZumoAI */
void zumoAI (void const *argument);                // thread function
int Init_ZumoAI(void);                             // thread initialization function
void Kill_ZumoAI(void);                            // thread termination function
extern osThreadId tid_zumoAI;                      // thread id
osThreadDef(zumoAI, osPriorityNormal, 1, 0);       // thread object



/*************************** Process Messages Mail Queue ********************/
/**
  @brief Process Meaage typedef
*/
typedef struct Message{
  char msg[101];
} Message_t;

extern osMailQId qid_MessageTX;            // mail id
extern osMailQId qid_MessageRX;            // mail id
osMailQDef (MessageTX, 10, Message_t);     // mail queue definition
osMailQDef (MessageRX, 4, Message_t);     // mail queue definition
void SendMessage(const char * fmt , ...);              // mail send function

/******************************** Timers *************************************/

/*Debounce timer for user button */
void Debounce_Callback (void const *arg);           // forward declaration of timer callback function
osTimerDef (DebounceTimer, Debounce_Callback);      // define timer
extern osTimerId tid_DebounceTimer;                 // timer id


#endif  
