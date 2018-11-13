//double buffered memory block, uses mblock, thread safe

#ifndef NLIB_SYS_DBMBLOCK_H
#define NLIB_SYS_DBMBLOCK_H

#include "nlib_sys_mblock.h"

//return codes for loading
#define DBMB_LOAD_SUCCESS   0   //data was loaded into the buffer
#define DBMB_LOAD_SWAPPED   -1  //buffers were swapped, data loaded to the buffer after swap
#define DBMB_LOAD_ERROR     -2  //error occured during swap, did not work

#define DBMB_READ_LOCKED    0
#define DBMB_READ_UNLOCKED  1
    
//define the 
typedef struct dbmbObj * DbmbObj;
    
//structure is NOT threadsafe, should not be used in different threads
struct dbmbObj{
    MbObj buf1;
    MbObj buf2;
    MbObj writeBuf;   //buffer being written to
    MbObj readBuf;    //buffer not written to, safe to read
    
    void (*enInt)(void);    //enable global interrupts
    void (*disInt)(void);   //disable global interrupts
};

//constructor, reserve memory for pointers only
DbmbObj dbmb_New();

//initializer
int dbmb_Init(DbmbObj self, unsigned int maxSize, void (*enInt_var)(void), void (*disInt_var)(void));

//packs data into the destination, up to U32
void dbmb_Pack(unsigned char * dest, unsigned int * data, unsigned int dataLen, unsigned int bytesPerData);

//swaps the write and read buffers
void dbmb_Swap(DbmbObj self);

//this does not swap, returns error code if no memory
int dbmb_LoadNoSwap(DbmbObj self, unsigned int * data, unsigned int dataLen, unsigned int bytesPerData);

//loads a dataLen number of bytes into the write buffer
//if the entire sequence does not fit (dataLen number of bytes not available in writeBuf, swap, then write to new writeBuf
int dbmb_Load(DbmbObj self, unsigned int * data, unsigned int dataLen, unsigned int bytesPerData);
    
//returns the head pointer of the read buffer
unsigned char * dbmb_GetReadData(DbmbObj self);

//returns the number of bytes valid in the read buffer
unsigned int dbmb_GetReadDataSize(DbmbObj self);

#endif
/* [] END OF FILE */
