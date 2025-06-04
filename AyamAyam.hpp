#ifndef AYAMAYAM_HPP
#define AYAMAYAM_HPP

#include <string>
#include <vector>
#include "json.hpp"

using namespace std;

struct Data {
    int jam;
    int menit;
    int level;
};

vector<string> getData(string messageRecv);
vector<Data> readFile(string fileName);
bool isEarlier(const Data& a, const Data& b);
int compareOverflow(const Data& a, const Data& b);
int compareUnderflow(const Data& a, const Data& b);
void quickSort(vector<Data>& arr, int low, int high, int (*cmp)(const Data&, const Data&));
void sortAndSaveOverflowUnderflow(vector<Data>& dataList);
void writeCritical(vector<Data> dataList);
bool convertToBinary(const string& txtFile, const string& binFile);

#endif
