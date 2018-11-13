/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Nov 23, 2011
Modified	Nov 23, 2011
Datetime representation and functions in C
******************************************************************************/

#include "nlib_sys_datetime.h"

//compares two date times
//returns -1 if a is earlier than b
//returns 1 if a is later than b
//returns 0 if a is same as b
int Datetime_Compare(Datetime a, Datetime b){
	
	//compare year
	if (a.mem.year < b.mem.year)
		return DATETIME_EARLIER;
	else if (a.mem.year > b.mem.year)
		return DATETIME_LATER;

	//compare month
	if (a.mem.month < b.mem.month)
		return DATETIME_EARLIER;
	else if (a.mem.month > b.mem.month)
		return DATETIME_LATER;

	//compare day
	if (a.mem.date < b.mem.date)
		return DATETIME_EARLIER;
	else if (a.mem.date > b.mem.date)
		return DATETIME_LATER;

	//compare hour
	if (a.mem.hour < b.mem.hour)
		return DATETIME_EARLIER;
	else if (a.mem.hour > b.mem.hour)
		return DATETIME_LATER;

	//compare minute
	if (a.mem.min < b.mem.min)
		return DATETIME_EARLIER;
	else if (a.mem.min > b.mem.min)
		return DATETIME_LATER;

	//compare seconds
	if (a.mem.sec < b.mem.sec)
		return DATETIME_EARLIER;
	else if (a.mem.sec > b.mem.sec)
		return DATETIME_LATER;

	//must be equal if it got this far
	return DATETIME_SAME;
}
