#include "shell_sort.h"
#include "data_generator.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

vector<size_t> ShellSort::generateGaps(size_t n) {
    vector<size_t> gaps;

    // 使用Knuth序列
    size_t gap = 1;
    while (gap < n) {
        gaps.push_back(gap);
        gap = gap * 3 + 1;
    }

    // 反转序列（从大到小）
    reverse(gaps.begin(), gaps.end());
    return gaps;
}

template<typename T, typename Compare>
void ShellSort::shellSortImpl(T* data, size_t n, Compare comp) {
    auto gaps = generateGaps(n);

    for (size_t gap : gaps) {
        for (size_t i = gap; i < n; i++) {
            T temp = data[i];
            size_t j = i;

            while (j >= gap && comp(temp, data[j - gap])) {
                data[j] = data[j - gap];
                j -= gap;
            }
            data[j] = temp;
        }
    }
}

// 整数排序
void ShellSort::sortInMemory(vector<int64_t>& arr) {
    shellSortImpl(arr.data(), arr.size(), less<int64_t>());
}

// 浮点数排序
void ShellSort::sortInMemory(vector<double>& arr) {
    shellSortImpl(arr.data(), arr.size(), less<double>());
}

// 字符串排序
void ShellSort::sortInMemory(vector<string>& arr) {
    shellSortImpl(arr.data(), arr.size(), less<string>());
}

// 文件排序接口
void ShellSort::sortIntegerFile(const string& inputFile, const string& outputFile) {
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

void ShellSort::sortDoubleFile(const string& inputFile, const string& outputFile) {
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

void ShellSort::sortStringFile(const string& inputFile, const string& outputFile) {
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
