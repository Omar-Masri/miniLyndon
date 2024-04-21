#include "alg3.h"
#include <stdio.h>
#include <stdlib.h>

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
           && strcmp(k_finger_to_string(array, ((Element *)(g_queue_peek_tail_link(queue)->data))->value, k, "_"),
                     k_finger_to_string(array, X->value, k, "_")) > 0) {

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

void print_array(GArray *array) {

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

GArray* get_k_fingers(char *line){
    GArray *array = g_array_new(FALSE, FALSE, sizeof(int));

    char *p = strtok(line, " "); //skip name
    p = strtok(NULL, " ");

    while(p != NULL){
        int num;

        if(strcmp(p,"|") != 0 && is_numeric(p)){
            num = atoi(p);
            g_array_append_val(array, num);

            p = strtok(NULL, " ");
        }
        else{
            p = strtok(NULL, " ");
            continue;
        }
    }

    return array;
}

char *k_finger_to_string(GArray *array, int i, int k, const char *separator) {

    if (array == NULL || separator == NULL) {
        return NULL;
    }

    size_t total_length = 0;
    int separator_length = strlen(separator);
    for (int x = i; x < i+k; x++) {
        int number_length = snprintf(NULL, 0, "%d", g_array_index(array, int, x));
        if (number_length < 0) {
            return NULL;
        }
        total_length += number_length+separator_length;
    }
    total_length -= separator_length;

    char *result = (char *)malloc(total_length + 1);
    if (result == NULL) {
        return NULL;
    }

    int offset = 0;
    for (int x = i; x < i+k; x++) {
        int chars_written = snprintf(result + offset, total_length - offset + 1, "%d",
                                     g_array_index(array, int, x));
        if (chars_written < 0 || chars_written > total_length - offset) {
            free(result);
            return NULL;
        }
        offset += chars_written; // Move offset
        if (x < i + k - 1) {
            // Append separator if not the last element
            strcpy(result + offset, separator);
            offset += separator_length;
        }
    }

    return result;
}

GArray* alg3(GArray* fingerprint, int w, int k, int (*phi)(GArray *array, int i, int k), int n) {
    GArray *rfinger;

    rfinger = g_array_new(0, 0, sizeof(Element *));

    GQueue *queue;

    queue = g_queue_new();

    int length = 0;

    for (int x = 0; x < n - k; x++) {
        Element *el = malloc(sizeof(Element *));

        int k_finger = g_array_index(fingerprint, int, x);

        length += k_finger;

        el->value = x;
        el->fingerprint = NULL;
        el->k_finger = supporting_length(fingerprint, x, k);
        el->index_offset = length - k_finger;

        insert(fingerprint, queue, el, supporting_length, k);
        //print_queue(queue);

        if (x >= w - 1) {
            Element *fetched = fetch(queue, x - w + 1);

            if(!(rfinger->len) ||
               (g_array_index(rfinger, Element *, rfinger->len - 1))->value != fetched->value){

                if(supporting_length(fingerprint, fetched->value, k) >= MIN_SUP_LENGTH){
                    char *result = k_finger_to_string(fingerprint, fetched->value, k, "_");
                    fetched->fingerprint = result;

                    g_array_append_val(rfinger, fetched);
                }
            }
        }
    }

    for(int x = n-k; x<n; x++){
        int k_finger = g_array_index(fingerprint, int, x);
        length += k_finger;
    }

    free(queue);

    g_array_append_val(fingerprint, length);

    return rfinger;

}

GHashTable *compute_k_finger_occurrences(GArray *fingerprint_list){
    GHashTable *hash_table = g_hash_table_new(g_str_hash, g_str_equal);

    for(int x=0; x<fingerprint_list->len ; x++){
        GArray* arr = g_array_index(fingerprint_list, GArray *, x);

        for(int y=0; y<arr->len ; y++){
            Element *val = g_array_index(arr, Element *, y);
            GArray *retrived = g_hash_table_lookup(hash_table, val->fingerprint);

            if(retrived == NULL)
                retrived = g_array_new(FALSE, FALSE, sizeof(Duo_int *));

            Duo_int *new = malloc(sizeof(Duo_int));
            new->first = x;
            new->second = val->value;
            new->third = val->index_offset;
            new->fourth = val->k_finger;

            g_array_append_val(retrived, new);

            g_hash_table_insert(hash_table, val->fingerprint, retrived);
        }
    }

    return hash_table;
}

gboolean filter_hash_table(gpointer key, gpointer value, gpointer user_data) {
    GArray *val = (GArray *)value;
    if(val->len > 1 && (MAX_K_FINGER_OCCURRENCE == -1 || val->len <= MAX_K_FINGER_OCCURRENCE))
        return FALSE;

    return TRUE;
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


void iterate_occurrence(gpointer key, gpointer value, gpointer user_data) {
    GHashTable *matches_dict = (GHashTable *)user_data;
    GArray *occ_list = (GArray *)value;

    for(int x=0; x<occ_list->len ; x++){
        Duo_int *first_occ = g_array_index(occ_list, Duo_int *, x);
        int f_index1 = first_occ->first;

        for(int y=x+1; y<occ_list->len ; y++){
            Duo_int *second_occ = g_array_index(occ_list, Duo_int *, y);
            int f_index2 = second_occ->first;

            if(f_index1 == f_index2)
                continue;

            //Duo_int curr = {f_index1, f_index2};

            Duo_int *curr = malloc(sizeof(Duo_int));
            curr->first = f_index1;
            curr->second = f_index2;

            offset_struct *current = g_hash_table_lookup(matches_dict, curr);

            if(current == NULL){
                current = malloc(sizeof(offset_struct));
                current->left_offset1 = first_occ->second;
                current->left_index_offset1 = first_occ->third;
                current->left_supp_length1 = first_occ->fourth;
                current->left_offset2 = second_occ->second;
                current->left_index_offset2 = second_occ->third;
                current->left_supp_length2 = second_occ->fourth;
                current->right_offset1 = first_occ->second;
                current->right_index_offset1 = first_occ->third;
                current->right_supp_length1 = first_occ->fourth;
                current->right_offset2 = second_occ->second;
                current->right_index_offset2 = second_occ->third;
                current->right_supp_length2 = second_occ->fourth;
                current->number = 1;

                g_hash_table_insert(matches_dict, curr, current);
            }else{
                if(current->left_offset1 > first_occ->second){

                    current->left_offset1 = first_occ->second;
                    current->left_index_offset1 = first_occ->third;
                    current->left_supp_length1 = first_occ->fourth;

                    current->left_offset2 = second_occ->second;
                    current->left_index_offset2 = second_occ->third;
                    current->left_supp_length2 = second_occ->fourth;

                }else if(current->left_offset1 == first_occ->second){
                    if(current->left_offset2 > second_occ->second){
                        current->left_offset2 = second_occ->second;
                        current->left_index_offset2 = second_occ->third;
                        current->left_supp_length2 = second_occ->fourth;
                    }
                }

                if(current->right_offset1 < first_occ->second){
                    current->right_offset1 = first_occ->second;
                    current->right_index_offset1 = first_occ->third;
                    current->right_supp_length1 = first_occ->fourth;

                    current->right_offset2 = second_occ->second;
                    current->right_index_offset2 = second_occ->third;
                    current->right_supp_length2 = second_occ->fourth;

                }else if(current->right_offset1 == first_occ->second){
                    if(current->right_offset2 < second_occ->second){
                        current->right_offset2 = second_occ->second;
                        current->right_index_offset2 = second_occ->third;
                        current->right_supp_length2 = second_occ->fourth;
                    }
                }

                current->number += 1;

                free(curr);
            }
        }
    }

}

GHashTable *compute_matches(GHashTable *k_finger_occurrences){
    GHashTable *matches_dict = g_hash_table_new(duo_int_hash, duo_int_equal); //send this trhoug user

    g_hash_table_foreach(k_finger_occurrences, iterate_occurrence, matches_dict);

    return matches_dict;
}

void iterate_overlap(gpointer key, gpointer value, gpointer user_data) {
    User_data *ud = (User_data *)user_data;
    GHashTable *overlap_dict = ud->overlap_dict;
    GArray *lenghts = ud->lenghts;

    Duo_int *f_index = (Duo_int *)key;
    offset_struct *current = (offset_struct *)value;

    if(MIN_SHARED_K_FINGERS == 1 || current->number >= MIN_SHARED_K_FINGERS){

        if(current->right_offset2 >= current->left_offset2){

            //length offset for left fingerprint
            int upstream_length1 = current->left_index_offset1;
            //length offset for right fingerprint
            int upstream_length2 = current->left_index_offset2;

            //supporting length left fingerprint
            int region_length1 = current->right_index_offset1 + current->right_supp_length1 - current->left_index_offset1;
            //supporting length right fingerprint
            int region_length2 = current->right_index_offset2 + current->right_supp_length2 - current->left_index_offset2;

            int read1_length = g_array_index(lenghts, int, f_index->first);
            int read2_length = g_array_index(lenghts, int, f_index->second);

            int min_cov_number = (int)((MIN_REGION_K_FINGER_COVERAGE * min(region_length1,region_length2)) / MIN_SUP_LENGTH);
            min_cov_number = min(min_cov_number, 15);

            if (current->number >= min_cov_number  &&
                (abs(region_length1-region_length2) <= MAX_DIFF_REGION_PERCENTAGE * max(region_length1,region_length2)
                 && max(region_length1,region_length2) >= MIN_REGION_LENGTH)){

                int min_up = min(upstream_length1,upstream_length2);

                int start_ov1 = upstream_length1 - min_up;
                int start_ov2 = upstream_length2 - min_up;

                int min_down = min(read1_length-(upstream_length1+region_length1), read2_length-(upstream_length2+region_length2));

                int end_ov1 = upstream_length1 + region_length1 + min_down;
                int end_ov2 = upstream_length2 + region_length2 + min_down;

                int ov_length = min(end_ov1-start_ov1, end_ov2-start_ov2);

                if (min(region_length1,region_length2) >= MIN_OVERLAP_COVERAGE * ov_length && ov_length >= MIN_OVERLAP_LENGTH){
                    Duo_int *dd = malloc(sizeof(Duo_int));
                    dd->first  = (f_index->first / 2);  //read_ids
                    dd->second = (f_index->second / 2);

                    int* val = g_hash_table_lookup(overlap_dict, dd);

                    if(val == NULL){
                        val = malloc(sizeof(int)*9);
                        val[0] = (f_index->first + 1) % 2;
                        val[1] = (f_index->second + 1) % 2;
                        val[2] = read1_length;
                        val[3] = read2_length;
                        val[4] = start_ov1;
                        val[5] = end_ov1;
                        val[6] = start_ov2;
                        val[7] = end_ov2;
                        val[8] = ov_length;

                        g_hash_table_insert(overlap_dict, dd, val);
                    } else{
                        if(ov_length > val[8]){
                            val[0] = (f_index->first + 1) % 2;
                            val[1] = (f_index->second + 1) % 2;
                            val[2] = read1_length;
                            val[3] = read2_length;
                            val[4] = start_ov1;
                            val[5] = end_ov1;
                            val[6] = start_ov2;
                            val[7] = end_ov2;
                            val[8] = ov_length;
                        }

                        free(dd);
                    }
                }
            }
        }
    }

}

GHashTable *compute_overlaps(GHashTable *k_finger_occurrences, GArray *lenghts){
    GHashTable *overlap_dict = g_hash_table_new(duo_int_hash, duo_int_equal); //send this trhoug user

    User_data ud = {overlap_dict, lenghts};

    g_hash_table_foreach(k_finger_occurrences, iterate_overlap, &ud);

    return overlap_dict;
}

void print_PAF(gpointer key, gpointer value, gpointer user_data){
    FILE *fp = (FILE *)user_data;
    int * v = (int *)value;
    Duo_int *k = (Duo_int *)key;

    int start1 = v[4];
    int end1 = v[5];

    int start2 = v[6];
    int end2 = v[7];

    if(v[0] == 0){
        int temp = start1;
        start1 = v[2] - end1;
        end1 = v[2] - temp;
    }

    if(v[1] == 0){
        int temp = start2;
        start2 = v[3] - end2;
        end2 = v[3] - temp;
    }

    int strand = v[0] ^ v[1];

    fprintf(fp, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", k->first, v[2], start1, end1, k->second, v[3], start2, end2, strand);
}

void write_PAF(GHashTable *overlap_dict, FILE *fp){
    g_hash_table_foreach(overlap_dict, print_PAF, fp);
}

int main(void){
    FILE *fp;
    FILE *output = OUTPUT;
    ssize_t read;
    char *line;
    size_t len;

    clock_t begin_total = clock();

    fp = INPUT;

    if (fp == NULL){
        printf("Error Reading File\n");
        exit(1);
    }

    GArray *minimizers = g_array_new(FALSE, FALSE, sizeof(GArray*));

    GArray *lengths = g_array_new(FALSE, FALSE, sizeof(int));

    clock_t begin = clock();

    while ((read = getline(&line, &len, fp)) != -1) {
        GArray *array = get_k_fingers(line);

        GArray* res = alg3(array, W, K, supporting_length, array->len);

        int le = g_array_index(array, int, array->len-1);

        g_array_append_val(lengths, le);

        g_array_append_val(minimizers, res);

        free(array);
    }

    fclose(fp);

    clock_t end = clock();

    printf("Number of Reads %d, Minimizers calculated in %f s\n", minimizers->len, (double)(end - begin) / CLOCKS_PER_SEC);

    /* GArray* first = g_array_index(minimizers, GArray *, 0); */
    /* GArray* second = g_array_index(minimizers, GArray *, 1); */
    /* GArray* third = g_array_index(minimizers, GArray *, 2); */
    /* GArray* fourth = g_array_index(minimizers, GArray *, 3); */
    /* GArray* fifth = g_array_index(minimizers, GArray *, 4); */

    /* printf("%d %d %d %d %d\n", first->len, second->len, third->len, fourth->len, fifth->len); */

    /* print_Element(g_array_index(second, Element *, 0)); */
    /* puts(""); */

    // DICTIONARY

    GHashTable *k_finger_occurrences = compute_k_finger_occurrences(minimizers);

    GArray *arr = g_hash_table_lookup(k_finger_occurrences, "3_1_3_14_19_3_1");

    //Eliminare le k-fingers che occorrono una volta sola nel set dei reads oppure che occorrono troppe volte.
    //max_k_finger_occurrence se valore -1 non controllare massimo

    guint num_before = g_hash_table_size(k_finger_occurrences);
    printf("Number of keys in k_finger_occurrences before filtering table: %u\n", num_before);

    // filter
    g_hash_table_foreach_remove(k_finger_occurrences, filter_hash_table, NULL);

    guint num_after = g_hash_table_size(k_finger_occurrences);
    printf("Number of keys in k_finger_occurrences after filtering table: %u\n", num_after);

    // Dizionari delle leftmost e rightmost k-fingers comuni

    GHashTable *matches_dict = compute_matches(k_finger_occurrences);

    /* Duo_int s = {447, 2837}; */

    /* print_offset_struct(g_hash_table_lookup(matches_dict, &s)); */

    guint num_keys = g_hash_table_size(matches_dict);
    printf("\nNumber of MATCHES in the hash table: %u\n", num_keys);

    //Calcolo Regioni Comuni

    GHashTable *overlap_dict = compute_overlaps(matches_dict, lengths);

    guint overlap_keys = g_hash_table_size(overlap_dict);
    printf("Number of OVERLAPS in the hash table: %u\n", overlap_keys);

    /* Duo_int ss = {0, 7794}; */

    /* int *be = g_hash_table_lookup(overlap_dict, &ss); */

    /* printf("%d %d %d %d %d %d %d %d %d", be[0], be[1], be[2], be[3], be[4], be[5], be[6], be[7], be[8]); */

    // WRITE PAF FILE

    write_PAF(overlap_dict, output);

    clock_t end_total = clock();
    printf("\nOverall time %f s\n", (double)(end_total - begin_total) / CLOCKS_PER_SEC);
}


//implementa read_ids !!!
// TODO RICORDA DUO_INT E RIFALLO
