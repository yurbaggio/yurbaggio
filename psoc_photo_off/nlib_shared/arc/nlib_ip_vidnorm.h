/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jul 27, 2010
Modified	Apr 08, 2011
FPGA Module for Video Capture using SPI
******************************************************************************/

#ifndef NLIB_IP_VIDNORM_H
#define NLIB_IP_VIDNORM_H

//define IP module address offsets
enum vidnormIaddrEnum{
	VIDNORM_IADDR_MODID	= 0x0000,
	VIDNORM_IADDR_VERID	= 0x0001,
	VIDNORM_IADDR_MODSIZE	= 0x0002,
	VIDNORM_IADDR_CONFDEGEN	= 0x0003,
	VIDNORM_IADDR_VSTAT	= 0x0004,
	VIDNORM_IADDR_HSCOUNT_H   = 0x0005,
	VIDNORM_IADDR_HSCOUNT_L   = 0x0006,
	VIDNORM_IADDR_VSFREQ_H   = 0x0007,
	VIDNORM_IADDR_VSFREQ_L   = 0x0008,
	VIDNORM_IADDR_DCTRL	= 0x0009,
	VIDNORM_IADDR_DDEL	= 0x000A
};

//ip Module ID
#define VIDNORM_IP_MODID		0x06

//define masks for each of the bits in LSTAT and RSTAT
#define VIDNORM_VSTAT_HPOL	0x1<<7
#define VIDNORM_VSTAT_VPOL	0x1<<6
#define VIDNORM_VSTAT_VDET	0x1<<4
#define VIDNORM_VSTAT_RES	0xF

//define degen control values
#define VIDNORM_DEGENCTRL_ON	0x01
#define VIDNORM_DEGENCTRL_OFF	0x00

typedef struct vidnormObj * VidnormObj;

//define sxclr object, undefine for DEBUG, this should be hidden usually!
struct vidnormObj{
	SpiFpgaObj spiFpga;
	unsigned int ipOffset;			//fpga ip offset
	unsigned int prevVstat;
};

//constructor, resrve memory
VidnormObj vidnorm_New();

//initialize video input capture module
unsigned char vidnorm_Init(VidnormObj self, SpiFpgaObj spiFpga_var, unsigned int ipOffset_var);

//verify that the module is present, fpga responds correctly
unsigned char vidnorm_Check(VidnormObj self);

//tests to see if there is video
int vidnorm_HasVideo(VidnormObj self);

//detects any changes to vStat, detect resolution change or no video
int vidnorm_HasChangedOrNoVideo(VidnormObj self);

//detects any changes to vStat, detect resolution change
int vidnorm_HasChanged(VidnormObj self);

//get video status
int vidnorm_GetVstat(VidnormObj self);

//gets resolution
int vidnorm_GetRes(VidnormObj self);

//gets the hsync count
int vidnorm_GetHscount(VidnormObj self);

//gets the vsync freq
int vidnorm_GetVsfreq(VidnormObj self);

//turn on degen 
void vidnorm_SetDegenCtrl(VidnormObj self, unsigned char ctrl);

//set degen delay, remember this value is SIGNED!
void vidnorm_SetDegenDel(VidnormObj self, char delay);

#endif
