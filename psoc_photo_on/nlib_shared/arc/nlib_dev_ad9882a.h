/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jul 1, 2010
Modified	Jun 24, 2011
header for ad9882a drivers
******************************************************************************/

#ifndef NLIB_DEV_AD9882A_H
#define NLIB_DEV_AD9882A_H

#define AD9882A_SYNCSTAT_AINT_VGA	0	//VGA interface is active
#define AD9882A_SYNCSTAT_AINT_DVI	1	//DVI interface is active
#define AD9882A_SYNCSTAT_DCLK_INACTIVE	0	//dvi clock active
#define AD9882A_SYNCSTAT_DCLK_ACTIVE	1	//dvi clock active

typedef struct ad9882aObj * Ad9882aObj;

//define a AD9882A Interface
typedef struct Ad9882aFuncStruct{
	int (*i2cPoll)(unsigned char addr);
	int (*i2cWrite)(unsigned char addr, unsigned char subaddr, unsigned char data);
	int (*i2cRead)(unsigned char addr, unsigned char subaddr);
}Ad9882aFunc;

//define a AD9882A Obj
struct ad9882aObj{
	Ad9882aFunc fTable;
	unsigned char devAddr;		//stores the i2c address
	unsigned char adcDelay;			//stores the adcDelay tick for the ad9882a
};

//constructor reserve memory
Ad9882aObj ad9882a_New();

//initiate an AD9882A device and associate address, function pointers must already be configured
int ad9882a_Init(Ad9882aObj self, Ad9882aFunc fTable_var, unsigned char devAddr_var, unsigned char adcDelay_var);

//check to see if device is present
int ad9882a_Check(Ad9882aObj self);

//set timing to 1280x1024
int ad9882a_Set12x10(Ad9882aObj self);

//change adc delay value
int ad9882a_SetAdcDelay(Ad9882aObj self, unsigned char value);

//gets the sync status
int ad9882a_GetActiveInterface(Ad9882aObj self);

//test to see if dvi clock active
int ad9882a_GetDviClk(Ad9882aObj self);

//set to dvi
int ad9882a_SetDvi(Ad9882aObj self);

//set to vga
int ad9882a_SetVga(Ad9882aObj self);

//set to auto
int ad9882a_SetAuto(Ad9882aObj self);

//start the device
void ad9882a_Start(Ad9882aObj self);

//stop the device
void ad9882a_Stop(Ad9882aObj self);

#endif
