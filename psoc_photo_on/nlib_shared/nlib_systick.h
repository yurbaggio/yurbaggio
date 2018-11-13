/******************************************************************************
Jianyi Liu
Created		July 24, 2016
Modified	July 24, 2016
systick counter
******************************************************************************/

#ifndef NLIB_SYSTICK_H
#define NLIB_SYSTICK_H

//Module object
typedef struct systickObj * SystickObj;

struct systickObj{
    unsigned long ticksPerMs;
    unsigned long tickCount;
	unsigned long count;
};

//constructor, reserve memory
SystickObj systick_New();

//initializes with divider
int systick_Init(SystickObj self, unsigned long ticksPerMs_var);

//call every systick
void systick_ProcessEvents(SystickObj self);

//get the count
unsigned long systick_GetCount(SystickObj self);

#endif