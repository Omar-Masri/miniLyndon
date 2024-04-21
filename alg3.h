#ifndef ALG3_H_
#define ALG3_H_

#include <glib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define K 7
#define W 15
#define MIN_SUP_LENGTH 40
#define MAX_K_FINGER_OCCURRENCE -1
#define MIN_SHARED_K_FINGERS 4
#define MIN_REGION_K_FINGER_COVERAGE 0.27
#define MAX_DIFF_REGION_PERCENTAGE 0.0
#define MIN_REGION_LENGTH 0
#define MIN_OVERLAP_COVERAGE 0.70
#define MIN_OVERLAP_LENGTH 600

#define INPUT fopen("../../Data/fingerprint_CFL_ICFL_COMB-30_s_300_noerr.txt", "r")
#define OUTPUT fopen("overlaps.paf","w")

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

typedef struct {
    int value;
    char* fingerprint;
    int k_finger;
    int index_offset;
} Element;

typedef struct {
    int first;
    int second;
    int third;
    int fourth;
} Duo_int;

typedef struct {
    int left_offset1;
    int left_index_offset1;
    int left_supp_length1;
    int left_offset2;
    int left_index_offset2;
    int left_supp_length2;
    int right_offset1;
    int right_index_offset1;
    int right_supp_length1;
    int right_offset2;
    int right_index_offset2;
    int right_supp_length2;
    int number;
} offset_struct;

typedef struct {
    GHashTable *overlap_dict;
    GArray *lenghts;
} User_data;


/**
 * @brief Print the contents of an Element struct.
 *
 * @param el The Element struct to print.
 */
void print_Element(Element *el);

/**
 * @brief Print the contents of a Duo_int struct.
 *
 * @param du The Duo_int struct to print.
 */
void print_Duo_int(Duo_int *du);

/**
 * @brief Print the contents of an offset_struct struct.
 *
 * @param of The offset_struct struct to print.
 */
void print_offset_struct(offset_struct *of);

/**
 * @brief Check if a string represents a numeric value.
 *
 * @param str The string to check.
 * @return true if the string is numeric, false otherwise.
 */
bool is_numeric(const char *str);

/**
 * @brief Calculate the supporting length of a subsequence in an array.
 *
 * @param array The array containing the sequence.
 * @param i The starting index of the subsequence.
 * @param k The length of the subsequence.
 * @return The supporting length of the subsequence.
 */
int supporting_length(GArray *array, int i, int k);

/**
 * @brief Insert an Element into a priority queue.
 *
 * @param array The array used for calculating priorities.
 * @param queue The priority queue.
 * @param X The Element to insert.
 * @param phi The priority function.
 * @param k The length of the subsequence.
 */
void insert(GArray *array, GQueue *queue, Element *X, int (*phi)(GArray *array, int i, int k), int k);

/**
 * @brief Insert an Element into a priority queue with lexicographical ordering.
 *
 * @param array The array used for calculating priorities.
 * @param queue The priority queue.
 * @param X The Element to insert.
 * @param phi The priority function.
 * @param k The length of the subsequence.
 */
void insertLex(GArray *array, GQueue *queue, Element *X, int (*phi)(GArray *array, int i, int k), int k);

/**
 * @brief Fetch the Element with the highest priority from a priority queue.
 *
 * @param queue The priority queue.
 * @param T The threshold value.
 * @return The Element with the highest priority.
 */
Element *fetch(GQueue *queue, int T);

/**
 * @brief Print the contents of a priority queue.
 *
 * @param queue The priority queue to print.
 */
void print_queue(GQueue *queue);

/**
 * @brief Print the contents of an array.
 *
 * @param array The array to print.
 */
void print_array(GArray *array);

/**
 * @brief Parse a string containing k-fingers into an array.
 *
 * @param line The input string.
 * @return A GArray containing the parsed k-fingers.
 */
GArray* get_k_fingers(char *line);

/**
 * @brief Convert a range of k-fingers to a string with a separator.
 *
 * @param array The array containing the k-fingers.
 * @param i The starting index of the range.
 * @param k The length of the range.
 * @param separator The separator to use between k-fingers.
 * @return A string representation of the k-fingers with the separator.
 */
char *k_finger_to_string(GArray *array, int i, int k, const char *separator);

/**
 * @brief Algorithm 3 for processing k-fingers.
 *
 * @param fingerprint The array containing the k-fingers.
 * @param w The window size.
 * @param k The length of the k-fingers.
 * @param phi The priority function.
 * @param n The total number of k-fingers.
 * @return A GArray containing the results of Algorithm 3.
 */
GArray* alg3(GArray* fingerprint, int w, int k, int (*phi)(GArray *array, int i, int k), int n);

/**
 * @brief Compute k-finger occurrences and store them in a hash table.
 *
 * @param fingerprint_list The list of k-finger arrays.
 * @return A GHashTable containing the k-finger occurrences.
 */
GHashTable *compute_k_finger_occurrences(GArray *fingerprint_list);

/**
 * @brief Filter a hash table based on certain criteria.
 *
 * @param key The key of the current entry.
 * @param value The value of the current entry.
 * @param user_data Additional user data.
 * @return TRUE if the entry should be removed, FALSE otherwise.
 */
gboolean filter_hash_table(gpointer key, gpointer value, gpointer user_data);

/**
 * @brief Custom hash function for Duo_int keys.
 *
 * @param key The key to hash.
 * @return The hash value.
 */
guint duo_int_hash(gconstpointer key);

/**
 * @brief Custom equality function for Duo_int keys.
 *
 * @param a The first key.
 * @param b The second key.
 * @return TRUE if the keys are equal, FALSE otherwise.
 */
gboolean duo_int_equal(gconstpointer a, gconstpointer b);

/**
 * @brief Iterate over k-finger occurrences and compute overlaps.
 *
 * @param key The key of the current occurrence.
 * @param value The value of the current occurrence.
 * @param user_data Additional user data.
 */
void iterate_occurrence(gpointer key, gpointer value, gpointer user_data);

/**
 * @brief Compute overlaps between k-finger occurrences and store them in a hash table.
 *
 * @param k_finger_occurrences The hash table containing k-finger occurrences.
 * @param lengths The lengths array.
 * @return A GHashTable containing the overlaps.
 */
GHashTable *compute_overlaps(GHashTable *k_finger_occurrences, GArray *lengths);

/**
 * @brief Print a PAF entry to a file.
 *
 * @param key The key of the PAF entry.
 * @param value The value of the PAF entry.
 * @param user_data The file pointer.
 */
void print_PAF(gpointer key, gpointer value, gpointer user_data);

/**
 * @brief Write PAF entries to a file.
 *
 * @param overlap_dict The hash table containing overlaps.
 * @param fp The file pointer.
 */
void write_PAF(GHashTable *overlap_dict, FILE *fp);

#endif // ALG3_H_
