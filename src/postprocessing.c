#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

// Function to calculate Hamming distance
int hamming_distance(const char *string1, const char *string2, int length, int start1, int start2) {
    int distance = 0;
    for (int i = 0; i < length; i++) {
        if (string1[start1 + i] != string2[start2 + i]) {
            distance++;
        }
    }
    return distance;
}

// Function to calculate Hamming distance
int hamming_distance_rc(const char *string1, const char *string2, int length, int start1, int end2) {
    int distance = 0;
    for (int i = 0; i < length; i++) {

        char nt;
        switch (string2[end2 - i - 1]) {
            case 'A':
                nt = 'T';
                break;
            case 'T':
                nt = 'A';
                break;
            case 'C':
                nt = 'G';
                break;
            case 'G':
                nt = 'C';
                break;
            default:
                nt = 0;
                break;
        }

        if (string1[start1 + i] != nt) {
            distance++;
        }
    }
    return distance;
}

int main(int argc, char *argv[]) {
    FILE *file;
    char *filename = argv[1];
    char *file_seq = NULL;
    size_t len = 0;
    ssize_t read;
    char line[1024];

    // Create a GHashTable to store sequences
    GHashTable *seq_table = g_hash_table_new(g_str_hash, g_str_equal);

    // Read the entire file content into memory
    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open file %s\n", filename);
        return 1;
    }

    char *key = NULL;
    while ((read = getline(&file_seq, &len, file)) != -1) {
        if (file_seq[0] == '>') {
            // The current line is a header, so store the sequence ID
            key = strdup(strtok(file_seq + 1, " \n"));  // skip '>' and remove newline
        } else {
            // The current line is a sequence, store it in the hash table
            char *value = strdup(strtok(file_seq, "\n"));
            g_hash_table_insert(seq_table, key, value);
        }
    }
    fclose(file);
    free(file_seq);

    // Process each line from stdin
    while (fgets(line, sizeof(line), stdin)) {
        char *r[11];
        char *token = strtok(line, "\t");
        int i = 0;

        while (token != NULL) {
            r[i++] = token;
            token = strtok(NULL, "\t");
        }

        char *read1 = r[0];
        char *read2 = r[5];
        int start1 = atoi(r[2]);
        int end1 = atoi(r[3]);
        int start2 = atoi(r[7]);
        int end2 = atoi(r[8]);
        int len1 = end1 - start1;
        int len2 = end2 - start2;
        char strand = r[4][0];
        int l2 = atoi(r[6]);
        int at = atoi(r[10]);

        // Retrieve sequences from the hash table
        char *seq1 = g_hash_table_lookup(seq_table, read1);
        char *seq2 = g_hash_table_lookup(seq_table, read2);

        int dis;

        if(strand == '-')
            dis = hamming_distance_rc(seq1, seq2, at, start1, end2);
        else
            dis = hamming_distance(seq1, seq2, at, start1, start2);

        int adjusted_score = at - dis;

        // Print the output line
        printf("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%d\t%d\t0\n",
               r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7], r[8], adjusted_score, at);

    }

    // Clean up memory
    g_hash_table_destroy(seq_table);

    return 0;
}
