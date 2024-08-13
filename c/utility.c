#include "utility.h"

extern pthread_mutex_t mutex;

char* substring(const char* str, int start, int end) {
    int len = strlen(str);

    if (start < 0 || end < start || end >= len)
        return NULL;

    int sub_len = end - start + 1;

    char* sub = (char*)malloc((sub_len + 1) * sizeof(char));
    if (sub == NULL)
        return NULL;

    strncpy(sub, str + start, sub_len);
    sub[sub_len] = '\0';

    return sub;
}

gboolean compare_arrays(GArray *array1, int s1, int s2, int k) {
    for (guint i = 0; i < k; i++) {
        int element1 = g_array_index(array1, int, s1+i);
        int element2 = g_array_index(array1, int, s2+i);

        if (element1 > element2)
            return 1;
        else if (element1 < element2)
            return -1;
    }

    return 0;
}

void print_Element(Element *el){
    fprintf(stderr,"Element: value %d, fingerprint: %lu", el->value, el->fingerprint);
}

void print_Occurrence(Occurrence *du){
    if(du == NULL)
        fprintf(stderr,"NULL");
    else
        fprintf(stderr,"(%d, %d, %d, %d)", du->first, du->second, du->third, du->fourth);
}

void print_offset_struct(offset_struct *of){
    if(of == NULL)
        fprintf(stderr,"NULL");
    else
        fprintf(stderr,"[%d, (%d, %d), %d, (%d, %d) %d, (%d, %d), %d, (%d, %d) | %d]",
               of->left_offset1, of->left_index_offset1, of->left_supp_length1,
               of->left_offset2, of->left_index_offset2, of->left_supp_length2,
               of->right_offset1,of->right_index_offset1, of->right_supp_length1,
               of->right_offset2,of->right_index_offset2, of->right_supp_length2,
               of->number);
}

bool is_numeric(const char *str) {
    if (str == NULL || *str == '\0')
        return false;

    for (int i = 0; str[i] != '\0'; i++) {
        if (!(str[i] >= '0' && str[i] <= '9'))
            return false;
    }

    return true;
}

unsigned int djb2(GArray *array, int i, int k)
{
	/* This is the djb2 string hash function */

	unsigned int result = 5381;
	int c;

	for(int x=i; x<i+k; x++){
        c = g_array_index(array, int, x);
		result = (result << 5) + result + c;
	}

	return result;
}

unsigned int djb2arr(int *array, int i, int k)
{
	/* This is the djb2 string hash function */

	unsigned int result = 5381;
	int c;

	for(int x=i; x<i+k; x++){
        c = array[x];
		result = (result << 5) + result + c;
	}

	return result;
}

/* D. J. Bernstein hash function */
unsigned long bernstein(GArray *array, int i, int k)
{
    unsigned long hash = 5381;
    int c;

    for(int x=i; x<i+k; x++){
        c = g_array_index(array, int, x);
        hash = 33 * hash ^ (unsigned char) c;
    }

    return hash;
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

        if(value->fingerprint == 0)
            free(value);
    }
    g_queue_push_tail(queue, X);
}

inline void insertLex(GArray *array, GQueue *queue, Element *X, int (*phi)(GArray *array, int i, int k), int k) {
    while (!g_queue_is_empty(queue)
           && compare_arrays(array, ((Element *)(g_queue_peek_tail_link(queue)->data))->value,
                      X->value, k) > 0) {

        Element *value = (Element *)g_queue_pop_tail(queue);

        if(value->fingerprint == 0)
            free(value);
    }
    g_queue_push_tail(queue, X);
}

Element *fetch(GQueue *queue, int T) {
    while (!g_queue_is_empty(queue) && ((Element *)g_queue_peek_head(queue))->value < T) {
        Element *value = (Element *)g_queue_pop_head(queue);

        if(value->fingerprint == 0)
            free(value);
    }
    return (Element *)g_queue_peek_head(queue);
}

void print_queue(GQueue *queue) {
    if (queue == NULL || g_queue_is_empty(queue)) {
        fprintf(stderr,"Queue is empty\n");
        return;
    }

    GList *current = queue->head;
    fprintf(stderr,"Queue contents: ");
    while (current != NULL) {
        Element *s = (Element *)current->data;
        fprintf(stderr,"%d ", s->value);
        current = current->next;
    }
    fprintf(stderr,"\n");
}

void print_array_Occurrence(GArray *array) {

    if (array == NULL || array->len == 0) {
        fprintf(stderr,"Array is empty\n");
        return;
    }

    fprintf(stderr,"Array contents:\n");
    for (guint i = 0; i < array->len; i++) {
        Occurrence *s = g_array_index(array, Occurrence *, i);
        fprintf(stderr,"(%d,%d)\n", s->first, s->second);
    }
    fprintf(stderr,"\n");
}

void print_array_Triple_fragment(GArray *array) {

    if (array == NULL || array->len == 0) {
        fprintf(stderr,"Array is empty\n");
        return;
    }

    fprintf(stderr,"Array contents:\n");
    for (guint i = 0; i < array->len; i++) {
        Triple_fragment *s = g_array_index(array, Triple_fragment *, i);
        fprintf(stderr,"(%d,%d,%d)\n", s->first, s->second->value, s->third->second);
    }
    fprintf(stderr,"\n");
}

static inline guint32 murmur_hash_32(uint32_t key) {
    key ^= key >> 16;
    key *= 0x85ebca6b;
    key ^= key >> 13;
    key *= 0xc2b2ae35;
    key ^= key >> 16;
    return key;
}

guint Occurrence_hash(gconstpointer key) {
    const Occurrence *point = (const Occurrence *)key;
    return murmur_hash_32(point->first)*33 ^ murmur_hash_32(point->second);    // mamma mia sto errore >:(
}

gboolean Occurrence_equal(gconstpointer a, gconstpointer b) {
    const Occurrence *point1 = (const Occurrence *)a;
    const Occurrence *point2 = (const Occurrence *)b;
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

void print_PAF_minimap(Duo_char *k, int *v, FILE *fp, GHashTable *set, bool rc){
    /*
    int start1 = v[4];
    int end1 = v[5];

    int start2 = v[6];
    int end2 = v[7];
    */

    if(v[0] == 1){
        int temp = v[4];
        v[4] = v[2] - v[5];
        v[5] = v[2] - temp;
    }

    if(v[1] == 1){
        int temp = v[6];
        v[6] = v[3] - v[7];
        v[7] = v[3] - temp;
    }

    int strand = v[0] ^ v[1];

    if(rc){
        int hash = djb2arr(v, 4, 4);
        if(g_hash_table_contains(set, GINT_TO_POINTER(hash)))
            return;
    }else{
        int hash = djb2arr(v, 4, 4);
        g_hash_table_insert(set, GINT_TO_POINTER(hash), NULL);
    }

    pthread_mutex_lock(&mutex);

    fprintf(fp, "%s\t%d\t%d\t%d\t%c\t%s\t%d\t%d\t%d\t%d\t%d\n", k->first, v[2], v[4],
            v[5], !strand ? '+' : '-' , k->second, v[3], v[6], v[7], v[8], v[8]);

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

void free_garray_Occurrence(GArray *array) {
    if (array == NULL) {
        return;
    }

    for (guint i = 0; i < array->len; i++) {
        Occurrence *element = g_array_index(array, Occurrence *, i);
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
    //free((char *)key);
    free_garray_Occurrence(value);
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

unsigned long read_off_memory_status()
{
  unsigned long dummy;
  const char* statm_path = "/proc/self/statm";
  unsigned long size,resident,share,text,lib,data,dt;

  FILE *f = fopen(statm_path,"r");
  if(!f){
    perror(statm_path);
    abort();
  }
  if(7 != fscanf(f,"%ld %ld %ld %ld %ld %ld %ld",
    &size,&resident,&share,&text,&lib,&data,&dt))
  {
    perror(statm_path);
    abort();
  }
  fclose(f);
  return size;
}
