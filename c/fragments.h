#ifndef FRAGMENTS_H_
#define FRAGMENTS_H_

#include "utility.h"

/* opaque glib structures */
typedef struct _GTreeNode GTreeNode;

struct _GTree
{
	GTreeNode        *root;
	GCompareDataFunc  key_compare;
	GDestroyNotify    key_destroy_func;
	GDestroyNotify    value_destroy_func;
	gpointer          key_compare_data;
	guint             nnodes;
	gint              ref_count;
};

struct _GTreeNode
{
	gpointer   key;         /* key for this node */
	gpointer   value;       /* value stored at this node */
	GTreeNode *left;        /* left subtree */
	GTreeNode *right;       /* right subtree */
	gint8      balance;     /* height (right) - height (left) */
	guint8     left_child;
	guint8     right_child;
};

typedef struct Point_int Point_int;
typedef struct Fragment Fragment;

struct Point_int {
    bool type;
    Fragment *fragment;
    unsigned first;
    unsigned second;
};

struct Fragment {
    Point_int *start;
    Point_int *end;
    Fragment *prec;
    Triple_int *triple;
    int score;
};

int compare_Point_int(const void *f, const void *s);

void print_Point_int(const Point_int *point);

void print_array_Point_int(GArray *array);

int maximal_colinear_subset(GArray *array, int start, int end, int k, offset_struct *o);

#endif // FRAGMENTS_H_
