#include "osObjects.h"                         // RTOS object definitions
#include "comms.h"
#include "MKL46Z4.h"                    // Device header

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "commands.h"
#include "bluetoothDMA.h"

/******************************** Thread: 'Comms' **************************/

 osThreadId tid_comms = 0;  // thread id

/**
  @brief Comms thread initialization function
*/
int Init_comms (void) {

  tid_comms = osThreadCreate (osThread(comms), NULL);
  if(!tid_comms) return(-1);
  
  return(0);
}

/**
  @brief This function sends a message in the same way as using printf. 
*/
void SendMessage(const char * fmt , ...){
  
 if(tid_comms){
  Message_t * Message;
  va_list vl;                                                // stores number and type of arguments
   
  Message = osMailAlloc(qid_MessageTX, 0);       // allocate memory for new message
   
  if (Message != 0){
    __disable_irq();                                         // disable interrupts
    va_start(vl, fmt);                                       // start agument iteration
    vsnprintf( Message->msg, 100, fmt, vl);                  // add arguments to buffor based on format string
    va_end( vl);                                             // finish argument iteration
    __enable_irq();                                          //enable interrupts
    Message->msg[100] = '\0';                                // Add null sign just in cass
    osMailPut(qid_MessageTX, Message);                       // send Mail
    
    /* trigger DMA1 interrupt if not busy */
    if(!(DMA0->DMA[1].DSR_BCR & DMA_DSR_BCR_BSY_MASK)){
      NVIC_SetPendingIRQ(DMA1_IRQn);
    }
  }
 }
}

/**
  @brief Get recieved sting from mail, pare and execute;
*/
void ParseAndExecute(void){
  osEvent osEvt;
  Message_t * Message;
  
  cmd_t NewCommand;
  while(1){
    osEvt = osMailGet(qid_MessageRX,0);
    if ( osEvt.status == osEventMail){
      Message = osEvt.value.p;
      NewCommand = parse_command(Message->msg);
      ExecuteCommand(&NewCommand);
    } else {
      break;
    }
  }  
}

/**
  @brief Comms main thread loop.
  This function waits for external signal and executes appropriate function.
*/
void comms (void const *argument) {
  osEvent SigEvt;
   
  while (1) {
    SigEvt = osSignalWait(0,osWaitForever);             
           
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
      osTimerStart(tid_DebounceTimer,500);
    } 
  }
}
