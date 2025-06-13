#ifndef AYAMAYAM_HPP
#define AYAMAYAM_HPP

#include <string>
#include <vector>
#include "json.hpp"

using namespace std;

struct Data {
    int jam;
    int menit;
    int detik;
    int level;
};


bool isEarlier(const Data& a, const Data& b);
int compareOverflow(const Data& a, const Data& b);
int compareUnderflow(const Data& a, const Data& b);
void quickSort(vector<Data>& arr, int low, int high, int (*cmp)(const Data&, const Data&));
void sortAndSaveOverflowUnderflow(vector<Data>& dataList);
vector<Data> readLogFile(const string& filename);
void writeCritical(const vector<Data>& entries, const string& outFile);
bool convertToBinary(const string& txtFile, const string& binFile);

#endif
