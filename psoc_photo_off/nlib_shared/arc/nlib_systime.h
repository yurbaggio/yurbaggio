/******************************************************************************
Engscope
Jianyi Liu
Created		May 11, 2014
Modified	May 11, 2014
retrieve system time in milliseconds
******************************************************************************/

#ifndef NLIB_SYS_SYSTIME_H
#define NLIB_SYS_SYSTIME_H

typedef struct sysTimeObj * SysTimeObj;

//define the object
struct sysTimeObj{
	int rawTime;
	int lastCall;
};

#endif
