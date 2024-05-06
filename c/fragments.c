#include "fragments.h"
#include "glibconfig.h"
#include <stdbool.h>

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

Point_int *new_Point_int(int first, int second, bool type, Fragment *fragment){
    Point_int *point = malloc(sizeof(Point_int));
    if(point == NULL)
        exit(1);

    point->first = first;
    point->second = second;
    point->type = type;
    point->fragment = fragment;

    return point;
}

gint compare_keys(gconstpointer a, gconstpointer b, gpointer userdata) {
    return GPOINTER_TO_INT(a) - GPOINTER_TO_INT(b);
}

GTreeNode *previous(GTree *tree, gconstpointer key)
{
  GTreeNode *node, *result;
  gint cmp;

  g_return_val_if_fail (tree != NULL, NULL);

  node = tree->root;
  if (!node)
    return NULL;

  result = NULL;
  while (1)
    {
      cmp = tree->key_compare(key, node->key, tree->key_compare_data);
      if (cmp >= 0)
        {
          result = node;

          if (!node->right_child)
            return result;

          node = node->right;
        }
      else
        {
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
    if(q_score > get_score((Point_int *)g_tree_node_value(previous(D, GINT_TO_POINTER(point->second))))){
        GTreeNode *q_succ;

        while(D->nnodes){

            q_succ = g_tree_lower_bound(D, GINT_TO_POINTER(point->second));
            if(q_succ != NULL){
                if(q_score <= get_score((Point_int *)g_tree_node_value(q_succ)))
                    break;
            }else
                break;


            g_tree_steal(D, g_tree_node_key(q_succ));
        }

        g_tree_insert(D, GINT_TO_POINTER(point->second), point);
    }
}

Point_int *RMQ(GTree *D, Point_int *point){
    if(point->fragment==NULL && point->type)
        return (Point_int *)g_tree_node_value(g_tree_node_last(D));

    return (Point_int *)g_tree_node_value(previous(D, GINT_TO_POINTER(point->second)));
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

int  maximal_colinear_subset(GArray *array, int start, int end, int k, offset_struct *o){

    GArray *Points_s = g_array_new(FALSE, FALSE, sizeof(Point_int *));
    GArray *Points_e = g_array_new(FALSE, FALSE, sizeof(Point_int *));

    for(int s = start; s < end; s++){
        Triple_int *value = g_array_index(array, Triple_int *, s);

        Fragment *fragment = malloc(sizeof(Fragment));

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
        if(c_point->type == true){
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

                    /* printf("(%d %d %d %d) %d", q->fragment->prec->start->first */
                    /*        ,q->fragment->prec->start->second */
                    /*        ,q->fragment->start->first */
                    /*        ,q->fragment->start->second */
                    /*        ,q->fragment->score); */
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

            }else{
                c_point->fragment->prec = q->fragment->prec == NULL ? q->fragment : q->fragment->prec;
                c_point->fragment->score = k + q->fragment->score;
            }
        }else{
            if(i == 0)
                g_tree_insert(D, GINT_TO_POINTER(0), c_point);
            else
                activate_Point_int(D, c_point);
        }
    }

    return -1;
}
