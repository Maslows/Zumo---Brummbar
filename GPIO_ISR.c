#include "osObjects.h"                      // RTOS object definitions
#include "motors.h"

/**  
  @brief ISR for Left_A encoder
*/
void PORTA_IRQHandler(void){
  /* Left A Encoder */
  if (LEFT_A_ENCODER_PORT->PCR[LEFT_A_ENCODER_PIN] & PORT_PCR_ISF_MASK){
    left_count++;
    if (EncFlags & LEFT){
      LeftTrackTurnDist--;
      if ( LeftTrackTurnDist <= 0 ) {
          EncFlags &= ~LEFT;
          SetTrackSpeed(LEFT, 0);
          if (EncFlags == 0){
            osSignalSet(tid_zumoAI,SIG_MOVE_COMPLETE);
          }
      }      
    }
    LEFT_A_ENCODER_PORT->PCR[LEFT_A_ENCODER_PIN] |= PORT_PCR_ISF_MASK;
  }
  
  /* Left B Encoder */
  if (LEFT_B_ENCODER_PORT->PCR[LEFT_B_ENCODER_PIN] & PORT_PCR_ISF_MASK){
    left_count++;
    if (EncFlags & LEFT){
      LeftTrackTurnDist--;
      if ( LeftTrackTurnDist <= 0 ) {
          EncFlags &= ~LEFT;
          SetTrackSpeed(LEFT, 0);
          if (EncFlags == 0){
            osSignalSet(tid_zumoAI,SIG_MOVE_COMPLETE);
          }
      }      
    }
    LEFT_B_ENCODER_PORT->PCR[LEFT_B_ENCODER_PIN] |= PORT_PCR_ISF_MASK;
  }
  
  /* Right B Encoder */
  if (RIGHT_B_ENCODER_PORT->PCR[RIGHT_B_ENCODER_PIN] & PORT_PCR_ISF_MASK){
    right_count++;
    if (EncFlags & RIGHT){
      RightTrackTurnDist--;
      if ( RightTrackTurnDist <= 0 ) {
          EncFlags &= ~RIGHT;
          SetTrackSpeed(RIGHT, 0);
          if (EncFlags == 0){
            osSignalSet(tid_zumoAI,SIG_MOVE_COMPLETE);
          }
      }      
    }
    RIGHT_B_ENCODER_PORT->PCR[RIGHT_B_ENCODER_PIN] |= PORT_PCR_ISF_MASK;
  }
   
}

/**  
  @brief ISR for Right_A encoder
*/
void PORTC_PORTD_IRQHandler(void){

  /* Check if encoder caused ISR */
  if (RIGHT_A_ENCODER_PORT->PCR[RIGHT_A_ENCODER_PIN] & PORT_PCR_ISF_MASK){
    right_count++;
    if (EncFlags & RIGHT){
      RightTrackTurnDist--;
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
