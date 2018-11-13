/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Dec 1, 2010
Modified	Dec 1, 2010
Maxim IC MAX3845 Integrated MAX3845 Header
******************************************************************************/

#ifndef NLIB_DEV_MAX3845_H
#define NLIB_DEV_MAX3845_H

typedef struct max3845Obj * Max3845Obj;

//define a max3845 Interface
typedef struct Max3845FuncStruct{
	int (*input0)(void);
	int (*input1)(void);
	void (*output0)(int);
	void (*output1)(int);
	void (*output2)(int);
	void (*output3)(int);
}Max3845Func;

//define max3845 object
struct max3845Obj{
	Max3845Func		fTable;
	int prevInput0;
	int prevInput1;
};

//constructor, resrve memory
Max3845Obj max3845_New();

//initialization
int max3845_Init(Max3845Obj self, Max3845Func fTable_var);

//detect input change
int max3845_HasChanged(Max3845Obj self);

//force port0, regardless of input
void mx3845_SetInput0(Max3845Obj self);

//force port1, regardless of input
void mx3845_SetInput1(Max3845Obj self);

//auto split, according to inputs
void max3845_SetAuto(Max3845Obj self);

#endif
