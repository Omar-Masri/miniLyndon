#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <algorithm>
#include <mutex>

#include "fingerprint_utils.h"
#include "factorization_comb.h"

using namespace std;

mutex mtx;

tuple<string, string> manipulate_read(string l_1, string l_2){
        string read_original = ""; //variabile per memorizzare la sequenza originale
        string read_rc = ""; //Variabile per memorizzare la sequenza R&C
        string id_gene = "";

        l_1.erase(l_1.begin()); // Rimuovi il carattere '>' dall'ID_GENE
        id_gene = l_1.substr(0, l_1.find(' '));

        read_original += id_gene + "_0 "; 
        read_rc += id_gene + "_1 "; 

        transform(l_2.begin(), l_2.end(), l_2.begin(), ::toupper);

        read_original += l_2;  //concatena ID e sequenza originale
        read_rc += reverse_complement(l_2) + "\n"; // concatena ID e sequenza reverse&complement
        read_original += "\n";

        return make_tuple(read_original, read_rc);
}



//Suddivide le lunghe letture in sottolunghezze
vector<string> substring_read(const string& str, int size = 300) {    
    vector<string> list_of_factors; // Lista per contenere le sottolunghezze

    // Se la lunghezza della stringa è minore della dimensione desiderata, aggiungila direttamente alla lista
    if (str.length() < size) {
        list_of_factors.push_back(str);
    } else {
        // Altrimenti, suddividi la stringa in sottosequenze della dimensione specificata
        for (size_t i = 0; i < str.length(); i += size) {
            string fact;
            // Se l'indice finale supera la lunghezza della stringa, prendi solo i caratteri rimanenti
            if (i + size > str.length()) {
                fact = str.substr(i);
            } else { // Altrimenti, prendi una sottostringa della dimensione specificata
                fact = str.substr(i, size);
            }
            list_of_factors.push_back(fact); 
        }
    }

     return list_of_factors;
}


string calculate_fingerprint(string s, int T = 30) {
    string id_gene = ""; // Variabile per l'ID del gene

    istringstream riga(s);
    string read;
    /*iss >> id_gene estrae una stringa dall'iss e la memorizza nella variabile id_gene.
    iss >> read estrae un'altra stringa dall'iss e la memorizza nella variabile read. */
    riga >> id_gene >> read;

    string lbl_id_gene = id_gene + " "; // Etichetta con l'ID del gene
    string new_line = lbl_id_gene + " "; // Nuova riga per le fingerprint

    vector<string> list_of_factors = substring_read(read, 5000);
    for (const auto& sft : list_of_factors) {
        vector<int> list_fact = factorization(sft, T); // Applica la fattorizzazione alla sottolunghezza

        // Aggiunge le lunghezze delle fingerprint alla riga delle fingerprint
        for (const auto& fact : list_fact) {
            new_line += to_string(fact) + " ";
        }
        new_line += "| ";
    }

    new_line += "\n";
    
    return new_line;
}


// Funzione per leggere il file FASTA, estrarre le letture e restituire la lista delle letture
void extract_long_reads(Args args, string name_file, int remainder) {
    ifstream file(name_file);

    string riga;
    string oldriga;
    int i = 0;
    while (getline(file, riga)) {
        if (!riga.empty()) { // Se la riga non è vuota, la aggiunge alla lista delle righe del file
            if((i % (args.n * 2)) / 2 == remainder){ //suddivide il contenuto dei file in modo omogeneo tra i thread
                if(i % 2 == 1){
                    string original, rc;
                    tie(original, rc) = manipulate_read(oldriga, riga);
                    string f_original = calculate_fingerprint(original, 20);
                    string f_rc = calculate_fingerprint(rc, 20);

                    mtx.lock();
                    cout << f_original << flush;
                    cout << f_rc << flush;
                    mtx.unlock();
                }
                oldriga = riga;
            }
            i++;
        }
    }
    file.close();
}

