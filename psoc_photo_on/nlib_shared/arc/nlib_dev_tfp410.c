/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Mar 31, 2010
Modified	May 11, 2011
source for tfp410 drivers
******************************************************************************/

//uses calloc, must include stdlib
#include <stdlib.h>

//this file ties every project together
#include "nlib_sys_err.h"
#include "nlib_dev_tfp410.h"

//important register address
#define TFP410_PD		0x08

//PD settings
#define TFP410_PD_ON_RISE	0x37	//by default use rising edge
#define TFP410_PD_ON_FALL	0x35	
#define TFP410_PD_OFF		0xFE
#define TFP410_PD_MASK		0x01
#define TFP410_MSEN_RSEN	0x20

//contants for generating internal DE at 1280x1024
const static unsigned char tfp410DeInt12x10_rom[][2] = {
	{0x32, 0x67},	//Hsync Back Porch set (DE_DLY)
	{0x33, 0x71},	//Internal DE, syncs active low
	{0x34, 0x29},	//Vsync Back Porch set (DE_TOP)
	{0x36, 0x00},	//Pixel Count set (DE_CNT)
	{0x37, 0x05},
	{0x38, 0x00},	//Line Count set (DE_LN)
	{0x39, 0x04}
};

//contants for generating internal DE at 1280x1024 on AD9882A, VGA
const static unsigned char tfp410DeInt12x10Vga_rom[][2] = {
	{0x32, 0x6B},	//Hsync Back Porch set (DE_DLY)
	{0x33, 0x71},	//Internal DE, syncs active low
	{0x34, 0x29},	//Vsync Back Porch set (DE_TOP)
	{0x36, 0x00},	//Pixel Count set (DE_CNT)
	{0x37, 0x05},
	{0x38, 0x00},	//Line Count set (DE_LN)
	{0x39, 0x04}
};

//constants for turning the internal DE off
const static unsigned char tfp410DeExt_rom[][2] = {
	{0x33, 0x00}	//reset DE back to default
};	

//constant for turning the device on
const static unsigned char tfp410PdOnRise_rom[][2] = {
	{0x08, TFP410_PD_ON_RISE}	//Power down off, latch on rising
};
const static unsigned char tfp410PdOnFall_rom[][2] = {
	{0x08, TFP410_PD_ON_FALL}	//Power down off, latch on rising
};
	
//constant for turning the device off (pd)
const static unsigned char tfp410PdOff_rom[][2] = {
	{0x08, TFP410_PD_OFF}	//Power down on, latch on rising
};

//constant for setting MSEN to RSEN
const static unsigned char tfp410SetRsen_rom[][2] = {
	{0x09, TFP410_MSEN_RSEN}
};

//constructor, reserve memory
Tfp410Obj tfp410_New(){
	Tfp410Obj self = (Tfp410Obj)calloc(1, sizeof(struct tfp410Obj));
	return self;
}

//initiate a TFP410 device and assiciate address, function pointers must already be configured
int tfp410_Init(Tfp410Obj self, Tfp410Func fTable_var, unsigned char devAddr_var){
	if(self == NULL)
		return NLIB_ERR_NOMEM;
		
	//initiate values
	self->devAddr	= devAddr_var;
	self->fTable	= fTable_var;
		
	return NLIB_ERR_NOERROR;
}	

//check to see if device is present
int tfp410_Check(Tfp410Obj self){
	if(self->fTable.i2cPoll(self->devAddr) != 0)
		return NLIB_ERR_NOPOLL;
	return NLIB_ERR_NOERROR;
}

//set DE to external
void tfp410_SetDeExt(Tfp410Obj self){
	self->fTable.i2cWrite(self->devAddr, tfp410DeExt_rom[0][0], tfp410DeExt_rom[0][1]);
}

//generate DE for 12x10 for DVI decoder
void tfp410_SetDeInt12x10(Tfp410Obj self){
	//load rom string
	unsigned int length = sizeof(tfp410DeInt12x10_rom)/sizeof(tfp410DeInt12x10_rom[0]);
	unsigned int i;
	for( i = 0; i< length; i ++)
		self->fTable.i2cWrite(self->devAddr, tfp410DeInt12x10_rom[i][0], tfp410DeInt12x10_rom[i][1]);
}

//generate DE for 12x10 ad9882 on VGA
void tfp410_SetDeInt12x10Vga(Tfp410Obj self){
	//load rom string
	unsigned int length = sizeof(tfp410DeInt12x10Vga_rom)/sizeof(tfp410DeInt12x10Vga_rom[0]);
	unsigned int i;
	for( i = 0; i< length; i ++)
		self->fTable.i2cWrite(self->devAddr, tfp410DeInt12x10Vga_rom[i][0], tfp410DeInt12x10Vga_rom[i][1]);
}


//turn device on
void tfp410_StartRise(Tfp410Obj self){
	self->fTable.i2cWrite(self->devAddr, tfp410PdOnRise_rom[0][0], tfp410PdOnRise_rom[0][1]);
}
void tfp410_StartFall(Tfp410Obj self){
	self->fTable.i2cWrite(self->devAddr, tfp410PdOnFall_rom[0][0], tfp410PdOnFall_rom[0][1]);
}

//turn device off
void tfp410_Stop(Tfp410Obj self){
	self->fTable.i2cWrite(self->devAddr, tfp410PdOff_rom[0][0], tfp410PdOff_rom[0][1]);
}

//returns PD register status
int tfp410_GetPd(Tfp410Obj self){
	return self->fTable.i2cRead(self->devAddr, TFP410_PD) & TFP410_PD_MASK;
}

//sets the MSEL bits in CTL_2_MODE to RSEN
void tfp410_SetRsen(Tfp410Obj self){
	self->fTable.i2cWrite(self->devAddr, tfp410SetRsen_rom[0][0], tfp410SetRsen_rom[0][1]);
}
