/******************************************************************************
Engscope.com
Jianyi Liu
Created		Dec 25, 2013
Modified	Dec 25, 2013
I2C header cortex M 
******************************************************************************/

#ifndef NLIB_I2C_H
#define NLIB_I2C_H

#include <cytypes.h>
	
typedef struct i2cObj * I2cObj;

//define a i2c function table
typedef struct I2cFuncStruct{
	void (*I2CStart)(void);
	uint32 (*I2CMasterSendStart)(uint32 slaveAddress, uint32 bitRnW);
	uint32 (*I2CMasterSendRestart)(uint32 slaveAddress, uint32 bitRnW);
	uint32 (*I2CMasterSendStop)(void);
	uint32 (*I2CMasterWriteByte)(uint32 theByte);
	uint32 (*I2CMasterReadByte)(uint32 ackNack);
	uint32 (*I2CMasterReadBuf)(uint32 slaveAddress, uint8 * rdData, uint32 cnt, uint32 mode);
	void  (*I2CMasterClearReadBuf)(void);
	uint32 (*I2CMasterStatus)(void);
	uint32 (*I2CMasterClearStatus)(void);
}I2cFunc;

//define a i2c interface table
typedef struct I2cIntStruct{
	int (*i2c_ByteWrite)(I2cObj self, unsigned char control, unsigned char address, unsigned char data);
	int (*i2c_PageWrite)(I2cObj self, unsigned char control, unsigned char address, unsigned char *wrptr, unsigned int length);
	int (*i2c_ByteRead)(I2cObj self, unsigned char control, unsigned char address);
	int (*i2c_PageRead)(I2cObj self, unsigned char control, unsigned char address, unsigned char *rdptr, unsigned int length);
	int (*i2c_AckPoll)(I2cObj self, unsigned char control);
}I2cInt;

//define i2c object
struct i2cObj{
	I2cFunc	fTable;	//base function table
	I2cInt	iTable;	//interface table for outside access
};

//prototypes
int i2c_ByteWrite(I2cObj self, unsigned char control, unsigned char address, unsigned char data);
int i2c_PageWrite(I2cObj self, unsigned char control, unsigned char address, unsigned char *wrptr, unsigned int length);
int i2c_ByteRead(I2cObj self, unsigned char control, unsigned char address);
int i2c_PageRead(I2cObj self, unsigned char control, unsigned char address, unsigned char *rdptr, unsigned int length);
int i2c_AckPoll(I2cObj self, unsigned char control);

//constructor reserve memory
I2cObj i2c_New();

//init object
int i2c_Init(I2cObj self, I2cFunc fTable_var);

#endif
