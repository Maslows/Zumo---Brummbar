#include "motors.h"

/**
  @brief set MOD for PWM period equal 50us - 20kHz  (clock 48MHz)  */
#define PWN_PERIOD_VALUE 2400
#define LEFT_PWM (4)
#define RIGHT_PWM (2)

volatile uint16_t TurnFlag = 0;
volatile uint32_t left_count = 0;
volatile uint32_t right_count = 0;
volatile int32_t LeftTrackTurnDist = 0;
volatile int32_t RightTrackTurnDist = 0;

uint8_t speed = 50;

void SetTrackDirection(track_t track, direction_t direction) {
  if (direction == FORWARD){
    switch(track){
      case LEFT:
        LEFT_PHASE_FPORT->PCOR |= LEFT_PHASE_PIN_MASK;
        break;
      case RIGHT:
        RIGHT_PHASE_FPORT->PCOR |= RIGHT_PHASE_PIN_MASK;
        break;
      case BOTH:
        LEFT_PHASE_FPORT->PCOR |= LEFT_PHASE_PIN_MASK;
        RIGHT_PHASE_FPORT->PCOR |= RIGHT_PHASE_PIN_MASK;
        break;
    }
    
  } else if (direction == REVERSE){
    switch(track){
      case LEFT:
        LEFT_PHASE_FPORT->PSOR |= LEFT_PHASE_PIN_MASK;
        break;
      case RIGHT:
        RIGHT_PHASE_FPORT->PSOR |= RIGHT_PHASE_PIN_MASK;
        break;
      case BOTH:
        LEFT_PHASE_FPORT->PSOR |= LEFT_PHASE_PIN_MASK;
        RIGHT_PHASE_FPORT->PSOR |= RIGHT_PHASE_PIN_MASK;
        break;
    } 
  }
}

void SetTrackSpeed(track_t track, uint8_t speed) {
  if (speed <= 100){
    uint16_t new_pwm_value = (PWN_PERIOD_VALUE*speed)/100;
    switch(track){
      case LEFT:
        TPM0->CONTROLS[LEFT_PWM].CnV = new_pwm_value;
        break;
      case RIGHT:
        TPM0->CONTROLS[RIGHT_PWM].CnV = new_pwm_value;
        break;
      case BOTH:
        TPM0->CONTROLS[LEFT_PWM].CnV = new_pwm_value;
        TPM0->CONTROLS[RIGHT_PWM].CnV = new_pwm_value;
        speed = 100;
        break;
    } 
  }
}


/**  
  @brief ISR for Left_A encoder
*/
void PORTA_IRQHandler(void){
  left_count++;
  if (TurnFlag & LEFT){
    LeftTrackTurnDist -= TEETH_DISTANCE_MM;
    if ( LeftTrackTurnDist <= 0 ) {
        TurnFlag &= ~LEFT;
        SetTrackSpeed(LEFT, 0);
    }
        
  }
  
  /* Clear interupt flag */
  LEFT_A_ENCODER_PORT->PCR[LEFT_A_ENCODER_PIN] |= PORT_PCR_ISF_MASK;
  
}

/**  
  @brief ISR for Right_A encoder
*/
void PORTC_PORTD_IRQHandler(void){
   right_count++;
  if (TurnFlag & RIGHT){
    RightTrackTurnDist -= TEETH_DISTANCE_MM;
    if ( LeftTrackTurnDist <= 0 ) {
        TurnFlag &= ~RIGHT;
        SetTrackSpeed(RIGHT, 0);
    }        
  }
  
  /* Clear interupt flag */
  RIGHT_A_ENCODER_PORT->PCR[RIGHT_A_ENCODER_PIN] |= PORT_PCR_ISF_MASK;
  
}

void driveForward(uint8_t speed){
  SetTrackDirection(BOTH,FORWARD);
  SetTrackSpeed(BOTH,speed);
}

void driveReverse(uint8_t speed){
  SetTrackDirection(BOTH,REVERSE);
  SetTrackSpeed(BOTH,speed);
}

void driveStop(void){
  TurnFlag = 0;
  SetTrackSpeed(BOTH,0);
}

void turnLeft(void){
  SetTrackDirection(LEFT,REVERSE);
  SetTrackDirection(RIGHT,FORWARD);
  SetTrackSpeed(BOTH,DEFAULT_TURNING_SPEED);
}
void turnRight(void){
  SetTrackDirection(LEFT,FORWARD);
  SetTrackDirection(RIGHT,REVERSE);
  SetTrackSpeed(BOTH,DEFAULT_TURNING_SPEED);
}

void rotate(int angle){
  
  if (angle <= -3 || angle >= 3){
    int32_t distance = (int32_t)((ZUMO_TURN_PERIMITER_MM * angle)/360.0);
    
    __disable_irq();
    
    if (angle > 0 ){
      SetTrackDirection(LEFT, FORWARD);
      SetTrackDirection(RIGHT, REVERSE);
    } else {
      SetTrackDirection(LEFT, REVERSE);
      SetTrackDirection(RIGHT, FORWARD);
      distance = -distance;
    }
    
    TurnFlag = BOTH;
    LeftTrackTurnDist = distance;
    RightTrackTurnDist = distance;
    SetTrackSpeed(BOTH,DEFAULT_TURNING_SPEED); 
    
    __enable_irq();
    
    
  }
};

void motors_init(void){
  
  /********************************** SETUP TPM0 *****************************/
  /* Select clocks */
  SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
  SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK
             |  SIM_SCGC5_PORTC_MASK
             |  SIM_SCGC5_PORTD_MASK;
  
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); 									/*set 'MCGFLLCLK clock or MCGPLLCLK/2' */
	SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; 						/*set "MCGPLLCLK clock with  fixed divide by two" - 48MHz*/
  
  /* Set prescaler to 1 */
	TPM0->SC &= ~TPM_SC_PS_MASK; // the same TPM_SC_PS(0)
	
	/* clear counter */
	TPM0->CNT = 0x00; 
	
  /* set counter period */
	TPM0->MOD = PWN_PERIOD_VALUE - 1;
  
  /**** Right engine - PWM *****/
	/* set TPM0 channel 2 - "Edge-aligned PWM High-true pulses"   */
	TPM0->CONTROLS[RIGHT_PWM].CnSC |= TPM_CnSC_MSB_MASK	
                                 |  TPM_CnSC_ELSB_MASK; 
	TPM0->CONTROLS[RIGHT_PWM].CnV = 0;
  
  /* Change port mux */
  RIGHT_PWM_PORT->PCR[RIGHT_PWM_PIN] |= PORT_PCR_MUX(4);
	
  
	/***** Left engine - PWM ******/
	/* set TPM0 channel 4 - "Edge-aligned PWM High-true pulses" */
	TPM0->CONTROLS[LEFT_PWM].CnSC |= TPM_CnSC_MSB_MASK	
                                |  TPM_CnSC_ELSB_MASK;												
	TPM0->CONTROLS[LEFT_PWM].CnV = 0;
  /* Change port mux */
  LEFT_PWM_PORT->PCR[LEFT_PWM_PIN] |= PORT_PCR_MUX(4);
	
  /* set PHASE pins (GPIO) as outputs */
  LEFT_PHASE_PORT->PCR[LEFT_PHASE_PIN] |= PORT_PCR_MUX(1);
  RIGHT_PHASE_PORT->PCR[RIGHT_PHASE_PIN] |= PORT_PCR_MUX(1);
	LEFT_PHASE_FPORT->PDDR |= (1 << LEFT_PHASE_PIN);
	RIGHT_PHASE_FPORT->PDDR |= (1 << RIGHT_PHASE_PIN);
   
  /* Enable TPM0 */
	TPM0->SC |= TPM_SC_CMOD(1);

  /******************************** SETUP ENCODERS ***************************/
  
  
  LEFT_A_ENCODER_FPORT->PDDR &= ~LEFT_A_ENCODER_MASK;                  /* Set as output */
  LEFT_A_ENCODER_PORT->PCR[LEFT_A_ENCODER_PIN] |= PORT_PCR_MUX(1)     /* Set MUX to GPIO */
                                               |  PORT_PCR_IRQC(11)   /* Enable Interupt on either edge */
                                               |  PORT_PCR_ISF_MASK;  /* Clear interupt flag */
  
  RIGHT_A_ENCODER_FPORT->PDDR &= ~RIGHT_A_ENCODER_MASK;                 /* Set as output */
  RIGHT_A_ENCODER_PORT->PCR[RIGHT_A_ENCODER_PIN] |= PORT_PCR_MUX(1)    /* Set MUX to GPIO */
                                                 |  PORT_PCR_IRQC(11)  /* Enable Interupt on either edge */ 
                                                 |  PORT_PCR_ISF_MASK; /* Clear interupt flag */
 
 
  NVIC_ClearPendingIRQ(PORTA_IRQn);				/* Clear NVIC any pending interrupts on PORTA */
	NVIC_EnableIRQ(PORTA_IRQn);							/* Enable NVIC interrupts source for encoder pin */
	NVIC_SetPriority (PORTA_IRQn, MOTORS_INTERUPT_PRIORITY);	// priority interrupt
	                          
  NVIC_ClearPendingIRQ(PORTC_PORTD_IRQn);				/* Clear NVIC any pending interrupts on PORTC_D */
	NVIC_EnableIRQ(PORTC_PORTD_IRQn);							/* Enable NVIC interrupts source for encoder pin */
	NVIC_SetPriority (PORTC_PORTD_IRQn, MOTORS_INTERUPT_PRIORITY);	// priority interrupt
	                                                    
}
