#ifndef _RBTREE_H_
#define _RBTREE_H_

#define BLACK	0
#define RED	1

typedef struct _RBTree RBTree;

typedef int (*CompareFunc) (const void *, const void *);
typedef void (*DestroyFunc) (void *);
typedef void (*ShowFunc) (void *);


RBTree *rbtree_new(CompareFunc, DestroyFunc, ShowFunc);
void rbtree_insert(RBTree*, const void *);
void rbtree_remove(RBTree*, const void *);
void rbtree_destroy(RBTree*);

#endif
