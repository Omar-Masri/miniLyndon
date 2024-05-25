#include "fragments.h"
#include "glibconfig.h"
#include "utility.h"
#include <stdbool.h>

// start opaque functions

static gboolean
g_tree_remove_internal_lower_bound (GTree         *tree,
                                    gconstpointer  key,
                                    gboolean       steal,
                                    int q_score);

static GTreeNode *
g_tree_node_rotate_left (GTreeNode *node)
{
  GTreeNode *right;
  gint a_bal;
  gint b_bal;

  right = node->right;

  if (right->left_child)
    node->right = right->left;
  else
    {
      node->right_child = FALSE;
      right->left_child = TRUE;
    }
  right->left = node;

  a_bal = node->balance;
  b_bal = right->balance;

  if (b_bal <= 0)
    {
      if (a_bal >= 1)
        right->balance = b_bal - 1;
      else
        right->balance = a_bal + b_bal - 2;
      node->balance = a_bal - 1;
    }
  else
    {
      if (a_bal <= b_bal)
        right->balance = a_bal - 2;
      else
        right->balance = b_bal - 1;
      node->balance = a_bal - b_bal - 1;
    }

  return right;
}

static GTreeNode *
g_tree_node_rotate_right (GTreeNode *node)
{
  GTreeNode *left;
  gint a_bal;
  gint b_bal;

  left = node->left;

  if (left->right_child)
    node->left = left->right;
  else
    {
      node->left_child = FALSE;
      left->right_child = TRUE;
    }
  left->right = node;

  a_bal = node->balance;
  b_bal = left->balance;

  if (b_bal <= 0)
    {
      if (b_bal > a_bal)
        left->balance = b_bal + 1;
      else
        left->balance = a_bal + 2;
      node->balance = a_bal - b_bal + 1;
    }
  else
    {
      if (a_bal <= -1)
        left->balance = b_bal + 1;
      else
        left->balance = a_bal + b_bal + 2;
      node->balance = a_bal + 1;
    }

  return left;
}

static GTreeNode *
g_tree_node_balance (GTreeNode *node)
{
  if (node->balance < -1)
    {
      if (node->left->balance > 0)
        node->left = g_tree_node_rotate_left (node->left);
      node = g_tree_node_rotate_right (node);
    }
  else if (node->balance > 1)
    {
      if (node->right->balance < 0)
        node->right = g_tree_node_rotate_right (node->right);
      node = g_tree_node_rotate_left (node);
    }

  return node;
}

/* internal remove routine */
static gboolean
g_tree_remove_internal_lower_bound (GTree         *tree,
                                    gconstpointer  key,
                                    gboolean       steal,
                                    int q_score)
{
  GTreeNode *node, *parent, *balance, *result;
  GTreeNode *path[MAX_GTREE_HEIGHT];
  int idx;
  gboolean left_node;

  if (!tree->root)
    return FALSE;

  idx = 0;
  int count = 0;
  path[idx++] = NULL;
  node = tree->root;

  result = NULL;
  while (1)
  {
    int cmp = tree->key_compare (key, node->key, tree->key_compare_data);
    if (cmp < 0)
      {
        result = node;
        count = idx;

        if (!node->left_child)
          break;

        path[idx++] = node;
        node = node->left;
      }
    else
      {
        if (!node->right_child)
          break;

        path[idx++] = node;
        node = node->right;
      }
  }

  if(result == NULL)
      return FALSE;

  if(q_score <= get_score((Point_int *)g_tree_node_value(result)))
      return FALSE;

  idx = count;
  node = result;

  /* The following code is almost equal to g_tree_remove_node,
   * except that we do not have to call g_tree_node_parent.
   */
  balance = parent = path[--idx];
  g_assert (!parent || parent->left == node || parent->right == node);
  left_node = (parent && node == parent->left);

  if (!node->left_child)
    {
      if (!node->right_child)
        {
          if (!parent)
            tree->root = NULL;
          else if (left_node)
            {
              parent->left_child = FALSE;
              parent->left = node->left;
              parent->balance += 1;
            }
          else
            {
              parent->right_child = FALSE;
              parent->right = node->right;
              parent->balance -= 1;
            }
        }
      else /* node has a right child */
        {
          GTreeNode *tmp = g_tree_node_next (node);
          tmp->left = node->left;

          if (!parent)
            tree->root = node->right;
          else if (left_node)
            {
              parent->left = node->right;
              parent->balance += 1;
            }
          else
            {
              parent->right = node->right;
              parent->balance -= 1;
            }
        }
    }
  else /* node has a left child */
    {
      if (!node->right_child)
        {
          GTreeNode *tmp = g_tree_node_previous (node);
          tmp->right = node->right;

          if (parent == NULL)
            tree->root = node->left;
          else if (left_node)
            {
              parent->left = node->left;
              parent->balance += 1;
            }
          else
            {
              parent->right = node->left;
              parent->balance -= 1;
            }
        }
      else /* node has a both children (pant, pant!) */
        {
          GTreeNode *prev = node->left;
          GTreeNode *next = node->right;
          GTreeNode *nextp = node;
          int old_idx = idx + 1;
          idx++;

          /* path[idx] == parent */
          /* find the immediately next node (and its parent) */
          while (next->left_child)
            {
              path[++idx] = nextp = next;
              next = next->left;
            }

          path[old_idx] = next;
          balance = path[idx];

          /* remove 'next' from the tree */
          if (nextp != node)
            {
              if (next->right_child)
                nextp->left = next->right;
              else
                nextp->left_child = FALSE;
              nextp->balance += 1;

              next->right_child = TRUE;
              next->right = node->right;
            }
          else
            node->balance -= 1;

          /* set the prev to point to the right place */
          while (prev->right_child)
            prev = prev->right;
          prev->right = next;

          /* prepare 'next' to replace 'node' */
          next->left_child = TRUE;
          next->left = node->left;
          next->balance = node->balance;

          if (!parent)
            tree->root = next;
          else if (left_node)
            parent->left = next;
          else
            parent->right = next;
        }
    }

  /* restore balance */
  if (balance)
    while (1)
      {
        GTreeNode *bparent = path[--idx];
        g_assert (!bparent || bparent->left == balance || bparent->right == balance);
        left_node = (bparent && balance == bparent->left);

        if(balance->balance < -1 || balance->balance > 1)
          {
            balance = g_tree_node_balance (balance);
            if (!bparent)
              tree->root = balance;
            else if (left_node)
              bparent->left = balance;
            else
              bparent->right = balance;
          }

        if (balance->balance != 0 || !bparent)
          break;

        if (left_node)
          bparent->balance += 1;
        else
          bparent->balance -= 1;

        balance = bparent;
      }

  if (!steal)
    {
      if (tree->key_destroy_func)
        tree->key_destroy_func (node->key);
      if (tree->value_destroy_func)
        tree->value_destroy_func (node->value);
    }

  g_slice_free (GTreeNode, node);

  tree->nnodes--;

  return TRUE;
}

static void free_array(GArray *Points){
    for(int i=0; i<Points->len; i++){
        Point_int *c_point = g_array_index(Points, Point_int *, i) ;
        if(c_point->fragment != NULL){
            if(c_point->type)
                free(c_point->fragment);
            free(c_point);
        }else
            free(c_point);
    }

    g_array_free(Points, TRUE);
}

static GArray* merge_arrays(GArray *arr1, GArray *arr2) {
    GArray *merged_array = g_array_new(FALSE, FALSE, sizeof(Point_int *));

    guint i = 0, j = 0;

    while (i < arr1->len && j < arr2->len) {
        Point_int *elem1 = g_array_index(arr1, Point_int *, i);
        Point_int *elem2 = g_array_index(arr2, Point_int *, j);

        if (elem1->first <= elem2->first) {
            g_array_append_val(merged_array, elem1);
            i++;
        } else {
            g_array_append_val(merged_array, elem2);
            j++;
        }
    }

    for (; i < arr1->len; i++) {
        Point_int *elem1 = g_array_index(arr1, Point_int *, i);
        g_array_append_val(merged_array, elem1);
    }

    for (; j < arr2->len; j++) {
        Point_int *elem2 = g_array_index(arr2, Point_int *, j);
        g_array_append_val(merged_array, elem2);
    }

    return merged_array;
}

static inline gboolean print_key_value(gpointer key, gpointer value, gpointer user_data) {
    g_print("%d\n", GPOINTER_TO_INT(key)); // Assuming keys are strings
    return FALSE; // Continue traversing
}

static inline void print_tree(GTree *D) {
    g_tree_foreach(D, (GTraverseFunc)print_key_value, NULL);
}

gboolean
g_tree_steal_lower_bound (GTree         *tree,
                          gconstpointer  key,
                          int q_score)
{
  gboolean removed;

  g_return_val_if_fail (tree != NULL, FALSE);

  removed = g_tree_remove_internal_lower_bound (tree, key, TRUE, q_score);

  return removed;
}

// end  opaque functions

void print_Point_int(const Point_int *point) {
    printf("Fragment %p, Type: %s, (%u, %u)\n", point->fragment, point->type ? "True" : "False"
           ,point->first, point->second);
}

void print_array_Point_int(GArray *array) {

    if (array == NULL || array->len == 0) {
        printf("Array is empty\n");
        return;
    }

    printf("Array contents:\n");
    for (guint i = 0; i < array->len; i++) {
        Point_int *s = g_array_index(array, Point_int *, i);
        print_Point_int(s);
    }
    printf("\n");
}

Point_int *new_Point_int(int first, int second, bool type, Fragment_Cartesian *fragment){
    Point_int *point = malloc(sizeof(Point_int));

    point->first = first;
    point->second = second;
    point->type = type;
    point->fragment = fragment;

    return point;
}

gint compare_keys(gconstpointer a, gconstpointer b, gpointer userdata) {
    return GPOINTER_TO_INT(a) - GPOINTER_TO_INT(b);
}

GTreeNode *previous(GTree *tree, gconstpointer key) {
    GTreeNode *node, *result;
    gint cmp;

    g_return_val_if_fail(tree != NULL, NULL);

    node = tree->root;
    if (!node)
        return NULL;

    result = NULL;
    while (1) {
        cmp = tree->key_compare(key, node->key, tree->key_compare_data);
        if (cmp > 0) {
            result = node;

            if (!node->right_child)
                return result;

            node = node->right;
        } else {
            if (!node->left_child)
                return result;

            node = node->left;
        }
    }
}

int get_score(Point_int *point){
    if(point->fragment == NULL)
        return -1;

    return point->fragment->score;
}

void activate_Point_int(GTree *D, Point_int *point){
    int q_score = point->fragment->score;
    GTreeNode *d = previous(D, GINT_TO_POINTER(point->second));
    Point_int *p = (Point_int *)g_tree_node_value(d);
    if(q_score > get_score((Point_int *)g_tree_node_value(previous(D, GINT_TO_POINTER(point->second))))){

        while(D->nnodes){
            if(!g_tree_steal_lower_bound(D, GINT_TO_POINTER(point->second), q_score))
                break;
        }

        g_tree_insert(D, GINT_TO_POINTER(point->second), point);
    }
}

Point_int *RMQ(GTree *D, Point_int *point){
    if(point->fragment==NULL && point->type)
        return (Point_int *)g_tree_node_value(g_tree_node_last(D));

    return (Point_int *)g_tree_node_value(previous(D, GINT_TO_POINTER(point->second)));
}

int maximal_colinear_subset(GArray *array, int start, int end, int k, offset_struct *o){

    GArray *Points_s = g_array_new(FALSE, FALSE, sizeof(Point_int *));
    GArray *Points_e = g_array_new(FALSE, FALSE, sizeof(Point_int *));

    for(int s = start; s < end; s++){
        Triple_fragment *value = g_array_index(array, Triple_fragment *, s);

        Fragment_Cartesian *fragment = malloc(sizeof(Fragment_Cartesian));

        Point_int *point_s = new_Point_int(value->second->value
                                           , value->third->second, true, fragment);
        Point_int *point_e = new_Point_int(value->second->value+k
                                           , value->third->second+k, false, fragment);

        fragment->start = point_s;
        fragment->end = point_e;
        fragment->prec = NULL;
        fragment->triple = value;
        fragment->score = 0;

        g_array_append_val(Points_s, point_s);
        g_array_append_val(Points_e, point_e);
    }

    GArray *Points = merge_arrays(Points_s, Points_e);

    g_array_free(Points_s, TRUE);
    g_array_free(Points_e, TRUE);

    Point_int *origin = malloc(sizeof(Point_int));
    Point_int *terminus = malloc(sizeof(Point_int));

    origin->first = 0;
    origin->second = 0;
    origin->type = false;
    origin->fragment = NULL;

    terminus->first = 0;
    terminus->second = 0;
    terminus->type = true;
    terminus->fragment = NULL;

    g_array_prepend_val(Points, origin);
    g_array_append_val(Points, terminus);

    GTree *D = g_tree_new_full(compare_keys, NULL, NULL, NULL);  //metti funzione free value

    for(int i=0; i<Points->len; i++){
        Point_int *c_point = g_array_index(Points, Point_int *, i);

        if(c_point->type == true){                    //start
            Point_int *q = RMQ(D, c_point);

            if(q->fragment == NULL){                  //origin
                c_point->fragment->prec = NULL;
                c_point->fragment->score = k;
            }else if(c_point->fragment == NULL){      //terminus
                if(q->fragment->prec != NULL){
                    o->left_offset1 = q->fragment->prec->start->first;
                    o->left_index_offset1 = q->fragment->prec->triple->second->index_offset;
                    o->left_supp_length1 = q->fragment->prec->triple->second->k_finger;

                    o->left_offset2 = q->fragment->prec->start->second;
                    o->left_index_offset2 = q->fragment->prec->triple->third->third;
                    o->left_supp_length2 = q->fragment->prec->triple->third->fourth;

                    o->right_offset1 = q->fragment->start->first;
                    o->right_index_offset1 = q->fragment->triple->second->index_offset;
                    o->right_supp_length1 = q->fragment->triple->second->k_finger;

                    o->right_offset2 = q->fragment->start->second;
                    o->right_index_offset2 = q->fragment->triple->third->third;
                    o->right_supp_length2 = q->fragment->triple->third->fourth;
                }
                else{
                    o->left_offset1 = q->fragment->start->first;
                    o->left_index_offset1 = q->fragment->triple->second->index_offset;
                    o->left_supp_length1 = q->fragment->triple->second->k_finger;

                    o->left_offset2 = q->fragment->start->second;
                    o->left_index_offset2 = q->fragment->triple->third->third;
                    o->left_supp_length2 = q->fragment->triple->third->fourth;

                    o->right_offset1 = q->fragment->start->first;
                    o->right_index_offset1 = q->fragment->triple->second->index_offset;
                    o->right_supp_length1 = q->fragment->triple->second->k_finger;

                    o->right_offset2 = q->fragment->start->second;
                    o->right_index_offset2 = q->fragment->triple->third->third;
                    o->right_supp_length2 = q->fragment->triple->third->fourth;
                }

                int score = q->fragment->score;

                free_array(Points);

                g_tree_destroy(D);

                return score;

            }else{                //end
                c_point->fragment->prec = q->fragment->prec == NULL ? q->fragment : q->fragment->prec;
                c_point->fragment->score = k + q->fragment->score;
            }
        }else{

          if(i == 0)
            g_tree_insert(D, GINT_TO_POINTER(-1), c_point);
          else
            activate_Point_int(D, c_point);
        }
    }

    return -1;
}
