#include "servo.h"
#include "LedLine.h"

/**
 @brief PIT ISR 
 This function handles PIT interupts
 - Channel 1: Used for LedLine.
 - Channel 2: Used to keep track of servo movement time.
*/
void PIT_IRQHandler(void){
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
		LedPITHandler();
	}
	if (PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) {
		ServoPITHandler();
	}
}
