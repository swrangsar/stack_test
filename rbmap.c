#include "rbmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


#define log_err(M)	{perror("error: RBMap: " M); goto error;}
#define log_msg(M)	{fprintf(stderr, "error: RBMap: " M "\n"); goto error;}


typedef enum {
	BLACK=0,
	RED
} Color;

typedef struct _Node Node;


struct _Node {
	Node *parent;
	Node *left;
	Node *right;
	void *key;
	void *value;
	Color color;
};

struct _RBMap {
	Node *root;
	CompareFunc cmp_func;
	DestroyFunc key_dst_func;
	DestroyFunc val_dst_func;
};


static Node *node_new(void *, void *);
static void node_destroy(Node*);
static Node *grandparent(const Node *);
static Node *get_uncle(const Node *);
static Node *get_sibling(const Node*);
static void rotate_left(Node*);
static void rotate_right(Node*);


static int insert(RBMap *, void*, void*); 
static void insert_cases(RBMap*, Node*);
static Node *search(RBMap *, const void *);

static int remove_node(RBMap*, Node*);
static Node *get_pred(RBMap*, Node*);
static int remove_child(RBMap*, Node*);
static void replace_with_child(RBMap*, Node*, Node*);
static void remove_cases(RBMap*, Node*);

static void inorder(RBMap *, TraverseFunc, void *);



static Node *node_new(void *key, void *value)
{
	Node *node;
	
	if (!(node = malloc(sizeof(*node))))
		log_err("node_new");

	node->parent = NULL;
	node->left = NULL;
	node->right = NULL;
	node->key = key;
	node->value = value;
	node->color = RED;

	return node;
error:
	return NULL;
}

static Node *grandparent(const Node *node)
{
	if (node && node->parent)
		return node->parent->parent;
	else
		return NULL;
}

static Node *get_uncle(const Node *node)
{
	Node *parent;
	Node *grandpa;

	if (node && (parent = node->parent) && (grandpa = parent->parent))
		return (parent == grandpa->left)?grandpa->right:grandpa->left;
	else
		return NULL;
}

static Node *get_sibling(const Node *node)
{
	Node *parent;

	if (!node)
		log_msg("sibling: node is null!");
	if (!(parent = node->parent))
		log_msg("sibling: parent is null!");

	return (node == parent->left)?parent->right:parent->left;
error:
	return NULL;
}

static void rotate_left(Node *node)
{
	Node *parent;
	Node *right;
	
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

static void rotate_right(Node *node)
{
	Node *parent;
	Node *left;

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

static void node_destroy(Node *node)
{
	free(node);
}


    
RBMap* rbmap_new(CompareFunc cmp, DestroyFunc key_dst, DestroyFunc val_dst)
{
	RBMap *tree;
	
	if (!cmp)
		log_msg("rbmap_new: null compare_func!");

	tree = malloc(sizeof(*tree));
	if (!tree)
		log_err("rbmap_new");

	tree->root = NULL;
	tree->cmp_func = cmp;
	tree->key_dst_func = key_dst;
	tree->val_dst_func = val_dst;

	return tree;
error:
	return NULL;
}

int rbmap_insert(RBMap *tree, void *key, void *value)
{
	if (!tree)
		log_msg("rbmap_insert: null tree");

	return insert(tree, key, value);

error:
	return -1;
}

static int insert(RBMap *tree, void *key, void *value)
{
	int res;
	Node *curr;
	Node *new;
	CompareFunc cmp_func;

	if (!tree)
		log_msg("insert: null tree!");

	if (!(curr = tree->root)) {
		if ((new = node_new(key, value))) {
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
			} else if ((new = node_new(key, value))) {
				curr->left = new;
				new->parent = curr;
				goto out;
			} else {
				goto error;
			}
		} else if (res > 0) {
			if (curr->right) {
				curr = curr->right;
			} else if ((new = node_new(key, value))) {
				curr->right = new;
				new->parent = curr;
				goto out;
			} else {
				goto error;
			}
		} else {
			if (key != curr->key && tree->key_dst_func)
				tree->key_dst_func(key);
			if (value != curr->value && tree->val_dst_func)
				tree->val_dst_func(curr->value);
			curr->value = value;
			return 0;
		}
	} while (1);

out:
	insert_cases(tree, new);
	return 0;
error:
	return -1;
}

static void insert_cases(RBMap *tree, Node *node)
{
	Node *parent;
	Node *granpa;
	Node *uncle;

	if (!tree)
		log_msg("insert_cases: tree is null!");


	do {
		if (!node)
			log_msg("insert_case1: null node!");
		if (!(parent = node->parent)) {
			node->color = BLACK;
			return;
		}

		/* insert case 2	*/
		if (BLACK == parent->color)
			return;

		/* insert case 3	*/
		if (!(granpa = grandparent(node)))
			log_msg("insert_case3: null granpa!");
		if ((uncle = get_uncle(node)) && RED == uncle->color) {
			parent->color = BLACK;
			uncle->color = BLACK;
			granpa->color = RED;
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

	parent->color = BLACK;
	granpa->color = RED;
	(node == parent->left)?rotate_right(granpa):rotate_left(granpa);

	if (!(parent->parent))
		tree->root = parent;

error:
	return;
}

void *rbmap_search(RBMap *tree, const void *key)
{
	Node *found;

	if (!tree)
		log_msg("rbmap_search: tree is null!");

	if ((found = search(tree, key)))
		return found->value;

error:
	return NULL;
}

static Node *search(RBMap *tree, const void *key)
{
	int res;
	Node *curr;
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


int rbmap_remove(RBMap *tree, const void *key)
{
	Node *found;

	if (!tree)
		log_msg("rbmap_remove: tree is null!");

	if ((found = search(tree, key)))
		return remove_node(tree, found);

	return 0;
error:
	return -1;
}

static int remove_node(RBMap *tree, Node *node)
{
	Node *pred;
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

static Node *get_pred(RBMap *tree, Node *node)
{
	Node *pred;

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

static int remove_child(RBMap *tree, Node *node)
{
	Node *child;

	if (!tree)
		log_msg("remove_child: tree is null!");
	if (!node)
		log_msg("remove_child: node is null!");

	child = node->left?node->left:node->right;

	if (BLACK == node->color) {
		if (child && RED == child->color)
			child->color = BLACK;
		else
			remove_cases(tree, node);
	}

	replace_with_child(tree, node, child);
	node = NULL;
	return 0;
error:
	return -1;
}

static void replace_with_child(RBMap *tree, Node *node, Node *child)
{
	Node *parent;

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
	if (tree->val_dst_func)
		tree->val_dst_func(node->value);
	node->value= NULL;
	node_destroy(node);
error:
	return;
}

static void remove_cases(RBMap *tree, Node *node)
{
	Node *parent;
	Node *sibling;
	Node *granpa;

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

		if (RED == sibling->color) {
			parent->color = RED;
			sibling->color = BLACK;
			granpa = parent->parent;

			if (node == parent->left)
				rotate_left(parent);
			else
				rotate_right(parent);

			if (!granpa)
				tree->root = sibling;
	
			break;

		/* remove case 3	*/
		} else if (BLACK == parent->color && BLACK == sibling->color
				&& (!sibling->left || BLACK == sibling->left->color)
				&& (!sibling->right || BLACK == sibling->right->color)) {
			sibling->color = RED;
			node = parent;
		} else {
			break;
		}
	} while (1);


	/* remove case 4	*/
	if (RED == parent->color && BLACK == sibling->color
			&& (!sibling->left || BLACK == sibling->left->color)
			&& (!sibling->right || BLACK == sibling->right->color)) {
		parent->color = BLACK;
		sibling->color = RED;
		return;
	}

	/* remove case 5	*/
	if (BLACK != sibling->color)
		log_msg("remove_case5: sibling is red!");

	if (node == parent->left
			&& (!sibling->right || BLACK == sibling->right->color)) {
		sibling->color = RED;
		sibling->left->color = BLACK;
		rotate_right(sibling);
	} else if (node == parent->right
			&& (!sibling->left || BLACK == sibling->left->color)) {
		sibling->color = RED;
		sibling->right->color = BLACK;
		rotate_left(sibling);
	}

	/* remove case 6	*/
	if (!(sibling = get_sibling(node)))
		log_msg("remove_case6: sibling is null!");

	sibling->color = parent->color;
	parent->color = BLACK;
	if (node == parent->left) {
		rotate_left(parent);
		if (!sibling->right)
			log_msg("remove_case6: sibling's child null!");
		if (RED != sibling->right->color)
			log_msg("remove_case6: sibling's child not RED!");

		sibling->right->color = BLACK;
	} else {
		rotate_right(parent);
		if (!sibling->left)
			log_msg("remove_case6: sibling's child null!");
		if (RED != sibling->left->color)
			log_msg("remove_case6: sibling's child not RED!");

		sibling->left->color = BLACK;
	}

	if (!sibling->parent)
		tree->root = sibling;

error:
	return;
}

void rbmap_foreach(RBMap *tree, TraverseFunc trav_func, void *data)
{
	inorder(tree, trav_func, data);
}

static void inorder(RBMap *tree, TraverseFunc trav_func, void *data)
{
	Node *curr;
	Node *prev;
	Node *next;

	if (!tree)
		log_msg("inorder: null tree!");
		
	prev = NULL;
	curr = tree->root;
	
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
			log_msg("inorder: curr is not l, r or p!");
		}
		prev = curr;
		curr = next;
	}

error:
	return;
}

void rbmap_clear(RBMap *tree)
{
	Node *curr;
	Node *parent;
	DestroyFunc key_dst_func;
	DestroyFunc val_dst_func;

	if (!tree)
		log_msg("rbmap_clear: null tree!");

	if (!(curr = tree->root))
		return;

	key_dst_func = tree->key_dst_func;
	val_dst_func = tree->val_dst_func;

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
			if (val_dst_func)
				val_dst_func(curr->value);
			node_destroy(curr);
			curr = parent;
		}
	} while (curr);

error:
	return;
}

void rbmap_destroy(RBMap *tree)
{
	if (!tree)
		return;

	rbmap_clear(tree);
	free(tree);
}
