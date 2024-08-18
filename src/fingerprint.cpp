#include <future>
#include <thread>
#include <iostream>
#include <sstream>
#include <tuple>
#include <fstream>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <getopt.h> 
#include <vector>
#include <atomic>

#include "fingerprint_utils.h"

using namespace std;

// Funzione per il calcolo parallelo delle impronte digitali
void calculate_fingerprints_parallel(Args args) {

    string input_fasta = args.path + args.fasta;

    // Numero di thread da utilizzare
    const int num_threads = args.n;
    vector<thread> threads;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(extract_long_reads, args, input_fasta, i);
    }

    // Attende la terminazione dei thread
    for (int i = 0; i < num_threads; ++i) {
        threads[i].join();
    }
}

    

int main(int argc, char* argv[]) {
    // Analizza gli argomenti dalla riga di comando
    Args args;
    int opt;

    while ((opt = getopt(argc, argv, "p:f:a:fp:n:")) != -1) {
        switch (opt) {
            case 'p':
                args.path = optarg;
                break;
            case 'f':
                if (optarg != NULL && optarg[0] == 'p')
                    args.fingerprint = optarg;
                else
                    args.type_factorization = optarg;
                break;
            case 'a':
                args.fasta = optarg;
                break;
            case 'n':
                args.n = stoi(optarg);
                break;
            default:
                std::cerr << "Uso: " << argv[0] << " [-p path] [-f type_factorization] [-a fasta] [-fp fingerprint] [-n n] " << std::endl;
                exit(EXIT_FAILURE);
        }
    }

    //esegui l'esperimento fingerprint per long reads
        calculate_fingerprints_parallel(args);

    return 0;
}
