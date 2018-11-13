/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Mar 31, 2010
Modified	Apr 19, 2011
Analog Change Detection (for accelerometer/other analog chips)
******************************************************************************/

//uses calloc, must include stdlib, stdarg
#include <stdlib.h>

//include proper headers
#include "nlib_sys_err.h"
#include "nlib_drv_adet.h"

//initiate values
//constructor, reserve memory
AdetObj adet_New(){
	AdetObj self = (AdetObj)calloc(1, sizeof(struct adetObj));
	return self;
}

//initializer
int adet_Init(AdetObj self, unsigned int bufferId_var, unsigned char sen_var, unsigned int pLim_var)
{
	if(self == NULL)
		return NLIB_ERR_NOMEM;
	
	//assign variables
	self->bufferId = bufferId_var;
	self->sen	= sen_var;
	
	//start the queue
	self->queue_idx = 0;

	//reset pCnt, assign limit
	if (pLim_var > 0)
		self->pLim = pLim_var - 1;	//set limit to one less, since we start count from zero
	else
		self->pLim = 0;

	self->pCnt = 0;
	
	return NLIB_ERR_NOERROR;
}

//adcScanSample() NEEDS TO BE CALLED ONCE before all instances of adet Calls
//IE, if there are two adet Objects, call adcScanSample() once, then call adet_Proc()
//for each of the objects
//this functions reads from the adc buffer
unsigned int adet_Proc(AdetObj self){

	//declare return variable
	unsigned int adetRet = 0;

	//test for pLim
	if (self->pCnt >= self->pLim){
		//reset pCnt
		self->pCnt = 0;

		//get next sample
		adetRet = ReadADC10(self->bufferId);
		self->queue[self->queue_idx] = adetRet;

		//increment the index counter
		if (self->queue_idx < (ADET_QSIZE-1))
			self->queue_idx ++;
		else
			self->queue_idx = 0;

		//determine biggest value change, find biggest, find smallest
		int i;
		self->vSmall = self->queue[0];
		for (i = 1; i < ADET_QSIZE; i++){
			if (self->vSmall > self->queue[i])
				self->vSmall = self->queue[i];
		}
		self->vBig = self->queue[0];
		for (i = 1; i < ADET_QSIZE; i++){
			if (self->vBig < self->queue[i])
				self->vBig = self->queue[i];
		}
		
	}else{
		self->pCnt++;
	}

	//return the measured ADC result
	return adetRet;
}

//sets the sensitivity to a new value
void adet_SetSensitivity(AdetObj self, unsigned char sen_var){
	self->sen	= sen_var;
}
