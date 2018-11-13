/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Mar 30, 2017
Modified	Mar 30, 2010
Override the interrupt routine in the library
Macros for various stuff
******************************************************************************/

#ifndef nlib_qp_h
#define nlib_qp_h

//QP contants
#define NLIB_NULL_SIG	0

//error event
typedef struct ErrEvtTag {
	QEvent super;
	int	errCode;
} ErrEvt;
typedef struct DefEvtTag {
	QEvent super;
}DefEvt;

#endif
