#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "MKL46Z4.h"                    // Device header

// User settings
/**
	@brief	Defines which UART will be used for communication.	
	@details	You can select any UART module (0-2).
						Default pins:
						<ul>
							<li> UART0 - PTA15(RX)   PTA14(TX)
							<li> UART1 - PTE1 (RX)   PTE0(TX)
							<li> UART2 - PTE17(RX)   PTE16(TX)
						</ul>
	@warning	When KL46Z is connected to ZUMO, UART 1 is useless, 
						because on PTE0 and PTE1 is I2C bus.
*/
#define UART_MODULE 0

/**
	@brief Defines speed of transmission.
	@details	Default communication baud rate in HC-06 is 9600.
						Others are available by changing settings in AT-mode of module.
*/
#define BAUD_RATE 9600

/**
	@brief Defines how important UART interrupts will be. It can be 1, 2 or 3.
*/
#define UART_IRQ_PRIORITY 3

// Preprocessor macros
/**
	@brief Preprocessor macro for automatic change of UART module
*/
#if UART_MODULE == 0
  #define UART UART0
#elif UART_MODULE==1
  #define UART UART1
#else
  #define UART UART2
#endif



// Main user functions
/**
	@brief Initialize the UART module and prepare both buffers
  @param baud_rate is speed of transmission. You can also use macro ::BAUD_RATE.
  @warning Be sure that CLOCK_SETUP in "system_MKL46Z4.c" equals 1.
*/
void bt_init( uint32_t baud_rate );


#endif
