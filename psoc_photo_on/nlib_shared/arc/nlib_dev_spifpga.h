/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jul 9, 2010
Modified	Jul 9, 2010
header for fpga, spi
******************************************************************************/

#ifndef NLIB_DEV_SPIFPGA_H
#define NLIB_DEV_SPIFPGA_H

typedef struct spiFpgaObj * SpiFpgaObj;

//define a spiFpga Function table
typedef struct SpiFpgaFuncStruct{
	unsigned char (*spiWrite)(unsigned char);
	void (*spiFpgaSs)(int);
}SpiFpgaFunc;

//define a spiFpga Interface table
typedef struct SpiFpgaIntStruct{
	void (*spiFpga_IntWrite)(SpiFpgaObj self, unsigned int addr, unsigned int data);
	unsigned int (*spiFpga_IntRead)(SpiFpgaObj self, unsigned int addr);
	void (*spiFpgaUpdate)(SpiFpgaObj self, unsigned int addr, unsigned int *table);
	void (*spiFpga_SeqWriteStart)(SpiFpgaObj self, unsigned int addr);
	void (*spiFpga_SeqWrite)(SpiFpgaObj self, unsigned int data);
	void (*spiFpga_SeqWriteStop)(SpiFpgaObj self);
}SpiFpgaInt;

//define a SpiFpga object
struct spiFpgaObj{
	SpiFpgaFunc	fTable;	//base function table
	SpiFpgaInt	iTable;	//interface table for outside access
};

//constructor reserve memory
SpiFpgaObj spiFpga_New();

//initiate interface to the EE prom
int spiFpga_Init(SpiFpgaObj self, SpiFpgaFunc fTable_var);

#endif
