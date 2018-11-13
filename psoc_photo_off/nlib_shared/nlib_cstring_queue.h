/*
 *  Created on: Nov 23, 2015
 *      Author: Jianyi
 */

#ifndef NLIB_CSTRING_QUEUE_H_
#define NLIB_CSTRING_QUEUE_H_

#include <stdarg.h>
#include "nlib_cstring_queue.h"

#define CQUEUE_QUEUE_NOTFULL  0
#define CQUEUE_QUEUE_FULL 1

#define CQUEUE_TRANS_SUCCESS      0
#define CQUEUE_TRANS_ENQUEUEFULL    -1
#define CQUEUE_TRANS_DEQUEUEEMPTY -2

//spi transfer queues
typedef struct cqueueObj * CqueueObj;

//object used to form a transfer
struct cqueueObj{

  //function pointers that enables or disables interrupts
  void (*enInt)(void);
  void (*disInt)(void);
  int (*va_print)(const char *format, ...);  //link this to either vprintf or vsprintf
  unsigned int bufferSize;
  unsigned int queueSize;

  //pointer, status of queue
  int status;
  unsigned int head;
  unsigned int tail;
  CstringObj * queue;                     //write requests, this is a pointer to a CstringObj object, use calloc to allocate queuesize of Cstring for the main queue storage
  CstringObj enqueueSpare;        //spare slot for enqueuing
  CstringObj dequeueSpare;        //spare slot for dequeuing
};

//constructor, reserve memory
CqueueObj cqueue_New();

//initialize, reset tail and head
int cqueue_Init(CqueueObj self, void (*enInt_var)(void), void (*disInt_var)(void), int (*va_print_var)(const char *, ...), unsigned int bufferSize_var, unsigned int queueSize_var);

//queue up a transfer
int cqueue_Enqueue(CqueueObj self, CstringObj data);

//queue up a transfer, print what was queued if successful
int cqueue_EnqueueDebug(CqueueObj self, CstringObj data);

//dequeue a transfer
int cqueue_Dequeue(CqueueObj self, CstringObj target);

//queue up a transfer, print what was queued if successful
int cqueue_DequeueDebug(CqueueObj self, CstringObj data);

#endif
