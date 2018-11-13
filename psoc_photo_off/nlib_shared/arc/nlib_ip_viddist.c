/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jul 27, 2010
Modified	Apr 08, 2011
FPGA Module for Video Distribution using SPI, works with VIDSTEREO or VIDMONO
******************************************************************************/

//uses calloc, must include stdlib
#include <stdlib.h>

//include proper headers
#include "nlib_sys_err.h"
#include "nlib_sys_conv.h"
#include "nlib_dev_spifpga.h"
#include "nlib_ip_viddist.h"

//constructor, resrve memory
ViddistObj viddist_New(){
	ViddistObj self = (ViddistObj)calloc(1, sizeof(struct viddistObj));
	return self;
}

//initiate values
unsigned char viddist_Init(ViddistObj self, SpiFpgaObj spiFpga_var, unsigned int ipOffset_var){
	if(self == NULL)
		return NLIB_ERR_NOMEM;
	
	//initiate values
	self->spiFpga	= spiFpga_var;
	self->ipOffset 	= ipOffset_var;
	
	return NLIB_ERR_NOERROR;
}

//check all devices are active
unsigned char viddist_StereoCheck(ViddistObj self){
	if(INTLO(self->spiFpga->iTable.spiFpga_IntRead(self->spiFpga, self->ipOffset + VIDDIST_IADDR_MODID)) != VIDSTEREO_IP_MODID)
		return NLIB_ERR_NOIP;
	return NLIB_ERR_NOERROR;
}
unsigned char viddist_MonoCheck(ViddistObj self){
	if(INTLO(self->spiFpga->iTable.spiFpga_IntRead(self->spiFpga, self->ipOffset + VIDDIST_IADDR_MODID)) != VIDMONO_IP_MODID)
		return NLIB_ERR_NOIP;
	return NLIB_ERR_NOERROR;
}


//tests to see if there is video on ANY of the channels
int viddist_HasVideo(ViddistObj self){
	if(self->spiFpga->iTable.spiFpga_IntRead(self->spiFpga, self->ipOffset + VIDDIST_IADDR_VSTAT) & (VIDDIST_VSTAT_VSDETL | VIDDIST_VSTAT_VSDETR))
		return 1;
	return 0;
}

//tests to see if there is video on LEFT channel
int viddist_HasLeftVideo(ViddistObj self){
	if(self->spiFpga->iTable.spiFpga_IntRead(self->spiFpga, self->ipOffset + VIDDIST_IADDR_VSTAT) & VIDDIST_VSTAT_VSDETL)
		return 1;
	return 0;
}

//tests to see if there is video on RIGHT channel
int viddist_HasRightVideo(ViddistObj self){
	if(self->spiFpga->iTable.spiFpga_IntRead(self->spiFpga, self->ipOffset + VIDDIST_IADDR_VSTAT) & VIDDIST_VSTAT_VSDETR)
		return 1;
	return 0;
}

//get video status	
int viddist_GetVstat(ViddistObj self){
	return self->spiFpga->iTable.spiFpga_IntRead(self->spiFpga, self->ipOffset + VIDDIST_IADDR_VSTAT);
}

//detects any changes to vStat, or no video
int viddist_HasChangedOrNoVideo(ViddistObj self){
	int ret = 0;
	int temp = self->spiFpga->iTable.spiFpga_IntRead(self->spiFpga, self->ipOffset + VIDDIST_IADDR_VSTAT);

	//test to see if no video
	if (!(temp & (VIDDIST_VSTAT_VSDETL | VIDDIST_VSTAT_VSDETR)))
		ret = 1;
	
	//test to see if video changed
	if (temp != self->prevVstat)
		ret = 1;
	
	self->prevVstat = temp;
	return ret;
}

//detects a change to vStat
int viddist_HasChanged(ViddistObj self){
	int ret = 0;
	int temp = self->spiFpga->iTable.spiFpga_IntRead(self->spiFpga, self->ipOffset + VIDDIST_IADDR_VSTAT);
	
	//test to see if video changed
	if (temp != self->prevVstat)
		ret = 1;
	
	self->prevVstat = temp;
	return ret;
}
