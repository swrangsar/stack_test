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


static Node *node_new(const void *);
static void node_destroy(Node*);

static int insert(RBTree *, Node*, Node*);

static void rotate_left(Node const*);
static void rotate_right(Node const*);

static Node *grandparent(const Node *);
static Node *uncle(const Node *);

static void rbnodeDel(rbtree *, rbnode *);
static void _rbtreeDel(rbtree *, rbnode *);
static void insert_cases(rbtree *, rbnode *);

static rbnode *sibling(const rbnode *);
static void _rbtreeRemove(rbtree *, rbnode *, const void *);
static void rbnodeRemove(rbtree *, rbnode *);
static rbnode *getPred(rbtree *, rbnode *);
static void removeChild(rbtree *, rbnode *);
static void replaceChild(rbtree *, rbnode *, rbnode *);
static void remove_cases(rbtree *, rbnode *);



RBTree* rbtree_new(CompareFunc cmp, DestroyFunc dst)
{
	RBTree *tree;
	
	tree = malloc(sizeof(*tree));
	if (!tree)
		log_err("rbtree_new");
	if (!cmp)
		log_err("rbtree_new: compare_func is NULL!";

	tree->root = NULL;
	tree->cmp_func = cmp;
	tree->dst_func = dst;

	return tree;
error:
	return NULL;
}

static Node *node_new(const void *data)
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

int rbtree_insert(RBTree *tree, const void *data)
{
	if (!tree)
		log_msg("rbtree_insert: null tree");
	if (!data) 
		log_msg("rbtree_insert: null data");

	return insert(tree, data);
		
error:
	return -1;
}

static int insert(RBTree *tree, const void *data)
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

static Node *uncle(const Node *node)
{
	Node *parent;
	Node *grandpa;

	if (node && (parent = node->parent) && (grandpa = parent->parent))
		return (parent == grandpa->left)?grandpa->right:grandpa->left;
	else
		return NULL;
}

static void rotate_left(Node const *node)
{
	Node *parent;
	Node *right;
	
	if (!n)
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

static void rotate_right(Node const *node)
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
    
void rbtreeRemove(rbtree *t, const void *data)
{
	errcheck(t, "rbtreeRemove: tree shouldn't be null!");
	errcheck(data, "rbtreeRemove: data shouldn't be null!");
	if (!t->root) return; 
	_rbtreeRemove(t, t->root, data);
}

void rbtreeDel(rbtree *t)
{
	errcheck(t, "rbtree is null!");
	if (t->root) _rbtreeDel(t, t->root);
	if (t->klass) rbtreeClassDel(t->klass);
	t->root = NULL;
	t->klass = NULL;
	free(t);
	t = NULL;
}

static void node_destroy(Node *node)
{
	free(node);
}

static void _rbtreeDel(rbtree *t, rbnode *n)
{
	errcheck(t, "rbtree is null!");
	if (n->left) _rbtreeDel(t, n->left);
	if (n->right) _rbtreeDel(t, n->right);
	rbnodeDel(t, n);
}

static void insert_cases(RBTree *t, Node *n)
{
	while (1) {
		debug("insert case 1\n");
		errcheck(n, "new inserted node cannot be null!");
		Node *p = n->parent;
		if (!p) {
			n->color = BLACK;
			return;
		}

		debug("insert case 2\n");
		errcheck(p, "p is null!");
		if (p->color == BLACK) return;

		debug("insert case 3\n");
		Node *g = grandparent(n);
		Node *u = uncle(n);
		errcheck(g, "grandparent does not exist!");
		if (u && RED == u->color) {
			p->color = BLACK;
			u->color = BLACK;
			g->color = RED;
			n = g;
			continue;
		}

		debug("insert case 4\n");
		if (n == p->right && p == g->left) {
			rotate_left(p);
			n = p;
		} else if (n == p->left && p == g->right) {
			rotate_right(p);
			n = p;
		} else {
			// do nothing
		}

		debug("insert case 5\n");
		p = n->parent;
		g = grandparent(n);
		errcheck(p, "parent does not exist!");
		errcheck(g, "grandparent does not exist!");
		p->color = BLACK;
		g->color = RED;
		(n == p->left)?rotate_right(g):rotate_left(g);
		if (!(p->parent)) {
			debug("insert_case5: p parent is null!\n");
			t->root = p;
		}
		return;
	}
}


// ****************************
// removal functions
// ****************************


static rbnode *sibling(const rbnode *n)
{
    errcheck(n, "sibling: node n shouldn't be null!");
    rbnode *p = n->parent;
    errcheck(p, "sibling: parent shouldn't be null!");
    return (n == p->left)?(p->right):(p->left);
}


static void _rbtreeRemove(rbtree *t, rbnode *n, const void *d)
{
    errcheck(t, "tree is null!");
    errcheck(n, "node is null!");
    errcheck(d, "data is null!");
    errcheck(t->klass, "tree klass is null!");
    errcheck(t->klass->cmp, "cmp fn ptr is null!");
    errcheck(n->data, "node data is null!");
    int res = t->klass->cmp(d, n->data);

    while (res) {
        if (res < 0) {
            if (!n->left) return;
            n = n->left;
        } else {
            if (!n->right) return;
            n = n->right;
        }
        errcheck(n->data, "node data is null!");
        res = t->klass->cmp(d, n->data);
    }
    rbnodeRemove(t, n);
}

static void rbnodeRemove(rbtree *t, rbnode *n)
{
    errcheck(t, "tree is null!");
    errcheck(n, "node is null!");
    printf("data to be deleted is ");
    errcheck(t->klass, "tree klass is null!");
    errcheck(t->klass->shw, "tree klash shw is null!");
    t->klass->shw(n->data);
    rbnode *p = n;
    if (n->left && n->right) {
        p = getPred(t, n);
        void *tempData = n->data;
        n->data = p->data;
        p->data = tempData;
    }
    removeChild(t, p);
}

static rbnode *getPred(rbtree *t, rbnode *n)
{
    errcheck(t, "tree is null!");
    errcheck(n, "node is null!");
    errcheck(n->left, "predecessor is null!");
    rbnode *p = n->left;
    while (p->right) {
        p = p->right;
    }
    return p;
}

static void removeChild(rbtree *t, rbnode *n)
{
    errcheck(t, "tree is null!");
    errcheck(n, "node is null!");
    printf("trying to remove child with data ");
    errcheck(t->klass, "tree klass is null!");
    errcheck(t->klass->shw, "tree klass shw is null!");
    t->klass->shw(n->data);

    rbnode *c = (n->left)?(n->left):(n->right);

    if (n->color == BLACK) {
        if (c && c->color == RED)
            c->color = BLACK;
        else
            remove_cases(t, n);
    }
    replaceChild(t, n, c);
    rbnodeDel(t, n);
}

static void replaceChild(rbtree *t, rbnode *n, rbnode *c)
{
    errcheck(t, "tree is null!");
    errcheck(n, "node is null!");
    rbnode *p = n->parent;
    if (p) {
        if (n == p->left) {
            p->left = c;
        } else {
            p->right = c;
        }
        if (c) c->parent = p;
    } else {
        t->root = c;
        if (c) c->parent = NULL;
    }
    if (c) {
        unsigned char tempc = c->color;
        c->color = n->color;
        n->color = tempc;
    } else {
        n->color = BLACK;
    }
}

static void remove_cases(rbtree *t, rbnode *n)
{
    while (1) {
        errcheck(t, "tree is null!");
        errcheck(n, "node is null!");
        debug("remove case 1\n");
        rbnode *p = n->parent;
        if (!p) return;
    
        debug("remove case 2\n");
        errcheck(p, "parent is null!");
        rbnode *s = sibling(n);
        errcheck(s, "sibling is null!");
        rbnode *g = p->parent;
        if (s->color == RED) {
            p->color = RED;
            s->color = BLACK;
            if (n == p->left) {
                rotate_left(p);
            } else {
                rotate_right(p);
            }
            if (!g) t->root = s;
        }

        debug("remove case 3\n");
        g = p->parent;
        s = sibling(n);
        errcheck(s, "sibling is null!");
        if (p->color == BLACK 
            && s->color == BLACK
            && (!s->left || s->left->color == BLACK)
            && (!s->right || s->right->color == BLACK)) {
            s->color = RED;
            n = p;
            continue;
        } 

        debug("remove case 4\n");
        if (p->color == RED
            && s->color == BLACK
            && (!s->left || s->left->color == BLACK)
            && (!s->right || s->right->color == BLACK)) {
            p->color = BLACK;
            s->color = RED;
            return;
        }

        debug("remove case 5\n");
        errcheck((s->color == BLACK), "sibling is red!");
    
        if (n == p->left &&
            (s->left && s->left->color == RED) &&
            (!s->right || s->right->color == BLACK)) {
            s->color = RED;
            s->left->color = BLACK;
            rotate_right(s);
        } else if (n == p->right &&
            (s->right && s->right->color == RED) &&
            (!s->left || s->left->color == BLACK)) {
            s->color = RED;
            s->right->color = BLACK;
            rotate_left(s);
        } else {
        }

        debug("remove case 6\n");
        p = n->parent;
        s = sibling(n);
        errcheck(p, "parent is null!");
        errcheck(s, "sibling is null!");
        
        s->color = p->color;
        p->color = BLACK;
        if (n == p->left) {
            rotate_left(p);
            errcheck(s->right, "rem case 6: sibling's child is null!");
            errcheck(s->right->color == RED, "rem case 6: sibling's child is not red!");
            s->right->color = BLACK;
        } else {
            rotate_right(p);
            errcheck(s->left, "rem case 6: sibling's child is null!");
            errcheck(s->left->color == RED, "rem case 6: sibling's child is not red!");
            s->left->color = BLACK;
        }
        if (!s->parent) t->root = s;
        printf("remove_case6 exited\n");
        return;
    }
}
