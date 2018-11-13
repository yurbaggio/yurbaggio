/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jul 16, 2007
Modified	Jun 5, 2013
Buttons headers
******************************************************************************/

#ifndef NLIB_DRV_SIGQP_H
#define NLIB_DRV_SIGQP_H

#include "nlib_drv_sig.h"

typedef struct sigqpObj * SigqpObj;

//define sxtmr object (device control object), undefine for DEBUG, this should be hidden usually!
struct sigqpObj{

	SigObj sig;					//non-qp sig object

	unsigned int rEvt;			//this is the signal rise edge event ID
	unsigned int fEvt;			//this is the signal fall edge event ID
};

//constructor, resrve memory
SigqpObj sigqp_New();

//initializer
int sigqp_Init(SigqpObj self,  int (*sigSample_func)(void), unsigned int pLim_var, unsigned int rEvt_var, unsigned int fEvt_var);

//process events
void sigqp_Proc(SigqpObj self);

//sig event
typedef struct SigEvtTag {
	QEvent super;
}SigEvt;

#endif
