#include "LedLine.h"
#include "osObjects.h"                         // RTOS object definitions

uint32_t LED_CHECK_INTERVAL = 2400000; // 100ms
uint32_t LED_DISCHARGE_TIME = 24000; // 1ms

void LedLine_Init(void){
  
  /* Enable clock gating for I/O ports and PITs */
  SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK
             |  SIM_SCGC5_PORTB_MASK
             |  SIM_SCGC5_PORTD_MASK;
  
  SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;																	/* Enable PIT Clock Gating */  
  
  /* Set MUX as GPIO */
  U1_PORT->PCR[U1_PIN] = PORT_PCR_MUX(1);
  U2_PORT->PCR[U2_PIN] = PORT_PCR_MUX(1);
  U3_PORT->PCR[U3_PIN] = PORT_PCR_MUX(1);
  U4_PORT->PCR[U4_PIN] = PORT_PCR_MUX(1);
  U5_PORT->PCR[U5_PIN] = PORT_PCR_MUX(1);
  U6_PORT->PCR[U6_PIN] = PORT_PCR_MUX(1);
  
  /* Set Ports as output */
  U1_FPORT->PDDR |= U1_PIN_MASK;
  U2_FPORT->PDDR |= U2_PIN_MASK;
  U3_FPORT->PDDR |= U3_PIN_MASK;
  U5_FPORT->PDDR |= U4_PIN_MASK;
  U5_FPORT->PDDR |= U5_PIN_MASK;
  U6_FPORT->PDDR |= U6_PIN_MASK;
  
  /* Set Ports to High state */
  U1_FPORT->PSOR |= U1_PIN_MASK;
  U2_FPORT->PSOR |= U2_PIN_MASK;
  U3_FPORT->PSOR |= U3_PIN_MASK;
  U4_FPORT->PSOR |= U4_PIN_MASK;
  U5_FPORT->PSOR |= U5_PIN_MASK; 
  U6_FPORT->PSOR |= U6_PIN_MASK;  
  
  PIT->CHANNEL[0].LDVAL = LED_CHECK_INTERVAL;												/* set PIT Load Value */
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;  										/* Enable interrupts in PIT module on channel 1 */
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;  										/* Enable Timer on given channel on channel 1 */
	
	/* Configure NVIC for PIT interupt */
	NVIC_ClearPendingIRQ(PIT_IRQn);															/*  Clear NVIC pending PIT interupts */
	NVIC_EnableIRQ(PIT_IRQn);																		/*  Enable NVIC interrupts source for PIT */
	NVIC_SetPriority(PIT_IRQn, 1);				                      /*  Set PIT interrupt priority */

  /* Enable PIT */
	PIT->MCR = 0x00;
}
 /**
  @brief LedLine PIT Interupt Handler 
  This function works in two states: Tick and Tock.
  In Tick, it reads values of ports and parses the result. 
  Then, it sets ports to output and sets timer to check interval.
  In Tock, it sets ports as input and sets timer to discharge time.
*/
void LedPITHandler(void){
  static enum { Tick, Tock } TickTock = Tick;
  uint32_t U1U6Values, U2U4U5Values, U3Values, Result;
  PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;      						/* Disable timer */
  switch (TickTock){
    case Tick:
      /* Reading values of ports */
      U1U6Values = U1_FPORT->PDIR;
      U2U4U5Values = U2_FPORT->PDIR;
      U3Values = U3_FPORT->PDIR;
      Result = 0;
      /* Parsing the result */
      if (U1U6Values & U1_PIN_MASK){
        Result += (1 << 5);
      }
      if (U2U4U5Values & U2_PIN_MASK){
        Result += (1 << 4);
      }
      if (U3Values & U3_PIN_MASK){
        Result += (1 << 3);
      }
      if (U2U4U5Values & U4_PIN_MASK){
        Result += (1 << 2);
      }
      if (U2U4U5Values & U5_PIN_MASK){
        Result += (1 << 1);
      }
      if (U1U6Values & U6_PIN_MASK){
        Result += (1 << 0);
      }
      /* Set Ports as output to charge capacitor */      
      U1_FPORT->PDDR |= U1_PIN_MASK | U6_PIN_MASK;
      U2_FPORT->PDDR |= U2_PIN_MASK | U4_PIN_MASK | U5_PIN_MASK;
      U3_FPORT->PDDR |= U3_PIN_MASK;
      /* Send meassage with result */
      SendMessage("LedLine:%u\n",Result);
      /* Change state to Tock */
      TickTock = Tock;
      /* Set Timers Check Interval */
      PIT->CHANNEL[0].LDVAL = LED_CHECK_INTERVAL;
      break;
    case Tock:
      /* Set Ports as input */
      U1_FPORT->PDDR &= ~(U1_PIN_MASK | U6_PIN_MASK);
      U2_FPORT->PDDR &= ~(U2_PIN_MASK | U4_PIN_MASK | U5_PIN_MASK);
      U3_FPORT->PDDR &= ~ U3_PIN_MASK;
      /* Change state to Tick */
      TickTock = Tick;
      /* Set Discharge Time */
      PIT->CHANNEL[0].LDVAL = LED_DISCHARGE_TIME;	   
      break;
  }
  PIT->CHANNEL[0].TFLG  |= PIT_TFLG_TIF_MASK; 									/* Clear Interupt Flag */
  PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;      						/* Disable timer */
}
  
