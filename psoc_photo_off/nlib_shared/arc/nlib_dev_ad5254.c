//AD5254 drivers

#include <stdlib.h>
#include <string.h>
#include "nlib_sys_conv.h"
#include "nlib_sys_trig.h"
#include "nlib_per_i2c.h"
#include "nlib_sys_err.h"
#include "nlib_sys_cproc.h"
#include "nlib_dev_ad5254.h"

//constructor
Ad5254Obj ad5254_New(){
    Ad5254Obj self = (Ad5254Obj) calloc(1, sizeof(struct ad5254Obj));
	return self;
}

//initialize ad5254
int ad5254_Init(Ad5254Obj self, I2cObj i2c, unsigned char ctlAddr, void (*wp_b_Write)(uint8 val), int (*va_print)(const char *format, ...)){
    
    if (self == NULL)
	    return NLIB_ERR_NOMEM;
    
    self->i2c = i2c;
    self->ctlAddr = ctlAddr;
    self->wp_b_Write = wp_b_Write;
    self->va_print = va_print;      //link print function
    
    self->wp_b_Write(0);            //set to write protect
    
    return NLIB_ERR_NOERROR;
}

//write RDAC
void ad5254_WriteRdac(Ad5254Obj self, int ch, unsigned char val){
    self->wp_b_Write(1);
    i2c_ByteWrite(self->i2c, self->ctlAddr, AD5254_MODE_RDAC | AD5254_SEL_REG | ((unsigned char) ch), val);
    i2c_ByteWrite(self->i2c, self->ctlAddr, AD5245_CMD_NOP | AD5254_SEL_CMD, 0);        //set NOP
    self->wp_b_Write(0);
}

//ucmd wrapper writing to RDAC
void ad5254_ucmdWriteRdac(Ad5254Obj self, const char * str){
	
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
        self->va_print("Need RDAC channel number\n\r");
        return;
    }else if ((ch < AD5254_CH_MIN) || (ch > AD5254_CH_MAX)){
        self->va_print("Invalid RDAC channel number, must be between %i and %i\n\r", AD5254_CH_MIN, AD5254_CH_MAX);
        return;
    }
    
	//3nd token write value
	pch = strtok(NULL, CPROC_DELIMITERS);
	int val = strtol(pch, &strTest, 10);
    if (strTest == pch){
        self->va_print("Need RDAC write value\n\r");
        return;
    }else if ((val < AD5254_VAL_MIN) || (val > AD5254_VAL_MAX)){
        self->va_print("Invalid RDAC write value, must be between %i and %i\n\r", AD5254_VAL_MIN, AD5254_VAL_MAX);
        return;
    }
    
    //write RDAC
    ad5254_WriteRdac(self, ch, (unsigned char) val);
    
    //debug
    self->va_print("Writing to RDAC channel %i with value %i\n\r\n\r", ch, val);
}

/* [] END OF FILE */
