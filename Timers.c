#include "osObjects.h"                         // RTOS object definitions


osTimerId tid_DebounceTimer;                  

/**
  @brief Callback function for debounce timer
  ISR routine set signal in comms.
  Software Timer starts counting for 500 miliseconds but noisy raising edges reset it.
  When Timer overflows Debounce_Callback is called.
 */
void Debounce_Callback (void const *arg){
  if(tid_zumoAI){
    osSignalSet(tid_comms,SIG_KILL_ZUMOAI);
  } else {
    osSignalSet(tid_comms,SIG_START_ZUMOAI);
  }
}
