/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jul 16, 2007
Modified		Apr 08, 2011
Buttons headers
******************************************************************************/

#ifndef NLIB_DRV_SIG_H
#define NLIB_DRV_SIG_H

typedef struct sigObj * SigObj;

//define sxtmr object (device control object), undefine for DEBUG, this should be hidden usually!
struct sigObj{
	
	int (*sigSample)(void);			//this is the function pointer that samples the button/input
	unsigned s0:1;		//sample0 register
	unsigned s1:1;		//sample1 register
	unsigned int pLim;	//signal sample count limit
	unsigned int pCnt;	//signal sample counter
};

//constructor, resrve memory
SigObj sig_New();

//initializer
int sig_Init(SigObj self,  int (*sigSample_func)(void), unsigned int pLim_var);

//process events
void sig_Proc(SigObj self);

//see if button is pressed, held or released
int sig_Value(SigObj self);
int sig_Rise(SigObj self);
int sig_Fall(SigObj self);

#endif
