/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Nov 23, 2011
Modified	Nov 23, 2011
Datetime representation and functions in C
******************************************************************************/

#ifndef NLIB_SYS_DATETIME_H
#define NLIB_SYS_DATETIME_H

#define DATETIME_EARLIER	-1
#define DATETIME_SAME		0
#define DATETIME_LATER		1

//define a time/date type structure
typedef union tagTime{
	struct{
		unsigned char sec;
		unsigned char min;
		unsigned char hour;
		unsigned char date;
		unsigned char month;
		unsigned char year;
	}mem;
} Datetime;

int Datetime_Compare(Datetime a, Datetime b);

#endif
