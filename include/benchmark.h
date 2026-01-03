#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <string>
#include <chrono>
#include <functional>
#include <vector>
#include <map>

// 性能结果结构
struct PerformanceResult {
    std::string algorithmName;
    std::string dataType;
    int64_t dataSize;
    double timeSeconds;
    size_t memoryUsageBytes;
    size_t peakMemoryBytes;
    bool isSorted;

    PerformanceResult() = default;
    PerformanceResult(const std::string& name, const std::string& type,
                     int64_t size, double time, size_t memory, bool sorted)
        : algorithmName(name), dataType(type), dataSize(size),
          timeSeconds(time), memoryUsageBytes(memory),
          peakMemoryBytes(memory), isSorted(sorted) {}

    std::string toString() const;
};

// 性能测试器类
class Benchmark {
public:
    // 测试单个排序算法（内存排序）
    static PerformanceResult testInMemoryAlgorithm(
        const std::string& algorithmName,
        const std::string& dataType,
        const std::vector<int64_t>& intData,
        const std::vector<double>& doubleData,
        const std::vector<std::string>& stringData,
        std::function<void(std::vector<int64_t>&)> intSortFunc,
        std::function<void(std::vector<double>&)> doubleSortFunc,
        std::function<void(std::vector<std::string>&)> stringSortFunc);

    // 测试文件排序算法
    static PerformanceResult testFileSortAlgorithm(
        const std::string& algorithmName,
        const std::string& inputFile,
        const std::string& outputFile,
        const std::string& dataType,
        std::function<void(const std::string&, const std::string&)> sortFunc);

    // 运行所有测试
    static std::vector<PerformanceResult> runAllTests();

    // 生成测试报告
    static void generateReport(const std::vector<PerformanceResult>& results);

    // 生成CSV报告
    static void generateCSVReport(const std::vector<PerformanceResult>& results,
                                 const std::string& filename);

    // 格式化内存大小
    static std::string formatMemory(size_t bytes);

    // 格式化时间
    static std::string formatTime(double seconds);

private:
    // 运行单个测试用例 - 这里修改了返回类型
    static PerformanceResult runTest(const std::string& algorithm,
                                    const std::string& dataType,
                                    int64_t size);
};

#endif // BENCHMARK_H
