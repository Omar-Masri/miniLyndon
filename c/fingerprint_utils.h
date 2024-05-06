// File: fingerprint_utils.h

#ifndef FINGERPRINT_UTILS_H   
#define FINGERPRINT_UTILS_H 

#include <vector>
#include <tuple>
#include <string>

using namespace std;

vector<string> extract_long_reads(string input_fasta);
tuple<vector<string>, vector<string>> compute_long_fingerprint_by_list(string fact_file, int T, vector<string> list_reads);

#endif // FINGERPRINT_UTILS_H 