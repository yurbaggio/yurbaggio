
#include <stdlib.h>
#include "nlib_sys_err.h"
#include "nlib_sys_mblock.h"

//constructor, reserve memory for pointers only
MbObj mb_New(){
    MbObj self = (MbObj)calloc(1, sizeof(struct mbObj));
    return self;
}

//initialize
int mb_Init(MbObj self, unsigned int maxSize){
    
    //test to see if memory reservation was successful
    if (self == NULL)
	    return NLIB_ERR_NOMEM;
    
    self->maxSize = maxSize;
    
    //reserve memory for entire block
    self->head = (unsigned char *)malloc(maxSize);
    
    //reset block
    mb_Reset(self);
    
    return NLIB_ERR_NOERROR;
}

//reset block
void mb_Reset(MbObj self){
    self->dataSize = 0;
    self->next = self->head;
    
    //zero out
    unsigned int i;
    for(i = 0; i < self->maxSize; i++){
        *(self->head + i) = 0xAA;
    }
}

//reserve memory and increment valid data, returns the pointer where bytesReverved number of bytes can be written to, in sequence
//returns NULL if there is not enough space in the memory block
unsigned char * mb_Reserve(MbObj self, unsigned int bytesReserved){
    
    //check if there is enough space to reserve
    if((bytesReserved + self->dataSize) > self->maxSize)
        return NULL;
    
    //return the location where we can start writing
    unsigned char * ret = self->next;
    self->next = (self->next + bytesReserved);
    
    //increment the bytes reserved
    self->dataSize += bytesReserved;
    
    return ret;
}

/* [] END OF FILE */
