//AD5254 drivers
#ifndef NLIB_DEV_AD5254_H
#define NLIB_DEV_AD5254_H

#include <cytypes.h>
#include "nlib_per_i2c.h"

#define AD5254_CTLADDR_LL   0b01011000
#define AD5254_CTLADDR_LH   0b01011010
#define AD5254_CTLADDR_HL   0b01011100
#define AD5254_CTLADDR_HH   0b01011110
    
#define AD5254_CH_MIN   0
#define AD5254_CH_MAX   3

#define AD5254_VAL_MIN  0
#define AD5254_VAL_MAX  0xFF
    
#define AD5254_MODE_EE      (1 << 5)
#define AD5254_MODE_RDAC    (0 << 5)
#define AD5254_SEL_CMD      (1 << 7)
#define AD5254_SEL_REG      (0 << 7)
    
#define AD5245_CMD_NOP      0
    
typedef struct ad5254Obj * Ad5254Obj;
    
struct ad5254Obj{
    unsigned char ctlAddr;      //i2c device address
  	I2cObj i2c;                 //i2c bus
    void (*wp_b_Write)(uint8 val);
    int (*va_print)(const char *format, ...);  //link this to either vprintf or vsprintf
};

//constructor
Ad5254Obj ad5254_New();

//initialize ad5254
int ad5254_Init(Ad5254Obj self, I2cObj i2c, unsigned char ctlAddr, void (*wp_b_Write)(uint8 val), int (*va_print)(const char *format, ...));

//write RDAC
void ad5254_WriteRdac(Ad5254Obj self, int ch, unsigned char val);

//ucmd wrapper writing to RDAC
void ad5254_ucmdWriteRdac(Ad5254Obj self, const char * str);

#endif

/* [] END OF FILE */
