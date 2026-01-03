#ifndef RADIX_SORT_H
#define RADIX_SORT_H

#include <vector>
#include <string>

class RadixSort {
public:
    // 整数排序（只支持非负整数）
    static void sortInMemory(std::vector<int64_t>& arr);

    // 字符串排序
    static void sortInMemory(std::vector<std::string>& arr);

    // 文件排序接口
    static void sortIntegerFile(const std::string& inputFile, const std::string& outputFile);
    static void sortStringFile(const std::string& inputFile, const std::string& outputFile);

    // 获取最大值
    template<typename T>
    static T getMax(const std::vector<T>& arr);

    // 计数排序（用于基数排序的辅助函数）
    static void countingSort(std::vector<int64_t>& arr, int exp);

    // 字符串基数排序的计数排序
    static void countingSort(std::vector<std::string>& arr, int pos);
};

#endif // RADIX_SORT_H
