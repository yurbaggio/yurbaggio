/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Mar 20, 2012
Modified	Mar 20, 2012
PID controller in C
******************************************************************************/

#ifndef NLIB_PID_H
#define NLIB_PID_H

//error list
#define PID_ERR_NOERROR	0	//no error has occured
#define PID_ERR_UNKNOWN	-1	//unknown error
#define PID_ERR_NOMEM	-5	//out of memory

typedef struct pidObj * PidObj;

//define an Interface table
typedef struct PidIntStruct{
	void (*pid_ResetError)(PidObj self);
	void (*pid_SetTarget)(PidObj self, float target_var);
	void (*pid_SetTuning)(PidObj self, float kp_var, float ki_var, float kd_var);
	float (*pid_GetKpTerm)(PidObj self);
	float (*pid_GetKiTerm)(PidObj self);
	float (*pid_GetKdTerm)(PidObj self);
	float (*pid_Compute)(PidObj self, float input);
}PidInt;

//define a PID object
struct pidObj{
	
	//interface table
	PidInt	iTable;
	
	//pid values
	float	kp;
	float	ki;
	float	kd;

	//pid calculated terms
	float	kp_term;
	float	ki_term;		//intergral error
	float	kd_term;

	//target value for input (setpoint)
	float	target;
	
	//current and previous errors
	float	prevInput;
	float	error;

	//output limits
	float	outputMin;
	float	outputMax;
};

//constuctor
PidObj pid_New();

//initialize
int pid_Init(PidObj self, float kp_var, float ki_var, float kd_var, float target_var, float outMin_var, float outMax_var);

//reset the errors
void pid_ResetError(PidObj self);

//change the target of the PID
void pid_SetTarget(PidObj self, float target_var);

//change tuning settings for the PID
void pid_SetTuning(PidObj self, float kp_var, float ki_var, float kd_var);

//term terms for debug
float pid_GetKpTerm(PidObj self);
float pid_GetKiTerm(PidObj self);
float pid_GetKdTerm(PidObj self);

//get output
float pid_Compute(PidObj self, float input);

#endif
