/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jul 16, 2007
Modified	Jun 5, 2013
Buttons headers
******************************************************************************/

//uses calloc, must include stdlib, stdarg
#include <stdlib.h>

//include proper headers
#include "nlib_err.h"
#include "nlib_btn.h"

//initiate values
//constructor, resrve memory
BtnObj btn_New(){
	BtnObj self = (BtnObj)calloc(1, sizeof(struct btnObj));
	return self;
}

//initializer
int btn_Init(BtnObj self, int (*btnSample_func)(void), unsigned int pressLim_var, unsigned int holdLim_var)
{
	if(self == NULL)
		return NLIB_ERR_NOMEM;
	
	//link sampling function
	self->btnSample = btnSample_func;
	
	//initiate values
	self->pressCounter = 0;
	self->holdCounter = 0;
  self->pressLim = pressLim_var;
	self->holdLim = holdLim_var;
	
	return NLIB_ERR_NOERROR;
}

//process events
void btn_Proc(BtnObj self){		
	if (!(self->btnSample())) {
		self->pressCounter++;
		self->holdCounter++;
		if (self->pressCounter > self->pressLim){
			self->pressCounter = self->pressLim;
			if(self->holdCounter <= self->holdLim){
				self->pFlag = 1;
				btn_RArm(self);
			}else{
				self->holdCounter = self->holdLim;
				self->hFlag = 1;
				btn_RDisarm(self);
			}
		}
	}
	else {
		self->pressCounter--;
		self->holdCounter = 0;
		if (self->pressCounter < ((-1) * self->pressLim)){
			self->pressCounter = ( (-1) * self->pressLim);
			self->pFlag = 0;
			btn_PArm(self);
		}
		self->hFlag = 0;
		btn_HArm(self);
	}
}

//see if button is pressed, held or released
int btn_Press(BtnObj self)	{	return self->pFlag & self->pArm;}
int btn_Release(BtnObj self)	{	return (!self->pFlag) & self->rArm;}
int btn_Hold(BtnObj self)	{	return self->hFlag & self->hArm;}

//see if button is pressed without trigger processing
int btn_Pressed(BtnObj self)	{	return self->pFlag;}

//button press arm/disarm
void btn_PArm(BtnObj self)	{	self->pArm = 1;}
void btn_PDisarm(BtnObj self){	self->pArm = 0;}

//button release arm/disarm
void btn_RArm(BtnObj self)	{	self->rArm = 1;}
void btn_RDisarm(BtnObj self){	self->rArm = 0;}

//button hold arm/disarm
void btn_HArm(BtnObj self)	{	self->hArm = 1;}
void btn_HDisarm(BtnObj self){	self->hArm = 0;}
