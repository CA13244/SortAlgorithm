#include "../include/benchmark.h"
#include "../include/data_generator.h"
#include "../include/shell_sort.h"
#include "../include/quick_sort.h"
#include "../include/merge_sort.h"
#include "../include/radix_sort.h"
#include "../include/external_sort.h"
#include "../include/memory_monitor.h"
#include "../include/file_utils.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace chrono;

// 性能结果转为字符串
string PerformanceResult::toString() const {
    stringstream ss;
    ss << "算法: " << algorithmName
       << ", 类型: " << dataType
       << ", 大小: " << dataSize
       << ", 时间: " << fixed << setprecision(6) << timeSeconds << "秒"
       << ", 内存: " << Benchmark::formatMemory(memoryUsageBytes)
       << ", 排序验证: " << (isSorted ? "成功" : "失败");
    return ss.str();
}

// 测试内存排序算法
PerformanceResult Benchmark::testInMemoryAlgorithm(
    const string& algorithmName,
    const string& dataType,
    const vector<int64_t>& intData,
    const vector<double>& doubleData,
    const vector<string>& stringData,
    function<void(vector<int64_t>&)> intSortFunc,
    function<void(vector<double>&)> doubleSortFunc,
    function<void(vector<string>&)> stringSortFunc) {

    PerformanceResult result;
    result.algorithmName = algorithmName;
    result.dataType = dataType;

    try {
        // 开始监控
        MemoryMonitor::start();
        auto startTime = high_resolution_clock::now();

        // 根据数据类型执行排序
        if (dataType == "int" && intSortFunc) {
            vector<int64_t> dataCopy = intData;
            result.dataSize = dataCopy.size();

            intSortFunc(dataCopy);
            result.isSorted = DataGenerator::isSorted(dataCopy);
        }
        else if (dataType == "double" && doubleSortFunc) {
            vector<double> dataCopy = doubleData;
            result.dataSize = dataCopy.size();

            doubleSortFunc(dataCopy);
            result.isSorted = DataGenerator::isSorted(dataCopy);
        }
        else if (dataType == "string" && stringSortFunc) {
            vector<string> dataCopy = stringData;
            result.dataSize = dataCopy.size();

            stringSortFunc(dataCopy);
            result.isSorted = DataGenerator::isSorted(dataCopy);
        }

        auto endTime = high_resolution_clock::now();
        MemoryMonitor::stop();

        // 计算时间
        auto duration = duration_cast<nanoseconds>(endTime - startTime);
        result.timeSeconds = duration.count() / 1e9;

        // 获取内存使用
        result.memoryUsageBytes = MemoryMonitor::getPeakUsage();
        result.peakMemoryBytes = result.memoryUsageBytes;

    } catch (const exception& e) {
        cerr << "测试失败 (" << algorithmName << "): " << e.what() << endl;
        result.isSorted = false;
    }

    return result;
}

// 测试文件排序算法
PerformanceResult Benchmark::testFileSortAlgorithm(
    const string& algorithmName,
    const string& inputFile,
    const string& outputFile,
    const string& dataType,
    function<void(const string&, const string&)> sortFunc) {

    PerformanceResult result;
    result.algorithmName = algorithmName;
    result.dataType = dataType;

    try {
        // 获取文件大小
        ifstream inFile(inputFile, ios::binary | ios::ate);
        if (!inFile) {
            throw runtime_error("无法打开输入文件: " + inputFile);
        }

        size_t fileSize = inFile.tellg();
        inFile.close();

        // 根据数据类型计算元素数量
        if (dataType == "int") {
            result.dataSize = fileSize / sizeof(int64_t);
        } else if (dataType == "double") {
            result.dataSize = fileSize / sizeof(double);
        } else if (dataType == "string") {
            // 对于字符串，需要实际读取来计数
            vector<string> data = DataGenerator::readStringData(inputFile);
            result.dataSize = data.size();
        }

        // 开始监控
        MemoryMonitor::start();
        auto startTime = high_resolution_clock::now();

        // 执行排序
        sortFunc(inputFile, outputFile);

        auto endTime = high_resolution_clock::now();
        MemoryMonitor::stop();

        // 计算时间
        auto duration = duration_cast<nanoseconds>(endTime - startTime);
        result.timeSeconds = duration.count() / 1e9;

        // 获取内存使用
        result.memoryUsageBytes = MemoryMonitor::getPeakUsage();
        result.peakMemoryBytes = result.memoryUsageBytes;

        // 验证排序结果
        if (dataType == "int") {
            result.isSorted = DataGenerator::verifyIntegerSorted(outputFile);
        } else if (dataType == "double") {
            result.isSorted = DataGenerator::verifyDoubleSorted(outputFile);
        } else if (dataType == "string") {
            result.isSorted = DataGenerator::verifyStringSorted(outputFile);
        }

    } catch (const exception& e) {
        cerr << "文件排序测试失败 (" << algorithmName << "): " << e.what() << endl;
        result.isSorted = false;
    }

    return result;
}

// 运行单个测试用例 - 修复函数签名
PerformanceResult Benchmark::runTest(const string& algorithm,
                                    const string& dataType,
                                    int64_t size) {

    // 生成输入输出文件名
    string inputFile = "data/test_" + dataType + "_" + to_string(size) + ".dat";
    string outputFile = "output/" + algorithm + "_" + dataType + "_" + to_string(size) + ".dat";

    // 检查输入文件是否存在
    ifstream testFile(inputFile);
    if (!testFile) {
        cout << "跳过测试: " << inputFile << " (文件不存在)" << endl;
        return PerformanceResult(); // 返回默认对象
    }
    testFile.close();

    cout << "运行测试: " << algorithm << " on " << dataType << " (" << size << ")" << endl;

    PerformanceResult result;

    try {
        if (algorithm == "ShellSort") {
            if (dataType == "int") {
                result = testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                             ShellSort::sortIntegerFile);
            } else if (dataType == "double") {
                result = testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                             ShellSort::sortDoubleFile);
            } else if (dataType == "string") {
                result = testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                             ShellSort::sortStringFile);
            }
        }
        else if (algorithm == "QuickSort") {
            if (dataType == "int") {
                result = testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                             QuickSort::sortIntegerFile);
            } else if (dataType == "double") {
                result = testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                             QuickSort::sortDoubleFile);
            } else if (dataType == "string") {
                result = testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                             QuickSort::sortStringFile);
            }
        }
        else if (algorithm == "MergeSort") {
            if (dataType == "int") {
                result = testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                             MergeSort::sortIntegerFile);
            } else if (dataType == "double") {
                result = testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                             MergeSort::sortDoubleFile);
            } else if (dataType == "string") {
                result = testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                             MergeSort::sortStringFile);
            }
        }
        else if (algorithm == "RadixSort") {
            if (dataType == "int") {
                result = testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                             RadixSort::sortIntegerFile);
            } else if (dataType == "string") {
                result = testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                             RadixSort::sortStringFile);
            }
        }
        else if (algorithm == "ExternalSort") {
            if (dataType == "int") {
                result = testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                             ExternalSort::sortIntegerFile);
            } else if (dataType == "double") {
                result = testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                             ExternalSort::sortDoubleFile);
            } else if (dataType == "string") {
                result = testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                             ExternalSort::sortStringFile);
            }
        }

    } catch (const exception& e) {
        cerr << "测试异常: " << e.what() << endl;
    }

    return result;
}

// 运行所有测试 - 修复这里的问题
vector<PerformanceResult> Benchmark::runAllTests() {
    cout << "\n开始运行所有测试..." << endl;

    vector<PerformanceResult> allResults;

    // 测试配置
    vector<string> algorithms = {"ShellSort", "QuickSort", "MergeSort", "RadixSort", "ExternalSort"};
    vector<string> dataTypes = {"int", "double", "string"};
    vector<int64_t> sizes = {1000000, 10000000}; // 先测试较小的规模

    for (const string& algo : algorithms) {
        for (const string& type : dataTypes) {
            // RadixSort不支持double类型
            if (algo == "RadixSort" && type == "double") continue;

            for (int64_t size : sizes) {
                PerformanceResult result = runTest(algo, type, size); // 这里修改
                if (result.dataSize > 0) { // 有效的测试结果
                    allResults.push_back(result); // 这里修改
                }
            }
        }
    }

    cout << "\n所有测试完成!" << endl;
    return allResults;
}

// 生成测试报告
void Benchmark::generateReport(const vector<PerformanceResult>& results) {
    cout << "\n" << string(120, '=') << endl;
    cout << "排序算法性能测试报告" << endl;
    cout << string(120, '=') << "\n" << endl;

    // 按算法和数据大小分组
    map<string, vector<PerformanceResult>> groupedResults;
    for (const auto& result : results) {
        string key = result.algorithmName + "_" + result.dataType;
        groupedResults[key].push_back(result);
    }

    cout << setw(20) << left << "算法"
         << setw(10) << right << "类型"
         << setw(15) << right << "数据规模"
         << setw(15) << right << "时间(秒)"
         << setw(20) << right << "内存使用"
         << setw(20) << right << "峰值内存"
         << setw(10) << right << "验证" << endl;
    cout << string(120, '-') << endl;

    for (const auto& [key, algoResults] : groupedResults) {
        for (const auto& result : algoResults) {
            cout << setw(20) << left << result.algorithmName
                 << setw(10) << right << result.dataType
                 << setw(15) << right << result.dataSize
                 << setw(15) << right << fixed << setprecision(6) << result.timeSeconds
                 << setw(20) << right << formatMemory(result.memoryUsageBytes)
                 << setw(20) << right << formatMemory(result.peakMemoryBytes)
                 << setw(10) << right << (result.isSorted ? "是" : "否") << endl;
        }
    }

    // 性能总结
    cout << "\n" << string(120, '=') << endl;
    cout << "性能总结" << endl;
    cout << string(120, '=') << "\n" << endl;

    // 按算法计算平均性能
    map<string, vector<double>> timeStats;
    map<string, vector<double>> memoryStats;

    for (const auto& result : results) {
        timeStats[result.algorithmName].push_back(result.timeSeconds);
        memoryStats[result.algorithmName].push_back(result.memoryUsageBytes / 1024.0 / 1024.0); // MB
    }

    cout << setw(20) << left << "算法"
         << setw(20) << right << "平均时间(秒)"
         << setw(20) << right << "最慢时间(秒)"
         << setw(20) << right << "平均内存(MB)"
         << setw(20) << right << "最大内存(MB)" << endl;
    cout << string(100, '-') << endl;

    for (const auto& [algo, times] : timeStats) {
        double avgTime = 0, maxTime = 0;
        double avgMem = 0, maxMem = 0;

        for (double t : times) {
            avgTime += t;
            maxTime = max(maxTime, t);
        }
        avgTime /= times.size();

        if (memoryStats.find(algo) != memoryStats.end()) {
            for (double m : memoryStats[algo]) {
                avgMem += m;
                maxMem = max(maxMem, m);
            }
            avgMem /= memoryStats[algo].size();
        }

        cout << setw(20) << left << algo
             << setw(20) << right << fixed << setprecision(6) << avgTime
             << setw(20) << right << fixed << setprecision(6) << maxTime
             << setw(20) << right << fixed << setprecision(2) << avgMem
             << setw(20) << right << fixed << setprecision(2) << maxMem << endl;
    }
}

// 生成CSV报告
void Benchmark::generateCSVReport(const vector<PerformanceResult>& results,
                                 const string& filename) {

    ofstream csvFile(filename);
    if (!csvFile) {
        cerr << "无法创建CSV文件: " << filename << endl;
        return;
    }

    // 写入CSV头部
    csvFile << "Algorithm,DataType,DataSize,TimeSeconds,MemoryUsageBytes,PeakMemoryBytes,IsSorted" << endl;

    // 写入数据
    for (const auto& result : results) {
        csvFile << result.algorithmName << ","
                << result.dataType << ","
                << result.dataSize << ","
                << fixed << setprecision(6) << result.timeSeconds << ","
                << result.memoryUsageBytes << ","
                << result.peakMemoryBytes << ","
                << (result.isSorted ? "true" : "false") << endl;
    }

    csvFile.close();
    cout << "CSV报告已生成: " << filename << endl;
}

// 格式化内存大小
string Benchmark::formatMemory(size_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    double size = bytes;

    while (size >= 1024 && unitIndex < 4) {
        size /= 1024;
        unitIndex++;
    }

    stringstream ss;
    ss << fixed << setprecision(2) << size << " " << units[unitIndex];
    return ss.str();
}

// 格式化时间
string Benchmark::formatTime(double seconds) {
    if (seconds < 1e-6) {
        return to_string(seconds * 1e9) + " ns";
    } else if (seconds < 1e-3) {
        return to_string(seconds * 1e6) + "  s";
    } else if (seconds < 1) {
        return to_string(seconds * 1e3) + " ms";
    } else {
        return to_string(seconds) + " s";
    }
}
