/*
 * nlib_per_sproc_circbuf.h
 *
 *	Xilinx AXI_QUAD_SPI in standard mode, transfer queue
 *
 *  Created on: Nov 23, 2015
 *      Author: Jianyi
 */

#ifndef NLIB_CSTRING_CIRCBUF_C_
#define NLIB_CSTRING_CIRCBUF_C_

#include <stdarg.h>
#include "nlib_cstring_circbuf.h"

#define CIRCBUF_TRANS_SUCCESS		    0
#define CIRCBUF_TRANS_GETNULLOBJECT     -1
#define CIRCBUF_TRANS_GETINVALIDOFFSET	-2

//spi transfer queues
typedef struct circbufObj * CircbufObj;

//object used to form a transfer
struct circbufObj{

  //function pointers that enables or disables interrupts
  void (*enInt)(void);
  void (*disInt)(void);
  int (*va_print)(const char *format, ...);  //link this to either vprintf or vsprintf
  unsigned int bufferSize;
  unsigned int queueSize;

  //pointer, status of queue
  unsigned int head;
  unsigned int objCount;                       //number of objects in the queue
  CstringObj * queue;	                //write requests, this is a pointer to a CstringObj object, use calloc to allocate queuesize of Cstring for the main queue storage
  CstringObj pushSpare;				//spare slot for enqueuing
};

//constructor, reserve memory
CircbufObj circbuf_New();

//initialize, reset objCount and head
int circbuf_Init(CircbufObj self, void (*enInt_var)(void), void (*disInt_var)(void), int (*va_print_var)(const char *, ...), unsigned int bufferSize_var, unsigned int queueSize_var);

//push next value to circular buffer
int circbuf_Push(CircbufObj self, CstringObj data);

//push next value to circular buffer, with debug output
int circbuf_PushDebug(CircbufObj self, CstringObj data);

//get get value of queue at noffset, copy to target, noffset is negative offset from the last pushed item
//IE, index of 0 is the last pushed item, index of 3 is the 3rd last pushed object
int circbuf_Get(CircbufObj self, unsigned int index, CstringObj target);

//get get value of queue at noffset, copy to target, with debug output
int circbuf_GetDebug(CircbufObj self, unsigned int index, CstringObj data);

//gets the pointer at noffset
char * circbuf_GetPtr(CircbufObj self, unsigned int noffset);

//test
#define CIRCBUF_TESTSTRINGSIZE 10
#define CIRCBUF_TESTQUEUESIZE 5

//initialize test
int cproc_TestCircbufInit(void (*enInt_var)(void), void (*disInt_var)(void), int (*va_print_var)(const char *, ...));

//debug enqueue, dequeue
void cproc_TestPush(char *str);

#endif
