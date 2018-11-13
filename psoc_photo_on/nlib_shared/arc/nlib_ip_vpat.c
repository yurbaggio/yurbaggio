/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Feb 14, 2012
Modified	Feb 14, 2012
Video Pattern Module on FPGA
******************************************************************************/

//uses calloc, must include stdlib
#include <stdlib.h>

//include proper headers
#include "nlib_sys_err.h"
#include "nlib_sys_conv.h"
#include "nlib_sys_color.h"
#include "nlib_dev_spifpga.h"
#include "nlib_ip_vpat.h"

//constructor, resrve memory
VpatObj vpat_New(){
	VpatObj self = (VpatObj)calloc(1, sizeof(struct vpatObj));
	return self;
}

//initiate values
unsigned char vpat_Init(VpatObj self, SpiFpgaObj spiFpga_var, unsigned int ipOffset_var){
	if(self == NULL)
		return NLIB_ERR_NOMEM;
	
	//initiate values
	self->spiFpga	= spiFpga_var;
	self->ipOffset	= ipOffset_var;
	
	return NLIB_ERR_NOERROR;
}

//check all devices are active
unsigned char vpat_Check(VpatObj self){
	if(INTLO(self->spiFpga->iTable.spiFpga_IntRead(self->spiFpga, self->ipOffset + VPAT_IADDR_MODID)) != VPAT_IP_MODID)
		return NLIB_ERR_NOIP;
	
	return NLIB_ERR_NOERROR;
}

//enable/disable module
void vpat_SetPd(VpatObj self, unsigned int pd){
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + VPAT_IADDR_MODEN, pd);
}
void vpat_SetPdOn(VpatObj self){
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + VPAT_IADDR_MODEN, VPAT_MODEN_ON);
}
void vpat_SetPdOff(VpatObj self){
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + VPAT_IADDR_MODEN, VPAT_MODEN_OFF);
}
unsigned int vpat_GetPd(VpatObj self){
	return(VPAT_MODEN_MASK & self->spiFpga->iTable.spiFpga_IntRead(self->spiFpga, self->ipOffset + VPAT_IADDR_MODEN));
}

//sets the color screen's color
void vpat_SetColor(VpatObj self, Color c){
	self->color	= c;
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + VPAT_IADDR_COLOR_R, c.mem.red);
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + VPAT_IADDR_COLOR_G, c.mem.green);
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + VPAT_IADDR_COLOR_B, c.mem.blue);	
}

//set the pattern to show
void vpat_SetPattern(VpatObj self, unsigned int pat){
	self->patsel = pat;
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + VPAT_IADDR_PATSEL, pat);
}
