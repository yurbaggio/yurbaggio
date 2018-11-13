/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jan 31, 2012
Modified	Jan 31, 2012
OSD Module on FPGA
******************************************************************************/

//uses calloc, must include stdlib, stdarg
#include <stdlib.h>
#include <stdarg.h>

//include proper headers
#include "nlib_sys_err.h"
#include "nlib_sys_conv.h"
#include "nlib_sys_color.h"
#include "nlib_dev_spifpga.h"
#include "nlib_ip_osd.h"

//define max color
#define OSD_MINCOLOR	1
#define OSD_MAXCOLOR	7

//define some masks for OSD character control
#define OSD_CHARMASK_TILE_DIS		(1<<15)		//bit 15, flags '%d' to disable, '%e' to enable
#define OSD_CHARMASK_BGCOLOR		(7<<12)		//bit 14 downto 12, flag '%fx' where x is color number
#define OSD_CHARMASK_BGCOLOR_SHIFT	12
#define OSD_CHARMASK_FGALPHA_DIS	(1<<11)		//bit 11, flags '%c' to disable, '%a' to enable
#define OSD_CHARMASK_FGCOLOR		(7<<8)		//bit 10 downto 8, flag '%bx' where x is color number
#define OSD_CHARMASK_FGCOLOR_SHIFT	8
#define OSD_CHARMASK_INVERT			(1<<7)		//bit 7, flags '%u' to disable, '%i' to enable

//character rom offset
#define OSD_CRAM_OFFSET				0x400
#define OSD_CRAM_SIZE				0x400
#define OSD_CRAM_LINEOFFSET			0x40

//constructor, resrve memory
OsdObj osd_New(){
	OsdObj self = (OsdObj)calloc(1, sizeof(struct osdObj));
	return self;
}

//initiate values
unsigned char osd_Init(OsdObj self, SpiFpgaObj spiFpga_var, unsigned int ipOffset_var){
	if(self == NULL)
		return NLIB_ERR_NOMEM;
	
	//initiate values
	self->spiFpga	= spiFpga_var;
	self->ipOffset	= ipOffset_var;
	
	//set fg color, alphas to default

	return NLIB_ERR_NOERROR;
}

//check all devices are active
unsigned char osd_Check(OsdObj self){
	if(INTLO(self->spiFpga->iTable.spiFpga_IntRead(self->spiFpga, self->ipOffset + OSD_IADDR_MODID)) != OSD_IP_MODID)
		return NLIB_ERR_NOIP;
	
	return NLIB_ERR_NOERROR;
}

//enable/disable module
void osd_SetPdOn(OsdObj self){
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + OSD_IADDR_MODEN, OSD_MODEN_ON);
}
void osd_SetPdOff(OsdObj self){
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + OSD_IADDR_MODEN, OSD_MODEN_OFF);
}
unsigned int osd_GetPd(OsdObj self){
	return(OSD_MODEN_MASK & self->spiFpga->iTable.spiFpga_IntRead(self->spiFpga, self->ipOffset + OSD_IADDR_MODEN));
}

//set foreground and background color
void osd_SetFgColor(OsdObj self, Color c){
	self->colorFg	= c;
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + OSD_IADDR_FG_COLOR_R, c.mem.red);
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + OSD_IADDR_FG_COLOR_G, c.mem.green);
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + OSD_IADDR_FG_COLOR_B, c.mem.blue);
}
void osd_SetBgColor(OsdObj self, Color c){
	self->colorBg	= c;
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + OSD_IADDR_BG_COLOR_R, c.mem.red);
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + OSD_IADDR_BG_COLOR_G, c.mem.green);
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + OSD_IADDR_BG_COLOR_B, c.mem.blue);
}

//assign colors 1 to 7
void osd_SetColor(OsdObj self, Color c, unsigned int c_num){
	//check if color is within bound
	if ((c_num < OSD_MINCOLOR) || (c_num > OSD_MAXCOLOR))
		return;

	//assign color, write to fpga
	self->color[c_num - 1] = c;
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + OSD_IADDR_COLOR1_R + 3*(c_num - 1), c.mem.red);
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + OSD_IADDR_COLOR1_G + 3*(c_num - 1), c.mem.green);
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + OSD_IADDR_COLOR1_B + 3*(c_num - 1), c.mem.blue);
}

//set alpha for fg and bg
void osd_SetFgAlpha(OsdObj self, unsigned char alpha){
	self->fgAlpha = alpha;
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + OSD_IADDR_FG_ALPHA, alpha);
}
void osd_SetBgAlpha(OsdObj self, unsigned char alpha){
	self->bgAlpha = alpha;
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + OSD_IADDR_BG_ALPHA, alpha);
}

//set start pixel location
void osd_SetPosition(OsdObj self, unsigned int x, unsigned int y){
	self->startX = x;
	self->startY = y;
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + OSD_IADDR_START_X_LSB, INTLO(x));
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + OSD_IADDR_START_X_MSB, INTHI(x));
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + OSD_IADDR_START_Y_LSB, INTLO(y));
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + OSD_IADDR_START_Y_MSB, INTHI(y));
}

//set size in rows and columns
void osd_SetSize(OsdObj self, unsigned char x, unsigned char y){
	self->sizeX = x;
	self->sizeY = y;
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + OSD_IADDR_SIZE_X, x);
	self->spiFpga->iTable.spiFpga_IntWrite(self->spiFpga, self->ipOffset + OSD_IADDR_SIZE_Y, y);
}

//clears the whole content ram location
void osd_Clear(OsdObj self){
	int i;

	self->spiFpga->iTable.spiFpga_SeqWriteStart(self->spiFpga, self->ipOffset + OSD_CRAM_OFFSET);
	for(i = 0; i < OSD_CRAM_SIZE; i ++)
		self->spiFpga->iTable.spiFpga_SeqWrite(self->spiFpga, 0x00);
	self->spiFpga->iTable.spiFpga_SeqWriteStop(self->spiFpga);
}

//print to osd on absolute line number, with color
void osd_PrintLine(OsdObj self, unsigned int line, const char *message){

	//keeps track of which flags to use
	unsigned int flag = 0;

	//keep track of where we are in the osd line
	int l = 0;
	self->spiFpga->iTable.spiFpga_SeqWriteStart(self->spiFpga, self->ipOffset + OSD_CRAM_OFFSET + line*OSD_CRAM_LINEOFFSET);


	//process the input string
	for (; *message != 0; message++){

		//if a switch is found
		if (*message == '%') {
			message++;
			
			//end of string
			if(*message == '\0') break;

			//escape the % character
			if(*message == '%')	goto print;

			//disable/enable tile
			if(*message == 't'){
				flag = flag | OSD_CHARMASK_TILE_DIS;
				continue;
			}
			if(*message == 'e'){
				flag = flag & ~OSD_CHARMASK_TILE_DIS;
				continue;
			}

			//disable/enable foreground alpha
			if(*message == 'h'){
				flag = flag | OSD_CHARMASK_FGALPHA_DIS;
				continue;
			}
			if(*message == 'a'){
				flag = flag & ~OSD_CHARMASK_FGALPHA_DIS;
				continue;
			}

			//invert/regular colors
			if(*message == 'w'){
				flag = flag | OSD_CHARMASK_INVERT;
				continue;
			}
			if(*message == 'r'){
				flag = flag & ~OSD_CHARMASK_INVERT;
				continue;
			}

			//choose colors
			if(*message == 'f'){

				//check to see if color is within range
				message++;
				if (*message >= '0' && *message <= '7'){
					flag = flag & ~OSD_CHARMASK_FGCOLOR;
					flag = flag | ((*message - '0') << OSD_CHARMASK_FGCOLOR_SHIFT);
				}
				continue;
			}
			if(*message == 'b'){

				//check to see if color is within range
				message++;
				if (*message >= '0' && *message <= '7'){
					flag = flag & ~OSD_CHARMASK_BGCOLOR;
					flag = flag | ((*message - '0') << OSD_CHARMASK_BGCOLOR_SHIFT);
				}
				continue;
			}

		//if no switch is found, print
		}else{

			print:{
				//if greater than the line can hold
				if(l++ >= OSD_CRAM_LINEOFFSET)	goto exit;

				self->spiFpga->iTable.spiFpga_SeqWrite(self->spiFpga, *message + flag);
			}
		}
	}

	exit:{

		//close the sequential write
		self->spiFpga->iTable.spiFpga_SeqWriteStop(self->spiFpga);
	}

}
