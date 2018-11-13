/*
 *  Created on: Nov 26, 2015
 *      Author: Jianyi
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "nlib_sys_err.h"
#include "nlib_per_cstring.h"
#include "nlib_per_cstring_dyqueue.h"
#include "nlib_per_cstring_dyqueue_test.h"

//Testing section, used to test enqueue and dequeue
static DyqueueObj debugTestQueue;
static int (*va_print_ptr)(const char *, ...);

//initialize test
int dyqueue_TestQueueInit(void (*enInt_var)(void), void (*disInt_var)(void), int (*va_print)(const char *, ...)){

  //print function
  va_print_ptr = va_print;

  //test cstring and Dyqueue
	debugTestQueue = Dyqueue_New();
	return Dyqueue_Init(debugTestQueue, enInt_var, disInt_var, DYQUEUE_TESTQUEUESIZE);
}

//debug enqueue, dequeue
void dyqueue_TestEnq(const char *str){

  CstringObj transfer = cstring_NewFromArray(str, strlen(str), strlen(str));
  if (transfer == NULL){
    va_print_ptr("Not enough memory to create test string, increase heap size\r\n");
      return;
  }

  va_print_ptr("Enqueue: %s\r\n", str);
	int ret = Dyqueue_Enqueue(debugTestQueue, &transfer);
	va_print_ptr("Enqueue return code is %d\r\n", ret);
	va_print_ptr("Queue status is %d\r\n", debugTestQueue->status);
	va_print_ptr("Head is %d\r\n", debugTestQueue->head);
	va_print_ptr("Tail is %d\r\n", debugTestQueue->tail);
	
  unsigned int i;
  for(i = 0; i < debugTestQueue->queueSize; i ++){
  if(*(debugTestQueue->queue + i) == NULL)
    va_print_ptr("Queue Location %d is empty\r\n", i);
  else
    va_print_ptr("Queue Location %d: %s\r\n", i, (*(debugTestQueue->queue + i))->ptr);
	}
  va_print_ptr("\r\n");
}

void dyqueue_TestDeq(const char *str){

  CstringObj transfer = NULL;

	int ret = Dyqueue_Dequeue(debugTestQueue, &transfer);
	if(transfer == NULL)
		va_print_ptr("Dequeue: Target is null\r\n");
	else{
		va_print_ptr("Dequeue: %s\r\n", transfer->ptr);
		cstring_Destroy(transfer);
	}
    
	va_print_ptr("Dequeue return code is %d\r\n", ret);
	va_print_ptr("Queue status is %d\r\n", debugTestQueue->status);
	va_print_ptr("Head is %d\r\n", debugTestQueue->head);
	va_print_ptr("Tail is %d\r\n", debugTestQueue->tail);
    
  unsigned int i;
	for(i = 0; i < debugTestQueue->queueSize; i ++){
		if(*(debugTestQueue->queue + i) == NULL)
			va_print_ptr("Queue Location %d is empty\r\n", i);
		else
			va_print_ptr("Queue Location %d: %s\r\n", i, (*(debugTestQueue->queue + i))->ptr);
	}
  va_print_ptr("\r\n");
}
