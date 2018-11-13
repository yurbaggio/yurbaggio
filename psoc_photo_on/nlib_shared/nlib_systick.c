/******************************************************************************
Jianyi Liu
Created		July 24, 2016
Modified	July 24, 2016
systick counter
******************************************************************************/

#include <stdlib.h>
#include "nlib_systick.h"
#include "nlib_err.h"

//constructor, reserve memory
SystickObj systick_New(){
	SystickObj self = (SystickObj) calloc(1, sizeof(struct systickObj));
	return self;
}

//initializes with divider
int systick_Init(SystickObj self, unsigned long ticksPerMs_var){
    
    if (self == NULL)
		return NLIB_ERR_NOMEM;

	self->ticksPerMs = ticksPerMs_var;
    self->tickCount = 0;
    self->count = 0;
    
	return NLIB_ERR_NOERROR;
}

//call every systick
void systick_ProcessEvents(SystickObj self){
    self->tickCount++;
    if (self->tickCount >= self->ticksPerMs){
        self->tickCount = 0;
        self->count++;
    }
}

//get the count
unsigned long systick_GetCount(SystickObj self){
    return self->count;
}