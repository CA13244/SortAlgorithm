#ifndef QUICK_SORT_H
#define QUICK_SORT_H

#include <vector>
#include <string>
#include <functional>
#include <stack>

class QuickSort {
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

    // 递归快速排序
    template<typename T, typename Compare>
    static void quickSortRecursive(std::vector<T>& arr, int low, int high, Compare comp);

    // 迭代快速排序
    template<typename T, typename Compare>
    static void quickSortIterative(std::vector<T>& arr, Compare comp);

    // 分区函数
    template<typename T, typename Compare>
    static int partition(std::vector<T>& arr, int low, int high, Compare comp);

    // 三路快速排序（处理重复元素）
    template<typename T, typename Compare>
    static void threeWayQuickSort(std::vector<T>& arr, int low, int high, Compare comp);
};

#endif // QUICK_SORT_H
