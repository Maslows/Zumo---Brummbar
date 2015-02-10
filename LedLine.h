#ifndef LEDLINE_H
#define LEDLINE_H

#include "MKL46Z4.h"                    // Device header

/**
  @brief Define Led Pin Numbers
*/

#define U1_PIN (4)
#define U1_PIN_MASK (1 << U1_PIN)
#define U1_PORT (PORTA)
#define U1_FPORT (FPTA)    

#define U2_PIN (3)
#define U2_PIN_MASK (1 << U2_PIN)
#define U2_PORT (PORTB)
#define U2_FPORT (FPTB)

#define U3_PIN (6)
#define U3_PIN_MASK (1 << U3_PIN)
#define U3_PORT (PORTD)
#define U3_FPORT (FPTD)

#define U4_PIN (0)
#define U4_PIN_MASK (1 << U4_PIN)
#define U4_PORT (PORTB)
#define U4_FPORT (FPTB)

#define U5_PIN (2)
#define U5_PIN_MASK (1 << U5_PIN)
#define U5_PORT (PORTB)
#define U5_FPORT (FPTB)

#define U6_PIN (5)
#define U6_PIN_MASK (1 << U6_PIN)
#define U6_PORT (PORTA)
#define U6_FPORT (FPTA)

extern uint32_t PIT_LOAD_VALUE; // 1ms
extern uint32_t LED_CHECK_INTERVAL; // 100ms

/* Functions */
void LedPITHandler(void);
void LedLine_Init(void);

#endif 
