//generic queue

#ifndef NLIB_SYS_QUEUE_H
#define NLIB_SYS_QUEUE_H
    
//define a node
typedef struct nqNode * NqNode;
struct nqNode{
    void *data;
    NqNode next;
};

//define node queue
typedef struct nqObj * NqObj;

struct nqObj{
    int q_dataSize;
    int q_listSize;
    NqNode head;
    NqNode tail;
    void (*enInt)(void);    //enable global interrupts
    void (*disInt)(void);   //disable global interrupts
    
    //this is a function that takes care of copying structs with pointers
    //assign as NULL if you want to use memcpy to shallow copy the data (IE all primitives)
    //if the structure is complex, a custom deepcopy function must be created accordingly
    //dest is the pointer to the destination structure, src is the pointer to the source data
    //make sure to cast the pointers accordingly, these are void pointers!
    void (*deepCopy)(void * dest, const void * src);
};

#define NQ_SUCCESS          0
#define NQ_ENQUE_FULL	    -1
#define NQ_DEQUE_EMPTY      -2
    
//constructor, reserve memory for list only
NqObj nq_New();

//init will reserve memory for listSize, this way we find out if there are issues with memory at boot time
//the struct for the pointer to data is of size dataSize
int nq_Init(NqObj self, int q_dataSize, int q_listSize, void (*enInt)(void), void (*disInt)(void), void (*deepCopy)(void *, const void *));

//enqueue data, will make a SHALLOW copy of data
//If it contains pointers, the reference will NOT be copied
int nq_Enq(NqObj self, const void *data);

//dequeue data, data is SHALLOW copied to data pointer
int nq_Deq(NqObj self, void *data);

//clear the queue
void nq_Clear(NqObj self);

//get number of objects in queue
int nq_Count(NqObj self);

//run test
void nq_Test(void (*enInt)(void), void (*disInt)(void), int (*va_print)(const char *format, ...));

#endif

/* [] END OF FILE */
