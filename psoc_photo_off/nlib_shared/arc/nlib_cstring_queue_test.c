#include <stdlib.h>
#include "nlib_cstring.h"
#include "nlib_cstring_queue.h"
#include "nlib_cstring_queue_test.h"

//Testing section, used to test enqueue and dequeue
CqueueObj debugTestQueue;

//initialize test
int cqueue_TestQueueInit(void (*enInt_var)(void), void (*disInt_var)(void), int (*va_print_var)(const char *, ...)){

    //test cstring and cqueue
  debugTestQueue = cqueue_New();
  return cqueue_Init(debugTestQueue, enInt_var, disInt_var, va_print_var, CQUEUE_TESTSTRINGSIZE, CQUEUE_TESTQUEUESIZE);
}

//debug enqueue, dequeue
void cqueue_TestEnq(char *str){
  CstringObj transfer = cstring_New(CQUEUE_TESTSTRINGSIZE);
    if (transfer == NULL){
        debugTestQueue->va_print("Not enough memory to create test string, increase heap size\r\n");
        return;
    }

    static unsigned char tracker = 0;

    debugTestQueue->va_print("Tracker is %d\r\n", tracker);
  *(transfer->ptr) = tracker++;
  int ret = cqueue_Enqueue(debugTestQueue, transfer);
    cstring_Destroy(transfer);

    debugTestQueue->va_print("Enqueue return code is %d\r\n", ret);
  debugTestQueue->va_print("Queue status is %d\r\n", debugTestQueue->status);
  debugTestQueue->va_print("Head is %d\r\n", debugTestQueue->head);
  debugTestQueue->va_print("Tail is %d\r\n", debugTestQueue->tail);

    unsigned int i;
    for(i = 0; i < debugTestQueue->queueSize; i ++){
    if(*(debugTestQueue->queue + i) == NULL)
      debugTestQueue->va_print("Queue Location %d is empty\r\n", i);
    else
      debugTestQueue->va_print("Queue Location %d is %d\r\n", i, *((*(debugTestQueue->queue + i))->ptr));
  }
    debugTestQueue->va_print("\r\n");
}
void cqueue_TestDeq(char *str){
  CstringObj transfer = cstring_New(CQUEUE_TESTSTRINGSIZE);
    if (transfer == NULL){
        debugTestQueue->va_print("Not enough memory to create test string, increase heap size\r\n");
        return;
    }

  int ret = cqueue_Dequeue(debugTestQueue, transfer);
  if(transfer == NULL)
    debugTestQueue->va_print("Target is null\r\n");
  else{
    debugTestQueue->va_print("Value of Dequeued %d\r\n", *(transfer->ptr));
    cstring_Destroy(transfer);
  }

  debugTestQueue->va_print("Dequeue return code is %d\r\n", ret);
  debugTestQueue->va_print("Queue status is %d\r\n", debugTestQueue->status);
  debugTestQueue->va_print("Head is %d\r\n", debugTestQueue->head);
  debugTestQueue->va_print("Tail is %d\r\n", debugTestQueue->tail);

    unsigned int i;
  for(i = 0; i < debugTestQueue->queueSize; i ++){
    if(*(debugTestQueue->queue + i) == NULL)
      debugTestQueue->va_print("Queue Location %d is empty\r\n", i);
    else
      debugTestQueue->va_print("Queue Location %d is %d\r\n", i, *((*(debugTestQueue->queue + i))->ptr));
  }
    debugTestQueue->va_print("\r\n");
}
