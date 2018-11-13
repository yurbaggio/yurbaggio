/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Mar 31, 2010
Modified	Apr 19, 2011
Analog Change Detection Driver Headers (for accelerometer/other analog chips)
******************************************************************************/

#ifndef NLIB_DRV_ADETQP_H
#define NLIB_DRV_ADETQP_H

#include "nlib_drv_adet.h"

typedef struct adetqpObj * AdetqpObj;

//define adet object
struct adetqpObj{

	AdetObj adet;			//non-qp sig object

	unsigned int aEvt;		//analog detection event number
};

//constructor, reserve memory
AdetqpObj adetqp_New();

//initializer
int adetqp_Init(AdetqpObj self, unsigned int bufferId_var, unsigned char sen_var, unsigned int pLim_var, unsigned int aEvt_var);

//process acquiring data from ADC
unsigned int adetqp_Proc(AdetqpObj self);

//adetect type event
typedef struct AdetEvtTag {
	QEvent super;
}AdetEvt;

#endif
