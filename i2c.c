/** \file i2c.h
	\brief Biblioteka zawierajaca definicje funkcji
*/
#include "i2c.h"

/** \fn void i2c0_set_tx_mode(void)
	\brief Funkcja ustawiajaca tryb wysylania
*/
void i2c0_set_tx_mode(void)
{
    I2C0->C1 |= I2C_C1_TX_MASK;

}

/** \fn void i2c0_set_rx_mode(void)
	\brief Funkcja ustawiajaca tryb odbierania
*/
void i2c0_set_rx_mode(void)
{
    I2C0->C1 &= ~I2C_C1_TX_MASK;
}

/** \fn void i2c0_set_slave_mode(void)
	\brief Funkcja ustawiajaca tryb slave
*/
void i2c0_set_slave_mode(void)
{
    I2C0->C1  &= ~I2C_C1_MST_MASK;
}

/** \fn void i2c0_set_master_mode(void)
	\brief Funkcja ustawiajaca tryb master
*/
void i2c0_set_master_mode(void)
{
    I2C0->C1  |=  I2C_C1_MST_MASK;
}

/** \fn void i2c0_give_nack(void)
	\brief Funkcja wysylajaca negarywne potwierdzenie
*/
void i2c0_give_nack(void)
{
    I2C0->C1 |= I2C_C1_TXAK_MASK;
}

/** \fn void i2c0_give_ack(void)
	\brief Funkcja wysylajaca pozytywne potwierdzenie
*/
void i2c0_give_ack(void)
{
    I2C0->C1 &= ~I2C_C1_TXAK_MASK;
}

/** \fn void i2c0_repeated_start(void)
	\brief Funkcja wywolujaca ponowny start transmisji
*/
void i2c0_repeated_start(void)
{
    I2C0->C1     |= 0x04;
}

/** \fn void i2c0_write_byte(uint8_t data)
	\brief Funkcja wpisujaca wyslane dane
	\param data Dane do wpisana
*/
void i2c0_write_byte(uint8_t data)
{
    I2C0->D = data;
}

/** \fn uint8_t i2c0_read_byte(void)
	\brief Funkcja zwracajaca odczytane dane
*/
uint8_t i2c0_read_byte(void)
{
    return I2C0->D;
}

/** \fn void i2c0_start(void)
	\brief Funkcja rozpoczynajaca wysylanie
*/
void i2c0_start(void)
{
    i2c0_set_master_mode();
    i2c0_set_tx_mode();
}

/** \fn void i2c0_stop(void)
	\brief Funkcja konczaca wysylanie, rozpoczynajaca odbieranie
*/
void i2c0_stop(void)
{
    i2c0_set_slave_mode();
    i2c0_set_rx_mode();
}

/** \fn void i2c0_wait()
	\brief Funkcja czekajaca 
*/
void i2c0_wait()
{
    // wait flag
    while((I2C0->S & I2C_S_IICIF_MASK)==0);
    // clear flag
    I2C0->S |= I2C_S_IICIF_MASK;
}

/** \fn uint16_t i2c0_get_ack()
	\brief Funkcja zwracajaca 1 po otrzymaniu potwierdzenia
*/
uint16_t i2c0_get_ack()
{
    if((I2C0->S & I2C_S_RXAK_MASK) == 0)
        return 1;
    else
        return 0;
}

/** \fn void hal_i2c0_deinit(void)
	\brief Funkcja wylaczajaca modul I2C
*/
void hal_i2c0_deinit(void)
{
    I2C0->C1 = 0x00;
    
    SIM->SCGC4 &= ~SIM_SCGC4_I2C0_MASK;
}

/** \fn void hal_i2c0_init(void)
	\brief Funkcja zalaczajaca modul I2C
*/
void hal_i2c0_init(void)  
{
  
   SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
   SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

    PORTE->PCR[24] = PORT_PCR_MUX(5);
    PORTE->PCR[25] = PORT_PCR_MUX(5);
	
	  I2C0->F  = 0x14; // baudrate
    I2C0->C1 = 0x80; // enable IIC
}

