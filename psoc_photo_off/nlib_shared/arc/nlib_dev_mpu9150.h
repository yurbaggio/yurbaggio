/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		May 4, 2014
Modified	May 4, 2014
Device wrapper for 9 axis IMU
******************************************************************************/

#ifndef NLIB_DEV_SPIEE_H
#define NLIB_DEV_SPIEE_H

#define MPUCAL_DATA_VALID	0x15FC
#define MPUCAL_DATA_SIZE	15
	
typedef struct mpuObj * MpuObj;

//define a imu function table, for i2c
typedef struct MpuFuncStruct{
	int (*i2cPoll)(unsigned char addr);
	int (*i2cWrite)(unsigned char addr, unsigned char subaddr, unsigned char data);
	int (*i2cRead)(unsigned char addr, unsigned char subaddr);
}MpuFunc;

//define mpu calibration object
typedef struct MpuCalObjStruct{
	short valid;                        // should contain the valid pattern if a good config
	short magValid;                     // true if mag data valid
	short magMinX;                      // mag min x value
	short magMaxX;                      // mag max x value
	short magMinY;                      // mag min y value
	short magMaxY;                      // mag max y value
	short magMinZ;                      // mag min z value
	short magMaxZ;                      // mag max z value 
	short accelValid;                   // true if accel data valid
	short accelMinX;                    // mag min x value
	short accelMaxX;                    // mag max x value
	short accelMinY;                    // mag min y value
	short accelMaxY;                    // mag max y value
	short accelMinZ;                    // mag min z value
	short accelMaxZ;                    // mag max z value
}MpuCalObj;

//define mpu object
struct mpuObj{
	MpuFunc fTable;
	MpuCalObj calData;
	int mpuRate;
	int magMix;
	int magRate;
	int mpuLpf;
	unsigned short mpuOrient;
};

#endif
