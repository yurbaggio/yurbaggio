/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jul 16, 2007
Modified	Jun 5, 2013
Buttons headers
******************************************************************************/

//uses calloc, must include stdlib, stdarg
#include <stdlib.h>

//if using QP
#include "qp_port.h"

//include proper headers
#include "nlib_err.h"
#include "nlib_btnqp.h"

//initiate values
//constructor, resrve memory
BtnqpObj btnqp_New(){
	BtnqpObj self = (BtnqpObj)calloc(1, sizeof(struct btnqpObj));
	return self;
}

int btnqp_Init(BtnqpObj self,  int (*btnSample_func)(void), unsigned int pEvt_var, unsigned int hEvt_var, unsigned int rEvt_var, int pressLim_var, int holdLim_var)
{
	if(self == NULL)
		return NLIB_ERR_NOMEM;
	
	//reserve memory for non-qp button
	self->btn = btn_New();
	
	//initialize non-qp button
	btn_Init(self->btn, btnSample_func, pressLim_var, holdLim_var);
	
	//initiate QP variables
	self->pEvt = pEvt_var;
	self->hEvt = hEvt_var;
	self->rEvt = rEvt_var;
	
	return NLIB_ERR_NOERROR;
}

//process events
void btnqp_Proc(BtnqpObj self){		
	
	//process non-qp button
	btn_Proc(self->btn);
	
	//if a button is pressed, released, or held, fire an event!
	//get an event pointer
	BtnEvt *pe;
	
	//fire the event
	if ((self->pEvt > Q_USER_SIG) && btn_Press(self->btn)){
		pe = Q_NEW(BtnEvt, self->pEvt);	
		QF_publish((QEvent *)pe);
		btn_PDisarm(self->btn);
	}
	if ((self->rEvt > Q_USER_SIG) && btn_Release(self->btn)){
		pe = Q_NEW(BtnEvt, self->rEvt);	
		QF_publish((QEvent *)pe);
		btn_RDisarm(self->btn);
	}
	if ((self->hEvt > Q_USER_SIG) && btn_Hold(self->btn)){
		pe = Q_NEW(BtnEvt, self->hEvt);	
		QF_publish((QEvent *)pe);
		btn_HDisarm(self->btn);
	}
}
