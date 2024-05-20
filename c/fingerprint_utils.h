// File: fingerprint_utils.h

#ifndef FINGERPRINT_UTILS_H   
#define FINGERPRINT_UTILS_H 

#include <vector>
#include <tuple>
#include <string>

using namespace std;

// Definizione di una struttura per memorizzare gli argomenti
struct Args {
    string path = "training/";
    string type_factorization = "CFL";
    string fasta = "transcript_genes.fa";
    string fingerprint = "prova_fingerprint.txt";
    string fact = "create";
    string shift = "shift";
    string filter = "list";
    int n = 1;
};

void extract_long_reads(Args args, string name_file, int remainder);
string compute_long_fingerprint(string s, int T);

#endif // FINGERPRINT_UTILS_H
