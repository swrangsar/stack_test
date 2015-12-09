#include "rbtree.h"

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>

#define log_err(M)	{perror("error: " M); goto error;}
#define log_msg(M)	{fprintf(stderr, "error: " M "\n"); goto error;}


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
	rbcmpf cmp;
	rbdstf dst;
	rbshwf shw;
};


static Node *node_new(const void *);
static int insert(RBTree *, Node*, Node*);

static void rbtreeClassDel(rbtreeClass *);
static void rbnodeDel(rbtree *, rbnode *);
static void _rbtreeDel(rbtree *, rbnode *);
static rbnode *grandparent(const rbnode *);
static rbnode *uncle(const rbnode *);
static void rotate_left(rbnode *);
static void rotate_right(rbnode *);
static void insert_cases(rbtree *, rbnode *);

static rbnode *sibling(const rbnode *);
static void _rbtreeRemove(rbtree *, rbnode *, const void *);
static void rbnodeRemove(rbtree *, rbnode *);
static rbnode *getPred(rbtree *, rbnode *);
static void removeChild(rbtree *, rbnode *);
static void replaceChild(rbtree *, rbnode *, rbnode *);
static void remove_cases(rbtree *, rbnode *);



RBTree* rbtree_new(CompareFunc cmp, DestroyFunc dst, ShowFunc shw)
{
	RBTree *tree;
	
	tree = malloc(sizeof(*tree));
	if (!tree)
		log_err("rbtree_new");
	if (!cmp)
		log_err("rbtree_new: compare_func is NULL!";

	tree->root = NULL;
	tree->cmp = cmp;
	tree->dst = dst;
	tree->shw = shw;

	return tree;
error:
	return NULL;
}

static Node *node_new(const void *data)
{
	Node *node;
	
	node = malloc(sizeof(*node));
	if (!node)
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
	Node *node;
	Node *root;
	int ret;
	
	if (!tree)
		log_msg("rbtree_insert: null tree");
	if (!data)
		return 0;

	if (!(node = node_new(data)))
		goto error;
	
	root = tree->root;
	if (root && insert(tree, root, node))
		insert_cases(tree, node);
	} else {
		tree->root = node;
		insert_cases(tree, node);
	}

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
	Node *p, *g;

	if (node && (p = node->parent) && (g = p->parent))
		return (p == g->left)?g->right:g->left;
	else
		return NULL;
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

// ******************************************
//      static functions
// ******************************************



static void rbnodeDel(rbtree *t, rbnode *n)
{
	if (!n) return;
	errcheck(t, "rbtree is null!");
	if (n->data) t->klass->dst(n->data);
	n->data = NULL;
	n->left = NULL;
	n->right = NULL;
	n->parent = NULL;
	free(n);
	n = NULL;
}


static void rbtreeClassDel(rbtreeClass *k)
{
	k->cmp = NULL;
	k->dst = NULL;
	k->shw = NULL;
	free(k);
	k = NULL;
}

static void _rbtreeDel(rbtree *t, rbnode *n)
{
	errcheck(t, "rbtree is null!");
	if (n->left) _rbtreeDel(t, n->left);
	if (n->right) _rbtreeDel(t, n->right);
	rbnodeDel(t, n);
}

static int insert(rbtree *t, rbnode *r, rbnode *n)
{
	errcheck(t, "tree is null!");
	errcheck(r, "rbnode *r is null!");
	errcheck(n, "rbnode *n is null!");
	errcheck(t->klass, "tree klass is null!");
	errcheck(t->klass->cmp, "cmp fn ptr is null!");
	errcheck(n->data, "new data is null!");
	errcheck(r->data, "old data is null!");

	int res = t->klass->cmp(n->data, r->data);
	while (res) {
		if (res < 0) {
			if (!r->left) {
				r->left = n;
				n->parent = r;
				return -1;
			}
			r = r->left;
		} else {
			if (!r->right) {
				r->right = n;
				n->parent = r;
				return 1;
			}
			r = r->right;
		}
		errcheck(r->data, "old data is null!");
		res = t->klass->cmp(n->data, r->data);
	}

	debug("_rbtreeInsert: rbnode already exist!\nreplacing node...\n");

	rbnode *op = r->parent;
	rbnode *ol = r->left;
	rbnode *or = r->right;
    unsigned char oc = r->color;
    if (op) {
        if (r == op->left) {
            op->left = n;
        } else {
            op->right = n;
        }
    } else {
        t->root = n;
    }
    if (ol) ol->parent = n;
    if (or) or->parent = n;
    r->parent = n->parent;
    r->left = n->left;
    r->right = n->right;
            
    n->parent = op;
    n->left = ol;
    n->right = or;
    n->color = oc;
          
    rbnodeDel(t, r);
    return 0;
}



static void rotate_left(rbnode *n)
{
    errcheck(n, "rotate_left: n is null!");
    rbnode *p = n->parent;
    rbnode *r = n->right;
    errcheck(r, "rotate_left: r is null!");
    if (p) {
        if (n == p->left) {
            p->left = r;
        } else {
            p->right = r;
        }
    }
    n->right = r->left;
    r->left = n;
    n->parent = r;
    r->parent = p;
    if (n->right) n->right->parent = n;
}

static void rotate_right(rbnode *n)
{
    errcheck(n, "rotate_right: n is null!");
    rbnode *p = n->parent;
    rbnode *l = n->left;
    errcheck(l, "rotate_right: l is null!");
    if (p) {
        if (n == p->left) {
            p->left = l;
        } else {
            p->right = l;
        }
    }
    n->left = l->right;
    l->right = n;
    n->parent = l;
    l->parent = p;
    if (n->left) n->left->parent = n;
}
    
static void insert_cases(rbtree *t, rbnode *n)
{
    while (1) {
        debug("insert case 1\n");
        errcheck(n, "new inserted node cannot be null!");
        rbnode *p = n->parent;
        if (!p) {
            n->color = BLACK;
            return;
        }

        debug("insert case 2\n");
        errcheck(p, "p is null!");
        if (p->color == BLACK) return;
        
        debug("insert case 3\n");
        rbnode *g = grandparent(n);
        rbnode *u = uncle(n);
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
