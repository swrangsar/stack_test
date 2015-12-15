#ifndef _RBMAP_H_
#define _RBMAP_H_

#include "types.h"

typedef struct _RBMap RBMap;
typedef int TraverseFunc(void *, void *, void *);

RBMap *rbmap_new(CompareFunc, DestroyFunc, DestroyFunc);
int rbmap_insert(RBMap *, void *, void *);
void *rbmap_search(RBMap *, const void *);
void rbmap_foreach(RBMap *, TraverseFunc, void *);
int rbmap_remove(RBMap *, const void *);
void rbmap_destroy(RBMap *);

#endif
