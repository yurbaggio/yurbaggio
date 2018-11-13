/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jan 31, 2012
Modified	Jan 31, 2012
OSD Module on FPGA
******************************************************************************/

#ifndef NLIB_IP_OSD_H
#define NLIB_IP_OSD_H

//define IP module address offsets
enum osdIaddrEnum{
	OSD_IADDR_MODID		= 0x0000,
	OSD_IADDR_VERID		= 0x0001,
	OSD_IADDR_MODSIZE	= 0x0002,
	OSD_IADDR_MODEN		= 0x0003,
	OSD_IADDR_START_X_LSB	= 0x0004,
	OSD_IADDR_START_X_MSB	= 0x0005,
	OSD_IADDR_START_Y_LSB	= 0x0006,
	OSD_IADDR_START_Y_MSB	= 0x0007,
	OSD_IADDR_SIZE_X		= 0x0008,
	OSD_IADDR_SIZE_Y		= 0x0009,
	OSD_IADDR_BG_ALPHA		= 0x000A,
	OSD_IADDR_BG_COLOR_R	= 0x000B,
	OSD_IADDR_BG_COLOR_G	= 0x000C,
	OSD_IADDR_BG_COLOR_B	= 0x000D,
	OSD_IADDR_FG_ALPHA		= 0x000E,
	OSD_IADDR_FG_COLOR_R	= 0x000F,
	OSD_IADDR_FG_COLOR_G	= 0x0010,
	OSD_IADDR_FG_COLOR_B	= 0x0011,
	OSD_IADDR_COLOR1_R		= 0x0012,
	OSD_IADDR_COLOR1_G		= 0x0013,
	OSD_IADDR_COLOR1_B		= 0x0014,
	OSD_IADDR_COLOR2_R		= 0x0015,
	OSD_IADDR_COLOR2_G		= 0x0016,
	OSD_IADDR_COLOR2_B		= 0x0017,
	OSD_IADDR_COLOR3_R		= 0x0018,
	OSD_IADDR_COLOR3_G		= 0x0019,
	OSD_IADDR_COLOR3_B		= 0x001A,
	OSD_IADDR_COLOR4_R		= 0x001B,
	OSD_IADDR_COLOR4_G		= 0x001C,
	OSD_IADDR_COLOR4_B		= 0x001D,
	OSD_IADDR_COLOR5_R		= 0x001E,
	OSD_IADDR_COLOR5_G		= 0x001F,
	OSD_IADDR_COLOR5_B		= 0x0020,
	OSD_IADDR_COLOR6_R		= 0x0021,
	OSD_IADDR_COLOR6_G		= 0x0022,
	OSD_IADDR_COLOR6_B		= 0x0023,
	OSD_IADDR_COLOR7_R		= 0x0024,
	OSD_IADDR_COLOR7_G		= 0x0025,
	OSD_IADDR_COLOR7_B		= 0x0026
};

//ip Module ID
#define OSD_IP_MODID		0x03

//default values
#define OSD_FG_ALPHA_DEF 0x7F;
#define OSD_BG_ALPHA_DEF 0x7F;

//color index
#define OSD_COLOR1	1
#define OSD_COLOR2	2
#define OSD_COLOR3	3
#define OSD_COLOR4	4
#define OSD_COLOR5	5
#define OSD_COLOR6	6
#define OSD_COLOR7	7

//on and off
#define OSD_MODEN_ON	1
#define OSD_MODEN_OFF	0
#define OSD_MODEN_MASK	1

typedef struct osdObj * OsdObj;

//define sxclr object, undefine for DEBUG, this should be hidden usually!
struct osdObj{
	//connection objects
	SpiFpgaObj spiFpga;
	unsigned int ipOffset;
	
	//define module on/off
	unsigned char moden;

	//define alphas
	unsigned char fgAlpha;
	unsigned char bgAlpha;

	//define colors
	Color colorFg;
	Color colorBg;
	Color color[7];
	
	//define size and start
	unsigned int startX;
	unsigned int startY;
	unsigned int sizeX;	//in rows
	unsigned int sizeY;	//in columns
};

//constructor, resrve memory
OsdObj osd_New();

//initiate values
unsigned char osd_Init(OsdObj self, SpiFpgaObj spiFpga_var, unsigned int ipOffset_var);

//enable/disable module
void osd_SetPdOn(OsdObj self);
void osd_SetPdOff(OsdObj self);
unsigned int osd_GetPd(OsdObj self);

//check all devices are active
unsigned char osd_Check(OsdObj self);

//set foreground and background color
void osd_SetFgColor(OsdObj self, Color c);
void osd_SetBgColor(OsdObj self, Color c);

//assign colors 1 to 7
void osd_SetColor(OsdObj self, Color c, unsigned int c_num);

//set alpha for fg and bg
void osd_SetFgAlpha(OsdObj self, unsigned char alpha);
void osd_SetBgAlpha(OsdObj self, unsigned char alpha);

//set start pixel location
void osd_SetPosition(OsdObj self, unsigned int x, unsigned int y);

//set size in rows and columns
void osd_SetSize(OsdObj self, unsigned char x, unsigned char y);

//clears the whole content ram location
void osd_Clear(OsdObj self);

//print to osd on absolute line number, with color
//switches for printline:
//	%t/%e - enable/disable tile
//	%fx - where x is a number, change foreground to color x
//	%bx - where x is a number, change background to color x
//	%a/%h - enable/disable foreground alpha
//	%r/%w - regular/invert background/foreground colors
void osd_PrintLine(OsdObj self, unsigned int line, const char *message);

#endif
