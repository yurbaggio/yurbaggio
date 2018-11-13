/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jul 1, 2010
Modified	Jul 1, 2010
header for flash, spi
******************************************************************************/

//for use with flash chips with larger than 16 bit addressable space
//example: M25Pxx line of flash memories

#ifndef NLIB_DEV_SPIFLASH_H
#define NLIB_DEV_SPIFLASH_H

typedef struct spiFlashObj * SpiFlashObj;

//define a spiFlash Function table
typedef struct SpiFlashFuncStruct{
	unsigned char (*spiWrite)(unsigned char);
	void (*spiFlashSs)(int);
}SpiFlashFunc;

//define a spiFlash Interface table
typedef struct SpiFlashIntStruct{
	unsigned char (*spiFlash_GetWipStatus)(SpiFlashObj self);
	unsigned char (*spiFlash_ByteRead)(SpiFlashObj self, unsigned int Sector, unsigned int Address);
	void (*spiFlash_BulkErase)(SpiFlashObj self);
	void (*spiFlash_SeqStart)(SpiFlashObj self, unsigned char dataH, unsigned char dataM, unsigned char dataL);
	void (*spiFlash_SeqWrite)(SpiFlashObj self, unsigned char data);
	void (*spiFlash_SeqFinish)(SpiFlashObj self);
}SpiFlashInt;

//define a SpiFlash object
struct spiFlashObj{
	SpiFlashFunc fTable;	//base function table
	SpiFlashInt	iTable;	//interface table for outside access
};

//constructor reserve memory
SpiFlashObj spiFlash_New();

//initiate interface to the EE prom
int spiFlash_Init(SpiFlashObj self, SpiFlashFunc fTable_var);

#endif
