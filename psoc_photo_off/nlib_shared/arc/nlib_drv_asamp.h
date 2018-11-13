/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Oct 31, 2011
Modified	Oct 31, 2011
Analog Sample Driver
******************************************************************************/

#ifndef NLIB_DRV_ASAMP_H
#define NLIB_DRV_ASAMP_H

typedef struct asampObj * AsampObj;

//define asamp object
struct asampObj{
	unsigned int bufferId;	//adc buffer number
    unsigned int pLim;		//keeps track of sampling period, will sample Every "pLim" calls to asamp
    unsigned int pCnt;		//counter for pLim
	unsigned int aValue;	//average analog value
	unsigned int *qPtr;		//pointer to the Queue keeping track of the running sum
	unsigned int qPtrIdx;	//index keeping track of the next location to write to
	unsigned int qPtrLim;	//keeps track of max number of samples to average
};

//constructor, reserve memory
AsampObj asamp_New();

//initializer
int asamp_Init(AsampObj asampInst, unsigned int bufferId_var, unsigned int pLim_var, unsigned int qLim_var);

//process acquiring data from ADC
unsigned int asamp_Proc(AsampObj asampInst);

//returns the averaged sample value
unsigned int asamp_GetValue(AsampObj asampInst);

#endif
