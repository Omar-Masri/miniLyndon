#include <string>
#include <vector>
#include <unordered_map>
#include <tuple>

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


vector<string> duval_(const string& s) {
    /* input: stringa s
    output: fattorizzazione di Lyndon (CFL) di s */
    int n = s.length();
    
    // Inizializzazione dell'indice di partenza
    int i = 0;
    vector<string> res;

    while (i < n) {
        // Inizializzazione degli indici per l'algoritmo di Duval
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
        //Aggiornamento dell'indice di partenza per la prossima iterazione
        i += j - k;
    }

    return res;

}


tuple<string, string> find_prefix(const string& w) {
    /*input: a string w
    output: (x, y) where x = w0, y = '' if w in an inverse Lyndon word
        w = xy, x = pp' where (p, p') ∈ Pref_bre(w), otherwise.
        p is an inverse Lyndon word which is a proper prefix of w = pv;
        p' is the bounded right extension of p in w.
        A bounder right extension is a proper prefix of v such that:
            - p' is an inverse Lyndon word
            - pz' is an inverse Lyndon word for each proper prefix z' of p'
            - pp' is not an inverse Lyndon word
            - p << p' (p < p' and p is not a proper prefix of p')
        Pref_bre(w) = {(p, p') | p is an inverse Lyndon word which is a non
            empty proper prefix of w }*/
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


tuple<string, string, string, int> find_bre(const string& x, const string& y) {
    /*
    input: (x, y) where w = xy is not an inverse Lyndon word;
        x = pp' = raurb, (p, p') ∈ Pref_bre(w)
    output: (p, p', y, last) = (rau, rb, y, |r|)*/
    string w = x + y;
    int n = x.length() - 1;
    vector<int> f = get_failure_function(x.substr(0, x.length() - 1));  // Calcolo della funzione di fallimento per Border(raur)
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


vector<string> icfl_(const string& w) {
    /* input: a string w
    output: calcola la  fattorizzazione inversa di w utilizzando l'algoritmo ICFL
        If w is an inverse lyndon word, ICFL(w) = w otherwise we have w=pv
        and ICFL(v) = (m1', ..., mk') and p' bounded right extension of p in w.
        ICFL(w) = (p) + ICFL(v)         if p' = rb <= m1'
                  (pm1', m2', ..., mk') if m1' <= r

    ossia:  Se w è una inverse Lyndon word, la sua fattorizzazione inversa sarà semplicemente w.
     Altrimenti, si suddivide w in due parti: p e v, dove p è un'estensione destra limitata di p' in w.
     Successivamente, ICFL viene applicato a v.
     Se p' = rb è minore o uguale a m1', la sottostringa più piccola nella fattorizzazione inversa di v, allora p viene aggiunto alla fattorizzazione di v.
    Altrimenti, viene combinato con m1' e la fattorizzazione di v.  */
    string x, y;
    //Trova il prefisso più lungo di w che è un'estensione limitata a destra di un suffisso di w
    tie(x, y) = find_prefix(w); //estrarre i valori restituiti dalla funzione find_prefix(w) e assegnarli alle variabili x e y.

    // Se il prefisso trovato è w stesso, significa che w è già una inverse Lyndon word
    if (x == w + "0") {
        return {w};
    }

    /*Trova p, bre, y, last
    p è il prefisso, bre è l'estensione limitata a destra di p in w, y è la parte rimanente di w dopo la rimozione di p e bre
    last è la lunghezza della sottostringa più piccola nella fattorizzazione inversa di y*/
    string p, bre;
    int last;
    tie(p, bre, y, last) = find_bre(x, y);

    //Calcola la fattorizzazione inversa di bre + y
    vector<string> l = icfl_(bre + y);

    //Verifica se la lunghezza della sottostringa più piccola nella fattorizzazione inversa di y è maggiore della lunghezza di p'
    if (l[0].length() > last) {
        //aggiungi p come primo elemento nella fattorizzazione inversa di y
        l.insert(l.begin(), p);
    } else {
        //combina p con la prima sottostringa nella fattorizzazione inversa di y
        l[0] = p + l[0];
    }
  
    return l;

}


vector<string> cfl_icfl_(const string& w, int cfl_max = 30) {
    //Inizializza una lista vuota per contenere i fattori risultanti della fattorizzazione CFL-ICFL di w
    vector<string> result;

    //Calcola la fattorizzazione CFL di w
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
            // Se la lunghezza del fattore non supera la soglia massima, aggiungi direttamente il fattore al risultato
            result.push_back(factor);
        }
    }

    // Restituisce i fattori risultanti della fattorizzazione CFL-ICFL di w
    return result;

}


//prende in input una sequenza 'seq',e un parametro 'k' per la fattorizzazione.
vector<string> d_duval_(const string& seq, int k) {
    vector<int> factors1;
    for (string factor : cfl_icfl_(seq, k))
        factors1.push_back(factor.size());
    
    // Calcola il complemento inverso della sequenza 'seq'
    string complement = reverse_complement(seq);

    //Calcola la fattorizzazione dei fattori del complemento inverso utilizzando l'algoritmo specificato
    vector<int> factors2;
    vector<string> cfl_icfl_complement = cfl_icfl_(complement);
    for (int i = cfl_icfl_complement.size() -1; i >= 0; i--)
       factors2.push_back(cfl_icfl_complement[i].size());

    // Inizializza le variabili per la sequenza rimanente e il risultato
    string rest = seq;
    vector<string> result;
    int i = 0;
    int n;
    // estrarre i fattori dalla sequenza in base alla comparazione dei fattori delle due fattorizzazioni
    while (!factors1.empty() && !factors2.empty()) {
        // Confronta le lunghezze del prossimo fattore nella prima e nella seconda fattorizzazione
        if (factors1[0] < factors2[0]) {
            //Se la lunghezza del prossimo fattore nella prima fattorizzazione è minore,
            // estrai la lunghezza del fattore dalla lista dei fattori della prima fattorizzazione
            n = factors1[0];  // Converti la stringa in un numero intero
            factors1.erase(factors1.begin());
            
            // Sottrai la lunghezza del fattore dalla lunghezza del prossimo fattore nella seconda fattorizzazione
            factors2[0] = factors2[0] - n;
            
            // Se la lunghezza del prossimo fattore nella seconda fattorizzazione è diventata zero, rimuovila dalla lista
            if (factors2[0] == 0) {               
                factors2.erase(factors2.begin());
            }
        }else{
            // altrimenti estrai la lunghezza del fattore dalla lista dei fattori della seconda fattorizzazione
            n = factors2[0];  // Converti la stringa in un numero intero
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

    // Se ci sono fattori rimanenti nella prima fattorizzazione, li aggiungi al risultato
    while (!factors1.empty()) {
        int n = factors1[0];  // Estrai la lunghezza del prossimo fattore dalla lista dei fattori
        factors1.erase(factors1.begin());  // Rimuovi l'elemento dalla lista dei fattori

        string f = rest.substr(0, n);  // Estrai il fattore corrispondente dalla sequenza
        rest = rest.substr(n);  // Aggiorna la sequenza rimanente
        result.push_back(f);  // Aggiungi il fattore estratto al risultato
    }
            
    // Se ci sono fattori rimanenti nella seconda fattorizzazione, li aggiungi al risultato
    while (!factors2.empty()) {
        int n = factors2[0];  // Estrai la lunghezza del prossimo fattore dalla lista dei fattori
        factors2.erase(factors2.begin());  // Rimuovi l'elemento dalla lista dei fattori

        string f = rest.substr(0, n);  // Estrai il fattore corrispondente dalla sequenza
        rest = rest.substr(n);  // Aggiorna la sequenza rimanente
        result.push_back(f);  // Aggiungi il fattore estratto al risultato
    }
    
    return result;
    
}


























