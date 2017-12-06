#include "rbset.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>


#define log_err(M)	{perror("error: struct RBSet: " M); goto error;}
#define log_msg(M)	{fprintf(stderr, "error: struct RBSet: " M "\n"); goto error;}


static struct rbnode *node_new(void *);
static void node_destroy(struct rbnode *);
static struct rbnode *grandparent(const struct rbnode *);
static struct rbnode *get_uncle(const struct rbnode *);
static struct rbnode *get_sibling(const struct rbnode *);
static void rotate_left(struct rbnode *);
static void rotate_right(struct rbnode *);


static int insert(struct RBSet *, void *); 
static void insert_cases(struct RBSet *, struct rbnode *);
static struct rbnode *search(struct RBSet *, const void *);

static int remove_node(struct RBSet*, struct rbnode*);
static struct rbnode *get_pred(struct RBSet*, struct rbnode*);
static int remove_child(struct RBSet*, struct rbnode*);
static void replace_with_child(struct RBSet *, struct rbnode *, struct rbnode *);
static void remove_cases(struct RBSet *, struct rbnode *);

static void inorder(struct RBSet *, SetIterFunc, void *);



static struct rbnode *node_new(void *key)
{
	struct rbnode *node;
	
	if (!(node = malloc(sizeof(*node))))
		log_err("node_new");

	node->parent = NULL;
	node->left = NULL;
	node->right = NULL;
	node->key = key;
	node->color = Red;

	return node;
error:
	return NULL;
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
	struct rbnode *parent;

	if (!node)
		log_msg("sibling: node is null!");
	if (!(parent = node->parent))
		log_msg("sibling: parent is null!");

	return (node == parent->left)?parent->right:parent->left;
error:
	return NULL;
}

static void rotate_left(struct rbnode *node)
{
	struct rbnode *parent;
	struct rbnode *right;
	
	if (!node)
		return;
	if (!(right = node->right))
		return;
	
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

static void rotate_right(struct rbnode *node)
{
	struct rbnode *parent;
	struct rbnode *left;

	if (!node)
		return;
	if (!(left = node->left))
		return;

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

static void node_destroy(struct rbnode *node)
{
	free(node);
}


    
struct RBSet* rbset_new(CompareFunc cmp, DestroyFunc key_dst)
{
	struct RBSet *tree;
	
	if (!cmp)
		log_msg("rbset_new: null compare_func!");

	tree = malloc(sizeof(*tree));
	if (!tree)
		log_err("rbset_new");

	tree->root = NULL;
	tree->cmp_func = cmp;
	tree->key_dst_func = key_dst;
	return tree;
error:
	return NULL;
}

int rbset_insert(struct RBSet *tree, void *key)
{
	if (!tree)
		log_msg("rbset_insert: null tree");

	return insert(tree, key);
error:
	return -1;
}

static int insert(struct RBSet *tree, void *key)
{
	int res;
	struct rbnode *curr;
	struct rbnode *new;
	CompareFunc cmp_func;

	if (!tree)
		log_msg("insert: null tree!");

	if (!(curr = tree->root)) {
		if ((new = node_new(key))) {
			tree->root = new;
			goto out;
		} else {
			goto error;
		}
	}

	if (!(cmp_func = tree->cmp_func))
		log_msg("insert: cmp_func is null!");

	do {
		res = cmp_func(key, curr->key);
		if (res < 0) {
			if (curr->left) {
				curr = curr->left;
			} else if ((new = node_new(key))) {
				curr->left = new;
				new->parent = curr;
				goto out;
			} else {
				goto error;
			}
		} else if (res > 0) {
			if (curr->right) {
				curr = curr->right;
			} else if ((new = node_new(key))) {
				curr->right = new;
				new->parent = curr;
				goto out;
			} else {
				goto error;
			}
		} else {
			if (tree->key_dst_func)
				tree->key_dst_func(key);
			return 0;
		}
	} while (1);

out:
	insert_cases(tree, new);
	return 0;
error:
	return -1;
}

static void insert_cases(struct RBSet *tree, struct rbnode *node)
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

int rbset_search(struct RBSet *tree, const void *key)
{
	if (!tree)
		log_msg("rbset_search: tree is null!");

	return search(tree, key)?0:-1;
error:
	return -1;
}

static struct rbnode *search(struct RBSet *tree, const void *key)
{
	int res;
	struct rbnode *curr;
	CompareFunc cmp_func;

	if (!tree)
		log_msg("search: tree is null!");
	if (!(curr = tree->root))
		return NULL;
	if (!(cmp_func = tree->cmp_func))
		log_msg("search: cmp_func is null!");

	while ((res = cmp_func(key, curr->key))) {
		if (res < 0) {
			if (curr->left)
				curr = curr->left;
			else
				return NULL;
		} else {
			if (curr->right)
				curr = curr->right;
			else
				return NULL;
		}
	}

	return curr;
error:
	return NULL;
}

int rbset_remove(struct RBSet *tree, const void *key)
{
	struct rbnode *found;

	if (!tree)
		log_msg("rbset_remove: tree is null!");
	if ((found = search(tree, key)))
		return remove_node(tree, found);

	return 0;
error:
	return -1;
}

static int remove_node(struct RBSet *tree, struct rbnode *node)
{
	struct rbnode *pred;
	void *temp_key;

	if (!tree)
		log_msg("remove_node: tree is null!");
	if (!node)
		log_msg("remove_node: node is null!");

	pred = node;
	if (node->left && node->right) {
		pred = get_pred(tree, node);

		temp_key = node->key;
		node->key = pred->key;
		pred->key = temp_key;
	}

	return remove_child(tree, pred);
error:
	return -1;
}

static struct rbnode *get_pred(struct RBSet *tree, struct rbnode *node)
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

static int remove_child(struct RBSet *tree, struct rbnode *node)
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

static void replace_with_child(struct RBSet *tree, struct rbnode *node, struct rbnode *child)
{
	struct rbnode *parent;

	if (!tree)
		log_msg("replace_child: tree is null!");
	if (!node)
		log_msg("replace_child: node is null!");

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
	node_destroy(node);
error:
	return;
}

static void remove_cases(struct RBSet *tree, struct rbnode *node)
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

void rbset_foreach(struct RBSet *tree, SetIterFunc iter_func, void *data)
{
	inorder(tree, iter_func, data);
}

static void inorder(struct RBSet *tree, SetIterFunc iter_func, void *data)
{
	struct rbnode *curr;
	struct rbnode *prev;
	struct rbnode *next;

	if (!tree)
		log_msg("inorder: null tree!");
		
	prev = NULL;
	curr = tree->root;
	
	while (curr) {
		if (prev == curr->parent) {
			if (!(next = curr->left)) {
				if (iter_func(curr->key, data))
					break;
				next = curr->right?curr->right:curr->parent;
			}
		} else if (prev == curr->left) {
			if (iter_func(curr->key, data))
				break;
			next = curr->right?curr->right:curr->parent;
		} else if (prev == curr->right) {
			next = curr->parent;
		} else {
			log_msg("inorder: curr is not l, r or p!");
		}
		prev = curr;
		curr = next;
	}

error:
	return;
}

void rbset_clear(struct RBSet *tree)
{
	struct rbnode *curr;
	struct rbnode *parent;
	DestroyFunc key_dst_func;

	if (!tree)
		log_msg("rbset_clear: null tree!");

	if (!(curr = tree->root))
		return;

	key_dst_func = tree->key_dst_func;

	do {
		if (curr->left) {
			curr = curr->left;
		} else if (curr->right) {
			curr = curr->right;
		} else {
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
			node_destroy(curr);
			curr = parent;
		}
	} while (curr);

error:
	return;
}

void rbset_destroy(struct RBSet *tree)
{
	if (!tree)
		return;

	rbset_clear(tree);
	free(tree);
}
