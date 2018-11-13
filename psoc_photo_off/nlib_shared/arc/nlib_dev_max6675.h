/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jul 1, 2010
Modified	Jul 1, 2010
header for eeproms, spi
******************************************************************************/

#ifndef NLIB_DEV_MAX6675_H
#define NLIB_DEV_MAX6675_H

//input detection
#define MAX6675_INPUT_DETECTED	0;
#define MAX6675_INPUT_NOTDETECTED 1;

typedef struct max6675Obj * Max6675Obj;

//define a max6675 Function table
typedef struct Max6675FuncStruct{
	unsigned char (*spiWrite)(unsigned char);
	void (*max6675Ss)(int);
}Max6675Func;

//define a max6675 Interface table
typedef struct Max6675IntStruct{
	float (*max6675_GetTemperature)(Max6675Obj self);		//get temperature
	float (*max6675_GetTempAvg)(Max6675Obj self, int n);	//get averaged temperature
	int (*max6675_GetInput)(Max6675Obj self);				//detects if thermocouple is connected
}Max6675Int;

//define a Max6675 object
struct max6675Obj{
	Max6675Func	fTable;	//base function table
	Max6675Int	iTable;	//interface table for outside access
};

//constructor reserve memory
Max6675Obj max6675_New();

//initiate interface to the EE prom
int max6675_Init(Max6675Obj self, Max6675Func fTable_var);

#endif
