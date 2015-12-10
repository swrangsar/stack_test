#ifndef _RBTREE_H_
#define _RBTREE_H_

typedef struct _RBTree RBTree;

typedef int (*CompareFunc) (const void *, const void *);
typedef void (*DestroyFunc) (void *);


RBTree *rbtree_new(CompareFunc, DestroyFunc);
void rbtree_insert(RBTree*, const void *);
void rbtree_remove(RBTree*, const void *);
void rbtree_destroy(RBTree*);

#endif
