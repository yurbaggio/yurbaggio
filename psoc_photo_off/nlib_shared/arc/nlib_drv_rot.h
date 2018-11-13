/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Dec 1, 2010
Modified	Nov 9, 2011
Rotary Encoder Header
******************************************************************************/

#ifndef NLIB_DRV_ROT_H
#define NLIB_DRV_ROT_H

typedef struct rotObj * RotObj;

//define a rotary encoder function interface
typedef struct RotFuncStruct{
	int (*sampleTapA)(void);
	int (*sampleTapB)(void);
}RotFunc;

//define rotary encoder object
struct rotObj{
	RotFunc fTable;
	unsigned int state;
	//unsigned int counterTapA;
	//unsigned int counterTapB;
	unsigned aFlag:1;
	unsigned bFlag:1;
	unsigned rotCw:1;
	unsigned rotCcw:1;
};

//constructor, reserve memory
RotObj rot_New();

//initializer, use a different intializer depending on if QP is used
int rot_Init(RotObj self,  RotFunc fTable_var);

//process events
void rot_Proc(RotObj self);

//see if encoder is turned
int rot_cw(RotObj self);
int rot_ccw(RotObj self);

#endif
