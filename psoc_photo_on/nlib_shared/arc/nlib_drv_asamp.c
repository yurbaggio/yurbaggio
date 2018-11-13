/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Oct 31, 2011
Modified	Oct 31, 2011
Analog Sample Driver
******************************************************************************/

//uses calloc, must include stdlib, stdarg
#include <stdlib.h>

//include proper headers
#include "nlib_sys_err.h"
#include "nlib_drv_asamp.h"

//initiate values
//constructor, reserve memory
AsampObj asamp_New(){
	AsampObj asampInst = (AsampObj)calloc(1, sizeof(struct asampObj));
	return asampInst;
}

//initializer
int asamp_Init(AsampObj asampInst, unsigned int bufferId_var, unsigned int pLim_var, unsigned int qLim_var)
{
	if(asampInst == NULL)
		return NLIB_ERR_NOMEM;
	
	//assign variables
	asampInst->bufferId = bufferId_var;
	
	//reset pCnt, assign limit
	if (pLim_var > 0)
		asampInst->pLim = pLim_var - 1;	//set limit to one less, since we start count from zero
	else
		asampInst->pLim = 0;
	asampInst->pCnt = 0;
	
	//reset sample queue's limit and index, assign limit
	if (qLim_var > 0)
		asampInst->qPtrLim = qLim_var;	//set limit to one less, since we start count from zero
	else
		asampInst->qPtrLim = 1;
	asampInst->qPtrIdx = 0;
	
	//reserve memory of Queue
	asampInst->qPtr = (unsigned int*)calloc(asampInst->qPtrLim, sizeof(unsigned int));
	
	return NLIB_ERR_NOERROR;
}

//adcScanSample() NEEDS TO BE CALLED ONCE before all instances of asamp Calls
//IE, if there are two asamp Objects, call adcScanSample() once, then call asamp_Proc()
//for each of the objects
//this function reads from the adc buffer
//this function creates a running sum for the sample limit assigned to sLim
unsigned int asamp_Proc(AsampObj asampInst){

	//declare return variable
	unsigned int asampRet = 0;

	//test for pLim
	if (asampInst->pCnt >= asampInst->pLim){
		//reset pCnt
		asampInst->pCnt = 0;

		//get next sample, put into barrel shifter
		asampRet = ReadADC10(asampInst->bufferId);

		(asampInst->qPtr)[asampInst->qPtrIdx] = asampRet;
		
		asampInst->qPtrIdx ++;
		if (asampInst->qPtrIdx >= asampInst->qPtrLim)
			asampInst->qPtrIdx = 0;
			
		//average the whole queue
		unsigned int sum;
		int i;
		for (i = 0; i < asampInst->qPtrLim; i ++)
			sum += (asampInst->qPtr)[i];
		asampInst->aValue = sum/(asampInst->qPtrLim);
		
	}else{
		asampInst->pCnt++;
	}

	//return the measured ADC result
	return asampRet;
}

//returns the averaged sample value
unsigned int asamp_GetValue(AsampObj asampInst){
	return asampInst->aValue;
}
