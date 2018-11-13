/******************************************************************************
 Jianyi Liu
 Command processing module that works with USB CDC 
 ******************************************************************************/

#include <stdlib.h>
#include "nlib_err.h"
#include "nlib_cstring.h"
#include "nlib_cproc_cdc.h"

//constructor, reserve memory
CcdcObj ccdc_New() {
	CcdcObj self = (CcdcObj) calloc(1, sizeof(struct ccdcObj));
	return self;
}

//initializes
int ccdc_Init(CcdcObj self, unsigned int maxBufferSize){
    if (self == NULL)
	    return NLIB_ERR_NOMEM;
    
    //reserve memory for incoming buffer
    self->incomingString = cstring_New(maxBufferSize);
    if (self->incomingString == NULL)
        return NLIB_ERR_NOMEM;
    
    self->incomingCount = 0;
    self->incomingCountLim = 0;
    
    return NLIB_ERR_NOERROR;
}

//write a new string to the getchar buffer
void ccdc_LoadNewGetCharBuffer(CcdcObj self, unsigned char *buffer, unsigned int length){
    
    //copy to string
    cstring_CopyFromArray((char *)buffer, self->incomingString, length);
    self->incomingCountLim = length;
    self->incomingCount = 0;    
}

//get remaining size of string loaded through ccdc_setupGetChar()
unsigned char ccdc_GetBufferSize(CcdcObj self){
    if (self->incomingCount < self->incomingCountLim)
        return 1;
    return 0;
}
		
//get the next character, reduce buffer size
unsigned char ccdc_GetChar(CcdcObj self){
    if (self->incomingCount < self->incomingCountLim)
        return self->incomingString->ptr[self->incomingCount++];
    return 0;
}
