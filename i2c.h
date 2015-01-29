#ifndef __I2C_H__
#define __I2C_H__

#include "MKL46Z4.h"

/* ----------------------------------------------------------------------------
   -- I2C
   ---------------------------------------------------------------------------- */


void i2c0_set_tx_mode(void);
void i2c0_set_rx_mode(void);
void i2c0_set_slave_mode(void);
void i2c0_set_master_mode(void);
void i2c0_give_nack(void);
void i2c0_give_ack(void);
void i2c0_repeated_start(void);
void i2c0_write_byte(uint8_t data);
uint8_t i2c0_read_byte(void);
void i2c0_start(void);
void i2c0_stop(void);
void i2c0_wait(void);
uint16_t i2c0_get_ack(void);
void hal_i2c0_deinit(void);
void hal_i2c0_init(void); 
#define I2C_READ  1
#define I2C_WRITE 0

#endif //__I2C_H__
