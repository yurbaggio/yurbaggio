/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jul 16, 2007
Modified	Jun 5, 2013
Buttons headers
******************************************************************************/

#ifndef NLIB_BTN_H
#define NLIB_BTN_H

#define	BTN_PRESSLIM_DEF		20
#define	BTN_HOLDLIM_DEF			1000

typedef struct btnObj * BtnObj;

//define button object (device control object), undefine for DEBUG, this should be hidden usually!
struct btnObj{
	
	int (*btnSample)(void);			//this is the function pointer that samples the button/input
	//int config;					//stores how this button is configured, allow hold signal or no hold
	int pressCounter;	//press button counter
  int pressLim;
	int holdCounter;	//hold button counter
	int holdLim;		//press limit, the hold and release limits are different for a button hold
	unsigned pFlag:1;		//press flag
	unsigned hFlag:1;		//hold flag
	unsigned pArm:1;		//press button armed, for button pressed policy
	unsigned rArm:1;		//release button armed, for button released policy
	unsigned hArm:1;		//hold button armed
};

//constructor, resrve memory
BtnObj btn_New();

//initializer
int btn_Init(BtnObj btnInst, int (*btnSample_func)(void), unsigned int pressLim_var, unsigned int holdLim_var);

//process events
void btn_Proc(BtnObj btnInst);

//see if button is pressed, held or released
int btn_Press(BtnObj btnInst);
int btn_Release(BtnObj btnInst);
int btn_Hold(BtnObj btnInst);

//see if button is pressed without trigger processing
int btn_Pressed(BtnObj btnInst);

//button press arm/disarm
void btn_PArm(BtnObj btnInst);
void btn_PDisarm(BtnObj btnInst);

//button release arm/disarm
void btn_RArm(BtnObj btnInst);
void btn_RDisarm(BtnObj btnInst);

//button hold arm/disarm
void btn_HArm(BtnObj btnInst);
void btn_HDisarm(BtnObj btnInst);

#endif
