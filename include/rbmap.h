#ifndef RBMAP_H_
#define RBMAP_H_

#include "types.h"


typedef enum {
	BLACK=0,
	RED
} Color;

struct RBMNode {
	struct RBMNode *parent;
	struct RBMNode *left;
	struct RBMNode *right;
	void *key;
	void *value;
	Color color;
};

struct RBMap {
	struct RBMNode *root;
	CompareFunc cmp_func;
	DestroyFunc key_dst_func;
	DestroyFunc val_dst_func;
};

typedef int TraverseFunc(void *key, void *val, void *data);

struct RBMap *rbmap_new(CompareFunc, DestroyFunc key_dst, DestroyFunc val_dst);
int rbmap_insert(struct RBMap *, void *key, void *val);
int rbmap_replace(struct RBMap *, void *key, void *val);
void *rbmap_search(struct RBMap *, const void *key);
void rbmap_foreach(struct RBMap *, TraverseFunc, void *data);
int rbmap_remove(struct RBMap *, const void *key);
void rbmap_clear(struct RBMap *);
void rbmap_destroy(struct RBMap *);

#endif
