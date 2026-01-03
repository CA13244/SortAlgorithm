#ifndef EXTERNAL_SORT_H
#define EXTERNAL_SORT_H

#include <string>
#include <cstdint>
#include <vector>

class ExternalSort {
public:
    // 多路归并外排序
    static void sortIntegerFile(const std::string& inputFile, const std::string& outputFile);
    static void sortDoubleFile(const std::string& inputFile, const std::string& outputFile);
    static void sortStringFile(const std::string& inputFile, const std::string& outputFile);

    // 设置内存限制（字节）
    static void setMemoryLimit(size_t limit) { memoryLimit = limit; }

private:
    static size_t memoryLimit;

    // 分割文件为有序的顺串
    template<typename T>
    static int createInitialRuns(const std::string& inputFile,
                                const std::string& tempDir,
                                bool (*compare)(const T&, const T&) = nullptr);

    // 多路归并
    template<typename T>
    static void mergeRuns(const std::vector<std::string>& runFiles,
                         const std::string& outputFile,
                         bool (*compare)(const T&, const T&) = nullptr);

    // 读取一批数据
    template<typename T>
    static std::vector<T> readChunk(const std::string& filename, size_t chunkSize);

    // 写入一批数据
    template<typename T>
    static void writeChunk(const std::string& filename, const std::vector<T>& data);

    // 比较函数
    static bool compareInt(const int64_t& a, const int64_t& b) { return a < b; }
    static bool compareDouble(const double& a, const double& b) { return a < b; }
    static bool compareString(const std::string& a, const std::string& b) { return a < b; }
};

#endif // EXTERNAL_SORT_H
