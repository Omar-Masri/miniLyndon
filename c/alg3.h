#ifndef ALG3_H_
#define ALG3_H_

#include "utility.h"
#include <glib.h>

GArray* get_k_fingers(char *line, char **read_id);
char *k_finger_to_string(GArray *array, int i, int k, const char *separator);
GArray* alg3(GArray* fingerprint, int w, int k, int (*phi)(GArray *array, int i, int k),
             void insertt(GArray *array, GQueue *queue, Element *X, int (*phi)(GArray *array, int i, int k), int k),
             int n);
GHashTable *compute_k_finger_occurrences(GArray *fingerprint_list);
gboolean filter_hash_table(gpointer key, gpointer value, gpointer user_data);
void iterate_occurrence(gpointer key, gpointer value, gpointer user_data);
GHashTable *compute_matches(GHashTable *k_finger_occurrences);
void iterate_overlap(gpointer key, gpointer value, gpointer user_data);
GHashTable *compute_overlaps(GHashTable *k_finger_occurrences, GArray *lenghts, GArray *read_ids);

#endif // ALG3_H_
