//generic queue

#include <stdlib.h>
#include <string.h>
#include "nlib_sys_err.h"
#include "nlib_sys_queue.h"

//constructor, reserve memory for list only
NqObj nq_New(){
    NqObj self = (NqObj)calloc(1, sizeof(struct nqObj));
    return self;
}

//init will reserve memory for listSize, this way we find out if there are issues with memory at boot time
//the struct for the pointer to data is of size dataSize
int nq_Init(NqObj self, int q_dataSize, int q_listSize, void (*enInt_var)(void), void (*disInt_var)(void), void (*deepCopy)(void *, const void *)){
    
    //test to see if memory reservation was successful
    if (self == NULL)
	    return NLIB_ERR_NOMEM;
    
    //reserve memory for node and for the data inside node
    self->head = (NqNode)calloc(1, sizeof(struct nqNode));
    if (self->head == NULL)
        return NLIB_ERR_NOMEM;
    
    self->enInt = enInt_var;
    self->disInt = disInt_var;
    
    self->q_dataSize = q_dataSize;
    self->q_listSize = q_listSize;
    
    self->deepCopy = deepCopy;
    
    NqNode traverse = self->head;
    
    //reserve one extra to make it easier for queue, waste a little bit of memory so the head/tail pointers never overlap
    int i;
    for(i = 1; i <= q_listSize; i ++){
        traverse->next = (NqNode)calloc(1, sizeof(struct nqNode));
        if (traverse->next == NULL)
            return NLIB_ERR_NOMEM;
        
        traverse = traverse->next;  //go to the next one
    }
    
    //link last instance one to head
    //we now have a circular set of pointers
    traverse->next = self->head;
    
    //now we reserve memory for the actuall stored data
    traverse = self->head;
    do{
        traverse->data = malloc(q_dataSize);  //reserve memory
        if (traverse->data == NULL)
            return NLIB_ERR_NOMEM;
        
        traverse = traverse->next;          //go to next node
    }while(traverse != self->head);
    
    //empty the queue, make tail equal to head
    self->tail = self->head;
    
    //memory reserved, exit with success
    return NLIB_ERR_NOERROR;
}

//enqueue data, will make a SHALLOW copy of data
//If it contains pointers, the reference will NOT be copied
int nq_Enq(NqObj self, const void *data){
    
    //atomic action
    self->enInt();
    
    //check if there's space, if the next one is the head, queue is full
    if(self->tail->next == self->head){
        //self->enInt();
        return NQ_ENQUE_FULL;
    }
    
    //copy the data
    if(self->deepCopy == NULL)
        memcpy(self->tail->data, data, self->q_dataSize);
    else
        self->deepCopy(self->tail->data, data);
    
    //move the tail
    self->tail = self->tail->next;
    
    self->enInt();
    return NQ_SUCCESS;
}

//dequeue data, data is SHALLOW copied to data pointer
int nq_Deq(NqObj self, void *data){
    
    //atomic action
    self->disInt();
    
    //check if empty
    if(self->head == self->tail){
        self->enInt();
        return NQ_DEQUE_EMPTY;
    }
    
    //copy out the data
    if(self->deepCopy == NULL)
        memcpy(data, self->head->data, self->q_dataSize);
    else
        self->deepCopy(data, self->head->data);
    
    //move the head
    self->head = self->head->next;
        
    //self->enInt();
    return NQ_SUCCESS;
}

//clear the queue
void nq_Clear(NqObj self){
    
    //atomic action
    self->disInt();
    
    self->tail = self->head;
    
    self->enInt();
}

//get number of objects in queue
int nq_Count(NqObj self){
    
    //atomic action
    self->disInt();
    
    int count = 0;
    
    NqNode traverse = self->head;
    
    while(traverse != self->tail){
        count ++;
        traverse = traverse->next;
    }
    
    self->enInt();
    return count;
}

/* [] END OF FILE */
