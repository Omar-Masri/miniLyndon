#ifndef ALG3_H_
#define ALG3_H_

#include "utility.h"
#include "fragments.h"
#include <glib.h>
#include <stdio.h>
#include <pthread.h>

GArray* alg3(GArray* fingerprint, int w, int k, int (*phi)(GArray *array, int i, int k),
             void (*insertt)(GArray *array, GQueue *queue, Element *X, int (*phi)(GArray *array, int i, int k), int k),
             int n);
GHashTable *compute_k_finger_occurrences(GArray *fingerprint_list);
void *thread_matches(void *args);
void compute_matches(GArray *minimizers, GHashTable *k_finger_occurrences, int k, FILE *fp,
                     GArray *lengths, GArray *read_ids, int start_thread, int end_thread);
void find_overlap(int first, int second, offset_struct *current, FILE *fp
                  , GArray *lenghts, GArray *read_ids, GHashTable *set, bool rc);

#endif // ALG3_H_
