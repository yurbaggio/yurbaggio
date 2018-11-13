/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Mar 31, 2010
Modified	May 11, 2011
header for ths8200 drivers
******************************************************************************/

#ifndef NLIB_DEV_THS8200_H
#define NLIB_DEV_THS8200_H

typedef struct ths8200Obj * Ths8200Obj;

//define a THS8200 Interface
typedef struct Ths8200FuncStruct{
	int (*i2cPoll)(unsigned char addr);
	int (*i2cWrite)(unsigned char addr, unsigned char subaddr, unsigned char data);
	int (*i2cRead)(unsigned char addr, unsigned char subaddr);
}Ths8200Func;

//define a THS8200 object
struct ths8200Obj{
	Ths8200Func fTable;
	unsigned char devAddr;
};

//constructor, reserve memory
Ths8200Obj ths8200_New();

//initiate a THS8200 device and assiciate address, function pointers must already be configured
int ths8200_Init(Ths8200Obj self, Ths8200Func fTable_var, unsigned char devAddr_var);

//check to see if device is present
int ths8200_Check(Ths8200Obj self);

//send init values to device
void ths8200_SetInit(Ths8200Obj self);

//set resolution
void ths8200_SetRes(Ths8200Obj self, unsigned char vidRes);

//turn device on
void ths8200_Start(Ths8200Obj self);

//turn on test bars
void ths8200_Test(Ths8200Obj self);

//turn device off
void ths8200_Stop(Ths8200Obj self);

#endif
