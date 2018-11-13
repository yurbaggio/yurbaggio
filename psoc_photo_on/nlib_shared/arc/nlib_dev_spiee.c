/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jul 1, 2010
Modified	Jul 1, 2010
source for eeproms, spi
******************************************************************************/

//uses calloc, must include stdlib
#include <stdlib.h>

#include "nlib_sys_conv.h"
#include "nlib_sys_err.h"
#include "nlib_dev_spiee.h"

//instruction set
#define SPIEE_CMD_READ     (unsigned)0b00000011
#define SPIEE_CMD_WRITE    (unsigned)0b00000010
#define SPIEE_CMD_WRDI     (unsigned)0b00000100
#define SPIEE_CMD_WREN     (unsigned)0b00000110
#define SPIEE_CMD_RDSR     (unsigned)0b00000101
#define SPIEE_CMD_WRSR     (unsigned)0b00000001

//define masks for following structure
//typedef struct SpiEeStatusStruct{
//	unsigned    WIP:1;
//	unsigned    WEL:1;
//	unsigned    BP0:1;
//	unsigned    BP1:1;
//	unsigned    RESERVED:3;
//	unsigned    WPEN:1;
//}SpiEeStatus;

#define SPIEE_STATMASK_WIP	1<<0
#define SPIEE_STATMASK_WEL	1<<1

//local prototypes
void spiEe_ByteWrite(SpiEeObj self, unsigned int addr, unsigned char data);
unsigned char spiEe_ByteRead(SpiEeObj self, unsigned int addr);
void spiEe_WriteEnable(SpiEeObj self);
void spiEe_WriteDisable(SpiEeObj self);
unsigned char spiEe_Poll(SpiEeObj self);
unsigned char spiEe_GetStatus(SpiEeObj self);

//create interface
static SpiEeInt iTable_var =
{
	spiEe_ByteWrite,
	spiEe_ByteRead,
	spiEe_Poll
};

//constructor reserve memory
SpiEeObj spiEe_New(){
	SpiEeObj self = (SpiEeObj)calloc(1, sizeof(struct spiEeObj));
	return self;
}

//initiate interface to the EE prom
int spiEe_Init(SpiEeObj self, SpiEeFunc fTable_var)
{
	if(self == NULL)
		return NLIB_ERR_NOMEM;

	//initiate values
	self->fTable	= fTable_var;
	self->iTable	= iTable_var;

	//assert the ss signal high
	self->fTable.spiEeSs(1);

	return NLIB_ERR_NOERROR;
}

//writes to the eeprom device
void spiEe_ByteWrite(SpiEeObj self, unsigned int addr, unsigned char data)
{
	unsigned char var;

    spiEe_WriteEnable(self);
    self->fTable.spiEeSs(0);

    var = self->fTable.spiWrite(SPIEE_CMD_WRITE);
    var = self->fTable.spiWrite(INTHI(addr));
    var = self->fTable.spiWrite(INTLO(addr));
    var = self->fTable.spiWrite(data);

    self->fTable.spiEeSs(1);

    // wait for completion of previous write operation
    while(spiEe_GetStatus(self) & SPIEE_STATMASK_WIP);
    
    spiEe_WriteDisable(self);
}

//reads from the eeprom device
unsigned char spiEe_ByteRead(SpiEeObj self, unsigned int addr)
{
	unsigned char var;

    self->fTable.spiEeSs(0);

    var = self->fTable.spiWrite(SPIEE_CMD_READ);

    var = self->fTable.spiWrite(INTHI(addr));
    var = self->fTable.spiWrite(INTLO(addr));
    var = self->fTable.spiWrite(0);

    self->fTable.spiEeSs(1);
    return var;
}

//enable write by changing status register
void spiEe_WriteEnable(SpiEeObj self)
{
	unsigned char var;

    self->fTable.spiEeSs(0);
    var = self->fTable.spiWrite(SPIEE_CMD_WREN);
    self->fTable.spiEeSs(1);
}

//disable write by changing status register
void spiEe_WriteDisable(SpiEeObj self)
{
	unsigned char var;

    self->fTable.spiEeSs(0);
    var = self->fTable.spiWrite(SPIEE_CMD_WRDI);
    self->fTable.spiEeSs(1);
}

//read the status regsiter
unsigned char spiEe_GetStatus(SpiEeObj self)
{
	unsigned char var;

	self->fTable.spiEeSs(0);
    var = self->fTable.spiWrite(SPIEE_CMD_RDSR);
    var = self->fTable.spiWrite(0);
    self->fTable.spiEeSs(1);

	return (var);
}

//polls to see if the SPI EEPROM is present
unsigned char spiEe_Poll(SpiEeObj self)
{
	unsigned char var;

	spiEe_WriteEnable(self);
	var = spiEe_GetStatus(self) & SPIEE_STATMASK_WEL;
	spiEe_WriteDisable(self);
	return var;
}
