/******************************************************************************
Engscope.com
Jianyi Liu
Created		Sep 19, 2015
Modified	Sep 19, 2015
motor look up table
******************************************************************************/

#ifndef NLIB_DRV_MOT_H
#define NLIB_DRV_MOT_H

typedef struct motTableObj * MotTableObj;

//define motTable object
struct motTableObj{
	float (*calcValue)(int index);	//calculates the values at index
	float *tableData;	//points to data
	int quadsize;			//table size
};

//constructor, resrve memory
MotTableObj motTable_New();

//initialization
//calcValue is the point to function to calculate the table
int motTable_Init(MotTableObj self, float (*calcValue_var)(int index), int quadsize_var);

//calculate table for one quadrant, should include the value at rad = 0 and rad = PI/2
//for example, if size = 10, value at index = 9 refers to value at rad = PI/2
//this means that if size = 10, then there are 9 values per quadrant
void motTable_CalcQuadTable(MotTableObj self);

//get lut value based on index
float motTable_GetValue(MotTableObj self, int index);

#endif