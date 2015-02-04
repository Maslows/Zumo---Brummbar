#include "osObjects.h"                         // RTOS object definitions


osTimerId tid_DebounceTimer;

void Debounce_Callback (void const *arg){
    if(tid_zumoAI){
    osSignalSet(tid_comms,SIG_KILL_ZUMOAI);
  } else {
    osSignalSet(tid_comms,SIG_START_ZUMOAI);
  }
}
