/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Dec 1, 2010
Modified	Dec 1, 2010
Maxim IC DS3231S Integrated DS3231S Driver
******************************************************************************/

//uses calloc, must include stdlib
#include <stdlib.h>

#include "nlib_sys_err.h"
#include "nlib_sys_datetime.h"
#include "nlib_dev_ds3231S.h"

//define memory map
#define DS3231S_SEC	0x00
#define DS3231S_MIN	0x01
#define DS3231S_HRS	0x02
#define DS3231S_DAY	0x03
#define DS3231S_DATE	0x04
#define DS3231S_MON	0x05
#define DS3231S_YRS	0x06
	
//alarm
#define DS3231S_ARM1_SEC	0x07
#define DS3231S_ARM1_MIN	0x08
#define DS3231S_ARM1_HRS	0x09
#define DS3231S_ARM1_DAY	0x0A
#define DS3231S_ARM2_SEC	0x0B
#define DS3231S_ARM2_MIN	0x0C
#define DS3231S_ARM2_HRS	0x0D
#define DS3231S_ARM2_DAY	0x0E

//controls
#define DS3231S_CTRL1	0x0E
#define DS3231S_CTRL2	0x0F
#define DS3231S_AGEOS	0x10
#define DS3231S_TEMPH	0x11
#define DS3231S_TEMPL	0x12

//create interface
static Ds3231sInt iTable_var =
{
	ds3231s_GetTime,
	ds3231s_SetTime
};

//constructor, resrve memory
Ds3231sObj ds3231s_New(){
	Ds3231sObj self = (Ds3231sObj)calloc(1, sizeof(struct ds3231sObj));
	return self;
}

//initialization
int ds3231s_Init(Ds3231sObj self, Ds3231sFunc fTable_var, unsigned char devAddr_var){
	if(self == NULL)
		return NLIB_ERR_NOMEM;

	//initiate values
	self->devAddr	= devAddr_var;
	self->fTable	= fTable_var;
	self->iTable	= iTable_var;

	return NLIB_ERR_NOERROR;
}

//sets a time into the device
void ds3231s_SetTime(Ds3231sObj self, Datetime setTime){

	//convert data
	Datetime writeTime;

	writeTime.mem.sec = (setTime.mem.sec/10)<<4 | (setTime.mem.sec%10);
	writeTime.mem.min = (setTime.mem.min/10)<<4 | (setTime.mem.min%10);
	writeTime.mem.hour = (setTime.mem.hour/10)<<4 | (setTime.mem.hour%10);
	writeTime.mem.date = (setTime.mem.date/10)<<4 | (setTime.mem.date%10);
	writeTime.mem.month = (setTime.mem.month/10)<<4 | (setTime.mem.month%10);
	writeTime.mem.year = (setTime.mem.year/10)<<4 | (setTime.mem.year%10);

	//write to device
	ds3231s_Wr(self, DS3231S_SEC, writeTime.mem.sec);
	ds3231s_Wr(self, DS3231S_MIN, writeTime.mem.min);
	ds3231s_Wr(self, DS3231S_HRS, writeTime.mem.hour);
	ds3231s_Wr(self, DS3231S_DATE, writeTime.mem.date);
	ds3231s_Wr(self, DS3231S_MON, writeTime.mem.month);
	ds3231s_Wr(self, DS3231S_YRS, writeTime.mem.year);
}

//reads the time from the DS3231S chip
Datetime ds3231s_GetTime(Ds3231sObj self){

	//extract data
	Datetime readTime;	//time that is read
	Datetime retTime;	//time that is returned

	readTime.mem.sec = ds3231s_Rd(self, DS3231S_SEC);
	readTime.mem.min = ds3231s_Rd(self, DS3231S_MIN);
	readTime.mem.hour = ds3231s_Rd(self, DS3231S_HRS);
	readTime.mem.date = ds3231s_Rd(self, DS3231S_DATE);
	readTime.mem.month = ds3231s_Rd(self, DS3231S_MON);
	readTime.mem.year = ds3231s_Rd(self, DS3231S_YRS);

	//convert data
	retTime.mem.sec = (readTime.mem.sec&0xF) + 10*((readTime.mem.sec&0x70)>>4);
	retTime.mem.min = (readTime.mem.min&0xF) + 10*((readTime.mem.min&0x70)>>4);
	retTime.mem.hour = (readTime.mem.hour&0xF) + 10*((readTime.mem.hour&0x30)>>4);
	retTime.mem.date = (readTime.mem.date&0xF) + 10*((readTime.mem.date&0x30)>>4);
	retTime.mem.month = (readTime.mem.month&0xF) + 10*((readTime.mem.month&0x10)>>4);
	retTime.mem.year = (readTime.mem.year&0xF) + 10*((readTime.mem.year&0xF0)>>4);

	return retTime;
}		

//TODO:  compares two times, and returns the difference
Datetime ds3231sCompareTime(Datetime timeA, Datetime timeB){
	Datetime retTime;
	return retTime;
}

//checks to see if everything is working
int ds3231s_Check(Ds3231sObj self){
	if(self->fTable.i2cPoll(self->devAddr) != 0)
		return NLIB_ERR_NOPOLL;
	return NLIB_ERR_NOERROR;
}

//reads one register
int ds3231s_Rd(Ds3231sObj self, unsigned char addr){
	return self->fTable.i2cRead(self->devAddr, addr);
}	

//writes to one register
int ds3231s_Wr(Ds3231sObj self, unsigned char addr, unsigned char data){
	return self->fTable.i2cWrite(self->devAddr, addr, data);
}
