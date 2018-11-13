/******************************************************************************
Engscope.com
Jianyi Liu
Created		Sep 19, 2015
Modified	Sep 19, 2015
motor look up table
******************************************************************************/

#include <stdlib.h>

#include "nlib_sys_err.h"
#include "nlib_sys_trig.h"
#include "nlib_drv_mot.h"

#define MOT_MAX_QUADSIZE	1024

//constructor, resrve memory
MotTableObj motTable_New(){
	MotTableObj self = (MotTableObj)calloc(1, sizeof(struct motTableObj));
	return self;
}

//initialization
int motTable_Init(MotTableObj self, float (*calcValue_var)(int index), int quadsize_var){
	if(self == NULL)
		return NLIB_ERR_NOMEM;

	//initiate values
	self->calcValue = calcValue_var;
	self->quadsize = quadsize_var;
	
	//check for max size
	if(self->quadsize >= MOT_MAX_QUADSIZE)
		return NLIB_ERR_UNKNOWN;
	
	//reserve table memory for one quadrant
	self->tableData = (float*)calloc(self->quadsize, sizeof(float));
	if (self->tableData == NULL)
		return NLIB_ERR_NOMEM;

	return NLIB_ERR_NOERROR;
}

//calculate table for one quadrant
void motTable_CalcQuadTable(MotTableObj self){
	int i;
	for( i = 0; i < self->quadsize; i ++){
		*(self->tableData + i) = self->calcValue(i);
	}
}

//get lut value based on index
float motTable_GetValue(MotTableObj self, int index){
	if(index >= self->quadsize)
		index = index%self->quadsize;
	
	return *(self->tableData + index);
}
