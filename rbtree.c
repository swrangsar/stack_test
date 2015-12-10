#include "rbtree.h"

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>

#define log_err(M)	{perror("error: RBTree: " M); goto error;}
#define log_msg(M)	{fprintf(stderr, "error: RBTree: " M "\n"); goto error;}


typedef enum {
	BLACK=0,
	RED
} Color;

typedef struct _Node Node;


struct _Node {
	Node *parent;
	Node *left;
	Node *right;
	void *data;
	Color color;
};

struct _RBTree {
	Node *root;
	CompareFunc cmp_func;
	DestroyFunc dst_func;
};


static Node *node_new(void *);
static void node_destroy(Node*);

static int insert(RBTree *, void*); 

static void rotate_left(Node*);
static void rotate_right(Node*);
static Node *grandparent(const Node *);
static Node *get_uncle(const Node *);
static Node *get_sibling(const Node*);
static void insert_cases(RBTree*, Node*);

static void rbtree_clear(RBTree*, Node*);
static int tree_remove(RBTree *, void const*);
static int remove_node(RBTree*, Node*);
static Node *get_pred(RBTree*, Node*);
static int remove_child(RBTree*, Node*);
static void replace_child(RBTree*, Node*, Node*);
static void remove_cases(RBTree*, Node*);



RBTree* rbtree_new(CompareFunc cmp, DestroyFunc dst)
{
	RBTree *tree;
	
	tree = malloc(sizeof(*tree));
	if (!tree)
		log_err("rbtree_new");
	if (!cmp)
		log_err("rbtree_new: null compare_func!");

	tree->root = NULL;
	tree->cmp_func = cmp;
	tree->dst_func = dst;

	return tree;
error:
	return NULL;
}

static Node *node_new(void *data)
{
	Node *node;
	
	if (!(node = malloc(sizeof(*node))))
		log_err("rbtree: node_new");

	node->parent = NULL;
	node->left = NULL;
	node->right = NULL;
	node->data = data;
	node->color = RED;

	return node;
error:
	return NULL;
}

int rbtree_insert(RBTree *tree, void *data)
{
	if (!tree)
		log_msg("rbtree_insert: null tree");
	if (!data) 
		log_msg("rbtree_insert: null data");

	return insert(tree, data);
		
error:
	return -1;
}

static int insert(RBTree *tree, void *data)
{
	int res;
	Node *curr;
	Node *new;
	CompareFunc cmp_func;

	if (!tree || !data)
		return -1;

	curr = tree->root;
	if (!curr) {
		if (!(new = node_new(data)))
			goto error;

		tree->root = new;
		goto out;
	}

	if (!(cmp_func = tree->cmp_func))
		log_msg("insert: cmp_func is null!");
		
	while ((res = cmp_func(data, curr->data))) {
		if (res < 0) {
			if (curr->left) {
				curr = curr->left;
			} else {
				if (!(new = node_new(data)))
					goto error;

				curr->left = new;
				new->parent = curr;
				goto out;
			}
		} else {
			if (curr->right) {
				curr = curr->right;
			} else {
				if (!(new = node_new(data)))
					goto error;

				curr->right = new;
				new->parent = curr;
				goto out;
			}
		}
	}

	if (data != curr->data && tree->dst_func)
		tree->dst_func(curr->data);

	curr->data = data;
	return 0;

out:
	insert_cases(tree, new);
	return 0;
error:
	return -1;
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
    
static void insert_cases(RBTree *tree, Node *node)
{
	Node *parent;
	Node *granpa;
	Node *uncle;

	if (!tree)
		log_msg("insert_cases: tree is null!");

	while (1) {
		if (!node)
			log_msg("insert_case1: node is null!");

		if (!(parent = node->parent)) {
			node->color = BLACK;
			return;
		}

		/* insert case 2	*/
		if (BLACK == parent->color)
			return;

		/* insert case 3	*/
		granpa = grandparent(node);
		if (!granpa)
			log_msg("insert_case3: granpa is null!");

		uncle = get_uncle(node);
		if (uncle && RED == uncle->color) {
			parent->color = BLACK;
			uncle->color = BLACK;
			granpa->color = RED;
			node = granpa;
			continue;
		}

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

		return;
	}

error:
	return;
}

int rbtree_remove(RBTree *tree, void *data)
{
	if (!tree)
		log_msg("rbtree_remove: tree is null!");
	if (!data)
		log_msg("rbtree_remove: data is null!");

	return tree_remove(tree, data);
error:
	return -1;
}

static int tree_remove(RBTree *tree, void const *data)
{
	int res;
	Node *root;
	Node *curr;
	CompareFunc cmp_func;

	if (!data)
		log_msg("remove: data is null!");
	if (!(root = tree->root))
		return 0;
		
	if (!(cmp_func = tree->cmp_func))
		log_msg("remove: cmp_func is null!");

	curr = root;
	while ((res = cmp_func(data, curr->data))) {
		if (res < 0) {
			if (curr->left)
				curr = curr->left;
			else
				return -1;
		} else {
			if (curr->right)
				curr = curr->right;
			else
				return -1;
		}
	}

	return remove_node(tree, curr);
error:
	return -1;
}

static int remove_node(RBTree *tree, Node *node)
{
	Node *pred;
	void *temp_data;

	if (!tree)
		log_msg("remove_node: tree is null!");
	if (!node)
		log_msg("remove_node: node is null!");
	
	pred = node;
	if (node->left && node->right) {
		pred = get_pred(tree, node);

		temp_data = node->data;
		node->data = pred->data;
		pred->data = temp_data;
	}

	return remove_child(tree, pred);
error:
	return -1;
}

static Node *get_pred(RBTree *tree, Node *node)
{
	Node *pred;

	if (!(pred = node->left))
		log_msg("get_pred: predecessor is NULL!");

	while (pred->right)
		pred = pred->right;

	return pred;
error:
	return NULL;
}

static int remove_child(RBTree *tree, Node *node)
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
	replace_child(tree, node, child);
	
	if (tree->dst_func)
		tree->dst_func(node->data);
	node_destroy(node);

	return 0;
error:
	return -1;
}

static void remove_cases(RBTree *tree, Node *node)
{
	Node *parent;
	Node *sibling;
	Node *granpa;

	if (!tree)
		log_msg("remove_cases: tree is null!");

	while (1) {
		/* remove case 1	*/
		if (!node)
			log_msg("remove_case1: node is null!");
		if (!(parent = node->parent))
			return;

		/* remove case 2	*/
		if (!(sibling = get_sibling(node)))
			log_msg("remove_case2: sibling is null!");

		granpa = parent->parent;
		if (RED == sibling->color) {
			parent->color = RED;
			sibling->color = BLACK;

			if (node == parent->left)
				rotate_left(parent);
			else
				rotate_right(parent);

			if (!granpa)
				tree->root = sibling;
		}

		/* remove case 3	*/
		granpa = parent->parent;
		if (!(sibling = get_sibling(node)))
			log_msg("remove_case2: sibling is null!");

		if (BLACK == parent->color && BLACK == sibling->color
				&& (!sibling->left || BLACK == sibling->left->color)
				&& (!sibling->right || BLACK == sibling->right->color)) {
			sibling->color = RED;
			node = parent;
			continue;
		}

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
				&& (sibling->left && RED == sibling->left->color)
				&& (!sibling->right || BLACK == sibling->right->color)) {
			sibling->color = RED;
			sibling->left->color = BLACK;
			rotate_right(sibling);
		} else if (node == parent->right
				&& (sibling->right && RED == sibling->right->color)
				&& (!sibling->left || BLACK == sibling->left->color)) {
			sibling->color = RED;
			sibling->right->color = BLACK;
			rotate_left(sibling);
		}

		/* remove case 6	*/
		if (!(parent = node->parent))
			log_msg("remove_case6: parent is null!");
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
		return;
	}

error:
	return;
}

static void replace_child(RBTree *tree, Node *node, Node *child)
{
	Node *parent;
	Color temp_color;

	if (!tree)
		log_msg("replace_child: tree is null!");
	if (!node)
		log_msg("replace_child: node is null!");

	parent = node->parent;
	if (parent) {
		if (node == parent->left)
			parent->left = child;
		else
			parent->right = child;

		if (child)
			child->parent = parent;
	} else {
		tree->root = child;
		if (child)
			child->parent = NULL;
	}

	if (child) {
		temp_color = child->color;
		child->color = node->color;
		node->color = temp_color;
	} else {
		node->color = BLACK;
	}
error:
	return;
}

void rbtree_destroy(RBTree *tree)
{
	if (!tree)
		return;

	if (tree->root)
		rbtree_clear(tree, tree->root);
	tree->root = NULL;

	free(tree);
}

static void node_destroy(Node *node)
{
	free(node);
}

static void rbtree_clear(RBTree *tree, Node *node)
{
	if (!tree)
		return;
	if (!node)
		return;

	if (node->left)
		rbtree_clear(tree, node->left);
	if (node->right)
		rbtree_clear(tree, node->right);

	if (tree->dst_func)
		tree->dst_func(node->data);
	node_destroy(node);
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
