#ifndef DATA_GENERATOR_H
#define DATA_GENERATOR_H

#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <random>

// 数据类型枚举
enum class DataType {
    INTEGER,          // 整型
    DOUBLE,           // 双精度浮点数
    STRING            // 字符串
};

// 数据规模枚举
enum class DataSize {
    SIZE_1M   = 1000000,
    SIZE_10M  = 10000000,
    SIZE_100M = 100000000,
    SIZE_500M = 500000000,
    SIZE_1G   = 1000000000
};

// 数据生成器类
class DataGenerator {
public:
    // 生成整数数据到文件
    static void generateIntegerData(const std::string& filename,
                                    int64_t size,
                                    bool allowDuplicates = false);

    // 生成浮点数数据到文件
    static void generateDoubleData(const std::string& filename,
                                   int64_t size,
                                   bool allowDuplicates = false);

    // 生成字符串数据到文件
    static void generateStringData(const std::string& filename,
                                   int64_t size,
                                   bool allowDuplicates = false);

    // 从二进制文件读取整数数据
    static std::vector<int64_t> readIntegerData(const std::string& filename);

    // 从二进制文件读取浮点数数据
    static std::vector<double> readDoubleData(const std::string& filename);

    // 从文本文件读取字符串数据
    static std::vector<std::string> readStringData(const std::string& filename);

    // 验证整数文件是否已排序
    static bool verifyIntegerSorted(const std::string& filename);

    // 验证浮点数文件是否已排序
    static bool verifyDoubleSorted(const std::string& filename);

    // 验证字符串文件是否已排序
    static bool verifyStringSorted(const std::string& filename);

    // 获取数据规模对应的数值
    static int64_t getSizeValue(DataSize size);

    // 生成更真实的数据集（模拟真实世界数据分布）
    static void generateRealisticIntegerData(const std::string& filename, int64_t size);
    static void generateRealisticDoubleData(const std::string& filename, int64_t size);
    static void generateRealisticStringData(const std::string& filename, int64_t size);

    // 模板方法：通用验证排序
    template<typename T>
    static bool isSorted(const std::vector<T>& arr,
                        std::function<bool(const T&, const T&)> comp = std::less<T>());

private:
    // 随机数引擎
    static std::mt19937_64& getRandomEngine();

    // 生成随机字符串
    static std::string generateRandomString(size_t minLen, size_t maxLen);

    // 生成更真实的整数分布
    static int64_t generateRealisticInteger();

    // 生成更真实的浮点数分布
    static double generateRealisticDouble();

    // 生成更真实的字符串
    static std::string generateRealisticString();
};

#endif // DATA_GENERATOR_H
