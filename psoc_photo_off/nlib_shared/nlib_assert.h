/******************************************************************************
Jianyi Liu
Created		July 24, 2016
Modified	July 24, 2016
assertions
******************************************************************************/

#ifndef NLIB_ASSERT_H
#define NLIB_ASSERT_H

//Module object
typedef struct assertObj * AssertObj;

struct assertObj{
	int (*va_print)(const char *, ...);         //print function
};

//constructor, reserve memory
AssertObj assert_New();

//initializes command processor
//only need a function to be able to print errors
int assert_Init(AssertObj self, int (*va_print_var)(const char *, ...));

//tests to make sure item is not null, stays in loop, use debug to find out where
void assert_IsNotNull(void * ptr);

//tests to make sure item is not null
void assert_IsNotNullPrint(AssertObj self, void * ptr, const char * label);

//tests to make sure item is not zero, stays in loop, use debug to find out where
void assert_IsZero(int val);

//tests to make sure item is not zero
void assert_IsZeroPrint(AssertObj self, int val, const char * label);

#endif