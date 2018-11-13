/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Mar 30, 2017
Modified	Mar 30, 2010
Override the interrupt routine in the library
Macros for various stuff
******************************************************************************/

#ifndef nlib_err_h
#define nlib_err_h

//some system level constants
//system errors
#define NLIB_ERR_NOERROR	0	//no error has occured
#define NLIB_ERR_UNKNOWN	-1	//unknown error
#define NLIB_ERR_NODEV		-2	//cannot find device
#define NLIB_ERR_NOIP		-3	//cannot find IP module in FPGA
#define NLIB_ERR_NOPROM		-4	//cannot find EEPROM
#define NLIB_ERR_NOPOLL		-5	//polling error
#define NLIB_ERR_NOMEM		-6	//out of memory
#define NLIB_ERR_INVALPARAM	-7	//parameters are invalid
#define NLIB_ERR_SIZE     -8  //sizing error
#define NLIB_ERR_TYPE     -9  //type error
#define NLIB_ERR_INVALRETURN -10  //returned an error

#endif
