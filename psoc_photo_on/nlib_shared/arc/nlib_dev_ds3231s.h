/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Dec 1, 2010
Modified	Dec 1, 2010
Maxim IC DS3231S Integrated DS3231S Header
******************************************************************************/

#ifndef NLIB_DEV_DS3231S_H
#define NLIB_DEV_DS3231S_H

#define DS3231S_ADDR_DEF	0xD0	//default factory address for ds3231S

typedef struct ds3231sObj * Ds3231sObj;

//define a ds3231s Interface
typedef struct Ds3231sFuncStruct{
	int (*i2cPoll)(unsigned char addr);
	int (*i2cWrite)(unsigned char addr, unsigned char subaddr, unsigned char data);
	int (*i2cRead)(unsigned char addr, unsigned char subaddr);
}Ds3231sFunc;

//define an ds3231s Interface table
typedef struct Ds3231sIntStruct{
	Datetime (*getDs3231sTime)(Ds3231sObj self);
	void (*setDs3231sTime)(Ds3231sObj self, Datetime);
}Ds3231sInt;

//define ds3231s object
struct ds3231sObj{
	Ds3231sFunc			fTable;
	Ds3231sInt			iTable;
	unsigned char	devAddr;	//stores the i2c address
	Datetime		ds3231sTime;	//stores the last time that was read from the ds3231s chip
};

//constructor, resrve memory
Ds3231sObj ds3231s_New();

//initialization
int ds3231s_Init(Ds3231sObj self, Ds3231sFunc fTable_var, unsigned char devAddr_var);

//sets a time into the device
void ds3231s_SetTime(Ds3231sObj self, Datetime setTime);

//reads the time from the DS3231S chip
Datetime ds3231s_GetTime(Ds3231sObj self);

//checks to see if everything is working
int ds3231s_Check(Ds3231sObj self);

//reads one register
int ds3231s_Rd(Ds3231sObj self, unsigned char addr);

//writes to one register
int ds3231s_Wr(Ds3231sObj self, unsigned char addr, unsigned char data);

#endif
