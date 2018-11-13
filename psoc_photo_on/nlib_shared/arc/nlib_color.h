/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Feb 10, 2012
Modified	Feb 10, 2012
Color representation and functions in C
******************************************************************************/

#ifndef NLIB_SYS_COLOR_H
#define NLIB_SYS_COLOR_H

#define COLOR_SAME		0
#define COLOR_DIFF		-1

//define a time/date type structure
typedef union tagColor{
	struct{
		unsigned char red;
		unsigned char green;
		unsigned char blue;
	}mem;
} Color;

//some global constants
extern const Color COLOR_BLACK;
extern const Color COLOR_WHITE;
extern const Color COLOR_RED;
extern const Color COLOR_GREEN;
extern const Color COLOR_BLUE;
extern const Color COLOR_CYAN;
extern const Color COLOR_YELLOW;
extern const Color COLOR_MAGENTA;
extern const Color COLOR_DARKGRAY;
extern const Color COLOR_GRAY;
extern const Color COLOR_LIGHTGRAY;

//compares two colors
//returns -1 COLOR_DIFF if a is different compared to b
//returns 0 COLOR_SAME if a is same compared to b
int Color_Compare(Color a, Color b);

#endif
