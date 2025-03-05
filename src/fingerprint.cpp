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

    bool has_path = false, has_type = false, has_fasta = false, has_n = false;

    while ((opt = getopt(argc, argv, "p:f:a:n:s:cF:t:h:r:m:")) != -1) {
        switch (opt) {
            case 'p':
                args.path = optarg;
                has_path = true;
                break;
            case 'f': {
                std::vector<std::string> valid_types = {"CFL", "ICFL", "CFL_ICFL", "CFL_ICFL_R"};
                std::string opt_value = optarg;

                bool is_valid = false;
                for (const auto& type : valid_types) {
                    if (opt_value == type) {
                        is_valid = true;
                        break;
                    }
                }

                if (is_valid) {
                    args.type_factorization = opt_value;
                    has_type = true;
                } else {
                    std::cerr << "Invalid -f option value. Must be one of: CFL, ICFL, CFL_ICFL, CFL_ICFL_R" << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;
            }
            case 'a':
                args.fasta = optarg;
                has_fasta = true;
                break;
            case 'n':
                args.n = std::stoi(optarg);
                has_n = true;
                break;
            case 's':
                args.segment_size = std::stoi(optarg);
                break;
            case 'c':
                args.comb = true;
                break;
            case 'F':
                args.fingerprint = optarg;
                break;
            case 't':
                args.fact = optarg;
                break;
            case 'h':
                args.shift = optarg;
                break;
            case 'r':
                args.filter = optarg;
                break;
            case 'm':
                args.cfl_max = std::stoi(optarg);
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " [OPTIONS]\n"
                          << "\n"
                          << "Options:\n"
                          << "  -p <path>                  (Required) Set the path\n"
                          << "  -f <type_factorization>    (Required) Set the type of factorization. Valid values are:\n"
                          << "                               CFL, ICFL, CFL_ICFL, CFL_ICFL_R\n"
                          << "  -a <fasta>                 (Required) Provide a FASTA file path\n"
                          << "  -n <n>                     (Required) Set the value for 'n' (integer)\n"
                          << "  -s <segment_size>          (Optional) Set the segment size (integer)\n"
                          << "  -c                         (Optional) Enable 'comb' flag\n"
                          << "  -F <fingerprint>           (Optional) Set the fingerprint file\n"
                          << "  -t <factorization>         (Optional) Set the factorization value\n"
                          << "  -h <shift>                 (Optional) Set the shift value\n"
                          << "  -r <filter>                (Optional) Set the filter value\n"
                          << "  -m <cfl_max>               (Optional) Set CFL max size (integer)\n"
                          << std::endl;
                exit(EXIT_FAILURE);
        }
    }

    // Check if all required arguments were provided
    if (!has_path || !has_type || !has_fasta || !has_n) {
        std::cerr << "Error: Missing required arguments!\n"
                  << "Required options: -p <path>, -f <type_factorization>, -a <fasta>, -n <n>\n"
                  << "Use -h for help.\n";
        exit(EXIT_FAILURE);
    }

    //esegui l'esperimento fingerprint per long reads
    calculate_fingerprints_parallel(args);

    std:cerr << "Factorization ended" << endl;

    return 0;
}
