/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Mar 31, 2010
Modified	Apr 19, 2011
Analog Change Detection (for accelerometer/other analog chips)
******************************************************************************/

//uses calloc, must include stdlib, stdarg
#include <stdlib.h>

//if using QP
#include "qp_port.h"

//include proper headers
#include "nlib_sys_err.h"
#include "nlib_drv_adetqp.h"

//initiate values
//constructor, reserve memory
AdetqpObj adetqp_New(){
	AdetqpObj self = (AdetqpObj)calloc(1, sizeof(struct adetqpObj));
	return self;
}

//initializer
int adetqp_Init(AdetqpObj self, unsigned int bufferId_var, unsigned char sen_var, unsigned int pLim_var, unsigned int aEvt_var)
{
	if(self == NULL)
		return NLIB_ERR_NOMEM;
	
	//reserve memory for non-qp button
	self->adet = adet_New();
	
	//initialize non-qp button
	adet_Init(self->adet, bufferId_var, sen_var, pLim_var);
	
	//initiate QP variables
	self->aEvt	= aEvt_var;
		
	return NLIB_ERR_NOERROR;
}

//adcScanSample() NEEDS TO BE CALLED ONCE before all instances of adet Calls
//IE, if there are two adet Objects, call adcScanSample() once, then call adet_Proc()
//for each of the objects
//this functions reads from the adc buffer
unsigned int adetqp_Proc(AdetqpObj self){

	//declare return variable
	unsigned int adetRet = 0;
	
	//process non-qp button
	adetRet = adet_Proc(self->adet);
		
	//publish event if sensitiviy has been crossed
	if((self->adet->pCnt == 0) && ((self->adet->vBig - self->adet->vSmall) >= ((int)self->adet->sen))){
		AdetEvt *ae = Q_NEW(AdetEvt, self->aEvt);
		QF_publish((QEvent *)ae);
	}

	//return the measured ADC result
	return adetRet;
}
