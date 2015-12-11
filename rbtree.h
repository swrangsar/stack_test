#ifndef _RBTREE_H_
#define _RBTREE_H_

#include "types.h"

typedef struct _RBTree RBTree;

RBTree *rbtree_new(CompareFunc, DestroyFunc);
int rbtree_insert(RBTree*, void *);
int rbtree_remove(RBTree*, void *);
void rbtree_destroy(RBTree*);

#endif
