#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define K 7
#define W 15

typedef struct {
    int value;
    int* fingerprint;
} Element;

int phi(int *pointer, int k){
    int val = 0;

    for(int x = 0; x<k; x++){
        val += *(pointer + x);
    }
    return val;
}

void insert(GQueue *queue, Element *X, int (*phi)(int*, int)) {
    int a,b;

    if(!g_queue_is_empty(queue)){
            GList *current = g_queue_peek_tail_link(queue);
            Element *s = (Element *)current->data;
            a = phi(s->fingerprint, K);
            b = phi(X->fingerprint, K);
    }

    while (!g_queue_is_empty(queue) && phi(((Element *)(g_queue_peek_tail_link(queue)->data))->fingerprint, K)
           > phi(X->fingerprint, K)) {
        g_queue_pop_tail(queue);
    }
    g_queue_push_tail(queue, X);
}

Element *fetch(GQueue *queue, int T) {
    while (!g_queue_is_empty(queue) && ((Element *)g_queue_peek_head(queue))->value < T) {
        g_queue_pop_head(queue);
    }
    return (Element *)g_queue_peek_head(queue);
}

void print_queue(GQueue *queue) {
    // Check if the queue is empty
    if (g_queue_is_empty(queue)) {
        printf("Queue is empty\n");
        return;
    }

    // Print the contents of the queue
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
    // Check if the array is empty
    if (array->len == 0) {
        printf("Array is empty\n");
        return;
    }

    // Print the contents of the array
    printf("Array contents:\n");
    for (guint i = 0; i < array->len; i++) {
        Element *s = g_array_index(array, Element *, i);
        printf("%d ", s->value);
    }
    printf("\n");
}


GArray* alg3(int* fingerprint, int w, int k, int (*phi)(int*, int), int n) {
    GArray *rfinger;

    rfinger = g_array_new(0, 0, sizeof(Element *));

    GQueue *queue;

    queue = g_queue_new();

    for (int x = 0; x < n - k; x++) {
        Element *el = malloc(sizeof(Element *));
        el->value = x;
        el->fingerprint = fingerprint + x;

        insert(queue, el, phi);
        //print_queue(queue);

        if (x >= w - 1) {
            Element *fetched = fetch(queue, x - w + 1);
            //printf("+++++ %d\n", rfinger->len);
            //fflush(stdout);



            if(!(rfinger->len) || (g_array_index(rfinger, Element *, rfinger->len - 1))->value != fetched->value){
                g_array_append_val(rfinger, fetched);
            }
        }
    }

    free(queue);

    return rfinger;

}


int main(void){
    FILE *fp;

    fp = fopen("../Data/prova.txt", "r");

    //read file into array
    int arr[1999999];
    int i;

    if (fp == NULL){
        printf("Error Reading File\n");
        exit(0);
    }

    for (i = 0; i < 1999999; i++){
        fscanf(fp, "%d,", &arr[i]);
    }


    clock_t begin = clock();

    GArray* res = alg3(arr, W, K, phi, sizeof(arr) / sizeof(arr[0]));

    clock_t end = clock();

    printf("result %d, in %f s\n", res->len, (double)(end - begin) / CLOCKS_PER_SEC);

    //print_array(res);
}
