#ifndef SHELL_SORT_H
#define SHELL_SORT_H

#include <vector>
#include <string>
#include <functional>

class ShellSort {
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

    // 通用的希尔排序实现
    template<typename T, typename Compare>
    static void shellSortImpl(T* data, size_t n, Compare comp);

    // 计算希尔排序的间隔序列
    static std::vector<size_t> generateGaps(size_t n);
};

#endif // SHELL_SORT_H
