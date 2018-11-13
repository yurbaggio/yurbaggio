/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Dec 1, 2010
Modified	Nov 9, 2011
Rotary Encoder Header
******************************************************************************/

#ifndef NLIB_DRV_ROTQP_H
#define NLIB_DRV_ROTQP_H

#include "nlib_drv_rot.h"

typedef struct rotqpObj * RotqpObj;

//define rotary encoder object
struct rotqpObj{

	RotObj rot;					//non-qp sig object

	unsigned int cwEvt;		//left turn event
	unsigned int ccwEvt;	//right turn event
};

//constructor, reserve memory
RotqpObj rotqp_New();

//initializer, use a different intializer depending on if QP is used
int rotqp_Init(RotqpObj self, RotFunc fTable_var, unsigned int cwEvt_var, unsigned int ccwEvt_var);

//process events
void rotqp_Proc(RotqpObj self);

//when QP is used, this is a btn event
typedef struct RotEvtTag {
	QEvent super;
}RotEvt;

#endif
