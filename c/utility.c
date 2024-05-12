#include "utility.h"

extern pthread_mutex_t mutex;

char* substring(const char* str, int start, int end) {
    int len = strlen(str);

    if (start < 0 || end < start || end >= len) {
        return NULL;
    }

    int sub_len = end - start + 1;

    char* sub = (char*)malloc((sub_len + 1) * sizeof(char));
    if (sub == NULL) {
        return NULL;
    }

    strncpy(sub, str + start, sub_len);
    sub[sub_len] = '\0';

    return sub;
}

gboolean compare_arrays(GArray *array1, int s1, int s2, int k) {

    for (guint i = 0; i < k; i++) {
        int element1 = g_array_index(array1, int, s1+i);
        int element2 = g_array_index(array1, int, s2+i);


        if (element1 > element2) {
            return 1;
        } else if (element1 < element2) {
            return -1;
        }
    }


    return 0;
}

void print_Element(Element *el){
    printf("Element: value %d, fingerprint: %s", el->value, el->fingerprint);
}

void print_Duo_int(Duo_int *du){
    if(du == NULL)
        printf("NULL");
    else
        printf("(%d, %d, %d, %d)", du->first, du->second, du->third, du->fourth);
}

void print_offset_struct(offset_struct *of){
    if(of == NULL)
        printf("NULL");
    else
        printf("[%d, (%d, %d), %d, (%d, %d) %d, (%d, %d), %d, (%d, %d) | %d]",
               of->left_offset1, of->left_index_offset1, of->left_supp_length1,
               of->left_offset2, of->left_index_offset2, of->left_supp_length2,
               of->right_offset1,of->right_index_offset1, of->right_supp_length1,
               of->right_offset2,of->right_index_offset2, of->right_supp_length2,
               of->number);
}

bool is_numeric(const char *str) {
    if (str == NULL || *str == '\0') {
        return false;
    }

    for (int i = 0; str[i] != '\0'; i++) {
        if (!(str[i] >= '0' && str[i] <= '9')) {
            return false;
        }
    }

    return true;
}

int supporting_length(GArray *array, int i, int k){
    if(array == NULL)
        return 0;

    int result = 0;

    for(int x=i; x<i+k; x++)
        result += g_array_index(array, int, x);

    return result;
}

void insert(GArray *array, GQueue *queue, Element *X, int (*phi)(GArray *array, int i, int k), int k) {
    while (!g_queue_is_empty(queue)
           && phi(array, ((Element *)(g_queue_peek_tail_link(queue)->data))->value, k) > phi(array, X->value, k)) {

        Element *value = (Element *)g_queue_pop_tail(queue);

        if(value->fingerprint == NULL)
            free(value);
    }
    g_queue_push_tail(queue, X);
}

void insertLex(GArray *array, GQueue *queue, Element *X, int (*phi)(GArray *array, int i, int k), int k) {
    while (!g_queue_is_empty(queue)
           && compare_arrays(array, ((Element *)(g_queue_peek_tail_link(queue)->data))->value,
                      X->value, k) > 0) {

        Element *value = (Element *)g_queue_pop_tail(queue);

        if(value->fingerprint == NULL)
            free(value);
    }
    g_queue_push_tail(queue, X);
}

Element *fetch(GQueue *queue, int T) {
    while (!g_queue_is_empty(queue) && ((Element *)g_queue_peek_head(queue))->value < T) {
        Element *value = (Element *)g_queue_pop_head(queue);

        if(value->fingerprint == NULL)
            free(value);
    }
    return (Element *)g_queue_peek_head(queue);
}

void print_queue(GQueue *queue) {
    if (queue == NULL || g_queue_is_empty(queue)) {
        printf("Queue is empty\n");
        return;
    }

    GList *current = queue->head;
    printf("Queue contents: ");
    while (current != NULL) {
        Element *s = (Element *)current->data;
        printf("%d ", s->value);
        current = current->next;
    }
    printf("\n");
}

void print_array_Duo_int(GArray *array) {

    if (array == NULL || array->len == 0) {
        printf("Array is empty\n");
        return;
    }

    printf("Array contents:\n");
    for (guint i = 0; i < array->len; i++) {
        Duo_int *s = g_array_index(array, Duo_int *, i);
        printf("(%d,%d)\n", s->first, s->second);
    }
    printf("\n");
}

void print_array_Triple_int(GArray *array) {

    if (array == NULL || array->len == 0) {
        printf("Array is empty\n");
        return;
    }

    printf("Array contents:\n");
    for (guint i = 0; i < array->len; i++) {
        Triple_int *s = g_array_index(array, Triple_int *, i);
        printf("(%d,%d,%d)\n", s->first, s->second->value, s->third->second);
    }
    printf("\n");
}

static inline guint32 murmur_hash_32(uint32_t key) {
    key ^= key >> 16;
    key *= 0x85ebca6b;
    key ^= key >> 13;
    key *= 0xc2b2ae35;
    key ^= key >> 16;
    return key;
}

guint duo_int_hash(gconstpointer key) {
    const Duo_int *point = (const Duo_int *)key;
    return murmur_hash_32(point->first)*33 ^ murmur_hash_32(point->second);    // mamma mia sto errore >:(
}

gboolean duo_int_equal(gconstpointer a, gconstpointer b) {
    const Duo_int *point1 = (const Duo_int *)a;
    const Duo_int *point2 = (const Duo_int *)b;
    return point1->first == point2->first && point1->second == point2->second;
}

guint tuple_int_hash(gconstpointer key) {
    const Tuple_int *point = (const Tuple_int *)key;
    return murmur_hash_32(point->first)*33 ^ murmur_hash_32(point->second);    // mamma mia sto errore >:(
}

gboolean tuple_int_equal(gconstpointer a, gconstpointer b) {
    const Tuple_int *point1 = (const Tuple_int *)a;
    const Tuple_int *point2 = (const Tuple_int *)b;
    return point1->first == point2->first && point1->second == point2->second;
}

guint duo_char_hash(gconstpointer key) {
    const Duo_char *point = (const Duo_char *)key;
    return g_str_hash(point->first)*33 ^ g_str_hash(point->second);    // mamma mia sto errore >:(
}

gboolean duo_char_equal(gconstpointer a, gconstpointer b) {
    const Duo_char *point1 = (const Duo_char *)a;
    const Duo_char *point2 = (const Duo_char *)b;
    return strcmp(point1->first, point2->first) == 0 && strcmp(point1->second, point2->second) == 0;
}

void print_PAF(Duo_char *k, int *v, FILE *fp){
    int start1 = v[4];
    int end1 = v[5];

    int start2 = v[6];
    int end2 = v[7];

    if(v[0] == 1){
        int temp = start1;
        start1 = v[2] - end1;
        end1 = v[2] - temp;
    }

    if(v[1] == 1){
        int temp = start2;
        start2 = v[3] - end2;
        end2 = v[3] - temp;
    }

    int strand = v[0] ^ v[1];

    pthread_mutex_lock(&mutex);
    fprintf(fp, "%s\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%d\n", k->first, v[2], start1, end1, k->second, v[3], start2, end2, strand);
    pthread_mutex_unlock(&mutex);
}

void print_PAF_minimap(Duo_char *k, int *v, FILE *fp){

    int start1 = v[4];
    int end1 = v[5];

    int start2 = v[6];
    int end2 = v[7];

    if(v[0] == 1){
        int temp = start1;
        start1 = v[2] - end1;
        end1 = v[2] - temp;
    }

    if(v[1] == 1){
        int temp = start2;
        start2 = v[3] - end2;
        end2 = v[3] - temp;
    }

    int strand = v[0] ^ v[1];

    pthread_mutex_lock(&mutex);

    //FILE *file = OUTPUT;

    fprintf(fp, "%s\t%d\t%d\t%d\t%c\t%s\t%d\t%d\t%d\t%d\n", k->first, v[2], start1,
            end1, !strand ? '+' : '-' , k->second, v[3], start2, end2, strand);

    //fclose(file);

    pthread_mutex_unlock(&mutex);
}

void free_partial_GArray(GArray *array, int start, int end){
    for(int s = start; s < end; s++){
        free(g_array_index(array, Element *, s));
    }
}

void free_garray_of_pointers(GArray *array) {
    if (array == NULL)
        return;

    for (guint i = 0; i < array->len; i++) {
        Element *ptr = g_array_index(array, Element *, i);
        if (ptr != NULL)
            free(ptr); // Free individual pointer
    }

    g_array_free(array, TRUE); // Free the GArray itself
}

void free_garray_duo_int(GArray *array) {
    if (array == NULL) {
        return;
    }

    for (guint i = 0; i < array->len; i++) {
        Duo_int *element = g_array_index(array, Duo_int *, i);
        free(element);
    }

    g_array_free(array, TRUE);
}

void free_garray_string(GArray *array) {
    if (array == NULL) {
        return;
    }

    for (guint i = 0; i < array->len; i++) {
        char *element = g_array_index(array, char *, i);
        free(element);
    }

    g_array_free(array, TRUE);
}

void free_key_value(gpointer key, gpointer value, gpointer user_data) {
    free(key);
    free(value);
}

void free_key_occurrences(gpointer key, gpointer value, gpointer user_data) {
    free((char *)key);
    free_garray_duo_int(value);
}

void free_key_overlaps(gpointer key, gpointer value, gpointer user_data) {
    Duo_char *dd = (Duo_char *)key;
    free(dd->first);
    free(dd->second);
    free(dd);
    free(value);
}

void calculate_usage(struct rusage *usage){
    if (getrusage(RUSAGE_SELF, usage)) {
        fprintf(stderr, "Failed to get memory usage.\n");
        exit(1);
    }
}
