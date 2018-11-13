/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jul 1, 2010
Modified	Jul 1, 2010
header for eeproms, spi
******************************************************************************/

#ifndef NLIB_DEV_SPIEE_H
#define NLIB_DEV_SPIEE_H

typedef struct spiEeObj * SpiEeObj;

//define a spiEe Function table
typedef struct SpiEeFuncStruct{
	unsigned char (*spiWrite)(unsigned char);
	void (*spiEeSs)(int);
}SpiEeFunc;

//define a spiEe Interface table
typedef struct SpiEeIntStruct{
	void (*spiEe_ByteWrite)(SpiEeObj self, unsigned int addr, unsigned char data);
	unsigned char (*spiEe_ByteRead)(SpiEeObj self, unsigned int addr);
	unsigned char (*spiEe_Poll)(SpiEeObj self);
}SpiEeInt;

//define a SpiEe object
struct spiEeObj{
	SpiEeFunc	fTable;	//base function table
	SpiEeInt	iTable;	//interface table for outside access
};

//constructor reserve memory
SpiEeObj spiEe_New();

//initiate interface to the EE prom
int spiEe_Init(SpiEeObj self, SpiEeFunc fTable_var);

#endif
