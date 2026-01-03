#include "merge_sort.h"
#include "data_generator.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>


using namespace std;

// 归并函数
template<typename T, typename Compare>
void MergeSort::merge(vector<T>& arr, int left, int mid, int right, Compare comp) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<T> L(n1), R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
        if (comp(L[i], R[j])) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

// 递归归并排序
template<typename T, typename Compare>
void MergeSort::mergeSortRecursive(vector<T>& arr, int left, int right, Compare comp) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        mergeSortRecursive(arr, left, mid, comp);
        mergeSortRecursive(arr, mid + 1, right, comp);

        // 如果已经有序，跳过合并
        if (comp(arr[mid + 1], arr[mid])) {
            merge(arr, left, mid, right, comp);
        }
    }
}

// 迭代归并排序
template<typename T, typename Compare>
void MergeSort::mergeSortIterative(vector<T>& arr, Compare comp) {
    int n = arr.size();

    for (int currSize = 1; currSize < n; currSize = 2 * currSize) {
        for (int left = 0; left < n - 1; left += 2 * currSize) {
            int mid = min(left + currSize - 1, n - 1);
            int right = min(left + 2 * currSize - 1, n - 1);

            if (mid < right) {
                merge(arr, left, mid, right, comp);
            }
        }
    }
}

// 整数排序
void MergeSort::sortInMemory(vector<int64_t>& arr) {
    mergeSortRecursive(arr, 0, arr.size() - 1, less<int64_t>());
}

// 浮点数排序
void MergeSort::sortInMemory(vector<double>& arr) {
    mergeSortRecursive(arr, 0, arr.size() - 1, less<double>());
}

// 字符串排序
void MergeSort::sortInMemory(vector<string>& arr) {
    mergeSortRecursive(arr, 0, arr.size() - 1, less<string>());
}

// 文件排序接口
void MergeSort::sortIntegerFile(const string& inputFile, const string& outputFile) {
    try {
        // 读取数据
        auto data = DataGenerator::readIntegerData(inputFile);

        // 排序
        sortInMemory(data);

        // 写入输出文件
        ofstream outFile(outputFile, ios::binary);
        if (!outFile) {
            throw runtime_error("无法打开输出文件： " + outputFile);
        }

        outFile.write(reinterpret_cast<const char*>(data.data()),
                     data.size() * sizeof(int64_t));
        outFile.close();

        cout << "整数文件排序完成： " << outputFile << endl;
    } catch (const exception& e) {
        cerr << "整数文件排序失败：" << e.what() << endl;
        throw;
    }
}

void MergeSort::sortDoubleFile(const string& inputFile, const string& outputFile) {
    try {
        // 读取数据
        auto data = DataGenerator::readDoubleData(inputFile);

        // 排序
        sortInMemory(data);

        // 写入输出文件
        ofstream outFile(outputFile, ios::binary);
        if (!outFile) {
            throw runtime_error("无法打开输出文件： " + outputFile);
        }

        outFile.write(reinterpret_cast<const char*>(data.data()),
                     data.size() * sizeof(double));
        outFile.close();

        cout << "浮点数文件排序完成： " << outputFile << endl;
    } catch (const exception& e) {
        cerr << "浮点数文件排序失败： " << e.what() << endl;
        throw;
    }
}

void MergeSort::sortStringFile(const string& inputFile, const string& outputFile) {
    try {
        // 读取数据
        auto data = DataGenerator::readStringData(inputFile);

        // 排序
        sortInMemory(data);

        // 写入输出文件
        ofstream outFile(outputFile);
        if (!outFile) {
            throw runtime_error("无法打开输出文件： " + outputFile);
        }

        for (const auto& str : data) {
            outFile << str << "\n";
        }
        outFile.close();

        cout << "字符串文件排序完成： " << outputFile << endl;
    } catch (const exception& e) {
        cerr << "字符串文件排序失败： " << e.what() << endl;
        throw;
    }
}
