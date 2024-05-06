

#include <sstream>
#include<fstream>
#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <algorithm>

#include "factorization_comb.h"

using namespace std;


vector<string> read_long_fasta_2_steps(vector<string> fasta_lines) {
    vector<string> lines; //Lista per contenere le righe processate
    int i = 0;

    while (true) {
        string read_original = ""; //variabile per memorizzare la sequenza originale
        string read_rc = ""; //Variabile per memorizzare la sequenza R&C
        string id_gene = ""; //Variabile per tenere traccia del passo di iterazione
        // ID_GENE
        string l_1 = fasta_lines[i];  
        
        l_1.erase(l_1.begin()); // Rimuovi il carattere '>' dall'ID_GENE
        id_gene = l_1;

        read_original += id_gene + "_0 "; //originale
        read_rc += id_gene + "_1 "; //reverse

        // Read
        string l_2 = fasta_lines[i + 1]; // Leggi la riga della sequenza
        
        read_original += l_2;  //concatena ID e sequenza originale       
        read_rc += reverse_complement(l_2); // concatena ID e sequenza reverse&complement
        
        lines.push_back(read_original);  // Aggiungi la sequenza originale alla lista delle righe processate
        lines.push_back(read_rc);  // Aggiungi la sequenza R&C alla lista delle righe processate

        i += 2;
        if (i >= fasta_lines.size()) {
            break;
        }
    }

    return lines;

}


//Suddivide le lunghe letture in sottolunghezze
vector<string> factors_string(const string& str, int size = 300) {    
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
            list_of_factors.push_back(fact); // Aggiungi la sottosequenza alla lista delle sottosequenze
        }
    }

     return list_of_factors;

}


tuple<vector<string>, vector<string>> compute_long_fingerprint_by_list(string fact_file = "no_create", int T = 30, vector<string> list_reads = {} ) {
    vector<string> fingerprint_lines; // Lista per contenere le righe delle impronte digitali
    vector<string> fingerprint_fact_lines; // Lista per contenere le righe delle impronte digitali con fattorizzazione

    string id_gene = ""; // Variabile per l'ID del gene

    for (string s : list_reads) { // Itera su ogni lettura nella lista delle read
        istringstream riga(s);
        string read;
        /*La prima operazione iss >> id_gene estrae una stringa dall'iss e la memorizza nella variabile id_gene. 
        La seconda operazione iss >> read estrae un'altra stringa dall'iss e la memorizza nella variabile read. */
        riga >> id_gene >> read; // Estrae l'ID del gene e la sequenza di nucleotidi dalla riga

        string lbl_id_gene = id_gene + " "; // Etichetta con l'ID del gene
        string new_line = lbl_id_gene + " "; // Nuova riga per le fingerprint
        string new_fact_line = lbl_id_gene + " "; // Nuova riga per le fingerprint con fattorizzazione

        vector<string> list_of_factors = factors_string(read, 300); // Suddivide la lettura in sottolunghezze di dimensione 300
        for (const auto& sft : list_of_factors) { // Itera su ogni sottolunghezza
            vector<string> list_fact = d_duval_(sft, T); // Applica la tecnica di fattorizzazione alla sottolunghezza

           // Aggiunge le lunghezze delle fingerprint alla riga delle fingerprint
            for (const auto& fact : list_fact) {               
                new_line += to_string(fact.length()) + " ";
            }
            new_line += "| ";

            // Se richiesto, aggiunge le fingerprint con fattorizzazione alla riga
            if (fact_file == "create") {
                for (const auto& fact : list_fact) {
                    new_fact_line += fact + " ";
                }
                new_fact_line += "| ";
            }
        }

        new_line += "\n"; // Aggiunge una nuova riga per le fingerprint
        new_fact_line += "\n"; // Aggiunge una nuova riga per le fingerprint con fattorizzazione
        fingerprint_lines.push_back(new_line); // Aggiunge la riga delle fingerprint alla lista
        fingerprint_fact_lines.push_back(new_fact_line); // Aggiunge la riga delle fingerprint con fattorizzazione alla lista
    }
    
    return make_tuple(fingerprint_lines, fingerprint_fact_lines);

}


// Funzione per leggere il file FASTA, estrarre le letture e restituire la lista delle letture
vector<string> extract_long_reads(string name_file = "fingerprint/ML/reads_150.fa") {
    //cout << "\n Extract long reads - start..." << endl;

    vector<string> lines; // Lista per contenere le reads
    vector<string> linesResult; // Lista per contenere le reads
    vector<string> fasta_lines;

    ifstream file(name_file); // Apre il file FASTA
    string riga;
    while (getline(file, riga)) { // Legge il file riga per riga
        if (!riga.empty()) { // Se la riga non è vuota, la aggiunge alla lista delle righe del file
            fasta_lines.push_back(riga);
        }         
    }
   
    // Chiude il file dopo aver letto tutte le righe
    file.close();

    // Legge le righe del file FASTA e estrae le letture
    lines = read_long_fasta_2_steps(fasta_lines);

    for (int l = 0; l < lines.size(); l++) { // Itera su ciascuna riga delle letture
        string s = lines[l];

        vector<string> str_line; 
        size_t pos = 0;
        string token;
        while ((pos = s.find(' ')) != string::npos) {// Splitta la riga
            token = s.substr(0, pos);
            str_line.push_back(token);

            s.erase(0, pos + 1);
        }
        str_line.push_back(s);

        if (str_line.size() > 1) { // Verifica se ci sono almeno due token
            string id_gene =  str_line[0]; // Estrae l'ID del gene dalla riga

            string lbl_id_gene = id_gene + " "; // Etichetta con l'ID del gene

            // UPPER fingerprint
            string sequence = str_line[1]; // Estrae la sequenza di nucleotidi dalla riga
            transform(sequence.begin(), sequence.end(), sequence.begin(), ::toupper); // Trasforma la sequenza in maiuscolo

            string new_line = lbl_id_gene + sequence + "\n"; // Crea la nuova riga con l'ID del gene e la sequenza
            linesResult.push_back(new_line); // Aggiunge la nuova riga alla lista delle letture
        }
    }

    //cout << "\nExtract long reads - stop!" << std::endl;

    return linesResult; // Restituisce la lista delle letture

}

