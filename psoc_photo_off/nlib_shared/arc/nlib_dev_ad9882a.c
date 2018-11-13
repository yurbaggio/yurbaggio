/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jul 1, 2010
Modified	Jun 24, 2011
source for ad9882a drivers
******************************************************************************/

//uses calloc, must include stdlib
#include <stdlib.h>

//this file ties every project together
#include "nlib_sys_err.h"
#include "nlib_dev_ad9882a.h"

//important register address
#define AD9882A_SYNCSTAT 0x15
#define AD9882A_ADCDELAY 0x04

//masks, constants for Synstatus
#define AD9882A_SYNCSTAT_AINT	1<<3	//active interface
#define AD9882A_SYNCSTAT_DCLK	1<<4	//dvi clock present

//contants for generating setting pll to accept 1280x1024
const static unsigned char ad9882aRes12x10_rom[][2] = {
	{0x01, 0x69}, //pll msb, to pixel count 1688
	{0x02, 0x90}, //pll lsb
	{0x03, 0xA8}, //set vco, pll loop filter
	//{0x04, 0x80}, //set ADC delay
	{0x05, 0x01}, //clamp placement
	{0x06, 0xF8}, //clamp duration
	{0x07, 0x74}, //HSync width
	{0x08, 0x3C}, //set gains (contrast)
	{0x09, 0x38}, //
	{0x0A, 0x30}, //
	{0x0B, 0xD8}, //set offset (brightness)
	{0x0C, 0xD0}, 
	{0x0D, 0xC8}, // 
	{0x0F, 0x7C}, //set to analog
	{0x10, 0x04}, //hsync active high, vsync active high
	{0x11, 0x83}, //disable coast, enable clamp clamp
	{0x17, 0x80}, //set according to datasheet
	{0x18, 0xC0}, //set according to datasheet
	{0x19, 0x7C} //set according to datasheet
};

//constants for switching to DVI
const static unsigned char ad9882aDvi_rom[][2] = {
	{0x0F, 0x7E}
};	

//constants for switching to VGA
const static unsigned char ad9882aVga_rom[][2] = {
	{0x0F, 0x7C}
};

//constants for setting to auto
const static unsigned char ad9882aAuto_rom[][2] = {
	{0x0F, 0x78}
};	

//constant for setting PD on
const static unsigned char ad9882aPdOn_rom[][2] = {
	{0x14, 0xE2}
};

//constant for setting PD off
const static unsigned char ad9882aPdOff_rom[][2] = {
	{0x14, 0xE0}
};

//constructor reserve memory
Ad9882aObj ad9882a_New(){
	Ad9882aObj self = (Ad9882aObj)calloc(1, sizeof(struct ad9882aObj));
	return self;
}

//initiate an AD9882A device and associate address, function pointers must already be configured
int ad9882a_Init(Ad9882aObj self, Ad9882aFunc fTable_var, unsigned char devAddr_var, unsigned char adcDelay_var){
	if(self == NULL)
		return NLIB_ERR_NOMEM;

	//initiate values
	self->devAddr	= devAddr_var;
	self->adcDelay	= adcDelay_var;
	self->fTable	= fTable_var;

	return NLIB_ERR_NOERROR;
}

//check to see if device is present
int ad9882a_Check(Ad9882aObj self){
	if(self->fTable.i2cPoll(self->devAddr) != 0)
		return NLIB_ERR_NOPOLL;
	return NLIB_ERR_NOERROR;
}

//set timing to 1280x1024
int ad9882a_Set12x10(Ad9882aObj self){
	//load rom string
	unsigned int length = sizeof(ad9882aRes12x10_rom)/sizeof(ad9882aRes12x10_rom[0]);
	unsigned int i;
	for( i = 0; i< length; i ++)
		self->fTable.i2cWrite(self->devAddr, ad9882aRes12x10_rom[i][0], ad9882aRes12x10_rom[i][1]);
}

//change adc delay value
int ad9882a_SetAdcDelay(Ad9882aObj self, unsigned char value){
	self->fTable.i2cWrite(self->devAddr, AD9882A_ADCDELAY, value);
}

//gets the sync status
int ad9882a_GetActiveInterface(Ad9882aObj self){
	return (self->fTable.i2cRead(self->devAddr, AD9882A_SYNCSTAT) & AD9882A_SYNCSTAT_AINT)>>3;
}

//test to see if dvi clock active
int ad9882a_GetDviClk(Ad9882aObj self){
	return (self->fTable.i2cRead(self->devAddr, AD9882A_SYNCSTAT) & AD9882A_SYNCSTAT_DCLK)>>4;
}

//set to dvi
int ad9882a_SetDvi(Ad9882aObj self){
	self->fTable.i2cWrite(self->devAddr, ad9882aDvi_rom[0][0], ad9882aDvi_rom[0][1]);
}	

//set to vga
int ad9882a_SetVga(Ad9882aObj self){
	self->fTable.i2cWrite(self->devAddr, ad9882aVga_rom[0][0], ad9882aVga_rom[0][1]);
}

//set to auto
int ad9882a_SetAuto(Ad9882aObj self){
	self->fTable.i2cWrite(self->devAddr, ad9882aAuto_rom[0][0], ad9882aAuto_rom[0][1]);
}	

//start the device
void ad9882a_Start(Ad9882aObj self){
	//turn the device on
	self->fTable.i2cWrite(self->devAddr, ad9882aPdOn_rom[0][0], ad9882aPdOn_rom[0][1]);
}

//stop the device
void ad9882a_Stop(Ad9882aObj self){
	//turn the device off
	self->fTable.i2cWrite(self->devAddr, ad9882aPdOff_rom[0][0], ad9882aPdOff_rom[0][1]);
}
