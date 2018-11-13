/******************************************************************************
Jianyi Liu
Command processing module that works with USB CDC 
******************************************************************************/

#ifndef NLIB_CPROC_CDC_H
#define NLIB_CPROC_CDC_H

#include "nlib_cstring.h"
    
//Module object
typedef struct ccdcObj * CcdcObj;

struct ccdcObj{
    unsigned int maxBufferSize;
    
    CstringObj incomingString;
    unsigned int incomingCount;
    unsigned int incomingCountLim;
};

//constructor, reserve memory
CcdcObj ccdc_New();

//initializes
int ccdc_Init(CcdcObj self, unsigned maxBufferSize);

//write a new string to the getchar buffer
void ccdc_LoadNewGetCharBuffer(CcdcObj self, unsigned char *buffer, unsigned int length);

//get remaining size of string loaded through ccdc_setupGetChar()
unsigned char ccdc_GetBufferSize(CcdcObj self);
		
//get the next character, reduce buffer size
unsigned char ccdc_GetChar(CcdcObj self);

#endif
