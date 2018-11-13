/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jul 27, 2010
Modified	Apr 08, 2011
FPGA Module for Video Capture using SPI
******************************************************************************/

//uses calloc, must include stdlib
#include <stdlib.h>

//include proper headers
#include "nlib_sys_err.h"
#include "nlib_sys_conv.h"
#include "nlib_dev_spifpga.h"
#include "nlib_ip_vidnorm.h"

//constructor, resrve memory
VidnormObj vidnorm_New(){
	VidnormObj self = (VidnormObj)calloc(1, sizeof(struct vidnormObj));
	return self;
}

//initiate values
unsigned char vidnorm_Init(VidnormObj self, SpiFpgaObj spiFpga_var, unsigned int ipOffset_var){
	if(self == NULL)
		return NLIB_ERR_NOMEM;
	
	//initiate values
	self->spiFpga	= spiFpga_var;
	self->ipOffset	= ipOffset_var;
	
	return NLIB_ERR_NOERROR;
}

//check all devices are active
unsigned char vidnorm_Check(VidnormObj self){
	if(INTLO(self->spiFpga->iTable.spiFpga_IntRead(self->spiFpga, self->ipOffset + VIDNORM_IADDR_MODID)) != VIDNORM_IP_MODID)
		return NLIB_ERR_NOIP;
	
	return NLIB_ERR_NOERROR;
}

//tests to see if there is video
int vidnorm_HasVideo(VidnormObj self){
	if(self->spiFpga->iTable.spiFpga_IntRead(self->spiFpga, self->ipOffset + VIDNORM_IADDR_VSTAT) & VIDNORM_VSTAT_VDET)
		return 1;
	return 0;
}

//detects any changes to vStat, detect resolution change or no video
int vidnorm_HasChangedOrNoVideo(VidnormObj self){
	int ret = 0;
	int temp = self->spiFpga->iTable.spiFpga_IntRead(self->spiFpga, self->ipOffset + VIDNORM_IADDR_VSTAT);

	//test to see if no video
	if (!(temp & (VIDNORM_VSTAT_VDET)))
		ret = 1;
	
	//test to see if video changed
	if (temp != self->prevVstat)
		ret = 1;
	
	self->prevVstat = temp;
	return ret;
}

//detects any changes to vStat, detect resolution change
int vidnorm_HasChanged(VidnormObj self){
	int ret = 0;
	int temp = self->spiFpga->iTable.spiFpga_IntRead(self->spiFpga, self->ipOffset + VIDNORM_IADDR_VSTAT);

	//test to see if video changed
	if (temp != self->prevVstat)
		ret = 1;
	
	self->prevVstat = temp;
	return ret;
}

//get video status	
int vidnorm_GetVstat(VidnormObj self){
	return self->spiFpga->iTable.spiFpga_IntRead(self->spiFpga, self->ipOffset + VIDNORM_IADDR_VSTAT);
}

//gets resolution
int vidnorm_GetRes(VidnormObj self){
	return (vidnorm_GetVstat(self) & VIDNORM_VSTAT_RES);
}

//gets the hsync count
int vidnorm_GetHscount(VidnormObj self){
	return (self->spiFpga->iTable.spiFpga_IntRead(self->spiFpga, self->ipOffset + VIDNORM_IADDR_HSCOUNT_H) * 256 + self->spiFpga->iTable.spiFpga_IntRead(self->spiFpga, self->ipOffset + VIDNORM_IADDR_HSCOUNT_L));
}

//gets the vsync freq
int vidnorm_GetVsfreq(VidnormObj self){
	return (self->spiFpga->iTable.spiFpga_IntRead(self->spiFpga, self->ipOffset + VIDNORM_IADDR_VSFREQ_H) * 256 + self->spiFpga->iTable.spiFpga_IntRead(self->spiFpga, self->ipOffset + VIDNORM_IADDR_VSFREQ_L));
}

//set Degen Enable Control
void vidnorm_SetDegenCtrl(VidnormObj self, unsigned char ctrl){
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + VIDNORM_IADDR_DCTRL, ctrl);
}	

//set Degen Delay, value is signed!
void vidnorm_SetDegenDel(VidnormObj self, char delay){
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + VIDNORM_IADDR_DDEL, delay);
}	
