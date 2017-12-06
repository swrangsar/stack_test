#include "rbmap.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "logmsg.h"


static struct rbnode *node_new(void *, void *);
static void node_destroy(struct rbnode*);
static struct rbnode *grandparent(const struct rbnode *);
static struct rbnode *get_uncle(const struct rbnode *);
static struct rbnode *get_sibling(const struct rbnode*);
static void rotate_left(struct rbnode*);
static void rotate_right(struct rbnode*);


static int insert(struct RBMap *, void*, void*, int); 
static void insert_cases(struct RBMap*, struct rbnode*);
static struct rbnode *search(struct RBMap *, const void *);

static int remove_node(struct RBMap*, struct rbnode*);
static struct rbnode *get_pred(struct RBMap*, struct rbnode*);
static int remove_child(struct RBMap*, struct rbnode*);
static void replace_with_child(struct RBMap*, struct rbnode*, struct rbnode*);
static void remove_cases(struct RBMap*, struct rbnode*);

static void inorder(struct RBMap *, TraverseFunc, void *);



static struct rbnode *node_new(void *key, void *value)
{
	struct rbnode *node = NULL;

	if ((node = malloc(sizeof(*node)))) {
		node->parent = NULL;
		node->left = NULL;
		node->right = NULL;
		node->key = key;
		node->value = value;
		node->color = Red;
	} else {
		log_err("node_new");
	}

	return node;
}

static struct rbnode *grandparent(const struct rbnode *node)
{
	if (node && node->parent)
		return node->parent->parent;
	else
		return NULL;
}

static struct rbnode *get_uncle(const struct rbnode *node)
{
	struct rbnode *parent;
	struct rbnode *grandpa;

	if (node && (parent = node->parent) && (grandpa = parent->parent))
		return (parent == grandpa->left)?grandpa->right:grandpa->left;
	else
		return NULL;
}

static struct rbnode *get_sibling(const struct rbnode *node)
{
	struct rbnode *sibling = NULL;
	struct rbnode *parent;

	if (node) {
		if ((parent = node->parent)) {
			sibling = (node == parent->left) ? parent->right : parent->left;
		} else {
			log_err("sibling: parent is null!\n");
		}
	} else {
		log_err("sibling: node is null!\n");
	}

	return sibling;
}

static void rotate_left(struct rbnode *node)
{
	struct rbnode *parent;
	struct rbnode *right;

	if (node) {
		if ((right = node->right)) {
			if ((parent = node->parent)) {
				if (node == parent->left)
					parent->left = right;
				else
					parent->right = right;
			}

			node->right = right->left;
			node->parent = right;
			right->left = node;
			right->parent = parent;

			if (node->right)
				node->right->parent = node;
		}
	}
}

static void rotate_right(struct rbnode *node)
{
	struct rbnode *parent;
	struct rbnode *left;

	if (node) {
		if ((left = node->left)) {
			if ((parent = node->parent)) {
				if (node == parent->left)
					parent->left = left;
				else
					parent->right = left;
			}

			node->left = left->right;
			node->parent = left;
			left->right = node;
			left->parent = parent;

			if (node->left)
				node->left->parent = node;
		}
	}
}

static void node_destroy(struct rbnode *node)
{
	free(node);
}

int rbmap_init(struct RBMap *tree, CompareFunc cmp, DestroyFunc key_dst, DestroyFunc val_dst)
{
	int ret_val = -1;

	if (tree) {
		if (cmp) {
			tree->root = NULL;
			tree->cmp_func = cmp;
			tree->key_dst_func = key_dst;
			tree->val_dst_func = val_dst;
			ret_val = 0;
		} else {
			log_err("null compare func\n");
		}
	} else {
		log_err("null tree pointer\n");
	}

	return ret_val;
}

int rbmap_insert(struct RBMap *tree, void *key, void *value)
{
	int ret_val = -1;

	if (tree) {
		ret_val = insert(tree, key, value, 0);
	} else {
		log_err("rbmap_insert: null tree\n");
	}

	return ret_val;
}

int rbmap_replace(struct RBMap *tree, void *key, void *value)
{
	int ret_val = -1;

	if (tree) {
		ret_val = insert(tree, key, value, 1);
	} else {
		log_err("rbmap_insert: null tree\n");
	}

	return ret_val;
}

static int insert(struct RBMap *tree, void *key, void *value, int replace)
{
	int ret_val = -1;
	int res;
	struct rbnode *curr;
	struct rbnode *new;
	CompareFunc cmp_func;

	if (tree) {
		if ((curr = tree->root)) {
			if ((cmp_func = tree->cmp_func)) {
				do {
					res = cmp_func(key, curr->key);
					if (res < 0) {
						if (curr->left) {
							curr = curr->left;
						} else if ((new = node_new(key, value))) {
							curr->left = new;
							new->parent = curr;
							insert_cases(tree, new);
							ret_val = 0;
							break;
						}
					} else if (res > 0) {
						if (curr->right) {
							curr = curr->right;
						} else if ((new = node_new(key, value))) {
							curr->right = new;
							new->parent = curr;
							insert_cases(tree, new);
							ret_val = 0;
							break;
						}
					} else {
						if (tree->val_dst_func)
							tree->val_dst_func(curr->value);
						curr->value = value;

						if (replace) {
							if (tree->key_dst_func)
								tree->key_dst_func(curr->key);
							curr->key = key;
						} else {
							if (tree->key_dst_func)
								tree->key_dst_func(key);
						}

						ret_val = 0;
						break;
					}
				} while (1);
			} else {
				log_err("insert: cmp_func is null\n");
			}
		} else {
			if ((new = node_new(key, value))) {
				tree->root = new;
				insert_cases(tree, new);
				ret_val = 0;
			}
		}
	} else {
		log_err("insert: null tree\n");
	}

	return ret_val;
}

static void insert_cases(struct RBMap *tree, struct rbnode *node)
{
	struct rbnode *parent;
	struct rbnode *granpa;
	struct rbnode *uncle;

	if (!tree)
		log_msg("insert_cases: tree is null!");


	do {
		if (!node)
			log_msg("insert_case1: null node!");
		if (!(parent = node->parent)) {
			node->color = Black;
			return;
		}

		/* insert case 2	*/
		if (Black == parent->color)
			return;

		/* insert case 3	*/
		if (!(granpa = grandparent(node)))
			log_msg("insert_case3: null granpa!");
		if ((uncle = get_uncle(node)) && Red == uncle->color) {
			parent->color = Black;
			uncle->color = Black;
			granpa->color = Red;
			node = granpa;
		} else {
			break;
		}
	} while (1);


	/* insert case 4	*/
	if (node == parent->right && parent == granpa->left) {
		rotate_left(parent);
		node = parent;
	} else if (node == parent->left && parent == granpa->right) {
		rotate_right(parent);
		node = parent;
	}


	/* insert case 5	*/
	if (!(parent = node->parent))
		log_msg("insert_case5: parent is null!");
	if (!(granpa = grandparent(node)))
		log_msg("insert_case5: granpa is null!");

	parent->color = Black;
	granpa->color = Red;
	(node == parent->left)?rotate_right(granpa):rotate_left(granpa);

	if (!(parent->parent))
		tree->root = parent;

error:
	return;
}

void *rbmap_search(struct RBMap *tree, const void *key)
{
	void *value = NULL;

	if (tree) {
		struct rbnode *found;
		if ((found = search(tree, key)))
			value = found->value;
	} else {
		log_err("rbmap_search: tree is null\n");
	}

	return value;
}

static struct rbnode *search(struct RBMap *tree, const void *key)
{
	struct rbnode *found = NULL;
	int res;
	struct rbnode *curr = NULL;
	CompareFunc cmp_func;

	if (tree) {
		if ((curr = tree->root)) {
			if ((cmp_func = tree->cmp_func)) {
				while ((res = cmp_func(key, curr->key))) {
					if (res < 0) {
						if (curr->left) {
							curr = curr->left;
						} else {
							curr = NULL;
							break;
						}
					} else {
						if (curr->right) {
							curr = curr->right;
						} else {
							curr = NULL;
							break;
						}
					}
				}
			} else {
				curr = NULL;
				log_err("search: cmp_func is null\n");
			}
		}
	} else {
		log_err("search: tree is null\n");
	}

	return curr;
}


int rbmap_remove(struct RBMap *tree, const void *key)
{
	struct rbnode *found;

	if (!tree)
		log_msg("rbmap_remove: tree is null!");

	if ((found = search(tree, key)))
		return remove_node(tree, found);

	return 0;
error:
	return -1;
}

static int remove_node(struct RBMap *tree, struct rbnode *node)
{
	struct rbnode *pred;
	void *temp_key;
	void *temp_val;

	if (!tree)
		log_msg("remove_node: tree is null!");
	if (!node)
		log_msg("remove_node: node is null!");

	pred = node;
	if (node->left && node->right) {
		pred = get_pred(tree, node);

		temp_key = node->key;
		temp_val = node->value;
		node->key = pred->key;
		node->value = pred->value;
		pred->key = temp_key;
		pred->value = temp_val;
	}

	return remove_child(tree, pred);
error:
	return -1;
}

static struct rbnode *get_pred(struct RBMap *tree, struct rbnode *node)
{
	struct rbnode *pred;

	if (!tree)
		log_msg("get_pred: tree is null!");
	if (!node)
		log_msg("get_pred: node is null!");
	if (!(pred = node->left))
		log_msg("get_pred: predecessor is NULL!");

	while (pred->right)
		pred = pred->right;

	return pred;
error:
	return NULL;
}

static int remove_child(struct RBMap *tree, struct rbnode *node)
{
	struct rbnode *child;

	if (!tree)
		log_msg("remove_child: tree is null!");
	if (!node)
		log_msg("remove_child: node is null!");

	child = node->left?node->left:node->right;

	if (Black == node->color) {
		if (child && Red == child->color)
			child->color = Black;
		else
			remove_cases(tree, node);
	}

	replace_with_child(tree, node, child);
	node = NULL;
	return 0;
error:
	return -1;
}

static void replace_with_child(struct RBMap *tree, struct rbnode *node, struct rbnode *child)
{
	struct rbnode *parent;

	if (tree) {
		if (node) {
			if ((parent = node->parent)) {
				if (node == parent->left)
					parent->left = child;
				else
					parent->right = child;
			} else {
				tree->root = child;
			}


			if (child) {
				child->parent = parent;
				child->color = node->color;
			}

			if (tree->key_dst_func)
				tree->key_dst_func(node->key);
			node->key = NULL;
			if (tree->val_dst_func)
				tree->val_dst_func(node->value);
			node->value= NULL;
			node_destroy(node);
		} else {
			log_err("replace_child: node is null\n");
		}
	} else {
		log_err("replace_child: tree is null\n");
	}
}

static void remove_cases(struct RBMap *tree, struct rbnode *node)
{
	struct rbnode *parent;
	struct rbnode *sibling;
	struct rbnode *granpa;

	if (!tree)
		log_msg("remove_cases: tree is null!");

	do {
		if (!node)
			log_msg("remove_case1: node is null!");
		if (!(parent = node->parent))
			return;

		/* remove case 2	*/
		if (!(sibling = get_sibling(node)))
			log_msg("remove_case2: sibling is null!");

		if (Red == sibling->color) {
			parent->color = Red;
			sibling->color = Black;
			granpa = parent->parent;

			if (node == parent->left)
				rotate_left(parent);
			else
				rotate_right(parent);

			if (!granpa)
				tree->root = sibling;

			break;

			/* remove case 3	*/
		} else if (Black == parent->color && Black == sibling->color
				&& (!sibling->left || Black == sibling->left->color)
				&& (!sibling->right || Black == sibling->right->color)) {
			sibling->color = Red;
			node = parent;
		} else {
			break;
		}
	} while (1);


	/* remove case 4	*/
	if (Red == parent->color && Black == sibling->color
			&& (!sibling->left || Black == sibling->left->color)
			&& (!sibling->right || Black == sibling->right->color)) {
		parent->color = Black;
		sibling->color = Red;
		return;
	}

	/* remove case 5	*/
	if (Black != sibling->color)
		log_msg("remove_case5: sibling is red!");

	if (node == parent->left
			&& (!sibling->right || Black == sibling->right->color)) {
		sibling->color = Red;
		sibling->left->color = Black;
		rotate_right(sibling);
	} else if (node == parent->right
			&& (!sibling->left || Black == sibling->left->color)) {
		sibling->color = Red;
		sibling->right->color = Black;
		rotate_left(sibling);
	}

	/* remove case 6	*/
	if (!(sibling = get_sibling(node)))
		log_msg("remove_case6: sibling is null!");

	sibling->color = parent->color;
	parent->color = Black;
	if (node == parent->left) {
		rotate_left(parent);
		if (!sibling->right)
			log_msg("remove_case6: sibling's child null!");
		if (Red != sibling->right->color)
			log_msg("remove_case6: sibling's child not Red!");

		sibling->right->color = Black;
	} else {
		rotate_right(parent);
		if (!sibling->left)
			log_msg("remove_case6: sibling's child null!");
		if (Red != sibling->left->color)
			log_msg("remove_case6: sibling's child not Red!");

		sibling->left->color = Black;
	}

	if (!sibling->parent)
		tree->root = sibling;

error:
	return;
}

void rbmap_foreach(struct RBMap *tree, TraverseFunc trav_func, void *data)
{
	inorder(tree, trav_func, data);
}

static void inorder(struct RBMap *tree, TraverseFunc trav_func, void *data)
{
	struct rbnode *next;

	if (tree) {
		struct rbnode *curr = tree->root;
		struct rbnode *prev = NULL;

		while (curr) {
			if (prev == curr->parent) {
				if (!(next = curr->left)) {
					if (trav_func(curr->key, curr->value, data))
						break;
					next = curr->right?curr->right:curr->parent;
				}
			} else if (prev == curr->left) {
				if (trav_func(curr->key, curr->value, data))
					break;
				next = curr->right?curr->right:curr->parent;
			} else if (prev == curr->right) {
				next = curr->parent;
			} else {
				log_err("inorder: curr is not l, r or p\n");
			}
			prev = curr;
			curr = next;
		}

	} else {
		log_err("inorder: null tree\n");
	}
}

void rbmap_clear(struct RBMap *tree)
{

	if (tree) {
		struct rbnode *curr;

		if ((curr = tree->root)) {
			DestroyFunc key_dst_func;
			DestroyFunc val_dst_func;
			key_dst_func = tree->key_dst_func;
			val_dst_func = tree->val_dst_func;

			do {
				if (curr->left) {
					curr = curr->left;
				} else if (curr->right) {
					curr = curr->right;
				} else {
					struct rbnode *parent;

					if ((parent = curr->parent)) {
						if (curr == parent->left)
							parent->left = NULL;
						else
							parent->right = NULL;
					} else {
						tree->root = NULL;
					}

					if (key_dst_func)
						key_dst_func(curr->key);
					if (val_dst_func)
						val_dst_func(curr->value);
					node_destroy(curr);
					curr = parent;
				}
			} while (curr);
		} else {
			log_err("rbmap_clear: null tree!");
		}
	}
}

void rbmap_destroy(struct RBMap *tree)
{
	if (tree) {
		rbmap_clear(tree);
		free(tree);
	}
}
