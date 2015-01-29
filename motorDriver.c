#include "motorDriver.h"
#include "compass.h"


//////////////////////////////////////// Global variables initialization ////////////////////////////////////////



// drive way and encoder turning functionality variables
uint32_t actualDistance = 0;
uint8_t flagLimitDistance = 0;
uint32_t limit = 0;
uint8_t speed = 50;

// compass turning functionality variables
uint8_t flagLimitAngle = 0;
uint32_t limitAngle = 0;
uint32_t offset = 0;

// distance counter functionality variables
uint32_t countersDistance = 0;
uint8_t flagCounting = 0;
uint8_t flagTurning = 0;

// turning speed
uint32_t turningSpeed = DEFAULT_TURNING_SPEED;

uint16_t azimuth = 0;

uint16_t tmp = 0;

//////////////////////////////////////// Functions ////////////////////////////////////////

void motorDriverInit(void)
{
	 // CLOCK_SETUP 1
	 //1 ... Multipurpose Clock Generator (MCG) in PLL Engaged External (PEE) mode
   //      Reference clock source for MCG module is an external crystal 8MHz
   //      Core clock = 48MHz, BusClock = 24MHz
	
	// connect the clock
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK
							| SIM_SCGC5_PORTC_MASK
							|	SIM_SCGC5_PORTD_MASK;
	
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	
	
	// choose signals for pins
	PORTA->PCR[6] |= PORT_PCR_MUX(3); // TPM0_CH3 - encoder
	PORTA->PCR[13] |= PORT_PCR_MUX(1); // PHASE - Left
	PORTC->PCR[9] |= PORT_PCR_MUX(1); // PHASE - Right
	PORTD->PCR[2] |= PORT_PCR_MUX(4); // TPM0_CH2 - PWM - Right
	PORTD->PCR[4] |= PORT_PCR_MUX(4); // TPM0_CH4 - PWM - Left
	PORTD->PCR[5] |= PORT_PCR_MUX(4); //TPM0_CH5 - encoder
	
	
	// set PHASE pins (GPIO) as output
	PTA->PDDR |= (1uL<<13);
	PTC->PDDR |= (1uL<<9);
	
	
	//////////////////////////////////////// PWM ////////////////////////////////////////
	
	//select clock for TMP0
	
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); // set 'MCGFLLCLK clock or MCGPLLCLK/2'
	
	SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; // set "MCGPLLCLK clock with fixed divide by two"
	
	// set "up-counting"
	TPM0->SC &= ~TPM_SC_CPWMS_MASK; // default set
	
	// divide by 1
	TPM0->SC &= ~TPM_SC_PS_MASK; // the same TPM_SC_PS(0)
	
	// clear counter
	TPM0->CNT = 0x00; 
	
	// set MOD for PWM period equal 4095 ( 12 bit)
	TPM0->MOD = V_MOD;
	
	////////////////////////////////////////////////////
	// Enable general TPM0 interrupt
	//TPM0->SC |= TPM_SC_TOIE_MASK;
	
	//////////////////////////////////////// ENGINE CHANNELS ////////////////////////////////////////
	
	
	//Right engine
	// set TPM0 channel 2 - "Edge-aligned PWM High-true pulses"
	TPM0->CONTROLS[2].CnSC |= TPM_CnSC_MSB_MASK	
												 |  TPM_CnSC_ELSB_MASK;
													
	// Default value for Right engine
	TPM0->CONTROLS[2].CnV = 0; // STOP
	
	
	//Left engine
	// set TPM0 channel 4 - "Edge-aligned PWM High-true pulses"
	TPM0->CONTROLS[4].CnSC |= TPM_CnSC_MSB_MASK	
												 |  TPM_CnSC_ELSB_MASK;
													
	// Default value for Left engine
	TPM0->CONTROLS[4].CnV = 0; // STOP
	
	
	
	//////////////////////////////////////// ENCODER CHANNELS ////////////////////////////////////////
	
	//Left Engine
	// set TPM0 channel 3 - "Input capture on rising or falling edge"
	TPM0->CONTROLS[3].CnSC |= TPM_CnSC_ELSA_MASK
												 |	TPM_CnSC_ELSB_MASK;
														
	
	//Right Engine
	// set TPM0 channel 5 - "Input capture on rising or falling edge"
	TPM0->CONTROLS[5].CnSC |= TPM_CnSC_ELSA_MASK
											   |	TPM_CnSC_ELSB_MASK;
																												
																												
	//////////////////////////////////////// ENCODER CHANNELS ////////////////////////////////////////
	
	
	NVIC_ClearPendingIRQ(TPM0_IRQn);				/* Clear NVIC any pending interrupts on TPM0 */
	NVIC_EnableIRQ(TPM0_IRQn);							/* Enable NVIC interrupts source for TPM0 module */
	
	NVIC_SetPriority (TPM0_IRQn, PRIORITY_INTERRUPT_DRIVE);	// priority interrupt
	
	// enable counter
	TPM0->SC |= TPM_SC_CMOD(1);
	
}


void TPM0_IRQHandler(void)
{
 /////////////////////////// counting distance////////////////////////////////////////////////////////////////
	if(!(flagTurning))
	{
		
		if(flagCounting && (TPM0->CONTROLS[DISTANCE_COUNTING_ENCODER].CnSC & TPM_CnSC_CHF_MASK))
		{
			countersDistance += ONE_TEETH_DISTANCE;
			TPM0->CONTROLS[DISTANCE_COUNTING_ENCODER].CnSC |= TPM_CnSC_CHF_MASK;
		}
	
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////drive and turn //////////////////////////////////////////////////////////////////
	if(flagLimitDistance && (TPM0->CONTROLS[3].CnSC & TPM_CnSC_CHF_MASK))
	{
	
		// counting distance handler
		actualDistance += ONE_TEETH_DISTANCE;
		
		// if distance reached: stop driving, turn off functionality and counting
		if( limit <= actualDistance)
		{
			driveStop();
			flagLimitDistance = 0;
			flagTurning = 0;
			TPM0->CONTROLS[DRIVE_AND_TURN_COUNTING_ENCODER].CnSC &= ~TPM_CnSC_CHIE_MASK;
		}
		
		TPM0->CONTROLS[DRIVE_AND_TURN_COUNTING_ENCODER].CnSC |= TPM_CnSC_CHF_MASK;
	}
	
	
	if(flagLimitAngle && (TPM0->CONTROLS[3].CnSC & TPM_CnSC_CHF_MASK))
	{
		driveStop();
		// get data needed to turn
		azimuth = eCompass();
		setTracksSpeed(  turningSpeed, turningSpeed);
	//	if (azimuth < offset) azimuth += 360;
		
		tmp = azimuth - offset;
		if( limitAngle <= (tmp) )
		{
			driveStop();
			flagLimitAngle = 0;
			flagTurning = 0;
			TPM0->CONTROLS[DRIVE_AND_TURN_COUNTING_ENCODER].CnSC &= ~TPM_CnSC_CHIE_MASK;
		}
		
		TPM0->CONTROLS[DRIVE_AND_TURN_COUNTING_ENCODER].CnSC |= TPM_CnSC_CHF_MASK;
	}
	
	
	

}


void setLeftTrackDir(int direction)
{
	
	if(direction == FORWARD)
	{
		PTA->PCOR |= LEFT_PHASE_PIN;
	}
	else if(direction == REVERSE)
	{
		PTA->PSOR |= LEFT_PHASE_PIN;
	}
	
}


void setLeftTrackSpeed(int speed)
{
	
	if( speed >= 0 )
	{
		if( speed < 100 )
		{
			TPM0->CONTROLS[4].CnV = V_MOD * speed/100;
		}
		else if ( speed == 100 )
		{
			TPM0->CONTROLS[4].CnV = V_MOD + 1;
		}
	}
	
}


void setRightTrackDir(int direction)
{
	
	if(direction == FORWARD)
	{
		PTC->PCOR |= RIGHT_PHASE_PIN;
	}
	else if(direction == REVERSE)
	{
		PTC->PSOR |= RIGHT_PHASE_PIN;
	}
	
}


void setRightTrackSpeed(int speed)
{
	
	if( speed >= 0 )
	{
		if( speed < 100 )
		{
			TPM0->CONTROLS[2].CnV = V_MOD * speed/100;
		}
		else if ( speed == 100 )
		{
			TPM0->CONTROLS[2].CnV = V_MOD + 1;
		}
	}
	
}


void setTracksDir(int leftDir, int rightDir)
{
	
	setLeftTrackDir( leftDir );
	setRightTrackDir( rightDir );
	
}


void setTracksSpeed(int leftSpeed, int rightSpeed)
{
	
	setLeftTrackSpeed( leftSpeed );
	setRightTrackSpeed( rightSpeed );
	
}


void driveForward(int speed)
{
	
	if( speed >= 0 && speed <= 100 )
	{
		setTracksDir( FORWARD, FORWARD);
		setTracksSpeed( speed, speed);
	}
	
}


void driveReverse(int speed)
{
	
	if( speed >= 0 && speed <= 100 )
	{
	setTracksDir( REVERSE, REVERSE);
	setTracksSpeed( speed, speed);
	}
	
}


void driveStop(void)
{
	
	setTracksSpeed( 0, 0);
	//setTracksDir( FORWARD, FORWARD);
	
}


void driveForwardDist(int speed, int distance)
{

	if(distance > 0)
	{
	
	// enable proper encoder to count forward distance
	TPM0->CONTROLS[DRIVE_AND_TURN_COUNTING_ENCODER].CnSC |= TPM_CnSC_CHIE_MASK;
	
	// clear actual distance counter and set proper limit
	actualDistance = 0;
	limit = distance * 100; // multiply by 100 to get in 0.1 mm
	
	//driveForward(speed);
	
	// activate mechanism
	flagLimitDistance = 1;
		
	}			 
				 

	
}


void driveReverseDist(int speed, int distance)
{
	
	if(distance > 0)
	{
	
	// enable proper encoder to count forward distance
	TPM0->CONTROLS[DRIVE_AND_TURN_COUNTING_ENCODER].CnSC |= TPM_CnSC_CHIE_MASK;
	
	// clear actual distance counter and set proper limit
	actualDistance = 0;
	limit = distance * 100; // multiply by 100 to get in 0.1 mm
		
	driveReverse(speed);
	
	// activate mechanism
	flagLimitDistance = 1;
	
		
	}
	
}


void driveAccelerate(int howMuch)
{
	
	// local variables initialization
	uint8_t tmp1 = 0 ;
	uint8_t tmp2 = 0 ;
	uint8_t tmp3 = 0 ;
	
	//Left ENGINE
	tmp1 = ((TPM0->CONTROLS[4].CnV * 100) / V_MOD ) ;
	
	//Right ENGINE
	tmp2 = ((TPM0->CONTROLS[2].CnV * 100 )/ V_MOD) ;
	
	for( tmp3 =0; tmp3 < howMuch ; tmp3++){
		
	      if((tmp1  + howMuch) >= 100){
		
		    TPM0->CONTROLS[4].CnV = V_MOD + 1;
		
	      }
				if((tmp2 + howMuch )>=100 ){
					
					TPM0->CONTROLS[2].CnV = V_MOD + 1;
				}
				if((tmp1 + howMuch) <= 0 ){
				
					TPM0->CONTROLS[4].CnV = 0;
				}
          if((tmp2 + howMuch) <= 0 ){
						
						TPM0->CONTROLS[2].CnV = 0;
					}
					
				if( (tmp1 + howMuch)>0 && (tmp1 + howMuch)<100){
		
	       TPM0->CONTROLS[4].CnV = ((tmp1 + tmp3)*V_MOD/100);
	
         }
				
				 if( (tmp2 + howMuch)>0 && (tmp2 + howMuch)<100){
		
	       TPM0->CONTROLS[2].CnV = ((tmp2 + tmp3)*V_MOD/100);
	
         }
	
				 wait4();
			 }
	
}


void turnLeftC(int degrees)
{
	
	limitAngle = degrees;
	offset = eCompass();
	
	// enable proper encoder to count forward distance
	TPM0->CONTROLS[DISTANCE_COUNTING_ENCODER].CnSC |= TPM_CnSC_CHIE_MASK;
	
	setLeftTrackDir( REVERSE );
	setRightTrackDir( FORWARD );
	setTracksSpeed(  turningSpeed, turningSpeed);
	
	// activate 
	flagLimitAngle = 1;
	flagTurning = 1;
	
	
}


void turnRightC(int degrees)
{
	limitAngle = degrees;
	offset = eCompass();	
	
	// enable proper encoder to count forward distance
	TPM0->CONTROLS[DISTANCE_COUNTING_ENCODER].CnSC |= TPM_CnSC_CHIE_MASK;
	
	setTracksDir( FORWARD , REVERSE );
	setTracksSpeed( turningSpeed, turningSpeed );
	
	// activate 
	flagLimitAngle = 1;
	flagTurning = 1;
	
}
void turnLeftE(int degrees)
{

	if(degrees > 0)
	{
	
		// enable proper encoder to count distance
		TPM0->CONTROLS[DRIVE_AND_TURN_COUNTING_ENCODER].CnSC |= TPM_CnSC_CHIE_MASK;
	
		// clear actual distance counter and set proper limit
		actualDistance = 0;
		limit = (degrees/5 - 1) * ONE_TEETH_DISTANCE ; // multiply by ONE_TEETH_DISTANCE to get in 0.1 mm
	
		setTracksDir(REVERSE,FORWARD);
		setTracksSpeed( turningSpeed, turningSpeed);
	
		// activate
		flagTurning = 1;
		flagLimitDistance = 1;
	
	}
	
}


void turnRightE(int degrees)
{
	if(degrees > 0)
	{
	
		// enable proper encoder to count distance
		TPM0->CONTROLS[DRIVE_AND_TURN_COUNTING_ENCODER].CnSC |= TPM_CnSC_CHIE_MASK;
	
		// clear actual distance counter and set proper limit
		actualDistance = 0;
		limit = (degrees/5-1) * ONE_TEETH_DISTANCE ; // multiply by ONE_TEETH_DISTANCE to get in 0.1 mm
	
		setTracksDir(FORWARD, REVERSE);
		setTracksSpeed( turningSpeed, turningSpeed);
	
		// activate
		flagTurning = 1;
		flagLimitDistance = 1;
	
	}

}
	

void countDistanceEnable(void)
{
	
	flagCounting = 1;
	
	// enable interrupts for encoder which handles functionality
	TPM0->CONTROLS[DISTANCE_COUNTING_ENCODER].CnSC |= TPM_CnSC_CHIE_MASK;
	
	// clear distance variable
	countersDistance = 0;

}


void countDistanceDisable(void)
{
	
	flagCounting = 0;
	
	// disable interrupts for both left and right encoder
	TPM0->CONTROLS[DISTANCE_COUNTING_ENCODER].CnSC &= ~TPM_CnSC_CHIE_MASK;
	
}


uint32_t getDistance(void)
{
	
	return countersDistance/100; // divide by 100 to get in cm
	
}


void wait4(void)
{
	
	int i = 0;
	for( i = 0 ; i < 799990 ; i++){}
	
}

