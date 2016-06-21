#ifndef RBSET_H
#define RBSET_H

#include "types.h"

typedef struct RBSet_ RBSet;
typedef int SetIterFunc(void *key, void *data);

RBSet *rbset_new(CompareFunc, DestroyFunc key_dst);
int rbset_insert(RBSet *, void *key);
int rbset_search(RBSet *, const void *key);
void rbset_foreach(RBSet *, SetIterFunc, void *data);
int rbset_remove(RBSet *, const void *key);
void rbset_clear(RBSet *);
void rbset_destroy(RBSet *);

#endif
