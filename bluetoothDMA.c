#include "bluetoothDMA.h"
#include "osObjects.h"
#include <string.h>

osMailQId qid_MessageTx;
osMailQId qid_MessageRx;

void bt_init( uint32_t baud_rate ){

	uint32_t divisor;
		
#if UART_MODULE==0
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK; 	// PTA14(TX) PTA15(RX) on mux(3) will be used.
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);
	SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	PORTA->PCR[14] |= PORT_PCR_MUX(3);
	PORTA->PCR[15] |= PORT_PCR_MUX(3);
	
#elif UART_MODULE==1
	SIM->SCGC4 |= SIM_SCGC4_UART1_MASK;		// 24MHz bus clk
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; 	// PTE0(TX) PTE1(RX) on mux(3) will be used.	
	PORTE->PCR[0] |= PORT_PCR_MUX(3);
	PORTE->PCR[1] |= PORT_PCR_MUX(3);
	
#elif UART_MODULE==2
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;		// 24MHz bus clk
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; 	// PTE16(TX) PTE17(RX) on mux(3) will be used.	
	PORTE->PCR[16] |= PORT_PCR_MUX(3);
	PORTE->PCR[17] |= PORT_PCR_MUX(3);
#endif

	// UART module disable
	UART->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);

	// Setting prescaler value
#if UART_MODULE==0
	divisor = (48000000/baud_rate)/16;
#else
	divisor = (24000000/baud_rate)/16;
#endif

	// Cearing prescaler register
	UART->BDH &= ~UART_BDH_SBR_MASK;
	UART->BDL &= ~UART_BDL_SBR_MASK;	
	
	// Writting prescaler value to right register
	UART->BDH |= UART_BDH_SBR(divisor>>8);
	UART->BDL |= UART_BDL_SBR(divisor);
	
	// One stop bit
	UART->BDH &= ~UART_BDH_SBNS_MASK;
	// No parity
	UART->C1 &= ~UART_C1_PE_MASK;
	// 8-bit mode
	UART->C1 &= ~UART_C1_M_MASK;
  
  /* Enable DMA transfer in UART */
  #if UART_MODULE == 0 
  UART->C5 |= UART0_C5_TDMAE_MASK;
  #else
  UART->C4 |= UART_C4_TDMAE_MASK;
  #endif
  UART->C2 |= UART_C2_TIE_MASK;
	
  
  /************ DMA CONFIG *********************/
  SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;    // Enable DMA clock gating
  SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK; // Enable DMAMUX clock gating
  

  
  /* Tx - DMA1 */
  DMAMUX0->CHCFG[1] = 0x00; // Disable DMA MUX
  DMAMUX0->CHCFG[1] = (3+(UART_MODULE*2)); // UART_TX  UART0 - 3 UART1 - 5 UART2 - 7;
  
  DMA0[1].DMA->DSR_BCR |= DMA_DSR_BCR_DONE_MASK;                  // Clear Done and other flags
  DMA0[1].DMA->DAR = UART->D;                                     // Set Destination Addres to UART->D
  
  DMA0[1].DMA->DCR &= ~(DMA_DCR_SSIZE_MASK | DMA_DCR_DSIZE_MASK); // Clear Source and Destination size.
  DMA0[1].DMA->DCR |= DMA_DCR_SSIZE(1)                            // Set Source size 8bit
                   |  DMA_DCR_DSIZE(1)                            // Set Destination size 8bit
                   |  DMA_DCR_SINC_MASK                           // Enable source increment
                   |  DMA_DCR_CS_MASK                             // Set clock steal - one transfer per request
                   |  DMA_DCR_ERQ_MASK                            // Enable external request
                   |  DMA_DCR_D_REQ_MASK                          // Disable external request when transfer is complete
                   |  DMA_DCR_EINT_MASK;                          // Enable interrupt when transfer is complete

  /* Rest of DMA configuration is set in ISR when new message is ready to be send */

	// UART module enable
	UART->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK);
}

//DMA channel 0 transfer complete/error interrupt - UART RX
void DMA0_IRQHandler(void);

//DMA channel 1 transfer complete/error interrupt - UART TX
void DMA1_IRQHandler(void){
  static Message_t * Message = 0;
  static osEvent MailEvt;
    
  DMAMUX0->CHCFG[1] &= ~DMAMUX_CHCFG_ENBL_MASK; // Disable DMA MUX
  
  /* Free previous message if any */
  if (Message != 0){
    osMailFree(qid_MessageTX,Message);       // free memory
    Message = 0;
  }
  
  MailEvt = osMailGet(qid_MessageTX,0);             // get mail 
  if (MailEvt.status == osEventMail){               // check if there is any mail
      Message = MailEvt.value.p;                    // point to the mail
      DMA0[1].DMA->SAR = (uint32_t)&(*Message).msg; // Set source address to mail body.
      DMA0[1].DMA->DSR_BCR = DMA_DSR_BCR_BCR(strlen(Message->msg)); // Set Byte Count Register to length of Msg body
      DMA0[1].DMA->DCR |= DMA_DCR_ERQ_MASK;          // Enable external request
  };
  
  DMA0[1].DMA->DSR_BCR |= DMA_DSR_BCR_DONE_MASK;  // Clear Done and other flags
  DMAMUX0->CHCFG[1] |= DMAMUX_CHCFG_ENBL_MASK;    // Enable DMA MUX
  
};