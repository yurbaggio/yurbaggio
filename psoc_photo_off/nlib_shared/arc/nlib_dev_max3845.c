/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Dec 1, 2010
Modified	Dec 1, 2010
Maxim IC MAX3845 Integrated MAX3845 Driver
******************************************************************************/

//uses calloc, must include stdlib
#include <stdlib.h>

#include "nlib_sys_err.h"
#include "nlib_dev_max3845.h"

//define memory map
#define MAX3845_SEC	0x00
#define MAX3845_MIN	0x01
#define MAX3845_HRS	0x02
#define MAX3845_DAY	0x03
#define MAX3845_DATE	0x04
#define MAX3845_MON	0x05
#define MAX3845_YRS	0x06
	
//alarm
#define MAX3845_ARM1_SEC	0x07
#define MAX3845_ARM1_MIN	0x08
#define MAX3845_ARM1_HRS	0x09
#define MAX3845_ARM1_DAY	0x0A
#define MAX3845_ARM2_SEC	0x0B
#define MAX3845_ARM2_MIN	0x0C
#define MAX3845_ARM2_HRS	0x0D
#define MAX3845_ARM2_DAY	0x0E

//controls
#define MAX3845_CTRL1	0x0E
#define MAX3845_CTRL2	0x0F
#define MAX3845_AGEOS	0x10
#define MAX3845_TEMPH	0x11
#define MAX3845_TEMPL	0x12

//constructor, resrve memory
Max3845Obj max3845_New(){
	Max3845Obj self = (Max3845Obj)calloc(1, sizeof(struct max3845Obj));
	return self;
}

//initialization
int max3845_Init(Max3845Obj self, Max3845Func fTable_var){
	if(self == NULL)
		return NLIB_ERR_NOMEM;

	//initiate values
	self->fTable	= fTable_var;

	return NLIB_ERR_NOERROR;
}

//detect input change
int max3845_HasChanged(Max3845Obj self){
	int ret = 0;
	int tInput0 = self->fTable.input0();
	int tInput1 = self->fTable.input1();

	if (tInput0 != self->prevInput0)
		ret = 1;
	if (tInput1 != self->prevInput1)
		ret = 1;

	self->prevInput0 = tInput0;
	self->prevInput1 = tInput1;

	return ret;
}

//force port0, regardless of input
void max3845_SetInput0(Max3845Obj self){
	self->fTable.output0(0);
	self->fTable.output1(0);
	self->fTable.output2(0);
	self->fTable.output3(0);
}

//force port1, regardless of input
void max3845_SetInput1(Max3845Obj self){
	self->fTable.output0(1);
	self->fTable.output1(1);
	self->fTable.output2(1);
	self->fTable.output3(1);
}

//auto split, according to inputs
void max3845_SetAuto(Max3845Obj self){
	if(self->fTable.input0() && self->fTable.input1()){
		self->fTable.output0(0);
		self->fTable.output1(0);
		self->fTable.output2(1);
		self->fTable.output3(1);
	}else if(!self->fTable.input0() && self->fTable.input1()){
		self->fTable.output0(1);
		self->fTable.output1(1);
		self->fTable.output2(1);
		self->fTable.output3(1);
	}else{
		self->fTable.output0(0);
		self->fTable.output1(0);
		self->fTable.output2(0);
		self->fTable.output3(0);
	}
}
