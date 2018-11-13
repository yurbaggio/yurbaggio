/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Dec 1, 2010
Modified	Dec 1, 2010
Rotary Encoder Driver
******************************************************************************/

//uses calloc, must include stdlib, stdarg
#include <stdlib.h>

//if using QP
#include "qp_port.h"

//include proper headers
#include "nlib_sys_err.h"
#include "nlib_drv_rotqp.h"

//initiate values
//constructor, reserve memory
RotqpObj rotqp_New(){
	RotqpObj self = (RotqpObj)calloc(1, sizeof(struct rotqpObj));
	return self;
}

//initializer, use a different intializer depending on if QP is used
int rotqp_Init(RotqpObj self, RotFunc fTable_var, unsigned int cwEvt_var, unsigned int ccwEvt_var)
{
	if(self == NULL)
		return NLIB_ERR_NOMEM;
	
	//reserve memory for non-qp function
	self->rot = rot_New();
	
	//initiate non-qp rot
	rot_Init(self->rot, fTable_var);
	
	self->cwEvt = cwEvt_var;
	self->ccwEvt = ccwEvt_var;
	
	return NLIB_ERR_NOERROR;
}

//process events
void rotqp_Proc(RotqpObj self){

	rot_Proc(self->rot);

	RotEvt *re;
	
	if ((self->cwEvt > Q_USER_SIG) && (self->rot->rotCw && !self->rot->rotCcw)){
		re = Q_NEW(RotEvt, self->cwEvt);
		QF_publish((QEvent *)re);
	}
	if ((self->ccwEvt > Q_USER_SIG) && (!self->rot->rotCw && self->rot->rotCcw)){
		re = Q_NEW(RotEvt, self->ccwEvt);
		QF_publish((QEvent *)re);
	}
}
