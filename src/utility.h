#ifndef UTILITY_H_
#define UTILITY_H_

#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/resource.h>

#define INPUT stdin
#define OUTPUT_FILE_NAME "./overlaps-noerr.paf"
#define OUTPUT stdout

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

typedef struct {
    unsigned value;
    unsigned k_finger;
    unsigned index_offset;
    unsigned fingerprint;
} Element;

typedef struct {
    unsigned first;
    unsigned second;
    unsigned third;
    unsigned fourth;
} Occurrence;

typedef struct {
    char *first;
    char *second;
} Duo_char;

typedef struct {
    int first;
    Element *second;
    Occurrence *third;
} Triple_fragment;

typedef struct {
    unsigned int left_offset1;          // you can remove this TODO
    unsigned int left_index_offset1;
    unsigned int left_supp_length1;     // you can remove this TODO
    unsigned int left_offset2;          // you can remove this TODO
    unsigned int left_index_offset2;
    unsigned int left_supp_length2;     // you can remove this TODO
    unsigned int right_offset1;         // you can remove this TODO
    unsigned int right_index_offset1;
    unsigned int right_supp_length1;
    unsigned int right_offset2;         // you can remove this TODO
    unsigned int right_index_offset2;
    unsigned int right_supp_length2;
    unsigned int number;
} offset_struct;

typedef struct {
    GHashTable *overlap_dict;
    GArray *lenghts;
    GArray *read_ids;
} User_data;

unsigned int djb2(GArray *array, int i, int k);
unsigned int djb2arr(int *array, int i, int k);
char* substring(const char* str, int start, int end);
gboolean compare_arrays(GArray *array1, int s1, int s2, int k);
void print_Element(Element *el);
void print_Occurrence(Occurrence *du);
void print_offset_struct(offset_struct *of);
bool is_numeric(const char *str);
int supporting_length(GArray *array, int i, int k);
void insert(GArray *array, GQueue *queue, Element *X, int (*phi)(GArray *array, int i, int k), int k);
void insertLex(GArray *array, GQueue *queue, Element *X, int (*phi)(GArray *array, int i, int k), int k);
Element* fetch(GQueue *queue, int T);
void print_queue(GQueue *queue);
void print_array_Occurrence(GArray *array);
void print_array_Triple_fragment(GArray *array);
guint Occurrence_hash(gconstpointer key);
gboolean Occurrence_equal(gconstpointer a, gconstpointer b);
guint duo_char_hash(gconstpointer key);
gboolean duo_char_equal(gconstpointer a, gconstpointer b);
void print_PAF_minimap(Duo_char *k, int *v, FILE *fp, GHashTable *set, bool rc);
void free_partial_GArray(GArray *array, int start, int end);
void free_garray_of_pointers(GArray *array);
void free_garray_Occurrence(GArray *array);
void free_garray_string(GArray *array);
void free_key_value(gpointer key, gpointer value, gpointer user_data);
void free_key_occurrences(gpointer key, gpointer value, gpointer user_data);
void free_key_overlaps(gpointer key, gpointer value, gpointer user_data);
void calculate_usage(struct rusage *usage);
unsigned long read_off_memory_status();

inline void *mymalloc(size_t size){
    void *result = malloc(size);

    if(result == NULL){
        fprintf(stderr, "unable to manually allocate memory!");
        exit(1);
    }

    return result;
}

#endif // UTILITY_H_
