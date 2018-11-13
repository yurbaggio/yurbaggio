/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Feb 10, 2012
Modified	Feb 10, 2012
Color representation and functions in C
******************************************************************************/

#include "nlib_sys_color.h"

//some global constants
const Color COLOR_BLACK 		=	{0x00, 0x00, 0x00};
const Color COLOR_WHITE			=	{0xFF, 0xFF, 0xFF};
const Color COLOR_RED			=	{0xFF, 0x00, 0x00};
const Color COLOR_GREEN			=	{0x00, 0xFF, 0x00};
const Color COLOR_BLUE			=	{0x00, 0x00, 0xFF};
const Color COLOR_CYAN			=	{0x00, 0xFF, 0xFF};
const Color COLOR_YELLOW		=	{0xFF, 0xFF, 0x00};
const Color COLOR_MAGENTA		=	{0xFF, 0x00, 0xFF};
const Color COLOR_DARKGRAY		=	{0x40, 0x40, 0x40};
const Color COLOR_GRAY			=	{0x80, 0x80, 0x80};
const Color COLOR_LIGHTGRAY		=	{0xC0, 0xC0, 0xC0};

//compares two colors
//returns -1 if a is different compared to b
//returns 0 if a is samecompared to b
int Color_Compare(Color a, Color b){
	
	//compare red
	if (a.mem.red != b.mem.red)
		return COLOR_DIFF;
	else if (a.mem.red > b.mem.red)
		return COLOR_DIFF;

	//compare green
	if (a.mem.green != b.mem.green)
		return COLOR_DIFF;
	else if (a.mem.green > b.mem.green)
		return COLOR_DIFF;

	//compare day
	if (a.mem.blue != b.mem.blue)
		return COLOR_DIFF;
	else if (a.mem.blue > b.mem.blue)
		return COLOR_DIFF;

	//must be equal if it got this far
	return COLOR_SAME;
}
