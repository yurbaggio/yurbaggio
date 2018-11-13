/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jul 16, 2007
Modified	Jun 5, 2013
Buttons headers
******************************************************************************/

#ifndef NLIB_DRV_BTNQP_H
#define NLIB_DRV_BTNQP_H

#include "qp_port.h"
#include "nlib_btn.h"

typedef struct btnqpObj * BtnqpObj;

//define button object (device control object), undefine for DEBUG, this should be hidden usually!
struct btnqpObj{

	BtnObj btn;					//non-qp button object
	
	unsigned int pEvt;			//this is the button pressed event ID
	unsigned int hEvt;			//this is the button hold event ID
	unsigned int rEvt;			//this is the button release event ID
};

//constructor, resrve memory
BtnqpObj btnqp_New();

//initializer
int btnqp_Init(BtnqpObj self,  int (*btnSample_func)(void), unsigned int pEvt_var, unsigned int hEvt_var, unsigned int rEvt_var, int pressLim_var, int holdLim_var);

//process events
void btnqp_Proc(BtnqpObj self);

//button event
typedef struct BtnEvtTag {
	QEvent super;
}BtnEvt;

#endif
