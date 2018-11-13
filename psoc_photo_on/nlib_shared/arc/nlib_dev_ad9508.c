//AD9508 drivers

#include <stdlib.h>
#include <string.h>
#include "nlib_sys_conv.h"
#include "nlib_sys_trig.h"
#include "nlib_per_i2c.h"
#include "nlib_sys_err.h"
#include "nlib_sys_cproc.h"
#include "nlib_dev_ad9508.h"

//constructor
Ad9508Obj ad9508_New(){
    Ad9508Obj self = (Ad9508Obj) calloc(1, sizeof(struct ad9508Obj));
	return self;
}

//initialize ad9508
int ad9508_Init(Ad9508Obj self, I2cObj i2c, unsigned char ctlAddr, int (*va_print)(const char *format, ...)){
    
    if (self == NULL)
	    return NLIB_ERR_NOMEM;
    
    self->i2c = i2c;
    self->ctlAddr = ctlAddr;
    self->va_print = va_print;      //link print function
    
    return NLIB_ERR_NOERROR;
}

//soft reset, bit is self clearing, just need to send the bit
void ad9508_Reset(Ad9508Obj self){
    i2c_ByteWrite16(self->i2c, self->ctlAddr, AD9508_ADDR_I2CCTRL, AD9508_I2CCTRL_RST);
}

//put device to sleep
void ad9508_Sleep(Ad9508Obj self){
    i2c_ByteWrite16(self->i2c, self->ctlAddr, AD9508_ADDR_SLEEP, AD9508_SLEEP_ENABLE);
}

//wake device
void ad9508_Wake(Ad9508Obj self){
    i2c_ByteWrite16(self->i2c, self->ctlAddr, AD9508_ADDR_SLEEP, AD9508_SLEEP_DISABLE);
}

//power down a clock output
void ad9508_PdClock(Ad9508Obj self, int baseAddr){
    i2c_ByteWrite16(self->i2c, self->ctlAddr, baseAddr + AD9508_ADDROS_OUTDRVR, AD9508_OUTDRVR_PD);
}

//set a clock output for CMOS
void ad9508_SetClockCmos(Ad9508Obj self, int baseAddr, int div, int phase, unsigned char cmosOptions){
    
    //power down first
    i2c_ByteWrite16(self->i2c, self->ctlAddr, baseAddr + AD9508_ADDROS_OUTDRVR, AD9508_OUTDRVR_PD);
    
    //divider
    i2c_ByteWrite16(self->i2c, self->ctlAddr, baseAddr + AD9508_ADDROS_OUTDIVLO, SHORTLO(div));
    i2c_ByteWrite16(self->i2c, self->ctlAddr, baseAddr + AD9508_ADDROS_OUTDIVHI, SHORTHI(div));
    
    //phase
    i2c_ByteWrite16(self->i2c, self->ctlAddr, baseAddr + AD9508_ADDROS_OUTPHALO, SHORTLO(phase));
    i2c_ByteWrite16(self->i2c, self->ctlAddr, baseAddr + AD9508_ADDROS_OUTPHAHI, SHORTHI(phase));
    
    //set cmos options
    i2c_ByteWrite16(self->i2c, self->ctlAddr, baseAddr + AD9508_ADDROS_OUTCMOS, cmosOptions);
    
    //power back up
    i2c_ByteWrite16(self->i2c, self->ctlAddr, baseAddr + AD9508_ADDROS_OUTDRVR, AD9508_OUTDRVR_SYNC_INC | AD9508_OUTDRVR_PHASE_NI | AD9508_OUTDRVR_MODE_CMOS);
}
    

//ucmd wrapper for set clock cmos
void ad9508_ucmdSetClockCmos(Ad9508Obj self, const char * str){
	
	//copy string, store in case it is needed
	char cpy[CPROC_LINEBUFF_SIZE + 1];
	strcpy(cpy, str);
	
	//tokenize, discard first since it is the command itself
	char *pch = strtok(cpy, CPROC_DELIMITERS);
    char *strTest;

    //2nd token is channel
    pch = strtok(NULL, CPROC_DELIMITERS);
	int ch = strtol(pch, &strTest, 10);
    if (strTest == pch){
        self->va_print("Need clock channel number\n\r");
        return;
    }else if ((ch < AD9508_CH_MIN) || (ch > AD9508_CH_MAX)){
        self->va_print("Invalid clock channel number, must be between %i and %i\n\r", AD9508_CH_MIN, AD9508_CH_MAX);
        return;
    }
    int baseAddr;
    switch(ch){
        case 0: baseAddr = AD9508_ADDR_OUT0; break;
        case 1: baseAddr = AD9508_ADDR_OUT1; break;
        case 2: baseAddr = AD9508_ADDR_OUT2; break;
        case 3: baseAddr = AD9508_ADDR_OUT3; break;
        default:    self->va_print("An unknown clock channel number error\n\r");    return;
    }
    
	//3nd token divider
	pch = strtok(NULL, CPROC_DELIMITERS);
	int div = strtol(pch, &strTest, 10);
    if (strTest == pch){
        self->va_print("Need clock divider value\n\r");
        return;
    }else if ((div > AD9508_OUTDIV_MAX) || (div < AD9508_OUTDIV_MIN)){
        self->va_print("Invalid divider value, must be between %i and %i\n\r", AD9508_OUTDIV_MIN, AD9508_OUTDIV_MAX);
        return;
    }
    
    //4rd token phase in steps of input clock
    pch = strtok(NULL, CPROC_DELIMITERS);
	int phase = strtol(pch, &strTest, 10);
    if (strTest == pch){
        self->va_print("Need phase step count, in 1/2 clock periods\n\r");
        return;
    }else if ((phase < AD9508_OUTPHA_MIN) || (phase > AD9508_OUTPHA_MAX)){
        self->va_print("Invalid phase step count value, must be between %i and %i in radians\n\r", AD9508_OUTPHA_MIN, AD9508_OUTPHA_MAX);
        return;
    }
    
    //5th token signal options
	pch = strtok(NULL, CPROC_DELIMITERS);
    if (strcmp(pch, "pn0") == 0){   //pos and neg, in phase
        ad9508_SetClockCmos(self, baseAddr, div - 1, phase, AD9508_OUTCMOS_OUTEN_ENABLE | AD9508_OUTCMOS_OUTPHASE_NI | AD9508_OUTCMOS_NOUTEN_ENABLE | AD9508_OUTCMOS_NOUTPHASE_NI);
    }else if (strcmp(pch, "pn180") == 0){   //pos and neg, 180 out of phase
        ad9508_SetClockCmos(self, baseAddr, div - 1, phase, AD9508_OUTCMOS_OUTEN_ENABLE | AD9508_OUTCMOS_OUTPHASE_NI | AD9508_OUTCMOS_NOUTEN_ENABLE | AD9508_OUTCMOS_NOUTPHASE_IN);
    }else if (strcmp(pch, "p") == 0){   //pos only
        ad9508_SetClockCmos(self, baseAddr, div - 1, phase, AD9508_OUTCMOS_OUTEN_ENABLE | AD9508_OUTCMOS_OUTPHASE_NI);
    }else if (strcmp(pch, "n") == 0){   //neg only
        ad9508_SetClockCmos(self, baseAddr, div - 1, phase, AD9508_OUTCMOS_NOUTEN_ENABLE | AD9508_OUTCMOS_NOUTPHASE_IN);
    }else{
        self->va_print("Need signal type: pn0, pn180, p or n\n\r");
        return;
    }
    
    //debug
    self->va_print("Setting channel %i clock with divider %i at %i phase setep, options %s\n\r\n\r", ch, div, phase, pch);
}

/* [] END OF FILE */
