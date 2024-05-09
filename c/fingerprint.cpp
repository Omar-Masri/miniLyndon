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
#include <getopt.h> // Per l'analisi degli argomenti dalla riga di comando in C++
#include <vector>
#include <atomic>

#include "fingerprint_utils.h"

using namespace std;

// Funzione per il calcolo parallelo delle impronte digitali
void calculate_fingerprints_parallel(Args args, string name_file) {
    // Numero di thread da utilizzare
    const int num_threads = args.n; // Modifica questo valore se necessario

    // Creazione dei thread
    vector<thread> threads;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(extract_long_reads, args, name_file, i);
    }

    // Attende la terminazione dei thread
    for (int i = 0; i < num_threads; ++i) {
        threads[i].join();
    }
}


// Funzione per eseguire il calcolo delle impronte digitali per le letture lunghe
void experiment_fingerprint_long_reads_step(Args args) {

    // Costruisce il percorso del file di input FASTA combinando il percorso args.path e il nome del file args.fasta.
    string input_fasta = args.path + args.fasta;

    // Stampa delle informazioni sull'avvio del calcolo delle impronte digitali
    cout << "\nCompute fingerprint by list (" << args.type_factorization << ", " << args.fact << ") - start..." << endl;

    ifstream file(input_fasta); // Apre il file FASTA

    int lines_size = count(istreambuf_iterator<char>(file), istreambuf_iterator<char>(), '\n');

    file.close();

    // Stampa la dimensione delle letture estratte.
    cout << "read_lines SIZE: " << lines_size << endl;

    std::streambuf *coutbuf = std::cout.rdbuf();
    
    // Calcola le impronte digitali in parallelo
    calculate_fingerprints_parallel(args, input_fasta);

    std::cout.rdbuf(coutbuf);

    file.close();

    // Stampa delle informazioni sul termine del calcolo delle impronte digitali
    cout << "\nCompute fingerprint by list (" << args.type_factorization << ", " << args.fact << ") - stop!" << endl;
}

void experiment_fingerprint_long_reads_step_minimizer(Args args) {

    // Costruisce il percorso del file di input FASTA combinando il percorso args.path e il nome del file args.fasta.
    string input_fasta = args.path + args.fasta;

    // Calcola le impronte digitali in parallelo
    calculate_fingerprints_parallel(args, input_fasta);
}
        

int main(int argc, char* argv[]) {
    // Analizza gli argomenti dalla riga di comando
    Args args;
    int opt;
    while ((opt = getopt(argc, argv, "t:p:f:a:fp:n:d:")) != -1) {
        switch (opt) {
            case 't':
                args.type = optarg;
                break;
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
            case 'd':
                args.debug = optarg;
                std::transform(args.debug.begin(), args.debug.end(), args.debug.begin(), ::toupper);
                break;
            default:
                std::cerr << "Uso: " << argv[0] << " [-t type] [-p path] [-f type_factorization] [-a fasta] [-fp fingerprint] [-n n] [-d d]" << std::endl;
                exit(EXIT_FAILURE);
        }
    }

    // Se il tipo è 'long_reads', esegui l'esperimento fingerprint per long reads
    if (args.type == "long_reads") {
        if(args.debug == "YES"){
            experiment_fingerprint_long_reads_step_minimizer(args);
        }else{
            cout << "\nFingerprint long reads...\n";
            experiment_fingerprint_long_reads_step(args);
        }
    }

    return 0;
}
