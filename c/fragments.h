#ifndef FRAGMENTS_H_
#define FRAGMENTS_H_

#include "utility.h"

/* opaque glib structures */
#define MAX_GTREE_HEIGHT 40

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
typedef struct Fragment_Cartesian Fragment_Cartesian;
/* opaque glib structures */

struct Point_int {
    bool type;
    Fragment_Cartesian *fragment;
    int first;
    int second;
};

struct Fragment_Cartesian {
    Point_int *start;
    Point_int *end;
    Fragment_Cartesian *prec;
    Triple_fragment *triple;
    int score;
};

gboolean g_tree_steal_lower_bound(GTree *tree, gconstpointer key, int q_score);
void print_Point_int(const Point_int *point);
void print_array_Point_int(GArray *array);
Point_int* new_Point_int(int first, int second, bool type, Fragment_Cartesian *fragment);
gint compare_keys(gconstpointer a, gconstpointer b, gpointer userdata);
GTreeNode* previous(GTree *tree, gconstpointer key);
int get_score(Point_int *point);
void activate_Point_int(GTree *D, Point_int *point);
Point_int* RMQ(GTree *D, Point_int *point);
int maximal_colinear_subset(GArray *array, int start, int end, int k, offset_struct *o);

#endif // FRAGMENTS_H_
