/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Dec 1, 2010
Modified	Dec 1, 2010
Rotary Encoder Driver
******************************************************************************/

//uses calloc, must include stdlib, stdarg
#include <stdlib.h>

//include proper headers
#include "nlib_sys_err.h"
#include "nlib_drv_rot.h"

#define ROT_STATE_ARMED		1
#define ROT_STATE_WAIT		2

//initiate values
//constructor, reserve memory
RotObj rot_New(){
	RotObj self = (RotObj)calloc(1, sizeof(struct rotObj));
	return self;
}

//initializer, use a different intializer depending on if QP is used
int rot_Init(RotObj self,  RotFunc fTable_var)
{
	if(self == NULL)
		return NLIB_ERR_NOMEM;
	
	//link sample functions
	self->fTable = fTable_var;
	
	//initialize values
	self->state = ROT_STATE_WAIT;
	
	return NLIB_ERR_NOERROR;
}

//process events
void rot_Proc(RotObj self){
	//state machine for read the direction of the encoder
	switch(self->state)
	{
		case ROT_STATE_ARMED:
			if(!(self->fTable.sampleTapA()) && self->fTable.sampleTapB()){
				self->state = ROT_STATE_WAIT;
				self->rotCw = 1;
				self->rotCcw = 0;
			}else if(self->fTable.sampleTapA() && !(self->fTable.sampleTapB())){
				self->state = ROT_STATE_WAIT;
				self->rotCw = 0;
				self->rotCcw = 1;
			}
			break;
		default:
			if(!(self->fTable.sampleTapA()) && !(self->fTable.sampleTapB()))
				self->state = ROT_STATE_ARMED;
			self->rotCw = 0;
			self->rotCcw = 0;
			break;
	}
}

//see if encoder is turned
int rot_cw(RotObj self)		{	return self->rotCw;}
int rot_ccw(RotObj self)	{	return self->rotCcw;}
