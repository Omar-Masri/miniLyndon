#include "alg3.h"

void print_Element(Element *el){
    printf("Element: value %d, fingerprint: %s", el->value, el->fingerprint);
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

Element *fetch(GQueue *queue, int T) {
    while (!g_queue_is_empty(queue) && ((Element *)g_queue_peek_head(queue))->value < T) {
        Element *value = (Element *)g_queue_pop_head(queue);

        if(value->fingerprint == NULL)
            free(value);
    }
    return (Element *)g_queue_peek_head(queue);
}

void print_queue(GQueue *queue) {
    if (g_queue_is_empty(queue)) {
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

    if (array->len == 0) {
        printf("Array is empty\n");
        return;
    }

    printf("Array contents:\n");
    for (guint i = 0; i < array->len; i++) {
        Element *s = g_array_index(array, Element *, i);
        printf("%d ", s->value);
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

    for (int x = 0; x < n - k; x++) {
        Element *el = malloc(sizeof(Element *));
        el->value = x;
        el->fingerprint = NULL;

        insert(fingerprint, queue, el, supporting_length, k);
        //print_queue(queue);

        if (x >= w - 1) {
            Element *fetched = fetch(queue, x - w + 1);
            //printf("+++++ %d\n", rfinger->len);
            //fflush(stdout);

            if(!(rfinger->len) ||
               (g_array_index(rfinger, Element *, rfinger->len - 1))->value != fetched->value){

                if(supporting_length(fingerprint, fetched->value, k) > MIN_SUP_LENGTH){
                    char *result = k_finger_to_string(fingerprint, fetched->value, k, "_");
                    fetched->fingerprint = result;

                    g_array_append_val(rfinger, fetched);
                }
            }
        }
    }

    free(queue);

    return rfinger;

}

int main(void){
    FILE *fp;
    ssize_t read;
    char *line;
    size_t len;

    fp = fopen("../../Data/fingerprint_CFL_ICFL_COMB-30_s_300_noerr.txt", "r");

    if (fp == NULL){
        printf("Error Reading File\n");
        exit(1);
    }

    GArray *minimizers = g_array_new(FALSE, FALSE, sizeof(GArray*));

    clock_t begin = clock();

    while ((read = getline(&line, &len, fp)) != -1) {
        GArray *array = get_k_fingers(line);

        GArray* res = alg3(array, W, K, supporting_length, array->len);

        g_array_append_val(minimizers, res);

        free(array);
    }

    fclose(fp);

    clock_t end = clock();

    printf("Number of Reads %d, Minimizers calculated in %f s\n", minimizers->len, (double)(end - begin) / CLOCKS_PER_SEC);

    GArray* first = g_array_index(minimizers, GArray *, 0);
    GArray* second = g_array_index(minimizers, GArray *, 1);
    GArray* third = g_array_index(minimizers, GArray *, 2);
    GArray* fourth = g_array_index(minimizers, GArray *, 3);
    GArray* fifth = g_array_index(minimizers, GArray *, 4);

    printf("%d %d %d %d %d\n", first->len, second->len, third->len, fourth->len, fifth->len);

    print_Element(g_array_index(second, Element *, 0));

}
