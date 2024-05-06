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

// Definizione di una struttura per memorizzare gli argomenti
struct Args {
    string type = "1f_np";
    string path = "training/";
    string type_factorization = "CFL";
    string fasta = "transcript_genes.fa";
    string fingerprint = "prova_fingerprint.txt";
    string fact = "create";
    string shift = "shift";
    string filter = "list";
    string debug = "NO";
    int n = 1;
};

tuple<vector<string>, vector<string>> calculate_fingerprint(vector<string> line, const Args& args) {

    return compute_long_fingerprint_by_list(args.fact, 30, line);
}


// Funzione per il calcolo parallelo delle impronte digitali
void calculate_fingerprints_parallel(const Args& args, const vector<vector<string>>& splitted_lines,
                                     vector<string>& fingerprint_lines,
                                     vector<string>& fingerprint_fact_lines) {
    // Numero di thread da utilizzare
    const int num_threads = args.n; // Modifica questo valore se necessario
    
    // Funzione che esegue il calcolo delle impronte digitali per un sottoinsieme di righe
    auto calculate_fingerprints_subset = [&](vector<string> splitted_lines_partizioni) {
        return calculate_fingerprint(splitted_lines_partizioni, args);
        
    };

    vector<string> fingerprint_line;
    vector<string> fingerprint_fact_line;
    // Creazione dei thread
    vector<future<tuple<vector<string>, vector<string>>>> threads;

    for (int i = 0; i < num_threads; ++i) {
        threads.push_back(async(calculate_fingerprints_subset, splitted_lines[i]));        
    }

    // Attende la terminazione dei thread
    for (int i = 0; i < num_threads; ++i) { 
        tie(fingerprint_line, fingerprint_fact_line) = threads[i].get();
   
        // Recupera il risultato dal thread
        fingerprint_fact_lines.insert(fingerprint_fact_lines.end(), fingerprint_fact_line.begin(),fingerprint_fact_line.end() );
        fingerprint_lines.insert(fingerprint_lines.end(), fingerprint_line.begin(), fingerprint_line.end() );
    }
}


// Funzione per eseguire il calcolo delle impronte digitali per le letture lunghe
void experiment_fingerprint_long_reads_step(Args args) {

    bool flag = args.debug == "YES";

    // Costruisce il percorso del file di input FASTA combinando il percorso args.path e il nome del file args.fasta.
    string input_fasta = args.path + args.fasta;

    // Estrae le letture lunghe dal file FASTA+
    vector<string> read_lines = extract_long_reads(input_fasta);

    // Stampa la dimensione delle letture estratte.
    cout << "read_lines SIZE: " << read_lines.size() << endl;

    if (read_lines.empty()) {
        std::cout << "No reads extracted!" << endl;
        exit(-1);
    }
    
    // Stampa delle informazioni sull'avvio del calcolo delle impronte digitali
    cout << "\nCompute fingerprint by list (" << args.type_factorization << ", " << args.fact << ") - start..." << endl;
    
    // Prepara il file per scrivere le fingerprint
    ofstream fingerprint_file(args.path + "fingerprint_" + args.type_factorization + ".txt"); //viene aperto automaticamente quando viene istanziato l'oggetto
    ofstream fact_fingerprint_file;

    // Se richiesto, apertura del file per scrivere le fattorizzazioni
    if (args.fact == "create") {
        fact_fingerprint_file.open(args.path + "fact_fingerprint_" + args.type_factorization + ".txt");
    }

    // SPLIT Divide le letture in gruppi per l'elaborazione multiprocessore
    int size = read_lines.size() / args.n;
    int remainder = read_lines.size() % args.n;
    vector<vector<string>> splitted_lines;
    int e;
    for (int i = 0; i < read_lines.size(); i = e) {
        e = i + size;

        if(remainder){
            e++;
            remainder--;
        }

        vector<string>tmp(read_lines.begin() + i, read_lines.begin() + min(e, (int)read_lines.size()));
        splitted_lines.push_back(vector<string>(tmp));  //costruttore di vector accetta due iteratori che definiscono il range di elementi da copiare nel nuovo vettore.  +controllo secondo parametro che non esca dal range
    } 

    vector<string> fingerprint_lines;
    vector<string> fingerprint_fact_lines;
    
    // Calcola le impronte digitali in parallelo
    calculate_fingerprints_parallel(args, splitted_lines, fingerprint_lines, fingerprint_fact_lines);

    // Scrittura delle impronte digitali nel file corrispondente   
    for (const auto& line : fingerprint_lines) {
        fingerprint_file << line;
    }
    // Se richiesto, scrittura delle fattorizzazioni nel file corrispondente
    if (args.fact == "create") {
    
        for (const auto& line : fingerprint_fact_lines) {
            fact_fingerprint_file << line;
        }
    }

    fingerprint_file.close();

    if (args.fact == "create") {
        fact_fingerprint_file.close();
    }

    // Stampa delle informazioni sul termine del calcolo delle impronte digitali
    cout << "\nCompute fingerprint by list (" << args.type_factorization << ", " << args.fact << ") - stop!" << endl;
}

void experiment_fingerprint_long_reads_step_minimizer(Args args) {

    // Costruisce il percorso del file di input FASTA combinando il percorso args.path e il nome del file args.fasta.
    string input_fasta = args.path + args.fasta;

    // Estrae le letture lunghe dal file FASTA+
    vector<string> read_lines = extract_long_reads(input_fasta);

    if (read_lines.empty()) {
        //std::cout << "No reads extracted!" << endl;
        exit(-1);
    }

    // SPLIT Divide le letture in gruppi per l'elaborazione multiprocessore
    int size = read_lines.size() / args.n;
    int remainder = read_lines.size() % args.n;
    vector<vector<string>> splitted_lines;
    int e;
    for (int i = 0; i < read_lines.size(); i = e) {
        e = i + size;

        if(remainder){
            e++;
            remainder--;
        }

        vector<string>tmp(read_lines.begin() + i, read_lines.begin() + min(e, (int)read_lines.size()));
        splitted_lines.push_back(vector<string>(tmp));  //costruttore di vector accetta due iteratori che definiscono il range di elementi da copiare nel nuovo vettore.  +controllo secondo parametro che non esca dal range
    }

    vector<string> fingerprint_lines;
    vector<string> fingerprint_fact_lines;

    // Calcola le impronte digitali in parallelo
    calculate_fingerprints_parallel(args, splitted_lines, fingerprint_lines, fingerprint_fact_lines);

    // Scrittura delle impronte digitali nel file corrispondente
    for (const auto& line : fingerprint_lines) {
        cout << line;
    }
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
