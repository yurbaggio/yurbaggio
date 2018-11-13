#ifndef NLIB_LIST_H
#define NLIB_LIST_H

#include "nlib_node.h"

//define a list
typedef struct listObj * ListObj;
struct listObj{
  NodeObj head;
  NodeObj tail;

  NodeObj freeHead;
  NodeObj freeTail;

  void (*enInt)(void);
  void (*disInt)(void);

  unsigned int listSize;
  unsigned int freeListSize;
  unsigned int maxListSize;

};

#define LIST_TRANS_SUCCESS       0  //succesful transaction
#define LIST_TRANS_INDEXERROR   -1  //index out of bounds
#define LIST_TRANS_NULLERROR    -2  //node pointer is NULL
#define LIST_TRANS_LISTEMPTY    -3  //list is empty
#define LIST_TRANS_LISTFULL     -4  //list is at maxListSize
#define LIST_TRANS_NOFREENODE   -5  //no free node is available
#define LIST_TRANS_FREEOVERFLOW -6  //too many free nodes, something is wrong

//constructor
ListObj list_New();

//initializer
int list_Init(ListObj self, void (*enInt_var)(void), void (*disInt_var)(void), unsigned int maxListSize, unsigned int structSize);

//removes a free node from free list, the node is now free floating
int list_GetFreeNode(ListObj self, NodeObj * destNode);

//add to linked list, srcNode is free floating and gets placed into the list
int list_Enqueue(ListObj self, NodeObj * srcNode);

//remove from head, send removed node to destNode, which becomes free floating
int list_Dequeue(ListObj self, NodeObj * destNode);

//indicate that the Node has been consumed, and should go back to free list
int list_ReleaseFreeNode(ListObj self, NodeObj * srcNode);

//get list size
int list_GetListSize(ListObj self);

//get free list size
int list_GetFreeSize(ListObj self);

#endif
