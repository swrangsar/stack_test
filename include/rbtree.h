#ifndef RBTREE_H_
#define RBTREE_H_

#include "types.h"


enum color {
	Black,
        Red
};

struct rbnode {
	struct rbnode *parent;
	struct rbnode *left;
	struct rbnode *right;
	void *key;
	void *value;
	enum color color;
};

struct rbtree {
	struct rbnode *root;
	CompareFunc cmp_func;
	DestroyFunc key_dst_func;
	DestroyFunc val_dst_func;
};

typedef int TraverseFunc(void *key, void *val, void *data);

int rbtree_init(struct rbtree *tree, CompareFunc cmp, DestroyFunc key_dst, DestroyFunc val_dst);
int rbtree_insert(struct rbtree *, void *key, void *val);
int rbtree_replace(struct rbtree *, void *key, void *val);
void *rbtree_search(struct rbtree *, const void *key);
void rbtree_foreach(struct rbtree *, TraverseFunc, void *data);
int rbtree_remove(struct rbtree *, const void *key);
void rbtree_clear(struct rbtree *);
void rbtree_destroy(struct rbtree *);

#endif  // RBTREE_H_
