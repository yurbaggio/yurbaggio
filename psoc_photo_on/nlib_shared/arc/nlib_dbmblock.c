//double buffered memory block, uses mblock, thread safe

#include <stdlib.h>
#include "nlib_sys_err.h"
#include "nlib_sys_conv.h"
#include "nlib_sys_mblock.h"
#include "nlib_sys_dbmblock.h"

//constructor, reserve memory for pointers only
DbmbObj dbmb_New(){
    DbmbObj self = (DbmbObj)calloc(1, sizeof(struct dbmbObj));
    return self;
}

//initializer
int dbmb_Init(DbmbObj self, unsigned int maxSize, void (*enInt_var)(void), void (*disInt_var)(void)){
    
    //test to see if memory reservation was successful
    if (self == NULL)
	    return NLIB_ERR_NOMEM;
    
    self->enInt = enInt_var;
    self->disInt = disInt_var;
    
    //reserve memory, check for heap shortage
    int status;
    self->buf1 = mb_New();
    self->buf2 = mb_New();
    
    status = mb_Init(self->buf1, maxSize);
    if (status != NLIB_ERR_NOERROR)
        return NLIB_ERR_NOMEM;
    mb_Init(self->buf2, maxSize);
    if (status != NLIB_ERR_NOERROR)
        return NLIB_ERR_NOMEM;
    
    //both buffers are reset at this point, select one for writing, one for reading
    //remember only buf1 and buf2 are allocated!  writePtr and readPtr are used just for accessing the buffers
    self->writeBuf = self->buf1;
    self->readBuf = self->buf2;
    
    mb_Reset(self->writeBuf);
    mb_Reset(self->readBuf);
    
    return NLIB_ERR_NOERROR;
}

//swaps the write and read buffers
void dbmb_Swap(DbmbObj self){
    MbObj swap = self->readBuf;
    self->readBuf = self->writeBuf;
    self->writeBuf = swap;
    mb_Reset(self->writeBuf);
}

//tries to reserve memory, swap if necessary
int dbmb_Reserve(DbmbObj self, unsigned int dataLen, unsigned char ** ptr){
    
    int ret = DBMB_LOAD_SUCCESS;
    
    *ptr = mb_Reserve(self->writeBuf, dataLen);
    
    //swap if there's not enough memory
    if(*ptr == NULL){
        
        dbmb_Swap(self);
        
        //now reserve memory, if this fails, then dataLen is way too long
        *ptr = mb_Reserve(self->writeBuf, dataLen);
        ret = DBMB_LOAD_SWAPPED;
        
        //if pointer is STILL null at this moment, something is very wrong, exit with error code
        if(*ptr == NULL){
            ret = DBMB_LOAD_ERROR;
        }
    }
    
    return ret;
}

//packs data into the destination, up to U32
void dbmb_Pack(unsigned char * dest, unsigned int * data, unsigned int dataLen, unsigned int bytesPerData){
    
    unsigned int i;
    for(i = 0; i < dataLen; i ++){
        
        *(dest + i*bytesPerData + 0) = INTLL2CHAR(*(data + i));     //1 byte
        
        if (bytesPerData > 1)
            *(dest + i*bytesPerData + 1) = INTLH2CHAR(*(data + i)); //2 bytes
        if (bytesPerData > 2)
            *(dest + i*bytesPerData + 2) = INTHL2CHAR(*(data + i)); //3 bytes
        if (bytesPerData > 3)
            *(dest + i*bytesPerData + 3) = INTHH2CHAR(*(data + i)); //4 bytes
    }
}

//this does not swap, returns error code if no memory
int dbmb_LoadNoSwap(DbmbObj self, unsigned int * data, unsigned int dataLen, unsigned int bytesPerData){
    
    //operation must be atomic
    self->disInt();
    
    unsigned char * ptr = mb_Reserve(self->writeBuf, dataLen * bytesPerData);
    
    if (ptr == NULL){
        self->enInt();
        return DBMB_LOAD_ERROR;
    }
    
    //pack the data according to bytes per Data
    dbmb_Pack(ptr, data, dataLen, bytesPerData);
    
    self->enInt();
    return DBMB_LOAD_SUCCESS;
}

//loads a dataLen number of bytes into the write buffer
//if the entire sequence does not fit (dataLen number of bytes not available in writeBuf, swap, then write to new writeBuf
int dbmb_Load(DbmbObj self, unsigned int * data, unsigned int dataLen, unsigned int bytesPerData){
    
    //operation must be atomic
    self->disInt();
    
    unsigned char * ptr;
    int ret = dbmb_Reserve(self, dataLen * bytesPerData, &ptr);
    
    if(ret == DBMB_LOAD_ERROR){
        self->enInt();
        return ret;
    }
    
    //we found a good block of memory, copy it over
    dbmb_Pack(ptr, data, dataLen, bytesPerData);
    
    self->enInt();
    return ret;
}

//returns the head pointer of the read buffer
unsigned char * dbmb_GetReadData(DbmbObj self){
    return self->readBuf->head;
}

//returns the number of bytes valid in the read buffer
unsigned int dbmb_GetReadDataSize(DbmbObj self){
    return self->readBuf->dataSize;
}

/* [] END OF FILE */
