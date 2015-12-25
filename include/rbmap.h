#ifndef _RBMAP_H_
#define _RBMAP_H_

#include "types.h"

typedef struct _RBMap RBMap;
typedef int TraverseFunc(void *key, void *val, void *data);

RBMap *rbmap_new(CompareFunc, DestroyFunc key_dst, DestroyFunc val_dst);
int rbmap_insert(RBMap *, void *key, void *val);
void *rbmap_search(RBMap *, const void *key);
void rbmap_foreach(RBMap *, TraverseFunc, void *data);
int rbmap_remove(RBMap *, const void *key);
void rbmap_clear(RBMap *);
void rbmap_destroy(RBMap *);

#endif
