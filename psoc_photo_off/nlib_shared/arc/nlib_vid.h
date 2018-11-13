/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jan 26, 2012
Created		Jan 26, 2012
Timing information for various resolutions
******************************************************************************/

//define available resolutions
#define VID_RES_12X10X60	0x00
#define VID_RES_12X10X75	0x01
#define VID_RES_10X7X60		0x02
#define VID_RES_10X7X75		0x03
#define VID_RES_8X6X60		0x05
#define VID_RES_8X6X75		0x04
#define VID_RES_6X4X60		0x06
#define VID_RES_6X4X75		0x08
#define VID_RES_1080P60		0x07
#define VID_RES_720P60		0x09
#define VID_RES_480P60		0x0A
#define VID_RES_UNKNOWN		0x0F

//define a structure that contains all timing information
typedef struct VidObjStruct{
	unsigned int hs_av;
	unsigned int hs_fp;
	unsigned int hs_sp;
	unsigned int hs_bp;
	unsigned int hs_total;
	unsigned int vs_av;
	unsigned int vs_fp;
	unsigned int vs_sp;
	unsigned int vs_bp;
	unsigned int vs_total;
}VidObj;

//returns the timing information
VidObj vid_GetTiming(unsigned char vidRes);

//returns string for representing the resolution
char* vid_GetResStr(unsigned char vidRes);
