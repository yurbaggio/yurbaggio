/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Mar 31, 2010
Modified	Apr 19, 2011
Analog Change Detection Driver Headers (for accelerometer/other analog chips)
******************************************************************************/

#ifndef NLIB_DRV_ADET_H
#define NLIB_DRV_ADET_H

//how long of a queue is used for detection, can define your own
#ifndef ADET_QSIZE
#define ADET_QSIZE 50
#endif

typedef struct adetObj * AdetObj;

//define adet object
struct adetObj{
	unsigned int bufferId;	//adc buffer number
	int queue[ADET_QSIZE];
	unsigned int queue_idx;
	unsigned char sen;			//keeps track of sensitivity
	unsigned int pLim;              //keeps track of sampling period, will sample Every "pLim" calls to adet
	unsigned int pCnt;              //counter for pLim
	unsigned int aEvt;		//analog detection event number
	int vBig;
	int vSmall;
};

//constructor, reserve memory
AdetObj adet_New();

//initializer
int adet_Init(AdetObj self, unsigned int bufferId_var, unsigned char sen_var, unsigned int pLim_var);

//process acquiring data from ADC
unsigned int adet_Proc(AdetObj self);

//sets the sensitivity to a new value
void adet_SetSensitivity(AdetObj self, unsigned char sen_var);

#endif
