#include "i2c.h"

void hal_dev_mma8451_init(void); 
uint8_t   hal_dev_mma8451_read_reg(uint8_t addr);
void hal_dev_mma8451_write_reg(uint8_t addr, uint8_t data);
uint16_t hal_dev_mma8451_test_reg_rw(void);
void hal_dev_mma8451_test_function(void);
extern void accel_init(void);
void accel_read(void);
void accel_test(void);
int iAcclReadX(void);
int iAcclReadY(void);
int iAcclReadZ(void);
