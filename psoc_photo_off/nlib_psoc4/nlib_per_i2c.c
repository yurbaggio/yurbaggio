/******************************************************************************
Engscope.com
Jianyi Liu
Created		Dec 25, 2013
Modified	Dec 25, 2013
I2C source cortex M 
******************************************************************************/

//uses calloc, must include stdlib
#include <stdlib.h>

//#include "nlib_sys_conv.h"
#include "nlib_err.h"
#include "nlib_i2c.h"

//create interface
static I2cInt iTable_var = 
{
	i2c_ByteWrite,
	i2c_PageWrite,
	i2c_ByteRead,
	i2c_PageRead,
	i2c_AckPoll,
};

//constructor, reserve memory
I2cObj i2c_New(){
	I2cObj self = (I2cObj)calloc(1, sizeof(struct i2cObj));
	return self;
}

//initiate interface
int i2c_Init(I2cObj self, I2cFunc fTable_var){
	if(self == NULL)
		return NLIB_ERR_NOMEM;
		
	//initiate values
	self->fTable	= fTable_var;
	self->iTable	= iTable_var;
	
	//initiate I2C
	self->fTable.I2CStart();
	
	return NLIB_ERR_NOERROR;
}

//write one byte
int i2c_ByteWrite(I2cObj self, unsigned char control, unsigned char address, unsigned char data){
	
	//ensure module is idle, start, test for collision
	if (self->fTable.I2CMasterSendStart(control >> 1, 0)){
		self->fTable.I2CMasterSendStop();
		return -1;
	}
	
	//send address byte;
	if (self->fTable.I2CMasterWriteByte(address)){
		self->fTable.I2CMasterSendStop();
		return -2;
	}
		
	//send data byte;
	if (self->fTable.I2CMasterWriteByte(data)){
		self->fTable.I2CMasterSendStop();
		return -3;
	}
		
	//if you got this far, send stop and return
	if (self->fTable.I2CMasterSendStop())
		return -4;
		
	//no errors
	return 0;
}

//write multiple bytes
int i2c_PageWrite(I2cObj self, unsigned char control, unsigned char address, unsigned char *wrptr, unsigned int length ){
	
	//ensure module is idle, start, test for collision
	if (self->fTable.I2CMasterSendStart(control >> 1, 0)){
		self->fTable.I2CMasterSendStop();
		return -1;
	}
	
	//send address byte;
	if (self->fTable.I2CMasterWriteByte(address)){
		self->fTable.I2CMasterSendStop();
		return -2;
	}
		
	//send data byte;
	unsigned int i;
	for (i = 0; i < length; i++){
		if (self->fTable.I2CMasterWriteByte(*(wrptr + i))){
			self->fTable.I2CMasterSendStop();
			return -3;
		}
	}
		
	//if you got this far, send stop and return
	if (self->fTable.I2CMasterSendStop())
		return -4;
		
	//no errors
	return 0;
}

int i2c_ByteRead(I2cObj self, unsigned char control, unsigned char address){
	
	//ensure module is idle, start, test for collision
	if (self->fTable.I2CMasterSendStart(control >> 1, 0)){
		self->fTable.I2CMasterSendStop();
		return -1;
	}
	
	//send address byte;
	if (self->fTable.I2CMasterWriteByte(address)){
		self->fTable.I2CMasterSendStop();
		return -2;
	}
	
	//send restart
	if (self->fTable.I2CMasterSendRestart(control >> 1, 1)){
		self->fTable.I2CMasterSendStop();
		return -3;
	}

	char ret = self->fTable.I2CMasterReadByte(1);
	
	//if you got this far, send stop and return
	if (self->fTable.I2CMasterSendStop())
		return -4;
	
	return (int)(ret);
}

int i2c_PageRead(I2cObj self, unsigned char control, unsigned char address, unsigned char *rdptr, unsigned int length){
	
	//ensure module is idle, start, test for collision
	if (self->fTable.I2CMasterSendStart(control >> 1, 0)){
		self->fTable.I2CMasterSendStop();
		return -1;
	}
	
	//send address byte;
	if (self->fTable.I2CMasterWriteByte(address)){
		self->fTable.I2CMasterSendStop();
		return -2;
	}

	//receive incoming bytes
	self->fTable.I2CMasterReadBuf(control >> 1, rdptr, length, 1);
	
	//wait for read to finish (I2C_MSTAT_RD_CMPLT)
	while(0u == (self->fTable.I2CMasterStatus() & ((uint16) 0x01u)));
	
	//clear buffer, flags
	self->fTable.I2CMasterClearReadBuf();
	self->fTable.I2CMasterClearStatus();
	
	return 0;
}

//poll the slave address
int i2c_AckPoll(I2cObj self, unsigned char control){
	
	//ensure module is idle, start, test for collision
	if (self->fTable.I2CMasterSendStart(control >> 1, 0)){
		self->fTable.I2CMasterSendStop();
		return -1;
	}
	
	//stop communications
	self->fTable.I2CMasterSendStop();
	
	//ack was received
	return 0;
	
}
