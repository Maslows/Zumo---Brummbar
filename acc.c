/** \file acc.h
	\brief Biblioteka zawierajaca definicje funkcji
*/
#include "acc.h"

/** \var signed short accel_x, accel_y, accel_z
	\var Aresultx, Aresulty, Aresultz
	\brief Wartosci na konkretnych osiach
*/
signed short accel_x, accel_y, accel_z;
signed short Aresultx, Aresulty, Aresultz;

/** \var int Atestx, Atesty, Atestz
	\brief Wartosci testowe na osiach
*/
int Atestx,Atesty,Atestz;

/** \def MMA8451_I2C_ADDRESS (0x1d<<1)
	\brief Adres akcelerometru
*/
#define MMA8451_I2C_ADDRESS (0x1d<<1) 

/** \fn void hal_dev_mma8451_init(void)
	\brief Inicjalizacja akcelerometru
*/
void hal_dev_mma8451_init(void)
{
  hal_i2c0_init();
}

/** \fn static void  pause(void)
	\brief Funkcja opozniajaca zapis i odczyt
*/
static void pause(void)
{
    int n;
    for(n=0; n<40; n++)
   {}
}

/** \fn uint8_t hal_dev_mma8451_read_reg(uint8_t addr)
	\brief Funkcja zwracajaca wartosc rejestru akcelerometru
	\param addr Adres odczytywanego rejestru
*/
uint8_t hal_dev_mma8451_read_reg(uint8_t addr)
{
    uint8_t result;

    i2c0_start();
    i2c0_write_byte(MMA8451_I2C_ADDRESS | I2C_WRITE);
    
    i2c0_wait();
    i2c0_get_ack();

    i2c0_write_byte(addr);
    i2c0_wait();
    i2c0_get_ack();

    i2c0_repeated_start();
    i2c0_write_byte(MMA8451_I2C_ADDRESS | I2C_READ);
    i2c0_wait();
    i2c0_get_ack();

    i2c0_set_rx_mode();

    i2c0_give_nack();
    result = i2c0_read_byte();
    i2c0_wait();

    i2c0_stop();
    result = i2c0_read_byte();
    pause();
    return result;
}

/** \fn uint8_t hal_dev_mma8451_write_reg(uint8_t addr, uint8_t data)
	\brief Funkcja wpisujaca wartosc do rejestru akcelerometru
	\param addr Adres rejestru do ktorego zostanie wpisana dana 
	\param data Dane wpisywane do rejestru
*/
void hal_dev_mma8451_write_reg(uint8_t addr, uint8_t data)
{
    i2c0_start();

    i2c0_write_byte(MMA8451_I2C_ADDRESS|I2C_WRITE);
    i2c0_wait();
    i2c0_get_ack();

    i2c0_write_byte(addr);
    i2c0_wait();
    i2c0_get_ack();

    i2c0_write_byte(data);
    i2c0_wait();
    i2c0_get_ack();

    i2c0_stop();
    pause();
}

/** \fn void accel_init(void)
	\brief Funkcja inicjalizujaca akcelerometr
*/
void accel_init(void)
{
    unsigned char tmp;
    hal_dev_mma8451_init();      
    tmp = hal_dev_mma8451_read_reg(0x2a);
    hal_dev_mma8451_write_reg(0x2a,tmp|0x01); 
}

/** \fn void accel_read(void)
	\brief Funkcja zczytujaca wartosci osi XYZ
*/
void accel_read(void)
{
    if((hal_dev_mma8451_read_reg(0x00)&0xf) != 0)
    {
        accel_x   = hal_dev_mma8451_read_reg(0x01)<<8;
        accel_x  |= hal_dev_mma8451_read_reg(0x02);
        accel_x >>= 2;

        accel_y   = hal_dev_mma8451_read_reg(0x03)<<8;
        accel_y  |= hal_dev_mma8451_read_reg(0x04);
        accel_y >>= 2;

        accel_z   = hal_dev_mma8451_read_reg(0x05)<<8;
        accel_z  |= hal_dev_mma8451_read_reg(0x06);
        accel_z >>= 2;

        Aresultx   = hal_dev_mma8451_read_reg(0x01)<<8;
        Aresultx  |= hal_dev_mma8451_read_reg(0x02);
        Aresultx >>= 8;

        Aresulty   = hal_dev_mma8451_read_reg(0x03)<<8;
        Aresulty  |= hal_dev_mma8451_read_reg(0x04);
        Aresulty >>= 8;

        Aresultz   = hal_dev_mma8451_read_reg(0x05)<<8;
        Aresultz  |= hal_dev_mma8451_read_reg(0x06);
        Aresultz >>= 8;

    }
}

/** \fn void accel_test(void)
	\brief Funkcja wykonujaca test akcelerometru
*/
void accel_test(void)
{
 accel_read();
 Atestx = Aresultx;
 Atesty = Aresulty;
 Atestz = Aresultz;
  
}

/** \fn int iAcclReadX(void)
	\brief Funkcja zwracajaca wartosc osi X
*/	
int iAcclReadX(void) {
    if((hal_dev_mma8451_read_reg(0x00)&0xf) != 0)
    {
        accel_x   = hal_dev_mma8451_read_reg(0x01)<<8;
        accel_x  |= hal_dev_mma8451_read_reg(0x02);
        accel_x >>= 2;
      
        Aresultx   = hal_dev_mma8451_read_reg(0x01)<<8;
        Aresultx  |= hal_dev_mma8451_read_reg(0x02);
        Aresultx >>= 8;
    }
    return Aresultx;
}

/** \fn int iAcclReadY(void)
	\brief Funkcja zwracajaca wartosc osi Y
*/
int iAcclReadY(void) {
    if((hal_dev_mma8451_read_reg(0x00)&0xf) != 0)
    {
        accel_y   = hal_dev_mma8451_read_reg(0x03)<<8;
        accel_y  |= hal_dev_mma8451_read_reg(0x04);
        accel_y >>= 2;
        
        Aresulty   = hal_dev_mma8451_read_reg(0x03)<<8;
        Aresulty  |= hal_dev_mma8451_read_reg(0x04);
        Aresulty >>= 8;
    }
    return Aresulty;
}

/** \fn int iAcclReadZ(void)
	\brief Funkcja zwracajaca wartosc osi Z
*/
int iAcclReadZ(void) {
    if((hal_dev_mma8451_read_reg(0x00)&0xf) != 0)
    {
        accel_z   = hal_dev_mma8451_read_reg(0x05)<<8;
        accel_z  |= hal_dev_mma8451_read_reg(0x06);
        accel_z >>= 2;

        Aresultz   = hal_dev_mma8451_read_reg(0x05)<<8;
        Aresultz  |= hal_dev_mma8451_read_reg(0x06);
        Aresultz >>= 8;   
    }
    return Aresultz;
}
