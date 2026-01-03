#include "data_generator.h"
#include "shell_sort.h"
#include "quick_sort.h"
#include "merge_sort.h"
#include "radix_sort.h"
#include "external_sort.h"
#include "benchmark.h"
#include "memory_monitor.h"
#include "file_utils.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <fstream>

using namespace std;
using namespace chrono;

// 创建必要的目录
void createDirectories() {
    vector<string> dirs = {"data", "output", "temp", "reports"};

    for (const auto& dir : dirs) {
        if (!FileUtils::directoryExists(dir)) {
            if (FileUtils::createDirectory(dir)) {
                cout << "创建目录: " << dir << endl;
            } else {
                cout << "警告: 无法创建目录 " << dir << endl;
            }
        }
    }
}

// 显示主菜单
void showMainMenu() {
    cout << "\n排序算法性能测试系统" << endl;
    cout << "======================" << endl;
    cout << "1. 生成测试数据" << endl;
    cout << "2. 运行算法测试" << endl;
    cout << "3. 清理测试文件" << endl;
    cout << "4. 查看内存使用" << endl;
    cout << "5. 验证排序结果" << endl;
    cout << "6. 生成真实数据" << endl;
    cout << "7. 运行完整性能测试" << endl;
    cout << "8. 显示系统信息" << endl;
    cout << "0. 退出系统" << endl;
    cout << "请选择: ";
}

// 显示数据生成菜单
void showDataGenerationMenu() {
    cout << "\n生成测试数据" << endl;
    cout << "==============" << endl;
    cout << "选择数据类型:" << endl;
    cout << "1. 整型数据 (int64_t)" << endl;
    cout << "2. 浮点数据 (double)" << endl;
    cout << "3. 字符串数据" << endl;
    cout << "0. 返回主菜单" << endl;
    cout << "请选择: ";
}

// 显示数据规模菜单
void showDataSizeMenu() {
    cout << "\n选择数据规模:" << endl;
    cout << "1. 1M (1,000,000)" << endl;
    cout << "2. 10M (10,000,000)" << endl;
    cout << "3. 100M (100,000,000)" << endl;
    cout << "4. 自定义大小" << endl;
    cout << "0. 返回" << endl;
    cout << "请选择: ";
}

// 显示算法选择菜单
void showAlgorithmMenu() {
    cout << "\n选择排序算法:" << endl;
    cout << "1. 希尔排序 (Shell Sort)" << endl;
    cout << "2. 快速排序 (Quick Sort)" << endl;
    cout << "3. 归并排序 (Merge Sort)" << endl;
    cout << "4. 基数排序 (Radix Sort)" << endl;
    cout << "5. 外排序 (External Sort)" << endl;
    cout << "6. 所有算法" << endl;
    cout << "0. 返回" << endl;
    cout << "请选择: ";
}

// 生成测试数据
void generateTestData() {
    int typeChoice, sizeChoice;

    showDataGenerationMenu();
    cin >> typeChoice;

    if (typeChoice == 0) return;

    string dataType;
    switch (typeChoice) {
        case 1: dataType = "int"; break;
        case 2: dataType = "double"; break;
        case 3: dataType = "string"; break;
        default:
            cout << "无效选择" << endl;
            return;
    }

    showDataSizeMenu();
    cin >> sizeChoice;

    if (sizeChoice == 0) return;

    int64_t dataSize;
    switch (sizeChoice) {
        case 1: dataSize = 1000000; break;
        case 2: dataSize = 10000000; break;
        case 3: dataSize = 100000000; break;
        case 4: {
            cout << "输入数据大小: ";
            cin >> dataSize;
            break;
        }
        default:
            cout << "无效选择" << endl;
            return;
    }

    // 生成文件名
    string filename = "data/test_" + dataType + "_" + to_string(dataSize) + ".dat";

    cout << "\n开始生成数据..." << endl;
    cout << "文件: " << filename << endl;
    cout << "类型: " << dataType << endl;
    cout << "大小: " << dataSize << endl;

    try {
        if (dataType == "int") {
            DataGenerator::generateIntegerData(filename, dataSize, false);
        } else if (dataType == "double") {
            DataGenerator::generateDoubleData(filename, dataSize, false);
        } else if (dataType == "string") {
            DataGenerator::generateStringData(filename, dataSize, false);
        }
        cout << "数据生成完成!" << endl;
    } catch (const exception& e) {
        cerr << "数据生成失败: " << e.what() << endl;
    }
}

// 生成真实数据
void generateRealisticData() {
    cout << "\n生成真实数据" << endl;
    cout << "==============" << endl;

    int typeChoice, sizeChoice;

    cout << "选择数据类型:" << endl;
    cout << "1. 真实整数数据" << endl;
    cout << "2. 真实浮点数数据" << endl;
    cout << "3. 真实字符串数据" << endl;
    cout << "0. 返回" << endl;
    cout << "请选择: ";
    cin >> typeChoice;

    if (typeChoice == 0) return;

    showDataSizeMenu();
    cin >> sizeChoice;

    if (sizeChoice == 0) return;

    int64_t dataSize;
    switch (sizeChoice) {
        case 1: dataSize = 1000000; break;
        case 2: dataSize = 10000000; break;
        case 3: dataSize = 100000000; break;
        case 4: {
            cout << "输入数据大小: ";
            cin >> dataSize;
            break;
        }
        default:
            cout << "无效选择" << endl;
            return;
    }

    string typeStr;
    switch (typeChoice) {
        case 1: typeStr = "real_int"; break;
        case 2: typeStr = "real_double"; break;
        case 3: typeStr = "real_string"; break;
        default: return;
    }

    string filename = "data/" + typeStr + "_" + to_string(dataSize) + ".dat";

    cout << "\n开始生成真实数据..." << endl;

    try {
        if (typeChoice == 1) {
            DataGenerator::generateRealisticIntegerData(filename, dataSize);
        } else if (typeChoice == 2) {
            DataGenerator::generateRealisticDoubleData(filename, dataSize);
        } else if (typeChoice == 3) {
            DataGenerator::generateRealisticStringData(filename, dataSize);
        }
        cout << "真实数据生成完成!" << endl;
    } catch (const exception& e) {
        cerr << "数据生成失败: " << e.what() << endl;
    }
}

// 测试单个算法
void testSingleAlgorithm() {
    cout << "\n测试单个算法" << endl;
    cout << "=============" << endl;

    int algoChoice;
    showAlgorithmMenu();
    cin >> algoChoice;

    if (algoChoice == 0) return;

    string algorithm;
    switch (algoChoice) {
        case 1: algorithm = "ShellSort"; break;
        case 2: algorithm = "QuickSort"; break;
        case 3: algorithm = "MergeSort"; break;
        case 4: algorithm = "RadixSort"; break;
        case 5: algorithm = "ExternalSort"; break;
        default:
            cout << "无效选择" << endl;
            return;
    }

    cout << "\n选择数据类型:" << endl;
    cout << "1. 整数" << endl;
    cout << "2. 浮点数" << endl;
    cout << "3. 字符串" << endl;
    cout << "0. 返回" << endl;
    cout << "请选择: ";

    int typeChoice;
    cin >> typeChoice;

    if (typeChoice == 0) return;

    string dataType;
    switch (typeChoice) {
        case 1: dataType = "int"; break;
        case 2: dataType = "double"; break;
        case 3: dataType = "string"; break;
        default: return;
    }

    cout << "\n输入数据大小: ";
    int64_t dataSize;
    cin >> dataSize;

    string inputFile = "data/test_" + dataType + "_" + to_string(dataSize) + ".dat";
    string outputFile = "output/" + algorithm + "_" + dataType + "_" + to_string(dataSize) + ".dat";

    // 检查输入文件是否存在
    ifstream testFile(inputFile);
    if (!testFile) {
        cout << "输入文件不存在，请先生成数据" << endl;
        return;
    }
    testFile.close();

    cout << "\n开始测试 " << algorithm << " 算法..." << endl;
    cout << "输入文件: " << inputFile << endl;
    cout << "输出文件: " << outputFile << endl;

    try {
        PerformanceResult result;

        if (algorithm == "ShellSort") {
            if (dataType == "int") {
                result = Benchmark::testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                                        ShellSort::sortIntegerFile);
            } else if (dataType == "double") {
                result = Benchmark::testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                                        ShellSort::sortDoubleFile);
            } else if (dataType == "string") {
                result = Benchmark::testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                                        ShellSort::sortStringFile);
            }
        }
        else if (algorithm == "QuickSort") {
            if (dataType == "int") {
                result = Benchmark::testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                                        QuickSort::sortIntegerFile);
            } else if (dataType == "double") {
                result = Benchmark::testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                                        QuickSort::sortDoubleFile);
            } else if (dataType == "string") {
                result = Benchmark::testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                                        QuickSort::sortStringFile);
            }
        }
        else if (algorithm == "MergeSort") {
            if (dataType == "int") {
                result = Benchmark::testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                                        MergeSort::sortIntegerFile);
            } else if (dataType == "double") {
                result = Benchmark::testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                                        MergeSort::sortDoubleFile);
            } else if (dataType == "string") {
                result = Benchmark::testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                                        MergeSort::sortStringFile);
            }
        }
        else if (algorithm == "RadixSort") {
            if (dataType == "int") {
                result = Benchmark::testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                                        RadixSort::sortIntegerFile);
            } else if (dataType == "string") {
                result = Benchmark::testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                                        RadixSort::sortStringFile);
            }
        }
        else if (algorithm == "ExternalSort") {
            if (dataType == "int") {
                result = Benchmark::testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                                        ExternalSort::sortIntegerFile);
            } else if (dataType == "double") {
                result = Benchmark::testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                                        ExternalSort::sortDoubleFile);
            } else if (dataType == "string") {
                result = Benchmark::testFileSortAlgorithm(algorithm, inputFile, outputFile, dataType,
                                                        ExternalSort::sortStringFile);
            }
        }

        // 输出结果
        cout << "\n测试结果:" << endl;
        cout << "算法: " << result.algorithmName << endl;
        cout << "数据类型: " << result.dataType << endl;
        cout << "数据大小: " << result.dataSize << endl;
        cout << "时间: " << fixed << setprecision(3) << result.timeSeconds << " 秒" << endl;
        cout << "内存: " << Benchmark::formatMemory(result.memoryUsageBytes) << endl;
        cout << "排序验证: " << (result.isSorted ? "成功" : "失败") << endl;

        cout << "排序结果已保存到: " << outputFile << endl;

    } catch (const exception& e) {
        cerr << "测试失败: " << e.what() << endl;
    }
}

// 验证排序结果
void verifySortResults() {
    cout << "\n验证排序结果" << endl;
    cout << "=============" << endl;

    cout << "输入要验证的文件名: ";
    string filename;
    cin >> filename;

    // 检查文件是否存在
    ifstream testFile(filename);
    if (!testFile) {
        cout << "文件不存在: " << filename << endl;
        return;
    }
    testFile.close();

    cout << "选择数据类型:" << endl;
    cout << "1. 整数" << endl;
    cout << "2. 浮点数" << endl;
    cout << "3. 字符串" << endl;
    cout << "请选择: ";

    int typeChoice;
    cin >> typeChoice;

    bool isSorted = false;
    try {
        if (typeChoice == 1) {
            isSorted = DataGenerator::verifyIntegerSorted(filename);
        } else if (typeChoice == 2) {
            isSorted = DataGenerator::verifyDoubleSorted(filename);
        } else if (typeChoice == 3) {
            isSorted = DataGenerator::verifyStringSorted(filename);
        } else {
            cout << "无效选择" << endl;
            return;
        }

        if (isSorted) {
            cout << "文件已正确排序" << endl;
        } else {
            cout << "文件未正确排序" << endl;
        }
    } catch (const exception& e) {
        cerr << "验证失败: " << e.what() << endl;
    }
}

// 显示系统信息
void showSystemInfo() {
    cout << "\n系统信息" << endl;
    cout << "=========" << endl;
    cout << "排序算法性能测试系统 v1.0" << endl;
    cout << "支持的算法:" << endl;
    cout << "  - 希尔排序 (Shell Sort)" << endl;
    cout << "  - 快速排序 (Quick Sort)" << endl;
    cout << "  - 归并排序 (Merge Sort)" << endl;
    cout << "  - 基数排序 (Radix Sort)" << endl;
    cout << "  - 外排序 (External Sort)" << endl;
    cout << endl;
    cout << "支持的数据类型:" << endl;
    cout << "  - 整数 (int64_t)" << endl;
    cout << "  - 浮点数 (double)" << endl;
    cout << "  - 字符串 (string)" << endl;
    cout << endl;
    cout << "支持的数据规模:" << endl;
    cout << "  - 1M (1,000,000)" << endl;
    cout << "  - 10M (10,000,000)" << endl;
    cout << "  - 100M (100,000,000)" << endl;
    cout << "  - 自定义大小" << endl;
}

int main() {
    cout << "排序算法性能测试系统 v1.0" << endl;
    cout << "==========================" << endl;

    // 创建必要的目录
    createDirectories();

    int choice;
    do {
        showMainMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                generateTestData();
                break;
            case 2:
                testSingleAlgorithm();
                break;
            case 3:
                // 清理测试文件
                FileUtils::cleanDirectory("output");
                FileUtils::cleanDirectory("temp");
                cout << "清理完成！" << endl;
                break;
            case 4:
                // 查看内存使用
                MemoryMonitor::start();
                MemoryMonitor::stop();
                cout << "\n当前内存使用: " << MemoryMonitor::format(MemoryMonitor::getPeakUsage()) << endl;
                break;
            case 5:
                verifySortResults();
                break;
            case 6:
                generateRealisticData();
                break;
            case 7:
                {
                    // 运行完整性能测试
                    auto results = Benchmark::runAllTests();
                    Benchmark::generateReport(results);
                    Benchmark::generateCSVReport(results, "reports/benchmark_results.csv");
                }
                break;
            case 8:
                showSystemInfo();
                break;
            case 0:
                cout << "\n退出系统，再见！" << endl;
                break;
            default:
                cout << "\n无效选择，请重试" << endl;
        }
    } while (choice != 0);

    return 0;
}
