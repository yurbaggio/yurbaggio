/* memory block data structure, accepts unsigned char */

#ifndef NLIB_SYS_MBLOCK_H
#define NLIB_SYS_MBLOCK_H

//define the 
typedef struct mbObj * MbObj;
    
//structure is NOT threadsafe, should not be used in different threads
struct mbObj{
    unsigned int maxSize;          //size of memory block
    unsigned char *head;
    unsigned char *next;    //where to put the next data
    unsigned int dataSize;         //how many bytes are used up
};

//constructor, reserve memory for pointers only
MbObj mb_New();

//initialize
int mb_Init(MbObj self, unsigned int maxSize);

//reset block
void mb_Reset(MbObj self);

//reserve memory and increment valid data, returns the pointer where bytesReverved number of bytes can be written to, in sequence
//returns NULL if there is not enough space in the memory block
unsigned char * mb_Reserve(MbObj self, unsigned int bytesReserved);

#endif

/* [] END OF FILE */
