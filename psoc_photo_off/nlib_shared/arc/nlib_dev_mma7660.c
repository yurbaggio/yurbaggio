/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Oct 12, 2011
Modified	Oct 12, 2011
source for mma7660fc accelerometer drivers
******************************************************************************/

//uses calloc, must include stdlib
#include <stdlib.h>

#include "nlib_sys_err.h"
#include "nlib_dev_mma7660.h"

#define MMA7660_MODE_OFF	0x00
#define MMA7660_MODE_ON		0xC1
#define MMA7660_PDET_MASK	0b00011111

//contants for generating setting pll to accept 1280x1024
const static unsigned char mma7660TapDetect_rom[][2] = {
	{0x07, MMA7660_MODE_OFF},	//turn device mode off first
	{0x05, 0x00},	//sleep counter
	{0x06, 0b11100111},	//interrupt setup, tap detect, 3 axis tilt
	{0x08, 0x00},	//auto-wake
	{0x09, 0x03},	//tap detect
	{0x0A, 0x10},	//tap debounce
	{0x07, MMA7660_MODE_ON},	//turn device mode on, interrupt active high (trigger on rising edge
};

//constructor reserve memory
Mma7660Obj mma7660_New(){
	Mma7660Obj self = (Mma7660Obj)calloc(1, sizeof(struct mma7660Obj));
	return self;
}

//initiate an MMA7660 device and associate address, function pointers must already be configured
int mma7660_Init(Mma7660Obj self, Mma7660Func fTable_var, unsigned char devAddr_var){
	if(self == NULL)
		return NLIB_ERR_NOMEM;

	//initiate values
	self->devAddr	= devAddr_var;
	self->fTable	= fTable_var;

	return NLIB_ERR_NOERROR;
}

//reset interrupt, done by reading from address 0x03
int mma7660_ResetInt(Mma7660Obj self){
	return self->fTable.i2cRead(self->devAddr, MMA7660_ADDR_TILT);
}

//change the sensitivity
void mma7660_SetSensitivity(Mma7660Obj self, int sen){
	self->fTable.i2cWrite(self->devAddr, MMA7660_ADDR_MODE, MMA7660_MODE_OFF);	//turn accel off
	self->fTable.i2cWrite(self->devAddr, MMA7660_ADDR_PDET, sen & MMA7660_PDET_MASK);	//change sensitivity
	self->fTable.i2cWrite(self->devAddr, MMA7660_ADDR_MODE, MMA7660_MODE_ON);	//turn accel on
}

//check to see if device is present
int mma7660_Check(Mma7660Obj self){
	if(self->fTable.i2cPoll(self->devAddr) != 0)
		return NLIB_ERR_NOPOLL;
	return NLIB_ERR_NOERROR;
}

//set timing to 1280x1024
int mma7660_SetTapDetect(Mma7660Obj self){
	//load rom string
	unsigned int length = sizeof(mma7660TapDetect_rom)/sizeof(mma7660TapDetect_rom[0]);
	unsigned int i;
	for( i = 0; i< length; i ++)
		self->fTable.i2cWrite(self->devAddr, mma7660TapDetect_rom[i][0], mma7660TapDetect_rom[i][1]);
}
