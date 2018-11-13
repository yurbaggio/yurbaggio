/*
 * nlib_per_sproc_Dyqueue.h
 *
 *	Xilinx AXI_QUAD_SPI in standard mode, transfer queue
 *
 *  Created on: Nov 23, 2015
 *      Author: Jianyi
 */

#ifndef NLIB_PER_CSTRING_DYQUEUE_H_
#define NLIB_PER_CSTRING_DYQUEUE_H_

#include <stdarg.h>
#include "nlib_per_cstring.h"

#define Dyqueue_QUEUE_NOTFULL	0
#define Dyqueue_QUEUE_FULL	1

#define Dyqueue_TRANS_SUCCESS            0
#define Dyqueue_TRANS_ENQUEUEFULL        -1
#define Dyqueue_TRANS_DEQUEUEEMPTY       -2

//spi transfer queues
typedef struct DyqueueObj * DyqueueObj;

//object used to form a transfer
struct DyqueueObj{

  //function pointers that enables or disables interrupts
  void (*enInt)(void);
  void (*disInt)(void);
  unsigned int queueSize;

  //pointer, status of queue
  int status;
  unsigned int head;
  unsigned int tail;
  CstringObj * queue;	                    //write requests, this is a pointer to a CstringObj object, use calloc to allocate queuesize of Cstring for the main queue storage
};

//constructor, reserve memory
DyqueueObj Dyqueue_New();

//initialize, reset tail and head
int Dyqueue_Init(DyqueueObj self, void (*enInt_var)(void), void (*disInt_var)(void), unsigned int queueSize_var);

//queue up a transfer
int Dyqueue_Enqueue(DyqueueObj self, CstringObj *data);

//dequeue a transfer
int Dyqueue_Dequeue(DyqueueObj self, CstringObj *target);

//get approx size of queue
int Dyqueue_GetSize(DyqueueObj self);


#endif
