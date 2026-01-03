#include "quick_sort.h"
#include "data_generator.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <stack>

using namespace std;

// 分区函数
template<typename T, typename Compare>
int QuickSort::partition(vector<T>& arr, int low, int high, Compare comp) {
    // 选择最后一个元素作为基准
    T pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (comp(arr[j], pivot)) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

// 三路快速排序（处理重复元素）
template<typename T, typename Compare>
void QuickSort::threeWayQuickSort(vector<T>& arr, int low, int high, Compare comp) {
    if (low >= high) return;

    T pivot = arr[low];
    int lt = low;      // arr[low..lt-1] < pivot
    int gt = high;     // arr[gt+1..high] > pivot
    int i = low + 1;   // arr[lt..i-1] == pivot

    while (i <= gt) {
        if (comp(arr[i], pivot)) {
            swap(arr[lt++], arr[i++]);
        } else if (comp(pivot, arr[i])) {
            swap(arr[i], arr[gt--]);
        } else {
            i++;
        }
    }

    threeWayQuickSort(arr, low, lt - 1, comp);
    threeWayQuickSort(arr, gt + 1, high, comp);
}

// 递归快速排序
template<typename T, typename Compare>
void QuickSort::quickSortRecursive(vector<T>& arr, int low, int high, Compare comp) {
    if (low < high) {
        // 对小数组使用插入排序优化
        if (high - low + 1 <= 20) {
            for (int i = low + 1; i <= high; i++) {
                T key = arr[i];
                int j = i - 1;
                while (j >= low && comp(key, arr[j])) {
                    arr[j + 1] = arr[j];
                    j--;
                }
                arr[j + 1] = key;
            }
            return;
        }

        int pi = partition(arr, low, high, comp);

        quickSortRecursive(arr, low, pi - 1, comp);
        quickSortRecursive(arr, pi + 1, high, comp);
    }
}

// 迭代快速排序
template<typename T, typename Compare>
void QuickSort::quickSortIterative(vector<T>& arr, Compare comp) {
    int n = arr.size();
    if (n <= 1) return;

    stack<pair<int, int>> stk;
    stk.push({0, n - 1});

    while (!stk.empty()) {
        auto [low, high] = stk.top();
        stk.pop();

        if (low < high) {
            int pi = partition(arr, low, high, comp);

            if (pi - 1 > low) stk.push({low, pi - 1});
            if (pi + 1 < high) stk.push({pi + 1, high});
        }
    }
}

// 整数排序
void QuickSort::sortInMemory(vector<int64_t>& arr) {
    // 使用三路快速排序处理可能存在的重复元素
    threeWayQuickSort(arr, 0, arr.size() - 1, less<int64_t>());
}

// 浮点数排序
void QuickSort::sortInMemory(vector<double>& arr) {
    threeWayQuickSort(arr, 0, arr.size() - 1, less<double>());
}

// 字符串排序
void QuickSort::sortInMemory(vector<string>& arr) {
    threeWayQuickSort(arr, 0, arr.size() - 1, less<string>());
}

// 文件排序接口
void QuickSort::sortIntegerFile(const string& inputFile, const string& outputFile) {
    try {
        // 读取数据
        auto data = DataGenerator::readIntegerData(inputFile);

        // 排序
        sortInMemory(data);

        // 写入输出文件
        ofstream outFile(outputFile, ios::binary);
        if (!outFile) {
            throw runtime_error("无法打开输出文件：" + outputFile);
        }

        outFile.write(reinterpret_cast<const char*>(data.data()),
                     data.size() * sizeof(int64_t));
        outFile.close();

        cout << "整数文件排序完成：" << outputFile << endl;
    } catch (const exception& e) {
        cerr << "整数文件排序失败：" << e.what() << endl;
        throw;
    }
}

void QuickSort::sortDoubleFile(const string& inputFile, const string& outputFile) {
    try {
        // 读取数据
        auto data = DataGenerator::readDoubleData(inputFile);

        // 排序
        sortInMemory(data);

        // 写入输出文件
        ofstream outFile(outputFile, ios::binary);
        if (!outFile) {
            throw runtime_error("无法打开输出文件：" + outputFile);
        }

        outFile.write(reinterpret_cast<const char*>(data.data()),
                     data.size() * sizeof(double));
        outFile.close();

        cout << "浮点数文件排序完成：" << outputFile << endl;
    } catch (const exception& e) {
        cerr << "浮点数文件排序失败：" << e.what() << endl;
        throw;
    }
}

void QuickSort::sortStringFile(const string& inputFile, const string& outputFile) {
    try {
        // 读取数据
        auto data = DataGenerator::readStringData(inputFile);

        // 排序
        sortInMemory(data);

        // 写入输出文件
        ofstream outFile(outputFile);
        if (!outFile) {
            throw runtime_error("无法打开输出文件：" + outputFile);
        }

        for (const auto& str : data) {
            outFile << str << "\n";
        }
        outFile.close();

        cout << "字符串文件排序完成：" << outputFile << endl;
    } catch (const exception& e) {
        cerr << "字符串文件排序失败：" << e.what() << endl;
        throw;
    }
}
