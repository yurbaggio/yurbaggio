/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Oct 12, 2011
Modified	Oct 12, 2011
header for  mma7660fc accelerometer drivers
******************************************************************************/

#ifndef NLIB_DEV_MMA7660_H
#define NLIB_DEV_MMA7660_H

#define MMA7660_ADDR_DEF	0x98	//default factory address for mma7660

//address list
#define MMA7660_ADDR_XOUT	0x00
#define MMA7660_ADDR_YOUT	0x01
#define MMA7660_ADDR_ZOUT	0x02
#define MMA7660_ADDR_TILT	0x03
#define MMA7660_ADDR_SRST	0x04
#define MMA7660_ADDR_SPCNT	0x05
#define MMA7660_ADDR_INTSU	0x06
#define MMA7660_ADDR_MODE	0x07
#define MMA7660_ADDR_SR		0x08
#define MMA7660_ADDR_PDET	0x09
#define MMA7660_ADDR_PD		0x0A

typedef struct mma7660Obj * Mma7660Obj;

//define a MMA7660A Interface
typedef struct Mma7660FuncStruct{
	int (*i2cPoll)(unsigned char addr);
	int (*i2cWrite)(unsigned char addr, unsigned char subaddr, unsigned char data);
	int (*i2cRead)(unsigned char addr, unsigned char subaddr);
}Mma7660Func;

//define a MMA7660A Obj
struct mma7660Obj{
	Mma7660Func fTable;
	unsigned char devAddr;	//stores the i2c address
};

//constructor reserve memory
Mma7660Obj mma7660_New();

//initiate an MMA7660A device and associate address, function pointers must already be configured
int mma7660_Init(Mma7660Obj self, Mma7660Func fTable_var, unsigned char mma7660Addr_var);

//reset the interrupts and service if needed
int mma7660_ResetInt(Mma7660Obj self);

//change the sensitivity
void mma7660_SetSensitivity(Mma7660Obj self, int sen);

//check to see if device is present
int mma7660_Check(Mma7660Obj self);

//set timing to 1280x1024
int mma7660_SetTapDetect(Mma7660Obj self);

#endif
