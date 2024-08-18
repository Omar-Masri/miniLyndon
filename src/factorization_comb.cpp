#include <string>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <iostream>

using namespace std;

string reverse_complement(string seq) {
    unordered_map<char, char> complement = {
        {'A', 'T'},
        {'C', 'G'},
        {'G', 'C'},
        {'T', 'A'},
        {'N', 'N'}
    };

    string result;
    result.reserve(seq.size());

    // Genera il complemento inverso iterando sulla sequenza invertita
    for (auto it = seq.rbegin(); it != seq.rend(); ++it) {
        result += complement[*it];
    }

    return result;
}

//algoritmo di Duval per la fattorizzazione CFL
vector<string> duval_(const string& s) {
    int n = s.length();
    
    int i = 0;
    vector<string> res;

    while (i < n) {
        int j = i + 1;
        int k = i;
        // ricerca della sottostringa di Lyndon
        while (j < n && s[k] <= s[j]) {
            if (s[k] < s[j]) {
                k = i;
            } else {
                k += 1;
            }
            j += 1;
        }

        // Aggiunta della sottostringa di Lyndon trovata alla lista risultato
        res.push_back(s.substr(i, j - k));
        i += j - k;
    }

    return res;
}

//algoritmo per trovare la prima occorrenza di ICFL, come estensione destra limitata
tuple<string, string> find_prefix(const string& w) {
    int n = w.length();
    if (n == 1) {
        return make_tuple(w + "0", "");
    }

    int i = 0;
    int j = 1;
    while (j < n - 1 && w[j] <= w[i]) {
        if (w[j] < w[i]) {
            i = 0;
        } else {
            i += 1;
        }
        j += 1;
    }

    if (j == n - 1) {
        if (w[j] <= w[i]) {
            return make_tuple(w + "0", "");
        }
    }

    return make_tuple(w.substr(0, j + 1), w.substr(j + 1));
}

//calcola la lunghezza dei prefissi esclusa la prima occorrenza
vector<int> get_failure_function(const string& s) {
    vector<int> f(s.length(), 0);
    int i = 1;
    int j = 0;
    int m = s.length();
    
    while (i < m) {
        if (s[j] == s[i]) {
            f[i] = j + 1;
            i += 1;
            j += 1;
        } else if (j > 0) {
            j = f[j - 1];
        } else {
            f[i] = 0;
            i += 1;
        }
    }
    
    return f;
}

//trova il prefisso che è una parola di Lyndon inversa
tuple<string, string, string, int> find_bre(const string& x, const string& y) {
    string w = x + y;
    int n = x.length() - 1;
    vector<int> f = get_failure_function(x.substr(0, x.length() - 1));
    int i = n - 1;
    int last = n;

    while (i >= 0) {
        if (w[f[i]] < x.back()) {
            last = f[i] - 1;
        }
        i = f[i] - 1;
    }

    return make_tuple(w.substr(0, n - last - 1),
                    w.substr(n - last - 1, last + 2),  //n + 1 - (n - last - 1)
                    y,
                    last + 1);

}

//calcola la  fattorizzazione inversa di w utilizzando l'algoritmo ICFL
vector<string> icfl_(const string& w) {
    string x, y;
    //Trova il prefisso più lungo di w che è un'estensione limitata a destra di un suffisso di w
    tie(x, y) = find_prefix(w); 

    // Se il prefisso trovato è w stesso, significa che w è già una inverse Lyndon word
    if (x == w + "0") {
        return {w};
    }

    string p, bre;
    int last;
    tie(p, bre, y, last) = find_bre(x, y);

    //Calcola la fattorizzazione inversa di bre + y
    vector<string> l = icfl_(bre + y);

    if (l[0].length() > last) {
        l.insert(l.begin(), p);
    } else {
        l[0] = p + l[0];
    }
  
    return l;
}

//fattorizzazione CFL-ICFL
vector<string> cfl_icfl_(const string& w, int cfl_max = 30) {
    vector<string> result;

    vector<string> cfl_fact = duval_(w);

    // Itera attraverso ciascun fattore nella fattorizzazione CFL di w
    for (const string& factor : cfl_fact) {
        // Se la lunghezza del fattore supera la soglia massima cfl_max
        if (factor.length() > cfl_max) {
            // Calcola la fattorizzazione ICFL del fattore utilizzando la funzione icfl
            vector<string> icfl_fact = icfl_(factor);
            
            // Aggiungi i fattori della fattorizzazione ICFL al risultato
            result.insert(result.end(), icfl_fact.begin(), icfl_fact.end());
        } else {
            result.push_back(factor);
        }
    }

    return result;
}

//fattorizzazione combinata tra seq e la sua reverse&complement
vector<string> factorization(const string& seq, int k) {
    vector<int> factors1;
    for (string factor : cfl_icfl_(seq, k))
        factors1.push_back(factor.size());
    
    string complement = reverse_complement(seq);

    //Calcola la fattorizzazione dei fattori del reverse&complement
    vector<int> factors2;
    vector<string> cfl_icfl_complement = cfl_icfl_(complement);
    for (int i = cfl_icfl_complement.size() -1; i >= 0; i--)
       factors2.push_back(cfl_icfl_complement[i].size());

    string rest = seq;
    vector<string> result;
    int i = 0;
    int n;
    // estrarre i fattori dalla sequenza in base alla comparazione dei fattori delle due fattorizzazioni
    while (!factors1.empty() && !factors2.empty()) {
        if (factors1[0] < factors2[0]) {
            //Se la lunghezza del prossimo fattore nella prima fattorizzazione è minore,
            // estrai la lunghezza del fattore dalla lista dei fattori della prima fattorizzazione
            n = factors1[0]; 
            factors1.erase(factors1.begin());
            
            // Sottrai la lunghezza del fattore dalla lunghezza del prossimo fattore nella seconda fattorizzazione
            factors2[0] = factors2[0] - n;
        }else{
            // altrimenti estrai la lunghezza del fattore dalla lista dei fattori della seconda fattorizzazione
            n = factors2[0];  
            factors2.erase(factors2.begin());
            
            // Sottrai la lunghezza del fattore dalla lunghezza del prossimo fattore nella prima fattorizzazione
            factors1[0] = factors1[0] - n;

            // Se la lunghezza del prossimo fattore nella seconda fattorizzazione è diventata zero, rimuovila dalla lista
            if (factors1[0] == 0) {
                factors1.erase(factors1.begin());
                }
        }
        
        // Estrai il fattore corrispondente dalla sequenza e aggiorna la sequenza rimanente
        string f = rest.substr(0, n);
        rest = rest.substr(n);

        // Aggiungi il fattore estratto al risultato
        result.push_back(f);
    }

    return result;
}


























