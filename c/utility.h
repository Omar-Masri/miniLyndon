#ifndef UTILITY_H_
#define UTILITY_H_

#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/resource.h>

#define K 7
#define W 15
#define MIN_SUP_LENGTH 30
#define MAX_K_FINGER_OCCURRENCE -1
#define MIN_SHARED_K_FINGERS 4
#define MIN_CHAIN_LENGTH 4
#define MIN_REGION_K_FINGER_COVERAGE 0.50
#define MAX_DIFF_REGION_PERCENTAGE 0.1
#define MIN_REGION_LENGTH 0
#define MIN_OVERLAP_COVERAGE 0.27
#define MIN_OVERLAP_LENGTH 100

#define NUM_THREADS 4

#define INPUT stdin
#define OUTPUT fopen("../overlaps-noerr.paf","w")

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

typedef struct {
    unsigned value;
    char* fingerprint;
    unsigned k_finger;
    unsigned index_offset;
} Element;

typedef struct {
    unsigned first;
    unsigned second;
    unsigned third;
    unsigned fourth;
} Duo_int;

typedef struct {
    unsigned first;
    unsigned second;
} Tuple_int;

typedef struct {
    char *first;
    char *second;
} Duo_char;

typedef struct {
    int first;
    Element *second;
    Duo_int *third;
} Triple_int;

typedef struct {
    unsigned int left_offset1;
    unsigned int left_index_offset1;
    unsigned int left_supp_length1;     // you can remove this
    unsigned int left_offset2;
    unsigned int left_index_offset2;
    unsigned int left_supp_length2;     // you can remove this
    unsigned int right_offset1;
    unsigned int right_index_offset1;
    unsigned int right_supp_length1;
    unsigned int right_offset2;
    unsigned int right_index_offset2;
    unsigned int right_supp_length2;
    unsigned int number;
} offset_struct;

typedef struct {
    GHashTable *overlap_dict;
    GArray *lenghts;
    GArray *read_ids;
} User_data;

char* substring(const char* str, int start, int end);
gboolean compare_arrays(GArray *array1, int s1, int s2, int k);
void print_Element(Element *el);
void print_Duo_int(Duo_int *du);
void print_offset_struct(offset_struct *of);
bool is_numeric(const char *str);
int supporting_length(GArray *array, int i, int k);
void insert(GArray *array, GQueue *queue, Element *X, int (*phi)(GArray *array, int i, int k), int k);
void insertLex(GArray *array, GQueue *queue, Element *X, int (*phi)(GArray *array, int i, int k), int k);
Element *fetch(GQueue *queue, int T);
void print_queue(GQueue *queue);
void print_array_Duo_int(GArray *array);
void print_array_Triple_int(GArray *array);
static inline guint32 murmur_hash_32(uint32_t key);
guint duo_int_hash(gconstpointer key);
gboolean duo_int_equal(gconstpointer a, gconstpointer b);
guint tuple_int_hash(gconstpointer key);
gboolean tuple_int_equal(gconstpointer a, gconstpointer b);
guint duo_char_hash(gconstpointer key);
gboolean duo_char_equal(gconstpointer a, gconstpointer b);
void print_PAF(Duo_char *k, int *v, FILE *fp);
void print_PAF_minimap(Duo_char *k, int *v, FILE *fp);
void free_garray_duo_int(GArray *array);
void free_garray_string(GArray *array);
void free_key_value(gpointer key, gpointer value, gpointer user_data);
void free_key_occurrences(gpointer key, gpointer value, gpointer user_data);
void free_key_overlaps(gpointer key, gpointer value, gpointer user_data);
void calculate_usage(struct rusage *usage);
void free_partial_GArray(GArray *array, int start, int end);
void free_garray_of_pointers(GArray *array);

#endif // UTILITY_H_
