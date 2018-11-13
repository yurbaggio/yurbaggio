/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Feb 14, 2012
Modified	Feb 14, 2012
Video Pattern Module on FPGA
******************************************************************************/

#ifndef NLIB_IP_VPAT_H
#define NLIB_IP_VPAT_H

//define pattern numbers
#define VPAT_PAT_COLOR		0
#define VPAT_PAT_CROSSHAIR	1
#define VPAT_PAT_GRADIENT	2
#define VPAT_PAT_SGRADIENT	3
#define VPAT_PAT_ALIGN0		4
#define VPAT_PAT_ALIGN1		5

//define IP module address offsets
enum vpatIaddrEnum{
	VPAT_IADDR_MODID	= 0x0000,
	VPAT_IADDR_VERID	= 0x0001,
	VPAT_IADDR_MODSIZE	= 0x0002,
	VPAT_IADDR_MODEN	= 0x0003,
	VPAT_IADDR_PATSEL	= 0x0004,
	VPAT_IADDR_COLOR_R	= 0x0005,
	VPAT_IADDR_COLOR_G	= 0x0006,
	VPAT_IADDR_COLOR_B	= 0x0007
};

//ip Module ID
#define VPAT_IP_MODID		0x0A

typedef struct vpatObj * VpatObj;

//define sxclr object, undefine for DEBUG, this should be hidden usually!
struct vpatObj{
	//connection objects
	SpiFpgaObj spiFpga;
	unsigned int ipOffset;
	
	//define pattern
	unsigned char patsel;
	
	//define colors
	Color color;
};

//on and off
#define VPAT_MODEN_ON	1
#define VPAT_MODEN_OFF	0
#define VPAT_MODEN_MASK	1

//constructor, resrve memory
VpatObj vpat_New();

//initialize video input capture module
unsigned char vpat_Init(VpatObj self, SpiFpgaObj spiFpga_var, unsigned int ipOffset_var);

//verify that the module is present, fpga responds correctly
unsigned char vpat_Check(VpatObj self);

//enable/disable module
void vpat_SetPd(VpatObj self, unsigned int pd);
void vpat_SetPdOn(VpatObj self);
void vpat_SetPdOff(VpatObj self);
unsigned int vpat_GetPd(VpatObj self);

//sets the color screen's color
void vpat_SetColor(VpatObj self, Color c);

//set the pattern to show
void vpat_SetPattern(VpatObj self, unsigned int pat);

#endif
