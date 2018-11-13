//AD9838 DDS drivers

#include <stdlib.h>
#include <string.h>
#include "nlib_sys_trig.h"
#include "nlib_sys_err.h"
#include "nlib_sys_conv.h"
#include "nlib_sys_cproc.h"
#include "nlib_dev_ad9838.h"

#define AD9838_FREQ_MAX     (16000000L)

#define AD9838_2POW28       (268435456L)
#define AD9838_2POW12       (4096L)

#define AD9838_ADDR_CONFIG    (0b00 << 14)
#define AD9838_ADDR_FREQ0     (0b01 << 14)
#define AD9838_ADDR_FREQ1     (0b10 << 14)
#define AD9838_ADDR_PHASE0    (0b110 << 13)
#define AD9838_ADDR_PHASE1    (0b111 << 13)

#define AD9838_CONFIG_B28     0b0010000000000000  //allows 28 bits loaded in two 16 bit words consecutively when B28=1, 14 bit MSB or LSB select when B28=0
#define AD9838_CONFIG_HLB     0b0001000000000000  //high or low word select whe using B28=0 
#define AD9838_CONFIG_FSEL    0b0000100000000000  //freq0 or freq1 select, freq0 when 0
#define AD9838_CONFIG_PSEL    0b0000010000000000  //phase0 or phase1 select, phase0 when 0
#define AD9838_CONFIG_PINSW   0b0000001000000000  //uses pins when set to 1
#define AD9838_CONFIG_RESET   0b0000000100000000  //reset when 1
#define AD9838_CONFIG_SLEEP1  0b0000000010000000  //disable internal MCLK when 1
#define AD9838_CONFIG_SLEEP12 0b0000000001000000  //disables the DAC output when 1
#define AD9838_CONFIG_SINE    0b0000000000000000  //sine option
#define AD9838_CONFIG_SAW     0b0000000000000010  //sawtooth option
#define AD9838_CONFIG_SQON    0b0000000000100000  //square option
#define AD9838_CONFIG_DIV2    0b0000000000001000  //square option

#define AD9838_FREQLSB_MASK   0x3FFF
#define AD9838_FREQMSB_SHIFT  14
#define AD9838_FREQMSB_MASK   (AD9838_FREQLSB_MASK << AD9838_FREQMSB_SHIFT)
#define AD9838_PHASEMASK      0x0FFF

//constructor
Ad9838Obj ad9838_New(){
    Ad9838Obj self = (Ad9838Obj) calloc(1, sizeof(struct ad9838Obj));
	return self;
}

//freq must be smaller than AD9838_FREQ_MAX
//initialize ad9838
int ad9838_Init(Ad9838Obj self, Ad9838Func fTable, long mclk, unsigned char mask, int (*va_print)(const char *format, ...)){
    
    if (self == NULL)
	    return NLIB_ERR_NOMEM;
    
    if (mclk > AD9838_FREQ_MAX)
        return NLIB_ERR_INVALPARAM;
    
    self->mclk = mclk;
    self->mask = mask;    
    self->fTable = fTable;
    self->va_print = va_print;      //link print function
    
    return NLIB_ERR_NOERROR;
}

//set dds
//frequency in Hz, phase in radians
//determines signal shape
void ad9838_SetDds(Ad9838Obj self, double freqHz, float phaseRad, unsigned int option){
    
    //store the frequency, phase and option values (can be used to be read later)
    self->freqHz = freqHz;
    self->phaseRad = phaseRad;
    self->option = option;
    
    //reset registers
    unsigned int reset = AD9838_ADDR_CONFIG | AD9838_CONFIG_B28 | AD9838_CONFIG_RESET | AD9838_CONFIG_PINSW | option;
    unsigned int start = AD9838_ADDR_CONFIG | AD9838_CONFIG_B28 | AD9838_CONFIG_PINSW | option;
    
    //calculate frequency
    unsigned long freg = FLOAT2LONG(freqHz * AD9838_2POW28 / (double)self->mclk);
    unsigned int flsb = AD9838_ADDR_FREQ0 | (freg & AD9838_FREQLSB_MASK);
    unsigned int fmsb = AD9838_ADDR_FREQ0 | ((freg & AD9838_FREQMSB_MASK) >> AD9838_FREQMSB_SHIFT);
    
    //calculate phase
    unsigned int preg = FLOAT2INT(phaseRad * AD9838_2POW12 / NT_2PI);
    unsigned int phase = AD9838_ADDR_PHASE0 | (preg & AD9838_PHASEMASK);
    
    //write to AD9838, select channel is multiple SS is used
    self->fTable.spiOutWrite(reset, self->mask);
    while(self->fTable.spiOutDone() == 0);
    self->fTable.spiOutWrite(flsb, self->mask);
    while(self->fTable.spiOutDone() == 0);
    self->fTable.spiOutWrite(fmsb, self->mask);
    while(self->fTable.spiOutDone() == 0);
    self->fTable.spiOutWrite(phase, self->mask);
    while(self->fTable.spiOutDone() == 0);
    self->fTable.spiOutWrite(start, self->mask);
    while(self->fTable.spiOutDone() == 0);
}

//sleep Dds
void ad9838_SleepDds(Ad9838Obj self){
    
    //set to sleep
    unsigned int sleep = AD9838_ADDR_CONFIG | AD9838_CONFIG_SLEEP1 | AD9838_CONFIG_SLEEP12;
    
    self->fTable.spiOutWrite(sleep, self->mask);
    while(self->fTable.spiOutDone() == 0);
}

//reset Dds
void ad9838_SwResetDdsSq2(Ad9838Obj self){
    
    //reset registers
    unsigned int reset = AD9838_ADDR_CONFIG | AD9838_CONFIG_B28 | AD9838_CONFIG_RESET | AD9838_CONFIG_PINSW | AD9838_CONFIG_SINE | AD9838_CONFIG_SQON | AD9838_CONFIG_DIV2;
    unsigned int start = AD9838_ADDR_CONFIG | AD9838_CONFIG_B28 | AD9838_CONFIG_PINSW | AD9838_CONFIG_SINE | AD9838_CONFIG_SQON | AD9838_CONFIG_DIV2;
    
    //write to AD9838
    self->fTable.spiOutWrite(reset, self->mask);
    while(self->fTable.spiOutDone() == 0);
    self->fTable.spiOutWrite(start, self->mask);
    while(self->fTable.spiOutDone() == 0);
}

//ucmd wrapper for set DDS
void ad9838_ucmdSetDds(Ad9838Obj self, const char * str){
	
	//copy string, store in case it is needed
	char cpy[CPROC_LINEBUFF_SIZE + 1];
	strcpy(cpy, str);
	
	//tokenize, discard first since it is the command itself
	char *pch = strtok(cpy, CPROC_DELIMITERS);
    char *strTest;

	//2nd token freq in Hz
	pch = strtok(NULL, CPROC_DELIMITERS);
	double freq = strtod(pch, &strTest);
    if (strTest == pch){
        self->va_print("Need frequency in Hz\n\r");
        return;
    }else if (freq > (self->mclk) || freq < 0){
        self->va_print("Invalid frequency value, %iHz\n\r", freq);
        return;
    }
    
    //3rd token phase, in rad
    pch = strtok(NULL, CPROC_DELIMITERS);
	float phase = strtof(pch, &strTest);
    if (strTest == pch){
        self->va_print("Need phase in radians\n\r");
        return;
    }else if ((phase > NT_2PI) || (phase < 0)){
        self->va_print("Invalid phase value, must be between 0 and %6f in radians\n\r", NT_2PI);
        return;
    }
    
    //4th token signal type
	pch = strtok(NULL, CPROC_DELIMITERS);
    if (strcmp(pch, "sine") == 0){
        ad9838_SetDds(self, freq, phase, AD9838_CONFIG_SINE);
    }else if (strcmp(pch, "saw") == 0){
        ad9838_SetDds(self, freq, phase, AD9838_CONFIG_SAW);
    }else if (strcmp(pch, "sinesq") == 0){
        ad9838_SetDds(self, freq, phase, AD9838_CONFIG_SINE | AD9838_CONFIG_SQON);
    }else if (strcmp(pch, "sinesq2") == 0){
        ad9838_SetDds(self, freq, phase, AD9838_CONFIG_SINE | AD9838_CONFIG_SQON | AD9838_CONFIG_DIV2);      
    }else{
        self->va_print("Need signal type: sine, saw or sinesq or sinesq2\n\r");
        return;
    }
    
    //debug
    self->va_print("Caclulated divide register float: %f\n\r", freq * AD9838_2POW28 / (double)self->mclk);
    self->va_print("Loaded divide register long: %i\n\r", FLOAT2LONG(freq * AD9838_2POW28 / self->mclk));
    self->va_print("Loaded phase register long: %i\n\r", FLOAT2INT(phase * AD9838_2POW12 / NT_2PI));
    
    self->va_print("Setting %f %s signal\n\r\n\r", freq, pch);
}

/* [] END OF FILE */
