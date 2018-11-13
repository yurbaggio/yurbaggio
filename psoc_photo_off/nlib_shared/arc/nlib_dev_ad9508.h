//AD9508 drivers
#ifndef NLIB_DEV_AD9508_H
#define NLIB_DEV_AD9508_H

#include <cytypes.h>
#include "nlib_per_i2c.h"

#define AD9508_CTLADDR_LL   0b11011000
#define AD9508_CTLADDR_LH   0b11011010
#define AD9508_CTLADDR_HL   0b11011100
#define AD9508_CTLADDR_HH   0b11011110
    
#define AD9508_CH_MIN   0
#define AD9508_CH_MAX   3
    
#define AD9508_ADDR_I2CCTRL     0x00
#define AD9508_I2CCTRL_RST      0x24

#define AD9508_ADDR_SLEEP       0x13
#define AD9508_SLEEP_ENABLE     0x10
#define AD9508_SLEEP_DISABLE    0x00
    
//start addresses of each of the clocks
#define AD9508_ADDR_OUT0        0x15
#define AD9508_ADDR_OUT1        0x1b
#define AD9508_ADDR_OUT2        0x21
#define AD9508_ADDR_OUT3        0x27

//offsets from the start addresses of each clock
#define AD9508_ADDROS_OUTDIVLO  0x00
#define AD9508_ADDROS_OUTDIVHI  0x01
#define AD9508_ADDROS_OUTPHALO  0x02
#define AD9508_ADDROS_OUTPHAHI  0x03
#define AD9508_ADDROS_OUTDRVR   0x04
#define AD9508_ADDROS_OUTCMOS   0x05

#define AD9508_OUTDIV_MIN   1       //offset by one
#define AD9508_OUTDIV_MAX   1024
    
#define AD9508_OUTPHA_MIN   0       //each step is 1/2 of input clock period
#define AD9508_OUTPHA_MAX   2047    
    
#define AD9508_OUTDRVR_PD        (0x1 << 7)
#define AD9508_OUTDRVR_SYNC_INC  (0x0 << 7)
#define AD9508_OUTDRVR_SYNC_EXC  (0x1 << 6)
#define AD9508_OUTDRVR_PHASE_HI  (0x0 << 4)
#define AD9508_OUTDRVR_PHASE_NI  (0x1 << 4)
#define AD9508_OUTDRVR_PHASE_IN  (0x2 << 4)
#define AD9508_OUTDRVR_PHASE_LO  (0x3 << 4)
#define AD9508_OUTDRVR_MODE_LV05 (0x0 << 1)
#define AD9508_OUTDRVR_MODE_LV07 (0x1 << 1)
#define AD9508_OUTDRVR_MODE_LV10 (0x2 << 1)
#define AD9508_OUTDRVR_MODE_LV12 (0x3 << 1)
#define AD9508_OUTDRVR_MODE_HS8  (0x4 << 1)
#define AD9508_OUTDRVR_MODE_HS11 (0x5 << 1)
#define AD9508_OUTDRVR_MODE_CMOS (0x6 << 1)
    
#define AD9508_OUTCMOS_OUTEN_ENABLE     (0x1 << 7)
#define AD9508_OUTCMOS_OUTEN_DISABLE    (0x0 << 7)
#define AD9508_OUTCMOS_OUTPHASE_HI      (0x0 << 5)
#define AD9508_OUTCMOS_OUTPHASE_NI      (0x1 << 5)
#define AD9508_OUTCMOS_OUTPHASE_IN      (0x2 << 5)
#define AD9508_OUTCMOS_OUTPHASE_LO      (0x3 << 5)
#define AD9508_OUTCMOS_NOUTEN_ENABLE    (0x1 << 4)
#define AD9508_OUTCMOS_NOUTEN_DISABLE   (0x0 << 4)
#define AD9508_OUTCMOS_NOUTPHASE_HI     (0x0 << 2)
#define AD9508_OUTCMOS_NOUTPHASE_NI     (0x1 << 2)
#define AD9508_OUTCMOS_NOUTPHASE_IN     (0x2 << 2)
#define AD9508_OUTCMOS_NOUTPHASE_LO     (0x3 << 2)
    
typedef struct ad9508Obj * Ad9508Obj;
    
struct ad9508Obj{
    unsigned char ctlAddr;      //i2c device address
  	I2cObj i2c;                 //i2c bus
    int (*va_print)(const char *format, ...);  //link this to either vprintf or vsprintf
};

//constructor
Ad9508Obj ad9508_New();

//initialize ad9508
int ad9508_Init(Ad9508Obj self, I2cObj i2c, unsigned char ctlAddr, int (*va_print)(const char *format, ...));

//soft reset, bit is self clearing, just need to send the bit
void ad9508_Reset(Ad9508Obj self);

//put device to sleep
void ad9508_Sleep(Ad9508Obj self);

//wake device
void ad9508_Wake(Ad9508Obj self);

//power down a clock output
void ad9508_PdClock(Ad9508Obj self, int baseAddr);

//set a clock output for CMOS
void ad9508_SetClockCmos(Ad9508Obj self, int baseAddr, int div, int phase, unsigned char cmosOptions);

//ucmd wrapper for set clock cmos
void ad9508_ucmdSetClockCmos(Ad9508Obj self, const char * str);

#endif

/* [] END OF FILE */
