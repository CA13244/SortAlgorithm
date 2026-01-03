#ifndef MERGE_SORT_H
#define MERGE_SORT_H

#include <vector>
#include <string>
#include <functional>

class MergeSort {
public:
    // 整数排序
    static void sortInMemory(std::vector<int64_t>& arr);

    // 浮点数排序
    static void sortInMemory(std::vector<double>& arr);

    // 字符串排序
    static void sortInMemory(std::vector<std::string>& arr);

    // 文件排序接口
    static void sortIntegerFile(const std::string& inputFile, const std::string& outputFile);
    static void sortDoubleFile(const std::string& inputFile, const std::string& outputFile);
    static void sortStringFile(const std::string& inputFile, const std::string& outputFile);

    // 递归归并排序
    template<typename T, typename Compare>
    static void mergeSortRecursive(std::vector<T>& arr, int left, int right, Compare comp);

    // 归并函数
    template<typename T, typename Compare>
    static void merge(std::vector<T>& arr, int left, int mid, int right, Compare comp);

    // 迭代归并排序
    template<typename T, typename Compare>
    static void mergeSortIterative(std::vector<T>& arr, Compare comp);
};

#endif // MERGE_SORT_H
