#include <stdlib.h>
#include "nlib_sys_queue.h"
#include "nlib_sys_queue_test.h"

//queue test
#define NQ_TEST_QUEUESIZE   5
NqObj TestQueue;

//print what's in the list
void nq_TestPrint(NqObj queue, int (*va_print)(const char *format, ...)){
    
    NqNode traverse = queue->head;

    //print
    va_print("Printing queue\n\r");
    
    //check empty
    if(queue->head == queue->tail){
        va_print("The queue is empty, nothing to print\n\r\n\r");
        return;
    }
    
    int i = 0;
    while(traverse->next != queue->tail->next){
        //remember queue->data is a type *void
        //first cast to a pointer of type INT, then derefence the data
        va_print("Value at location head + %d is: %d\n\r", i, (*(int*)(traverse->data)));
        traverse = traverse->next;
        i++;
    }
    
    va_print("Total of %d nodes found in queue\n\r\n\r", nq_Count(queue));
}


//run test
void nq_Test(void (*enInt)(void), void (*disInt)(void), int (*va_print)(const char *format, ...)){
    
    //initialize
    int status;
    TestQueue = nq_New();
    status = nq_Init(TestQueue, sizeof(int), NQ_TEST_QUEUESIZE, enInt, disInt, NULL);
    va_print("Queue init status %d\n\r\n\r", status);
    
    //enqueue
    va_print("Enqueue value of 1 to 3\n\r");
    int i;
    for(i = 1; i <= 3; i ++){
        status = nq_Enq(TestQueue, &i);
        nq_TestPrint(TestQueue, va_print);
        if(status == NQ_ENQUE_FULL){
            va_print("Queue is full, breaking loop\n\r");
            break;
        }
    }
    
    //print what's in the queue
    nq_TestPrint(TestQueue, va_print);
        
    //dequeue
    int data;
    do{
        status = nq_Deq(TestQueue, &data);
        
        if(status == NQ_SUCCESS){
            va_print("Dequeue successful: %d\n\r", data);
        }else{
            va_print("Queue is empty\n\r", data);
        }
        
        nq_TestPrint(TestQueue, va_print);
    }while(status == NQ_SUCCESS);
    
    //enqueue
    va_print("Enqueue value of 7 to 15\n\r");
    for(i = 7; i <= 15; i ++){
        status = nq_Enq(TestQueue, &i);
        nq_TestPrint(TestQueue, va_print);
        if(status == NQ_ENQUE_FULL){
            va_print("Queue is full, breaking loop\n\r");
            break;
        }
    }
    
    //dequeue
    do{
        status = nq_Deq(TestQueue, &data);
        
        if(status == NQ_SUCCESS){
            va_print("Dequeue successful: %d\n\r", data);
        }else{
            va_print("Queue is empty\n\r", data);
        }
        
        nq_TestPrint(TestQueue, va_print);
    }while(status == NQ_SUCCESS);
    
    //enqueue
    va_print("Enqueue value of 44 to 70\n\r");
    for(i = 44; i <= 70; i ++){
        status = nq_Enq(TestQueue, &i);
        nq_TestPrint(TestQueue, va_print);
        if(status == NQ_ENQUE_FULL){
            va_print("Queue is full, breaking loop\n\r");
            break;
        }
    }
    
    //empty
    va_print("Emptying queue\n\r");
    nq_Clear(TestQueue);
    nq_TestPrint(TestQueue, va_print);
    
    //enqueue
    va_print("Enqueue value of 80 to 99\n\r");
    for(i = 80; i <= 99; i ++){
        status = nq_Enq(TestQueue, &i);
        nq_TestPrint(TestQueue, va_print);
        if(status == NQ_ENQUE_FULL){
            va_print("Queue is full, breaking loop\n\r");
            break;
        }
    }
    
    //dequeue
    do{
        status = nq_Deq(TestQueue, &data);
        
        if(status == NQ_SUCCESS){
            va_print("Dequeue successful: %d\n\r", data);
        }else{
            va_print("Queue is empty\n\r", data);
        }
        
        nq_TestPrint(TestQueue, va_print);
    }while(status == NQ_SUCCESS);
}

/* [] END OF FILE */
