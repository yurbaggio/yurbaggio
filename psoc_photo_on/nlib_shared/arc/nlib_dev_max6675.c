/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jul 1, 2010
Modified	Jul 1, 2010
source for eeproms, spi
******************************************************************************/

//uses calloc, must include stdlib
#include <stdlib.h>

#include "nlib_sys_err.h"
#include "nlib_dev_max6675.h"

//shifts for converting
#define MAX6675_LSBINTSHIFT	5
#define MAX6675_MSBINTSHIFT	3

//masks for acquiring date
#define MAX6675_INPUTSHIFT	2
#define MAX6675_LSBSHIFT	3
#define MAX6675_MSBSHIFT	5
#define MAX6675_INPUTMASK	(1 << MAX6675_INPUTSHIFT)
#define MAX6675_LSBMASK		(0x1F << MAX6675_LSBSHIFT)
#define MAX6675_LSBFRACMASK	(0x03 << MAX6675_LSBSHIFT)
#define MAX6675_MSBMASK		0x7F

//local prototypes
float max6675_GetTemperature(Max6675Obj self);
float max6675_GetTempAvg(Max6675Obj self, int n);
int max6675_GetInput(Max6675Obj self);

//create interface
static Max6675Int iTable_var =
{
	max6675_GetTemperature,
	max6675_GetTempAvg,
	max6675_GetInput
};

//constructor reserve memory
Max6675Obj max6675_New(){
	Max6675Obj self = (Max6675Obj)calloc(1, sizeof(struct max6675Obj));
	return self;
}

//initiate interface to the EE prom
int max6675_Init(Max6675Obj self, Max6675Func fTable_var)
{
	if(self == NULL)
		return NLIB_ERR_NOMEM;

	//initiate values
	self->fTable	= fTable_var;
	self->iTable	= iTable_var;

	//assert the ss signal high
	self->fTable.max6675Ss(1);

	return NLIB_ERR_NOERROR;
}

//get temperature from device
float max6675_GetTemperature(Max6675Obj self)
{
	unsigned char retLo, retHi;
	int retInt;
	int retFrac;
	float ret;

    self->fTable.max6675Ss(0);

    retHi = self->fTable.spiWrite(0) & MAX6675_MSBMASK;
	retLo = self->fTable.spiWrite(0) & MAX6675_LSBMASK;
	 
    self->fTable.max6675Ss(1);
    
    retInt = (retHi << MAX6675_MSBINTSHIFT) + (retLo >> MAX6675_LSBINTSHIFT);
	retFrac = (retLo & MAX6675_LSBFRACMASK) >> MAX6675_LSBSHIFT;

	ret = (float)retInt + (float)retFrac*0.25;

    return ret;
}

//get temperature n times, average
float max6675_GetTempAvg(Max6675Obj self, int n)
{
	float ret = 0;
	int i;
	for(i = 0; i < n; i ++)
		ret += max6675_GetTemperature(self);
	
    return (ret/n);
}

//get input detection from device
int max6675_GetInput(Max6675Obj self)
{
	unsigned char retLo;
	unsigned char ret;

    self->fTable.max6675Ss(0);

    self->fTable.spiWrite(0);
	retLo = self->fTable.spiWrite(0) & MAX6675_INPUTMASK;
	 
    self->fTable.max6675Ss(1);
    
    ret = retLo >> MAX6675_INPUTSHIFT;
    return ret;
}
