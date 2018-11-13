/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Mar 31, 2010
Modified	May 11, 2011
header for tfp410 drivers
******************************************************************************/

#ifndef NLIB_DEV_TFP410_H
#define NLIB_DEV_TFP410_H

typedef struct tfp410Obj * Tfp410Obj;

//define a TFP410 Interface
typedef struct Tfp410FuncStruct{
	int (*i2cPoll)(unsigned char addr);
	int (*i2cWrite)(unsigned char addr, unsigned char subaddr, unsigned char data);
	int (*i2cRead)(unsigned char addr, unsigned char subaddr);
}Tfp410Func;

//define a TFP410 object
struct tfp410Obj{
	Tfp410Func fTable;
	unsigned char devAddr;
};

//constructor, reserve memory
Tfp410Obj tfp410_New();

//initiate a TFP410 device and assiciate address, function pointers must already be configured
int tfp410_Init(Tfp410Obj self, Tfp410Func fTable_var, unsigned char devAddr_var);

//check to see if device is present
int tfp410_Check(Tfp410Obj self);

//set DE to external
void tfp410_SetDeExt(Tfp410Obj self);

//generate DE for 12x10 for DVI decoder
void tfp410_SetDeInt12x10(Tfp410Obj self);

//generate DE for 12x10 ad9882 on VGA
void tfp410_SetDeInt12x10Vga(Tfp410Obj self);

//turn device on
void tfp410_StartRise(Tfp410Obj self);
void tfp410_StartFall(Tfp410Obj self);

//turn device off
void tfp410_Stop(Tfp410Obj self);

//returns PD register status
int tfp410_GetPd(Tfp410Obj self);

//sets the MSEL bits in CTL_2_MODE to RSEN
void tfp410_SetRsen(Tfp410Obj self);

#endif
