#include "alg3.h"

GArray* get_k_fingers(char *line, char **read_id){
    GArray *array = g_array_new(FALSE, FALSE, sizeof(int));

    char *p = strtok(line, " "); //skip name
    char *read = malloc(sizeof(char) * strlen(p) + 1);
    strcpy(read, p);
    *read_id = read;
    p = strtok(NULL, " ");

    while(p != NULL){
        if(strcmp(p,"|") != 0 && is_numeric(p)){
            int num = atoi(p);
            g_array_append_val(array, num);
        }

        p = strtok(NULL, " ");
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
        offset += chars_written;
        if (x < i + k - 1) {
            // Append separator if not the last element
            strcpy(result + offset, separator);
            offset += separator_length;
        }
    }

    return result;
}

GArray* alg3(GArray* fingerprint, int w, int k, int (*phi)(GArray *array, int i, int k),
             void insertt(GArray *array, GQueue *queue, Element *X, int (*phi)(GArray *array, int i, int k), int k),
             int n) {
    GArray *rfinger;

    rfinger = g_array_new(0, 0, sizeof(Element *));

    GQueue *queue;

    queue = g_queue_new();

    int length = 0;

    for (int x = 0; x < n - k; x++) {
        Element *el = malloc(sizeof(Element));

        int k_finger = g_array_index(fingerprint, int, x);

        length += k_finger;

        el->value = x;
        el->fingerprint = NULL;
        el->k_finger = supporting_length(fingerprint, x, k);
        el->index_offset = length - k_finger;

        insertt(fingerprint, queue, el, supporting_length, k);
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
        length += g_array_index(fingerprint, int, x);
    }


    while (!g_queue_is_empty(queue)) {
        Element *data = (Element *)g_queue_pop_head(queue);
        if(data->fingerprint == NULL)
            free(data);
    }

    g_queue_free(queue);

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

            free(val);
        }
        g_array_free(arr, TRUE);
    }

    return hash_table;
}

gboolean filter_hash_table(gpointer key, gpointer value, gpointer user_data) {
    GArray *val = (GArray *)value;
    if(val->len > 1 && (MAX_K_FINGER_OCCURRENCE == -1 || val->len <= MAX_K_FINGER_OCCURRENCE))
        return FALSE;

    free_key_occurrences(key, value, NULL);

    return TRUE;
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

            Tuple_int *curr = malloc(sizeof(Tuple_int));
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

        //free(first_occ);
    }

    //g_array_free(occ_list, TRUE);

    //free((char *)key);
}

GHashTable *compute_matches(GHashTable *k_finger_occurrences){
    GHashTable *matches_dict = g_hash_table_new(tuple_int_hash, tuple_int_equal);

    g_hash_table_foreach(k_finger_occurrences, iterate_occurrence, matches_dict);

    return matches_dict;
}

void iterate_overlap(gpointer key, gpointer value, gpointer user_data) {
    User_data *ud = (User_data *)user_data;
    GHashTable *overlap_dict = ud->overlap_dict;
    GArray *lenghts = ud->lenghts;
    GArray *read_ids = ud->read_ids;

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
                    Duo_char *dd = malloc(sizeof(Duo_char));

                    char *r1  = g_array_index(read_ids, char *, f_index->first);
                    int f_len = strlen(r1);
                    dd->first = substring(r1, 0, f_len-3);     //remove last two chars blahblah_0

                    char *r2  = g_array_index(read_ids, char *, f_index->second);
                    int s_len = strlen(r2);
                    dd->second = substring(r2, 0, s_len-3);     //remove last two chars blahblah_0

                    int* val = g_hash_table_lookup(overlap_dict, dd);

                    if(val == NULL){
                        val = malloc(sizeof(int)*9);
                        val[0] = r1[f_len-1] - '0';
                        val[1] = r2[s_len-1] - '0';
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
                            val[0] = r1[f_len-1] - '0';
                            val[1] = r2[s_len-1] - '0';
                            val[2] = read1_length;
                            val[3] = read2_length;
                            val[4] = start_ov1;
                            val[5] = end_ov1;
                            val[6] = start_ov2;
                            val[7] = end_ov2;
                            val[8] = ov_length;
                        }

                        free(dd->first);
                        free(dd->second);
                        free(dd);
                    }
                }
            }
        }
    }

}

GHashTable *compute_overlaps(GHashTable *k_finger_occurrences, GArray *lenghts, GArray *read_ids){
    GHashTable *overlap_dict = g_hash_table_new(duo_char_hash, duo_char_equal);

    User_data ud = {overlap_dict, lenghts, read_ids};

    g_hash_table_foreach(k_finger_occurrences, iterate_overlap, &ud);

    return overlap_dict;
}

int main(void){
    struct rusage usage;
    double convert = 1e-6;
    FILE *fp;
    FILE *output = OUTPUT;
    ssize_t read = 0;
    char *line = NULL;
    size_t len = 0;

    clock_t begin_total = clock();

    fp = INPUT;

    if (fp == NULL){
        printf("Error Reading File\n");
        exit(1);
    }

    GArray *minimizers = g_array_new(FALSE, FALSE, sizeof(GArray*));

    GArray *lengths = g_array_new(FALSE, FALSE, sizeof(int));

    GArray *read_ids = g_array_new(FALSE, FALSE, sizeof(char *));

    clock_t begin = clock();

    while ((read = getline(&line, &len, fp)) != -1) {
        char *read_id;

        GArray *array = get_k_fingers(line, &read_id);

        GArray* res = alg3(array, W, K, supporting_length, insertLex, array->len);

        g_array_append_val(read_ids, read_id);

        int le = g_array_index(array, int, array->len-1);

        g_array_append_val(lengths, le);

        g_array_append_val(minimizers, res);

        g_array_free(array, TRUE);
    }

    fclose(fp);

    clock_t end = clock();

    calculate_usage(&usage);

    printf("Number of Reads %d, Minimizers calculated in %f s, Memory: %.2g GB \n", minimizers->len, (double)(end - begin) / CLOCKS_PER_SEC, usage.ru_maxrss*convert);

    /* GArray* first = g_array_index(minimizers, GArray *, 0); */
    /* GArray* second = g_array_index(minimizers, GArray *, 1); */
    /* GArray* third = g_array_index(minimizers, GArray *, 2); */
    /* GArray* fourth = g_array_index(minimizers, GArray *, 3); */
    /* GArray* fifth = g_array_index(minimizers, GArray *, 4); */
    /* GArray* sixth = g_array_index(minimizers, GArray *, 5); */
    /* GArray* s7 = g_array_index(minimizers, GArray *, 6); */
    /* GArray* s8 = g_array_index(minimizers, GArray *, 7); */
    /* GArray* s9 = g_array_index(minimizers, GArray *, 8); */
    /* GArray* s10 = g_array_index(minimizers, GArray *, 9); */

    /* printf("%d %d %d %d %d %d %d %d %d %d\n", first->len, second->len, third->len, fourth->len, fifth->len, */
    /*        sixth->len,s7->len,s8->len,s9->len,s10->len); */

    /* print_Element(g_array_index(second, Element *, 0)); */
    /* puts(""); */

    // DICTIONARY

    GHashTable *k_finger_occurrences = compute_k_finger_occurrences(minimizers);

    g_array_free(minimizers, TRUE);

    //GArray *arr = g_hash_table_lookup(k_finger_occurrences, "3_1_3_14_19_3_1");

    //Eliminare le k-fingers che occorrono una volta sola nel set dei reads oppure che occorrono troppe volte.
    //max_k_finger_occurrence se valore -1 non controllare massimo

    calculate_usage(&usage);

    guint num_before = g_hash_table_size(k_finger_occurrences);
    printf("Number of keys in k_finger_occurrences before filtering table: %u, Memory: %.2g GB \n", num_before, usage.ru_maxrss*convert);

    // filter
    g_hash_table_foreach_remove(k_finger_occurrences, filter_hash_table, NULL);

    calculate_usage(&usage);

    guint num_after = g_hash_table_size(k_finger_occurrences);
    printf("Number of keys in k_finger_occurrences after filtering table: %u, Memory: %.2g GB \n", num_after, usage.ru_maxrss*convert);

    // Dizionari delle leftmost e rightmost k-fingers comuni

    GHashTable *matches_dict = compute_matches(k_finger_occurrences);

    g_hash_table_foreach(k_finger_occurrences, free_key_occurrences, NULL);
    g_hash_table_destroy(k_finger_occurrences);

    /* Duo_int s = {55, 161516}; */

    /* print_offset_struct(g_hash_table_lookup(matches_dict, &s)); */

    calculate_usage(&usage);

    guint num_keys = g_hash_table_size(matches_dict);
    printf("\nNumber of MATCHES in the hash table: %u, Memory: %.2g GB \n", num_keys, usage.ru_maxrss*convert);

    //Calcolo Regioni Comuni

    GHashTable *overlap_dict = compute_overlaps(matches_dict, lengths, read_ids);

    g_hash_table_foreach(matches_dict, free_key_value, NULL);
    g_hash_table_destroy(matches_dict);

    g_array_free(lengths, TRUE);
    free_garray_string(read_ids);

    calculate_usage(&usage);

    guint overlap_keys = g_hash_table_size(overlap_dict);
    printf("Number of OVERLAPS in the hash table: %u, Memory: %.2g GB \n", overlap_keys, usage.ru_maxrss*convert);

    /* Duo_int ss = {0, 7794}; */

    /* int *be = g_hash_table_lookup(overlap_dict, &ss); */

    /* printf("%d %d %d %d %d %d %d %d %d", be[0], be[1], be[2], be[3], be[4], be[5], be[6], be[7], be[8]); */

    // WRITE PAF FILE

    write_PAF(overlap_dict, print_PAF_minimap, output);

    clock_t end_total = clock();

    calculate_usage(&usage);

    printf("\nOverall time %f s\n", (double)(end_total - begin_total) / CLOCKS_PER_SEC);

    g_hash_table_foreach(overlap_dict, free_key_overlaps, NULL);
    g_hash_table_destroy(overlap_dict);

    return 0;
}
