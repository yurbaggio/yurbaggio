/*
 *  Created on: Nov 26, 2015
 *      Author: Jianyi
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "nlib_sys_err.h"
#include "nlib_per_cstring.h"
#include "nlib_per_cstring_dyqueue.h"

//constructor, reserve memory
DyqueueObj Dyqueue_New(){
  DyqueueObj self = (DyqueueObj) calloc(1, sizeof(struct DyqueueObj));
  return self;
}

//initialize, reset tail and head
int Dyqueue_Init(DyqueueObj self, void (*enInt_var)(void), void (*disInt_var)(void), unsigned int queueSize_var){

  if (self == NULL)
    return NLIB_ERR_NOMEM;

  //connect interrupt enable/disable
  self->enInt = enInt_var;
  self->disInt = disInt_var;
  self->queueSize = queueSize_var;

  //reset queue
  self->status = Dyqueue_QUEUE_NOTFULL;
  self->head = 0;
  self->tail = 0;

  //initialize transfer objects in the queue
  self->queue = (CstringObj *) calloc(self->queueSize, sizeof(CstringObj *));

  return NLIB_ERR_NOERROR;
}

//queue up a transfer, swap with the spare
int Dyqueue_Enqueue(DyqueueObj self, CstringObj *data){

  //critical section, acquire lock
  self->disInt();

  //check for full, return error
  if(self->status == Dyqueue_QUEUE_FULL){
    self->enInt();
    return Dyqueue_TRANS_ENQUEUEFULL;
  }

  //queue, increment head
  *(self->queue + self->head++) = *data;
  if(self->head >= self->queueSize)
    self->head = 0;

  //checkfull
  if((self->head == self->tail) && (self->status == Dyqueue_QUEUE_NOTFULL))
    self->status = Dyqueue_QUEUE_FULL;

  //exit critical section, release lock
  self->enInt();
  return Dyqueue_TRANS_SUCCESS;
}

//dequeue a transfer, swap with the spare
int Dyqueue_Dequeue(DyqueueObj self, CstringObj *target){

  //critical section, acquire lock
  self->disInt();

  //check for empty
  if((self->head == self->tail) && (self->status == Dyqueue_QUEUE_NOTFULL)){
    *target = NULL;
    self->enInt();
    return Dyqueue_TRANS_DEQUEUEEMPTY;
  }

  //dequeue, increment tail
  *target = *(self->queue + self->tail++);
  if(self->tail >= self->queueSize)
    self->tail = 0;

  self->status = Dyqueue_QUEUE_NOTFULL;

  //exit critical section, release lock
  self->enInt();
  return Dyqueue_TRANS_SUCCESS;
}

//get approx size of queue
int Dyqueue_GetSize(DyqueueObj self){

  int ret;

  //critical section, acquire lock
  self->disInt();

  if(self->status == Dyqueue_QUEUE_FULL)
    ret = self->queueSize;
  else if((self->head == self->tail) && (self->status != Dyqueue_QUEUE_FULL))
    ret = 0;
  else if(self->head > self->tail)
    ret = self->head - self->tail;
  else
    ret = self->head + (self->queueSize - self->tail);

  self->enInt();

  return ret;
}
