/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jul 27, 2010
Modified	Apr 08, 2011
FPGA Module for Video Distribution using SPI, works with VIDSTEREO or VIDMONO
******************************************************************************/

#ifndef NLIB_IP_VIDDIST_H
#define NLIB_IP_VIDDIST_H

//define IP module address offsets
enum viddistIaddrEnum{
	VIDDIST_IADDR_MODID	= 0x0000,
	VIDDIST_IADDR_VERID	= 0x0001,
	VIDDIST_IADDR_MODSIZE	= 0x0002,
	VIDDIST_IADDR_VSTAT		= 0x0003
};

//ip Module ID
#define VIDSTEREO_IP_MODID		0x05
#define VIDMONO_IP_MODID		0x08

//define masks for each of the bits in LSTAT and RSTAT
#define VIDDIST_VSTAT_VSWL	0x1<<3
#define VIDDIST_VSTAT_VSWR	0x1<<2
#define VIDDIST_VSTAT_VSDETL	0x1<<1
#define VIDDIST_VSTAT_VSDETR	0x1

typedef struct viddistObj * ViddistObj;

//define sxclr object, undefine for DEBUG, this should be hidden usually!
struct viddistObj{
	SpiFpgaObj spiFpga;
	unsigned int ipOffset;			//fpga ip offset
	unsigned int prevVstat;
    //unsigned int prevVstat[4];	//previous vstat value
	//unsigned int prevVstatIndex;	//counter for index of prevVstat
};

//constructor, resrve memory
ViddistObj viddist_New();

//initialize video input capture module
unsigned char viddist_Init(ViddistObj self, SpiFpgaObj spiFpga_var, unsigned int ipOffset_var);

//verify that the module is present, fpga responds correctly
unsigned char viddist_StereoCheck(ViddistObj self);
unsigned char viddist_MonoCheck(ViddistObj self);

//tests to see if there is video
int viddist_HasVideo(ViddistObj self);

//tests to see if there is video on LEFT channel
int viddist_HasLeftVideo(ViddistObj self);

//tests to see if there is video on RIGHT channel
int viddist_HasRightVideo(ViddistObj self);

//gets the left status
int viddist_GetVstat(ViddistObj self);

//detects any changes to vStat, or no video
int viddist_HasChangedOrNoVideo(ViddistObj self);

//detects a change to vStat
int viddist_HasChanged(ViddistObj self);

#endif
