/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Mar 31, 2010
Modified	May 11, 2011
source for ths8200 drivers
******************************************************************************/

//uses calloc, must include stdlib
#include <stdlib.h>

//this file ties every project together
#include "nlib_sys_err.h"
#include "nlib_sys_vid.h"
#include "nlib_dev_ths8200.h"

//important register address
#define THS8200_CHIPCTL		0x03
#define THS8200_CHIPCTL_ON	0x81
#define THS8200_CHIPCTL_OFF	0x85
#define THS8200_CHIPCTL_TEST	0xA1

//turn devce on
const static unsigned char ths8200PdOn_rom[][2] = {
	{0x03, THS8200_CHIPCTL_ON}
};

//turn device off
const static unsigned char ths8200PdOff_rom[][2] = {
	{0x03, THS8200_CHIPCTL_OFF}
};

//test values
const static unsigned char ths8200Test_rom[][2] = {
	{0x03, THS8200_CHIPCTL_TEST}
};

//init values
const static unsigned char ths8200Init_rom[][2] = {
	{0x1c, 0x38}, //set datapath
	{0x1d, 0x00}, //set blanking level to 0
	{0x1e, 0x00},
	{0x1f, 0x00},
	{0x20, 0x00},
	{0x21, 0x00},
	{0x22, 0x00},
	{0x23, 0x00},
	{0x24, 0x00},
	{0x38, 0x87}, //set to vesa slave
	{0x39, 0x0F},
	{0x3b, 0xFF}, //fill with padding for vertical field size
	{0x72, 0x01},
	{0x75, 0x01},
	{0x76, 0x00},
	{0x77, 0x0F},
	{0x78, 0xFF},
	{0x79, 0x00}, //set h offset, set to 0 for progressive
	{0x7a, 0x01}, 
	{0x7b, 0x00}, //set v offset, set to 0 for progressive
	{0x7c, 0x01},
	{0x82, 0xDF}  //set output
};

//constructor, reserve memory
Ths8200Obj ths8200_New(){
	Ths8200Obj self = (Ths8200Obj)calloc(1, sizeof(struct ths8200Obj));
	return self;
}

//initiate a THS8200 device and assiciate address, function pointers must already be configured
int ths8200_Init(Ths8200Obj self, Ths8200Func fTable_var, unsigned char devAddr_var){
	if(self == NULL)
		return NLIB_ERR_NOMEM;
		
	//initiate values
	self->devAddr	= devAddr_var;
	self->fTable	= fTable_var;
		
	return NLIB_ERR_NOERROR;
}	

//check to see if device is present
int ths8200_Check(Ths8200Obj self){
	if(self->fTable.i2cPoll(self->devAddr) != 0)
		return NLIB_ERR_NOPOLL;
	return NLIB_ERR_NOERROR;
}

//send init values to device
void ths8200_SetInit(Ths8200Obj self){
	//load rom string
	unsigned int length = sizeof(ths8200Init_rom)/sizeof(ths8200Init_rom[0]);
	unsigned int i;
	for( i = 0; i< length; i ++)
		self->fTable.i2cWrite(self->devAddr, ths8200Init_rom[i][0], ths8200Init_rom[i][1]);
}

//set resolution
void ths8200_SetRes(Ths8200Obj self, unsigned char vidRes){
	//get timing information
	VidObj res = vid_GetTiming(vidRes);
	
	//set total horizontal pixels
	self->fTable.i2cWrite(self->devAddr, 0x34, NIB2CHAR(0, INTHI(res.hs_total)));
	self->fTable.i2cWrite(self->devAddr, 0x35, INTLO(res.hs_total));

	//set total vertical lines
	self->fTable.i2cWrite(self->devAddr, 0x39, NIB2CHAR(INTHI(res.vs_total), 0xF));
	self->fTable.i2cWrite(self->devAddr, 0x3a, INTLO(res.vs_total));

	//set h sync porch, front porch
	self->fTable.i2cWrite(self->devAddr, 0x70, INTLO(res.hs_sp));
	self->fTable.i2cWrite(self->devAddr, 0x71, INTHI(res.hs_sp) << 6);

	//set v sync porch, front porch
	self->fTable.i2cWrite(self->devAddr, 0x73, INTLO(res.vs_sp));
	self->fTable.i2cWrite(self->devAddr, 0x74, INTHI(res.vs_sp) << 6);
}

//turn device on
void ths8200_Start(Ths8200Obj self){
	self->fTable.i2cWrite(self->devAddr, ths8200PdOn_rom[0][0], ths8200PdOn_rom[0][1]);
}

//turn device off
void ths8200_Stop(Ths8200Obj self){
	self->fTable.i2cWrite(self->devAddr, ths8200PdOff_rom[0][0], ths8200PdOff_rom[0][1]);
}

//put test bars
void ths8200_Test(Ths8200Obj self){
	self->fTable.i2cWrite(self->devAddr, ths8200Test_rom[0][0], ths8200Test_rom[0][1]);
}
