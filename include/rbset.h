#ifndef RBSET_H_
#define RBSET_H_

#include "types.h"
#include "rbmap.h"


struct RBSet {
	struct rbnode *root;
	CompareFunc cmp_func;
	DestroyFunc key_dst_func;
};

typedef int SetIterFunc(void *key, void *data);


struct RBSet *rbset_new(CompareFunc, DestroyFunc key_dst);
int rbset_insert(struct RBSet *, void *key);
int rbset_search(struct RBSet *, const void *key);
void rbset_foreach(struct RBSet *, SetIterFunc, void *data);
int rbset_remove(struct RBSet *, const void *key);
void rbset_clear(struct RBSet *);
void rbset_destroy(struct RBSet *);

#endif
