#ifndef _RBSET_H_
#define _RBSET_H_

#include "types.h"

typedef struct _RBSet RBSet;
typedef int SetIterFunc(void *key, void *data);

RBSet *rbset_new(CompareFunc, DestroyFunc key_dst);
int rbset_insert(RBSet *, void *key);
int rbset_search(RBSet *, const void *key);
void rbset_foreach(RBSet *, SetIterFunc, void *data);
int rbset_remove(RBSet *, const void *key);
void rbset_clear(RBSet *);
void rbset_destroy(RBSet *);

#endif
