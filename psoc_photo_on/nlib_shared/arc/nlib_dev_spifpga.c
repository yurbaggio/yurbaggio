/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jul 1, 2010
Modified	Jul 1, 2010
source for fpga register access, spi
******************************************************************************/

//uses calloc, must include stdlib
#include <stdlib.h>

#include "nlib_sys_err.h"
#include "nlib_sys_conv.h"
#include "nlib_dev_spifpga.h"

//instruction set
#define SPIFPGA_CMD_READ     (unsigned)0b00000011
#define SPIFPGA_CMD_WRITE    (unsigned)0b00000010

//local prototypes
void spiFpga_IntWrite(SpiFpgaObj self, unsigned int addr, unsigned int data);
unsigned int spiFpga_IntRead(SpiFpgaObj self, unsigned int addr);
void spiFpga_Update(SpiFpgaObj self, unsigned int offset, unsigned int *table);
void spiFpga_SeqWriteStart(SpiFpgaObj self, unsigned int addr);
void spiFpga_SeqWrite(SpiFpgaObj self, unsigned int data);
void spiFpga_SeqWriteStop(SpiFpgaObj self);

//create interface
static SpiFpgaInt iTable_var =
{
	spiFpga_IntWrite,
	spiFpga_IntRead,
	spiFpga_Update,
	spiFpga_SeqWriteStart,
	spiFpga_SeqWrite,
	spiFpga_SeqWriteStop,
};

//constructor reserve memory
SpiFpgaObj spiFpga_New(){
	SpiFpgaObj self = (SpiFpgaObj)calloc(1, sizeof(struct spiFpgaObj));
	return self;
}

//set the ports
int spiFpga_Init(SpiFpgaObj self, SpiFpgaFunc fTable_var)
{
	if(self == NULL)
		return NLIB_ERR_NOMEM;
		
	//initiate values
	self->fTable	= fTable_var;
	self->iTable	= iTable_var;
	
    // Set/reset fpga module by toggling the SS pin
	self->fTable.spiFpgaSs(0);
	self->fTable.spiFpgaSs(1);
	
	return NLIB_ERR_NOERROR;
}

//fpga Write
void spiFpga_IntWrite(SpiFpgaObj self, unsigned int addr, unsigned int data)
{
    self->fTable.spiFpgaSs(0);

    self->fTable.spiWrite(SPIFPGA_CMD_WRITE);

    self->fTable.spiWrite(INTHI(addr));
    self->fTable.spiWrite(INTLO(addr));

    self->fTable.spiWrite(INTHI(data));
	self->fTable.spiWrite(INTLO(data));

    self->fTable.spiFpgaSs(1);
}

//start sequential write
void spiFpga_SeqWriteStart(SpiFpgaObj self, unsigned int addr){
    self->fTable.spiFpgaSs(0);

    self->fTable.spiWrite(SPIFPGA_CMD_WRITE);
	
	self->fTable.spiWrite(INTHI(addr));
    self->fTable.spiWrite(INTLO(addr));
}

//send sequential write data
void spiFpga_SeqWrite(SpiFpgaObj self, unsigned int data){
	self->fTable.spiWrite(INTHI(data));
    self->fTable.spiWrite(INTLO(data));
}

//stop sequential write data
void spiFpga_SeqWriteStop(SpiFpgaObj self){
    self->fTable.spiFpgaSs(1);
}

//fpga read
unsigned int spiFpga_IntRead(SpiFpgaObj self, unsigned int addr)
{
	unsigned char retLo, retHi;
	unsigned int ret;

    self->fTable.spiFpgaSs(0);

	self->fTable.spiWrite(SPIFPGA_CMD_READ);

    self->fTable.spiWrite(INTHI(addr));
    self->fTable.spiWrite(INTLO(addr));

    retHi = self->fTable.spiWrite(0);
	retLo = self->fTable.spiWrite(0);
	 
    self->fTable.spiFpgaSs(1);
    
    ret = (retHi << 8) + retLo;
    return ret;
}

//update color tables
void spiFpga_Update(SpiFpgaObj self, unsigned int offset, unsigned int *table)
{
	int i;

	self->fTable.spiFpgaSs(0);

    self->fTable.spiWrite(SPIFPGA_CMD_WRITE);

    self->fTable.spiWrite(INTHI(offset));
    self->fTable.spiWrite(INTLO(offset));
	
	for(i=0; i< 256; i++){
		self->fTable.spiWrite(INTHI(*(table+i)));
		self->fTable.spiWrite(INTLO(*(table+i)));
	}	

	self->fTable.spiFpgaSs(1);
}
