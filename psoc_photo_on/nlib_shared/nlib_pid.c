/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Mar 20, 2012
Modified	Mar 20, 2012
PID controller in C
******************************************************************************/

//uses calloc, must include stdlib
#include <stdlib.h>

#include "nlib_err.h"
#include "nlib_pid.h"

//local prototypes
void pid_ResetError(PidObj self);
void pid_SetTarget(PidObj self, float target_var);
void pid_SetTuning(PidObj self, float kp_var, float ki_var, float kd_var);
float pid_GetKpTerm(PidObj self);
float pid_GetKiTerm(PidObj self);
float pid_GetKdTerm(PidObj self);
float pid_Compute(PidObj self, float input);

//create interface
static PidInt iTable_var = {
	pid_ResetError,
	pid_SetTarget,
	pid_SetTuning,
	pid_GetKpTerm,
	pid_GetKiTerm,
	pid_GetKdTerm,
	pid_Compute
};

//constructor, reserve memory
PidObj pid_New(){
	PidObj self = (PidObj)calloc(1, sizeof(struct pidObj));
	return self;
}

//initiate interface to the EE prom
int pid_Init(PidObj self, float kp_var, float ki_var, float kd_var, float target_var, float outMin_var, float outMax_var){
	if(self == NULL)
		return NLIB_ERR_NOMEM;

	//initiate values
	self->iTable	= iTable_var;

	//set initial target
	self->target = target_var;

	//set tuning
	self->kp = kp_var;
	self->ki = ki_var;
	self->kd = kd_var;

	//set output limits
	self->outputMin = outMin_var;
	self->outputMax = outMax_var;

	//set integral error to zero
	self->ki_term = 0;

	return NLIB_ERR_NOERROR;
}

//reset the errors
void pid_ResetError(PidObj self){
	self->kp_term = 0;
	self->ki_term = 0;
	self->kd_term = 0;
}

//change the target of the PID
void pid_SetTarget(PidObj self, float target_var){
	self->target = target_var;
}

//change tuning settings for the PID
void pid_SetTuning(PidObj self, float kp_var, float ki_var, float kd_var){
	self->kp = kp_var;
	self->ki = ki_var;
	self->kd = kd_var;
}

//term terms for debug
float pid_GetKpTerm(PidObj self){
	return self->kp_term;
}
float pid_GetKiTerm(PidObj self){
	return self->ki_term;
}
float pid_GetKdTerm(PidObj self){
	return self->kd_term;
}

//get output
float pid_Compute(PidObj self, float input){

	//add to error of integral, cap the max/min
	self->error = self->target - input;

	//compute terms
	self->kp_term = (self->kp)*(self->error);
	self->ki_term += (self->ki)*(self->error);
	self->kd_term = (self->kd)*(input - self->prevInput);

	//reset windup
	if (self->ki_term > self->outputMax)
		self->ki_term = self->outputMax;
	else if (self->ki_term < self->outputMin)
		self->ki_term = self->outputMin;

	//compute output
	float out = self->kp_term + self->ki_term + self->kd_term;
	if (out > self->outputMax)
		out = self->outputMax;
	else if (out < self->outputMin)
		out = self->outputMin;

	//store previous error
	self->prevInput = input;

	return out;
}
