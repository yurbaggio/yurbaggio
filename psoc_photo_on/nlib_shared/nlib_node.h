#ifndef NLIB_NODE_H
#define NLIB_NODE_H

//define a node
typedef struct nodeObj * NodeObj;
struct nodeObj{
  void *data;
  NodeObj next;
  //NodeObj prev;
};

#endif