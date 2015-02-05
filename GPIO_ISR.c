#include "osObjects.h"                      // RTOS object definitions
#include "motors.h"

/**  
  @brief ISR for Left_A encoder
*/
void PORTA_IRQHandler(void){

  if (EncFlags & LEFT){
    LeftTrackTurnDist -= TEETH_DISTANCE_MM;
    if ( LeftTrackTurnDist <= 0 ) {
        EncFlags &= ~LEFT;
        SetTrackSpeed(LEFT, 0);
        if (EncFlags == 0){
          osSignalSet(tid_zumoAI,SIG_MOVE_COMPLETE);
        }
    }      
  }
  
  /* Clear interupt flag */
  LEFT_A_ENCODER_PORT->PCR[LEFT_A_ENCODER_PIN] |= PORT_PCR_ISF_MASK;
  
}

/**  
  @brief ISR for Right_A encoder
*/
void PORTC_PORTD_IRQHandler(void){

  /* Check if encoder caused ISR */
  if (RIGHT_A_ENCODER_PORT->PCR[RIGHT_A_ENCODER_PIN] & PORT_PCR_ISF_MASK){
    if (EncFlags & RIGHT){
      RightTrackTurnDist -= TEETH_DISTANCE_MM;
      if ( RightTrackTurnDist <= 0 ) {
          EncFlags &= ~RIGHT;
          SetTrackSpeed(RIGHT, 0);
          if (EncFlags == 0){
            osSignalSet(tid_zumoAI,SIG_MOVE_COMPLETE);
          }
      }        
    }
    /* Clear interupt flag */
    RIGHT_A_ENCODER_PORT->PCR[RIGHT_A_ENCODER_PIN] |= PORT_PCR_ISF_MASK;
  }
  
  /* Check if user button caused interupt */
  if (PORTD->PCR[7] & PORT_PCR_ISF_MASK){
    osSignalSet(tid_comms,SIG_START_DEBOUNCE_TIMER);
    PORTD->PCR[7] |= PORT_PCR_ISF_MASK;
  }
}
