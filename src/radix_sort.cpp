#include "radix_sort.h"
#include "data_generator.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

using namespace std;

// 获取最大值
template<typename T>
T RadixSort::getMax(const vector<T>& arr) {
    if (arr.empty()) return T();

    T mx = arr[0];
    for (const auto& val : arr) {
        if (val > mx) {
            mx = val;
        }
    }
    return mx;
}

// 计数排序（用于基数排序的辅助函数）
void RadixSort::countingSort(vector<int64_t>& arr, int exp) {
    int n = arr.size();
    vector<int64_t> output(n);
    vector<int> count(10, 0);

    // 统计每个位上的数字出现次数
    for (int i = 0; i < n; i++) {
        int digit = (arr[i] / exp) % 10;
        count[digit]++;
    }

    // 计算累计次数
    for (int i = 1; i < 10; i++) {
        count[i] += count[i - 1];
    }

    // 根据计数数组将元素放入输出数组
    for (int i = n - 1; i >= 0; i--) {
        int digit = (arr[i] / exp) % 10;
        output[count[digit] - 1] = arr[i];
        count[digit]--;
    }

    // 将输出数组复制回原数组
    for (int i = 0; i < n; i++) {
        arr[i] = output[i];
    }
}

// 字符串基数排序的计数排序
void RadixSort::countingSort(vector<string>& arr, int pos) {
    int n = arr.size();
    vector<string> output(n);
    vector<int> count(256, 0); // ASCII字符集

    // 统计每个字符出现次数
    for (int i = 0; i < n; i++) {
        char ch = (pos < arr[i].size()) ? arr[i][pos] : 0;
        count[ch]++;
    }

    // 计算累计次数
    for (int i = 1; i < 256; i++) {
        count[i] += count[i - 1];
    }

    // 根据计数数组将元素放入输出数组
    for (int i = n - 1; i >= 0; i--) {
        char ch = (pos < arr[i].size()) ? arr[i][pos] : 0;
        output[count[ch] - 1] = arr[i];
        count[ch]--;
    }

    // 将输出数组复制回原数组
    for (int i = 0; i < n; i++) {
        arr[i] = output[i];
    }
}

// 整数排序（只支持非负整数）
void RadixSort::sortInMemory(vector<int64_t>& arr) {
    // 先找出负数，单独处理
    vector<int64_t> negatives;
    vector<int64_t> nonNegatives;

    for (const auto& val : arr) {
        if (val < 0) {
            negatives.push_back(-val); // 转换为正数处理
        } else {
            nonNegatives.push_back(val);
        }
    }

    // 对非负数进行基数排序
    if (!nonNegatives.empty()) {
        int64_t maxVal = getMax(nonNegatives);

        for (int exp = 1; maxVal / exp > 0; exp *= 10) {
            countingSort(nonNegatives, exp);
        }
    }

    // 对负数进行基数排序（先转换为正数）
    if (!negatives.empty()) {
        int64_t maxVal = getMax(negatives);

        for (int exp = 1; maxVal / exp > 0; exp *= 10) {
            countingSort(negatives, exp);
        }

        // 将排序后的负数反转并转换回负数
        reverse(negatives.begin(), negatives.end());
        for (auto& val : negatives) {
            val = -val;
        }
    }

    // 合并结果
    arr.clear();
    arr.insert(arr.end(), negatives.begin(), negatives.end());
    arr.insert(arr.end(), nonNegatives.begin(), nonNegatives.end());
}

// 字符串排序
void RadixSort::sortInMemory(vector<string>& arr) {
    if (arr.empty()) return;

    // 找出最大长度
    size_t maxLen = 0;
    for (const auto& str : arr) {
        if (str.size() > maxLen) {
            maxLen = str.size();
        }
    }

    // 从最低位开始排序
    for (int pos = maxLen - 1; pos >= 0; pos--) {
        countingSort(arr, pos);
    }
}

// 文件排序接口
void RadixSort::sortIntegerFile(const string& inputFile, const string& outputFile) {
    try {
        // 读取数据
        auto data = DataGenerator::readIntegerData(inputFile);

        // 排序
        sortInMemory(data);

        // 写入输出文件
        ofstream outFile(outputFile, ios::binary);
        if (!outFile) {
            throw runtime_error("无法打开输出文件: " + outputFile);
        }

        outFile.write(reinterpret_cast<const char*>(data.data()),
                     data.size() * sizeof(int64_t));
        outFile.close();

        cout << "整数文件排序完成: " << outputFile << endl;
    } catch (const exception& e) {
        cerr << "整数文件排序失败: " << e.what() << endl;
        throw;
    }
}

void RadixSort::sortStringFile(const string& inputFile, const string& outputFile) {
    try {
        // 读取数据
        auto data = DataGenerator::readStringData(inputFile);

        // 排序
        sortInMemory(data);

        // 写入输出文件
        ofstream outFile(outputFile);
        if (!outFile) {
            throw runtime_error("无法打开输出文件: " + outputFile);
        }

        for (const auto& str : data) {
            outFile << str << "\n";
        }
        outFile.close();

        cout << "字符串文件排序完成: " << outputFile << endl;
    } catch (const exception& e) {
        cerr << "字符串文件排序失败: " << e.what() << endl;
        throw;
    }
}
