/** \file mag.h
	\brief Biblioteka zawierajaca definicje funkcji
*/
#include "mag.h"

/** \var signed short mag_x, mag_y, mag_z
	\var Mresultx, Mresulty, Mresultz
	\brief Wartosci na konkretnych osiach
*/
signed short mag_x, mag_y, mag_z;
signed short Mresultx, Mresulty, Mresultz;

/** \var int Mtestx, Mtesty,  Mtestz
	\brief Wartosci testowe na osiach
*/
int Mtestx,Mtesty,Mtestz;

/** \def MAG3110_I2C_ADDRESS (0x0E<<1)
	\brief Adres magnetometru
*/
#define MAG3110_I2C_ADDRESS (0x0E<<1)


/** \fn void hal_dev_mag3110_init(void)
	\brief Inicjalizacja magnetometru
*/
void hal_dev_mag3110_init(void)
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

/** \fn uint8_t hal_dev_mag3110_read_reg(uint8_t addr)
	\brief Funkcja zwracajaca wartosc rejestru magnetometru
	\param addr Adres odczytywanego rejestru
*/
uint8_t hal_dev_mag3110_read_reg(uint8_t addr)
{
    uint8_t result;

    i2c0_start();
    i2c0_write_byte(MAG3110_I2C_ADDRESS | I2C_WRITE);
    
    i2c0_wait();
		i2c0_get_ack();

    i2c0_write_byte(addr);
    i2c0_wait();
		i2c0_get_ack();
	
    i2c0_repeated_start();
    i2c0_write_byte(MAG3110_I2C_ADDRESS | I2C_READ);
    i2c0_wait();
		i2c0_get_ack();
	
    i2c0_set_rx_mode();

    i2c0_give_nack();
    result = i2c0_read_byte();
    i2c0_wait();
		i2c0_get_ack();

    i2c0_stop();
    result = i2c0_read_byte();
    pause();
    return result;
}

/** \fn uint8_t hal_dev_mag3110_write_reg(uint8_t addr, uint8_t data)
	\brief Funkcja wpisujaca wartosc do rejestru magnetometru
	\param addr Adres rejestru do ktorego zostanie wpisana dana 
	\param data Dane wpisywane do rejestru
*/
void hal_dev_mag3110_write_reg(uint8_t addr, uint8_t data)
{
    i2c0_start();

    i2c0_write_byte( MAG3110_I2C_ADDRESS|I2C_WRITE);
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

/** \fn void mag_init(void)
	\brief Funkcja inicjalizujaca magnetometr
*/
void mag_init(void)
{
    hal_dev_mag3110_init();     
		hal_dev_mag3110_write_reg(MAG_CTRL_REG1,0x00); 
    hal_dev_mag3110_write_reg(MAG_CTRL_REG2,0x80); 
    hal_dev_mag3110_write_reg(MAG_CTRL_REG1,0x01); 
	
}

/** \fn void mag_read(void)
	\brief Funkcja zczytujaca wartosci osi XYZ
*/
void mag_read(void)
{
    if((hal_dev_mag3110_read_reg(0x00)&0xf) != 0)
    {
            
      mag_x   = hal_dev_mag3110_read_reg(0x01)<<8;
      mag_x  |= hal_dev_mag3110_read_reg(0x02);

      mag_y   = hal_dev_mag3110_read_reg(0x03)<<8;
      mag_y  |= hal_dev_mag3110_read_reg(0x04);

      mag_z   = hal_dev_mag3110_read_reg(0x05)<<8;
      mag_z  |= hal_dev_mag3110_read_reg(0x06);
  
      Mresultx   = hal_dev_mag3110_read_reg(0x01)<<8;
      Mresultx  |= hal_dev_mag3110_read_reg(0x02);
      
      Mresulty   = hal_dev_mag3110_read_reg(0x03)<<8;
      Mresulty  |= hal_dev_mag3110_read_reg(0x04);

      Mresultz   = hal_dev_mag3110_read_reg(0x05)<<8;
      Mresultz  |= hal_dev_mag3110_read_reg(0x06);
    }
}

/** \fn void mag_test(void)
	\brief Funkcja wykonujaca test magnetometru
*/
void mag_test(void)
{
 mag_read();
 Mtestx = Mresultx;
 Mtesty = Mresulty;
 Mtestz = Mresultz;
}

/** \fn int iMagReadX(void)
	\brief Funkcja zwracajaca wartosc osi X
*/	
int iMagReadX(void) {
   if((hal_dev_mag3110_read_reg(0x00)&0xf) != 0)
    {
      mag_x   = hal_dev_mag3110_read_reg(0x01)<<8;
      mag_x  |= hal_dev_mag3110_read_reg(0x02);
      
      Mresultx   = hal_dev_mag3110_read_reg(0x01)<<8;
      Mresultx  |= hal_dev_mag3110_read_reg(0x02);
    }
    return Mresultx;
}

/** \fn int iMagReadY(void)
	\brief Funkcja zwracajaca wartosc osi Y
*/	
int iMagReadY(void) {
     if((hal_dev_mag3110_read_reg(0x00)&0xf) != 0)
    {
      mag_y   = hal_dev_mag3110_read_reg(0x03)<<8;
      mag_y  |= hal_dev_mag3110_read_reg(0x04);

      Mresulty   = hal_dev_mag3110_read_reg(0x03)<<8;
      Mresulty  |= hal_dev_mag3110_read_reg(0x04);      
    }
    return Mresulty;
}

/** \fn int iMagReadZ(void)
	\brief Funkcja zwracajaca wartosc osi Z
*/	
int iMagReadZ(void) {
       if((hal_dev_mag3110_read_reg(0x00)&0xf) != 0)
    {
      mag_z   = hal_dev_mag3110_read_reg(0x05)<<8;
      mag_z  |= hal_dev_mag3110_read_reg(0x06);

      Mresultz   = hal_dev_mag3110_read_reg(0x05)<<8;
      Mresultz  |= hal_dev_mag3110_read_reg(0x06);
    }
    return Mresultz;
}
