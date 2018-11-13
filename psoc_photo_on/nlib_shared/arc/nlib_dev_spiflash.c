/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jul 1, 2010
Modified	Jul 1, 2010
source for flash, spi
******************************************************************************/

//for use with flash chips with larger than 16 bit addressable space
//example: M25Pxx line of flash memories

//uses calloc, must include stdlib
#include <stdlib.h>

#include "nlib_sys_conv.h"
#include "nlib_sys_err.h"
#include "nlib_dev_spiflash.h"

//instruction set
#define SPIFLASH_CMD_WRDI		(unsigned)0b00000100	//write enable
#define SPIFLASH_CMD_WREN		(unsigned)0b00000110	//write disable
#define SPIFLASH_CMD_READ_ID	(unsigned)0b10011111	//read chip id
#define SPIFLASH_CMD_RDSR		(unsigned)0b00000101	//read status
#define SPIFLASH_CMD_WRSR		(unsigned)0b00000001	//write status
#define SPIFLASH_CMD_READ		(unsigned)0b00000011	//read
#define SPIFLASH_CMD_WRITE		(unsigned)0b00000010	//write
#define SPIFLASH_CMD_BERASE		(unsigned)0b11000111	//bulk erase
#define SPIFLASH_CMD_SERASE		(unsigned)0b11011000	//sector erase

//define masks for following structure
//struct for the status register
//struct  SPIFLASH_STATREG{
//	unsigned	WIP:1;
//	unsigned	WEL:1;
//	unsigned	BP0:1;
//	unsigned	BP1:1;
//	unsigned	BP2:1;
//	unsigned	RESERVED:2;
//	unsigned	WPEN:1;
//};

#define SPIFLASH_STATMASK_WIP	1<<0
#define SPIFLASH_STATMASK_WEL	1<<1

//local prototypes
void spiFlash_ByteWrite(SpiFlashObj self, unsigned int Sector, unsigned int Address, unsigned char Data);
unsigned char spiFlash_ByteRead(SpiFlashObj self, unsigned int Sector, unsigned int Address);
void spiFlash_WriteEnable(SpiFlashObj self);
void spiFlash_WriteDisable(SpiFlashObj self);
unsigned char spiFlash_GetStatus(SpiFlashObj self);
unsigned char spiFlash_GetWipStatus(SpiFlashObj self);
void spiFlash_BulkErase(SpiFlashObj self);
void spiFlash_SeqStart(SpiFlashObj self, unsigned char dataH, unsigned char dataM, unsigned char dataL);
void spiFlash_SeqWrite(SpiFlashObj self, unsigned char data);
void spiFlash_SeqFinish(SpiFlashObj self);
unsigned char spiFlash_Poll(SpiFlashObj self);

//create interface
static SpiFlashInt iTable_var =
{
	spiFlash_GetWipStatus,
	spiFlash_ByteRead,
	spiFlash_BulkErase,
	spiFlash_SeqStart,
	spiFlash_SeqWrite,
	spiFlash_SeqFinish
};

//constructor reserve memory
SpiFlashObj spiFlash_New(){
	SpiFlashObj self = (SpiFlashObj)calloc(1, sizeof(struct spiFlashObj));
	return self;
}

//initiate interface to the EE prom
int spiFlash_Init(SpiFlashObj self, SpiFlashFunc fTable_var)
{
	if(self == NULL)
		return NLIB_ERR_NOMEM;

	//initiate values
	self->fTable	= fTable_var;
	self->iTable	= iTable_var;

	//assert the ss signal high
	self->fTable.spiFlashSs(1);

	return NLIB_ERR_NOERROR;
}

//writes to the flash device
void spiFlash_ByteWrite(SpiFlashObj self, unsigned int Sector, unsigned int Address, unsigned char Data)
{
	unsigned char var;
	
    spiFlash_WriteEnable(self);
    self->fTable.spiFlashSs(0);

    var = self->fTable.spiWrite(SPIFLASH_CMD_WRITE);
	var = self->fTable.spiWrite(INTLO(Sector));
    var = self->fTable.spiWrite(INTHI(Address));
    var = self->fTable.spiWrite(INTLO(Address));
    var = self->fTable.spiWrite(Data);

    self->fTable.spiFlashSs(1);

    // wait for completion of previous write operation
    while(spiFlash_GetStatus(self) & SPIFLASH_STATMASK_WIP);
    
    spiFlash_WriteDisable(self);
}

//reads from the flash device
unsigned char spiFlash_ByteRead(SpiFlashObj self, unsigned int Sector, unsigned int Address)
{
	unsigned char var;

    self->fTable.spiFlashSs(0);

    var = self->fTable.spiWrite(SPIFLASH_CMD_READ);

	var = self->fTable.spiWrite(INTLO(Sector));
    var = self->fTable.spiWrite(INTHI(Address));
    var = self->fTable.spiWrite(INTLO(Address));

    var = self->fTable.spiWrite(0);

    self->fTable.spiFlashSs(1);
    return var;
}

//enable write by changing status register
void spiFlash_WriteEnable(SpiFlashObj self)
{
	unsigned char var;
    self->fTable.spiFlashSs(0);
    var = self->fTable.spiWrite(SPIFLASH_CMD_WREN);
    self->fTable.spiFlashSs(1);
}

//disable write by changing status register
void spiFlash_WriteDisable(SpiFlashObj self)
{
	unsigned char var;
    self->fTable.spiFlashSs(0);
    var = self->fTable.spiWrite(SPIFLASH_CMD_WRDI);
    self->fTable.spiFlashSs(1);
}

//read the status regsiter
unsigned char spiFlash_GetStatus(SpiFlashObj self)
{
	unsigned char var;

	self->fTable.spiFlashSs(0);
    var = self->fTable.spiWrite(SPIFLASH_CMD_RDSR);
    var = self->fTable.spiWrite(0);
    self->fTable.spiFlashSs(1);

	return (var);
}

//read the WIP bit
unsigned char spiFlash_GetWipStatus(SpiFlashObj self)
{
	return (spiFlash_GetStatus(self) & SPIFLASH_STATMASK_WIP);
}

//bulk erase
void spiFlash_BulkErase(SpiFlashObj self)
{
	spiFlash_WriteEnable(self);

	self->fTable.spiFlashSs(0);
	self->fTable.spiWrite(SPIFLASH_CMD_BERASE);

	self->fTable.spiFlashSs(1);
}

//starts sequence of flash block writes
void spiFlash_SeqStart(SpiFlashObj self, unsigned char dataH, unsigned char dataM, unsigned char dataL)
{
	spiFlash_WriteEnable(self);
	self->fTable.spiFlashSs(0);

	self->fTable.spiWrite(SPIFLASH_CMD_WRITE);
	self->fTable.spiWrite(dataH);
	self->fTable.spiWrite(dataM);
	self->fTable.spiWrite(dataL);
}

//send sequence of flash block writes
void spiFlash_SeqWrite(SpiFlashObj self, unsigned char data)
{
	self->fTable.spiWrite(data);
}

//finishes sequence of flash block writes
void spiFlash_SeqFinish(SpiFlashObj self)
{
	self->fTable.spiFlashSs(1);

	// wait for completion of previous write operation
	while(spiFlash_GetWipStatus(self));
}

//polls to see if the SPI SPIFLASH is present
unsigned char spiFlash_Poll(SpiFlashObj self)
{
	unsigned char var;
	
	spiFlash_WriteEnable(self);
	var = spiFlash_GetStatus(self) & SPIFLASH_STATMASK_WEL;
	spiFlash_WriteDisable(self);
	return var;
}
