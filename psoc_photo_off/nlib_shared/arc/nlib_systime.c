/******************************************************************************
Engscope
Jianyi Liu
Created		May 11, 2014
Modified	May 11, 2014
retrieve system time in milliseconds
******************************************************************************/

//uses calloc, must include stdlib
#include <stdlib.h>
#include "nlib_sys_err.h"
#include "nlib_sys_systime.h"

//constructor, reserve memory
SysTimeObj sysTime_New(){
	SysTimeObj self = (SysTimeObj)calloc(1, sizeof(struct sysTimeObj));
	return self;
}

//initiate and test memory
int sysTime_Init(SysTimeObj self){
	if(self == NULL)
		return NLIB_ERR_NOMEM;
	
	//reset time counters
	self.rawTime = 0;
	self.lastCall = 0;
	
	return NLIB_ERR_NOERROR;
}

//process events, increment time, call every ms
void sysTime_ProcessEvents(){
	
	//increment
	ratTime++;
	
	//check for roll over
	
}