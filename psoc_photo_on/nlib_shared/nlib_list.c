//generic linked list implementation
#include <stdlib.h>
#include <stdio.h>
#include "nlib_err.h"
#include "nlib_list.h"

//internal method, add to a tail pointer
//check full before calling this
void list_AddNodeToTail(NodeObj * head, NodeObj * tail, NodeObj * srcNode){

  //test first add
  if(*head == NULL && *tail == NULL){
    *head = *srcNode;
    //(*head)->prev = NULL;
  }else{
    (*tail)->next = *srcNode;  //link old tail's next to srcNode
    //(*srcNode)->prev = *tail; //link prev of srcNode to old tail
  }
  *tail = *srcNode;  //tail is now pointing to srcNode
  (*tail)->next = NULL;  //make sure the tail points to NULL for next
}

//internal method, remove from a head pointer
//check empty before calling this
void list_RemoveNodeFromHead(NodeObj * head, NodeObj * tail, NodeObj * destNode){

  *destNode = *head;  //destNode points to same as head

  //test last remove
  if(*head == *tail){
    *head = NULL;
    *tail = NULL;
  }else{
    *head = (*destNode)->next; //move the head to the next node
    //(*head)->prev = NULL;
  }

  //clean up destination Node
  (*destNode)->next = NULL;
  //(*destNode)->prev = NULL;
}

//Node constructor
NodeObj node_New(){
  NodeObj self = (NodeObj) calloc(1, sizeof(struct nodeObj)); //guaranteed to be zero
  return self;
}

//constructor
ListObj list_New(){
  ListObj self = (ListObj) calloc(1, sizeof(struct listObj));
  return self;
}

//initializer
//structSize is the size of the data structure pointed to by Node->data
int list_Init(ListObj self, void (*enInt_var)(void), void (*disInt_var)(void), unsigned int maxListSize, unsigned int structSize){

  if (self == NULL)
    return NLIB_ERR_NOMEM;

  //connect interrupt enable/disable
  self->enInt = enInt_var;
  self->disInt = disInt_var;

  self->head = NULL;
  self->tail = NULL;

  self->freeHead = NULL;
  self->freeTail = NULL;

  self->listSize = 0;
  self->freeListSize = maxListSize;
  self->maxListSize = maxListSize;

  //try to reserve a bunch of nodes
  unsigned int i;
  for(i = 0; i < maxListSize; i ++){
    NodeObj n = node_New();
    if(n == NULL)
      return NLIB_ERR_NOMEM;    //check memory

    n->data = calloc(1, structSize);
    if(n->data == NULL)
      return NLIB_ERR_NOMEM;

    list_AddNodeToTail(&(self->freeHead), &(self->freeTail), &n);
  }

  return NLIB_ERR_NOERROR;
}

//removes a free node from free list, the node is now free floating
int list_GetFreeNode(ListObj self, NodeObj * destNode){

  //disable interrupts
  self->disInt();

  //if no free node is available
  if (self->freeHead == NULL && self->freeTail == NULL){
    *destNode = NULL;
    self->enInt();
    return LIST_TRANS_NOFREENODE;
  }

  //remove a free node, node is now free floating
  list_RemoveNodeFromHead(&(self->freeHead), &(self->freeTail), destNode);
  self->freeListSize--;
  self->enInt();
  return LIST_TRANS_SUCCESS;
}

//add to linked list, srcNode is free floating and gets placed into the list
int list_Enqueue(ListObj self, NodeObj * srcNode){

  //check data
  if (*srcNode == NULL)
    return LIST_TRANS_NULLERROR;

  //disable interrupts
  self->disInt();

  //check maxListSize
  if (self->listSize >= self->maxListSize){
    self->enInt();                //enable interrupts
    return LIST_TRANS_LISTFULL;
  }

  //add this node to list
  list_AddNodeToTail(&(self->head), &(self->tail), srcNode);
  self->listSize++;             //grow list size
  self->enInt();                //enable interrupts
  return LIST_TRANS_SUCCESS;
}


//remove from head, send removed node to destNode, which becomes free floating
int list_Dequeue(ListObj self, NodeObj * destNode){

  //disable interrupts
  self->disInt();

  //check for empty list
  if (self->head == NULL && self->tail == NULL){
    *destNode = NULL;
    self->enInt();
    return LIST_TRANS_LISTEMPTY;
  }

  //get the head from list
  list_RemoveNodeFromHead(&(self->head), &(self->tail), destNode);
  self->listSize--;
  self->enInt();
  return LIST_TRANS_SUCCESS;
}

//indicate that the Node has been consumed, and should go back to free list
int list_ReleaseFreeNode(ListObj self, NodeObj * srcNode){

  //check data
  if (*srcNode == NULL)
    return LIST_TRANS_NULLERROR;

  //disable interrupts
  self->disInt();

  //check maxListSize
  if (self->freeListSize >= self->maxListSize){
    self->enInt();                //enable interrupts
    return LIST_TRANS_LISTFULL;
  }

  //free up the node back to free list
  list_AddNodeToTail(&(self->freeHead), &(self->freeTail), srcNode);
  self->freeListSize++;
  self->enInt();
  return LIST_TRANS_SUCCESS;
}

//get list size
int list_GetListSize(ListObj self){
  return self->listSize;
}

//get free list size
int list_GetFreeSize(ListObj self){
  return self->freeListSize;
}

/*

//transfer lists
#define RXQ_SIZE 10
#define RXQ_STRING_SIZE 37
ListObj rxq;
CstringObj rxqData[RXQ_SIZE];

//init in init function\
{
  //init lists
  int i;
  for(i = 0; i < RXQ_SIZE; i ++){
    rxqData[i] = cstring_New(RXQ_STRING_SIZE);
  }

  rxq = list_New();
  status = list_Init(rxq, enInt, disInt, RXQ_SIZE, (void **)rxqData);
  assert_IsZeroPrint(assertDev, status, "rxq Add");
}

//test list
#define TESTSIZE 12
void ucmdTestList(const char * str){

  char test[TESTSIZE][8] = {
    "test  1",
    "test  2",
    "test  3",
    "test  4",
    "test  5",
    "test  6",
    "test  7",
    "test  8",
    "test  9",
    "test 10",
    "test 11",
    "test 12",
  };

  int status;
  int i;

  xil_printf("start test\n\r");
  xil_printf("listsize %d | freesize %d \n\r", list_GetListSize(rxq), list_GetFreeSize(rxq));

  //enqueue
  i = 0;
  NodeObj n;
  CstringObj s;
  do{
    xil_printf("enq %d\n\r", i);

    status = list_GetFreeNode(rxq, &n); //get a free node
    xil_printf("enq %d | free node %d |\n\r", i, status);
    if (status != LIST_TRANS_SUCCESS)
      break;
    xil_printf("listsize %d | freesize %d \n\r", list_GetListSize(rxq), list_GetFreeSize(rxq));

    //manipulate data
    s = (CstringObj)(n->data);
    cstring_CopyFromArray(test[i], s, 8);
    xil_printf("enq %d | %s |\n\r", i, test[i]);

    //queue the free node
    status = list_Enqueue(rxq, &n);
    xil_printf("enq %d | enqueue %d |\n\r", i, status);
    if (status != LIST_TRANS_SUCCESS)
      break;

    xil_printf("\n\r");
    i++;
  }while(1);

  //read what's in the queue
  n = rxq->head;
  for(i = 0; i < rxq->listSize; i ++){
    s = (CstringObj)(n->data);
    xil_printf("reading list %d: %s\n\r", i, (s)->ptr);
    n = n->next;
  }

  xil_printf("listsize %d | freesize %d \n\r", list_GetListSize(rxq), list_GetFreeSize(rxq));

  //dequeue
  i = 0;
  do{
    xil_printf("deq %d\n\r", i);

    status = list_Dequeue(rxq, &n);
    xil_printf("deq %d | dequeue %d\n\r", i, status);
    if (status != LIST_TRANS_SUCCESS)
      break;
    xil_printf("listsize %d | freesize %d \n\r", list_GetListSize(rxq), list_GetFreeSize(rxq));

    //safely read the data
    s = (CstringObj)(n->data);
    xil_printf("deq %d | %s\n\r", i, (s)->ptr);

    //release free node
    status = list_ReleaseFreeNode(rxq, &n);
    xil_printf("deq %d | release node %d\n\r", i, status);
    if (status != LIST_TRANS_SUCCESS)
      break;

    xil_printf("\n\r");
    i ++;
  }while(1);

  xil_printf("listsize %d | freesize %d \n\r", list_GetListSize(rxq), list_GetFreeSize(rxq));
}
*/
