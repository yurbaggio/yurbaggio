/*
 *  Created on: Nov 26, 2015
 *      Author: Jianyi
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "nlib_err.h"
#include "nlib_cstring.h"
#include "nlib_cstring_queue.h"

//constructor, reserve memory
CqueueObj cqueue_New(){
  CqueueObj self = (CqueueObj) calloc(1, sizeof(struct cqueueObj));
  return self;
}

//initialize, reset tail and head
int cqueue_Init(CqueueObj self, void (*enInt_var)(void), void (*disInt_var)(void), int (*va_print_var)(const char *, ...), unsigned int bufferSize_var, unsigned int queueSize_var){

  if (self == NULL)
    return NLIB_ERR_NOMEM;

  //connect interrupt enable/disable
  self->enInt = enInt_var;
  self->disInt = disInt_var;
  self->bufferSize = bufferSize_var;
    self->queueSize = queueSize_var;
  self->va_print = va_print_var;

  //reset queue
  self->status = CQUEUE_QUEUE_NOTFULL;
  self->head = 0;
  self->tail = 0;

  //initialize transfer objects in the queue
    self->queue = (CstringObj *) calloc(self->queueSize, sizeof(CstringObj *));
  unsigned int i;
  for (i = 0; i < self->queueSize; i++){
    *(self->queue + i) = cstring_New(self->bufferSize);
        if(*(self->queue + i) == NULL)
            return NLIB_ERR_NOMEM;
  }

  //initialize the spare transfer object
  self->enqueueSpare = cstring_New(self->bufferSize);
    if (self->enqueueSpare == NULL)
        return NLIB_ERR_NOMEM;

    self->dequeueSpare = cstring_New(self->bufferSize);
    if (self->dequeueSpare == NULL)
        return NLIB_ERR_NOMEM;

  return NLIB_ERR_NOERROR;
}

//queue up a transfer, print what was queued if successful
int cqueue_EnqueueDebug(CqueueObj self, CstringObj data){

  int status = cqueue_Enqueue(self, data);

  if(status == CQUEUE_TRANS_SUCCESS){
    int i;
    self->va_print("-<-Enq:");
    for(i = 0; i < 8; i++)
      self->va_print("\t0x%02x", *(data + i));
    self->va_print("\r\n");
  }

  return status;
}

//queue up a transfer, swap with the spare
int cqueue_Enqueue(CqueueObj self, CstringObj data){

  //critical section, acquire lock
  self->disInt();

  //check for full, return error
  if(self->status == CQUEUE_QUEUE_FULL){
    self->enInt();
    return CQUEUE_TRANS_ENQUEUEFULL;
  }

  //copy data to spare
  cstring_Copy(data, self->enqueueSpare);

  //queue, increment head
  CstringObj swap = *(self->queue + self->head);
  *(self->queue + self->head++) = self->enqueueSpare;
  self->enqueueSpare = swap;

  if(self->head >= self->queueSize)
    self->head = 0;

  //checkfull
  if(self->head == self->tail)
    self->status = CQUEUE_QUEUE_FULL;

  //exit critical section, release lock
  self->enInt();
  return CQUEUE_TRANS_SUCCESS;
}

//queue up a transfer, print what was queued if successful
int cqueue_DequeueDebug(CqueueObj self, CstringObj data){

  int status = cqueue_Dequeue(self, data);

  if(status == CQUEUE_TRANS_SUCCESS){
    self->va_print("->-Deq:");
    int i;
    for(i = 0; i < 8; i++)
      self->va_print("\t0x%02x", *(data + i));
    self->va_print("\r\n");
  }

  return status;
}

//dequeue a transfer, swap with the spare
int cqueue_Dequeue(CqueueObj self, CstringObj target){

  //critical section, acquire lock
  self->disInt();

  //check for empty
  if((self->head == self->tail) && (self->status != CQUEUE_QUEUE_FULL)){
    self->enInt();
    return CQUEUE_TRANS_DEQUEUEEMPTY;
  }

  //dequeue, increment tail
  CstringObj swap = self->dequeueSpare;
  self->dequeueSpare = *(self->queue + self->tail);
  *(self->queue + self->tail++) = swap;

  //copy spare to target
  cstring_Copy(self->dequeueSpare, target);

  if(self->tail >= self->queueSize)
    self->tail = 0;

  self->status = CQUEUE_QUEUE_NOTFULL;

  //exit critical section, release lock
  self->enInt();
  return CQUEUE_TRANS_SUCCESS;
}
