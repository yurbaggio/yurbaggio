/******************************************************************************
Jianyi Liu
Created		July 24, 2016
Modified	July 24, 2016
assertions
******************************************************************************/

#include <stdlib.h>
#include "nlib_assert.h"
#include "nlib_err.h"

//constructor, reserve memory
AssertObj assert_New(){
	AssertObj self = (AssertObj) calloc(1, sizeof(struct assertObj));
	return self;
}

//initializes command processor
//only need a function to be able to print errors
int assert_Init(AssertObj self, int (*va_print_var)(const char *, ...)){
    
    if (self == NULL)
		return NLIB_ERR_NOMEM;

	//assign pointers for printing and receiving characters
	self->va_print = va_print_var;
    
	return NLIB_ERR_NOERROR;
}

//tests to make sure item is not null, stays in loop, use debug to find out where
void assert_IsNotNull(void * ptr){
    if (ptr == NULL)
        while(1);
}

//tests to make sure item is not null
void assert_IsNotNullPrint(AssertObj self, void * ptr, const char * label){
    if (ptr == NULL){
        self->va_print("Void pointer detected at label: %s\r\n", label);
		while(1);
    }
}

//tests to make sure item is not zero, stays in loop, use debug to find out where
void assert_IsZero(int val){
    if (val != 0)
        while(1);
}

//tests to make sure item is not zero
void assert_IsZeroPrint(AssertObj self, int val, const char * label){
    if (val != 0){
        self->va_print("Non zero value detected at label: %s\r\n", label);
        self->va_print("Value tested: %d\r\n", val);
		while(1);
    }
}
