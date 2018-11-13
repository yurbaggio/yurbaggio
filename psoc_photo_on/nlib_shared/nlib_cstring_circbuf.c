/*
 *  Created on: Nov 26, 2015
 *      Author: Jianyi
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "nlib_err.h"
#include "nlib_cstring.h"
#include "nlib_cstring_circbuf.h"

//constructor, reserve memory
CircbufObj circbuf_New(){
  CircbufObj self = (CircbufObj) calloc(1, sizeof(struct circbufObj));
  return self;
}

//initialize, reset objCount and head
int circbuf_Init(CircbufObj self, void (*enInt_var)(void), void (*disInt_var)(void), int (*va_print_var)(const char *, ...), unsigned int bufferSize_var, unsigned int queueSize_var){

  if (self == NULL)
    return NLIB_ERR_NOMEM;

  //connect interrupt enable/disable
  self->enInt = enInt_var;
  self->disInt = disInt_var;
  self->bufferSize = bufferSize_var;
  self->queueSize = queueSize_var;
  self->va_print = va_print_var;

  //reset queue
  self->head = 0;
  self->objCount = 0;

  //initialize transfer objects in the queue
  self->queue = (CstringObj *) calloc(self->queueSize, sizeof(CstringObj *));
  if(self->queue == NULL)
    return NLIB_ERR_NOMEM;
	
  unsigned int i;
  for (i = 0; i < self->queueSize; i++){
    *(self->queue + i) = cstring_New(self->bufferSize);
  if(*(self->queue + i) == NULL)
    return NLIB_ERR_NOMEM;
  }

  //initialize the spare transfer object
  self->pushSpare = cstring_New(self->bufferSize);
  if (self->pushSpare == NULL)
    return NLIB_ERR_NOMEM;
	
  return NLIB_ERR_NOERROR;
}

//push next value to circular buffer, with debug output
int circbuf_PushDebug(CircbufObj self, CstringObj data){

  int status = circbuf_Push(self, data);

  if(status == CIRCBUF_TRANS_SUCCESS){
    int i;
    self->va_print("-<-Enq:");
    for(i = 0; i < 8; i++)
      self->va_print("\t0x%02x", *(data + i));
    self->va_print("\r\n");
  }

  return status;
}

//push next value to circular buffer
int circbuf_Push(CircbufObj self, CstringObj data){

  //critical section, acquire lock
  self->disInt();

  //copy data to spare
  cstring_Copy(data, self->pushSpare);

  //queue, increment head
  CstringObj swap = *(self->queue + self->head);
  *(self->queue + self->head++) = self->pushSpare;
  self->pushSpare = swap;

  if(self->head >= self->queueSize)
    self->head = 0;

  //checkfull
  if(self->objCount < self->queueSize)
    self->objCount ++;

  //exit critical section, release lock
  self->enInt();
  return CIRCBUF_TRANS_SUCCESS;
}

//get get value of queue at noffset, copy to target, with debug output
int circbuf_GetDebug(CircbufObj self, unsigned int noffset, CstringObj data){
  int status = circbuf_Get(self, noffset, data);

  if(status == CIRCBUF_TRANS_SUCCESS){
    self->va_print("->-Deq:");
    int i;
    for(i = 0; i < 8; i++)
      self->va_print("\t0x%02x", *(data + i));
    self->va_print("\r\n");
  }

  return status;
}

//get get value of queue at noffset, copy to target, noffset is negative offset from the last pushed item
//IE, index of 0 is the last pushed item, index of 3 is the 3rd last pushed object
int circbuf_Get(CircbufObj self, unsigned int noffset, CstringObj target){

  //check noffset, too big
  if (noffset >= self->queueSize){
    self->enInt();
    return CIRCBUF_TRANS_GETINVALIDOFFSET;
  }

  //critical section, acquire lock
  self->disInt();

  //get index from noffset, roll over if negative
  int index = self->head - (noffset + 1);
  if (index < 0)
    index += self->queueSize;

  //check for empty object at index
  if((self->objCount < self->queueSize) && (index >= (int)self->head)){
    self->enInt();
    return CIRCBUF_TRANS_GETNULLOBJECT;
  }

  //copy spare to target
  cstring_Copy(*(self->queue + index), target);

  //exit critical section, release lock
  self->enInt();
  return CIRCBUF_TRANS_SUCCESS;
}

//gets the pointer at index
char * circbuf_GetPtr(CircbufObj self, unsigned int noffset){
	
  //check noffset, too big
  if (noffset >= self->queueSize){
    return NULL;
  }

  //critical section, acquire lock
  self->disInt();

  //get index from noffset, roll over if negative
  int index = self->head - (noffset + 1);
  if (index < 0)
    index += self->queueSize;

      //check for empty object at index
  if((self->objCount < self->queueSize) && (index >= (int)self->head)){
    self->enInt();
    return NULL;
  }

  //exit critical section, release lock
  self->enInt();
  return ((*(self->queue + index))->ptr);
}

//Testing section, used to test push and get
CircbufObj debugTestCircbuf;

//Testing section, use debugTestQueue to test enqueue and dequeue
int cproc_TestCircbufInit(void (*enInt_var)(void), void (*disInt_var)(void), int (*va_print_var)(const char *, ...)){
    
  //test cstring and circbuf
  debugTestCircbuf = circbuf_New();
  return circbuf_Init(debugTestCircbuf, enInt_var, disInt_var, va_print_var, CIRCBUF_TESTSTRINGSIZE, CIRCBUF_TESTQUEUESIZE);
}

//debug enqueue, dequeue
void cproc_TestPush(char *str){
  CstringObj transfer = cstring_New(CIRCBUF_TESTSTRINGSIZE);
  if (transfer == NULL){
    debugTestCircbuf->va_print("Not enough memory to create test string, increase heap size\r\n");
    return;
  }

  static unsigned char tracker = 0;

  debugTestCircbuf->va_print("Tracker is %d\r\n", tracker);
    *(transfer->ptr) = tracker++;
    int ret = circbuf_Push(debugTestCircbuf, transfer);

  debugTestCircbuf->va_print("Enqueue return code is %d\r\n", ret);
    debugTestCircbuf->va_print("Head is %d\r\n", debugTestCircbuf->head);
    debugTestCircbuf->va_print("objCount is %d\r\n", debugTestCircbuf->objCount);

  unsigned int i;
  for(i = 0; i < debugTestCircbuf->queueSize; i ++){
  int ret = circbuf_Get(debugTestCircbuf, i, transfer);
    if(ret != CIRCBUF_TRANS_SUCCESS)
      debugTestCircbuf->va_print("Queue Location %d is empty, ret code %d\r\n", i, ret);
    else
      debugTestCircbuf->va_print("Queue Location %d is %d, ret code %d\r\n", i, *(transfer->ptr), ret);
  }
  debugTestCircbuf->va_print("\r\n");
  cstring_Destroy(transfer);
}
