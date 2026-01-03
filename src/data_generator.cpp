#include "data_generator.h"
#include <fstream>
#include <iostream>
#include <random>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <cfloat>

using namespace std;

// DataGenerator 实现
int64_t DataGenerator::getSizeValue(DataSize size) {
    switch (size) {
        case DataSize::SIZE_1M:   return 1000000;
        case DataSize::SIZE_10M:  return 10000000;
        case DataSize::SIZE_100M: return 100000000;
        case DataSize::SIZE_500M: return 500000000;
        case DataSize::SIZE_1G:   return 1000000000;
        default: return 1000000;
    }
}

std::mt19937_64& DataGenerator::getRandomEngine() {
    static thread_local std::mt19937_64 engine(
        chrono::steady_clock::now().time_since_epoch().count()
    );
    return engine;
}

void DataGenerator::generateIntegerData(const std::string& filename,
                                        int64_t size,
                                        bool allowDuplicates) {

    ofstream outFile(filename, ios::binary);
    if (!outFile) {
        throw runtime_error("无法打开文件：" + filename);
    }

    const int64_t BATCH_SIZE = 1000000;  // 每批100万个
    auto& engine = getRandomEngine();

    if (allowDuplicates) {
        // 允许重复的随机数
        uniform_int_distribution<int64_t> dist(-1000000000, 1000000000);

        for (int64_t i = 0; i < size; i += BATCH_SIZE) {
            int64_t batchSize = min(BATCH_SIZE, size - i);
            vector<int64_t> batch(batchSize);

            for (int64_t j = 0; j < batchSize; j++) {
                batch[j] = dist(engine);
            }

            outFile.write(reinterpret_cast<const char*>(batch.data()),
                         batchSize * sizeof(int64_t));

            if (i > 0 && i % (10 * BATCH_SIZE) == 0) {
                cout << "生成整数数据进度: " << (i * 100.0 / size) << "%" << endl;
            }
        }
    } else {
        // 生成不重复的随机数（使用哈希）
        for (int64_t i = 0; i < size; i += BATCH_SIZE) {
            int64_t batchSize = min(BATCH_SIZE, size - i);
            vector<int64_t> batch(batchSize);

            // 使用哈希函数生成伪随机但唯一的数字
            for (int64_t j = 0; j < batchSize; j++) {
                int64_t index = i + j;
                // 使用乘法哈希
                const int64_t multiplier = 6364136223846793005ULL;
                const int64_t increment = 1442695040888963407ULL;
                batch[j] = index * multiplier + increment;
            }

            // 打乱批次内顺序
            shuffle(batch.begin(), batch.end(), engine);

            outFile.write(reinterpret_cast<const char*>(batch.data()),
                         batchSize * sizeof(int64_t));

            if (i > 0 && i % (10 * BATCH_SIZE) == 0) {
                cout << "生成整数数据进度: " << (i * 100.0 / size) << "%" << endl;
            }
        }
    }

    outFile.close();
    cout << "整数数据生成完成: " << filename << " (大小: " << size << ")" << endl;
}

void DataGenerator::generateDoubleData(const std::string& filename,
                                       int64_t size,
                                       bool allowDuplicates) {

    ofstream outFile(filename, ios::binary);
    if (!outFile) {
        throw runtime_error("无法打开文件: " + filename);
    }

    const int64_t BATCH_SIZE = 1000000;
    auto& engine = getRandomEngine();
    uniform_real_distribution<double> dist(-1000000.0, 1000000.0);

    for (int64_t i = 0; i < size; i += BATCH_SIZE) {
        int64_t batchSize = min(BATCH_SIZE, size - i);
        vector<double> batch(batchSize);

        for (int64_t j = 0; j < batchSize; j++) {
            batch[j] = dist(engine);
        }

        outFile.write(reinterpret_cast<const char*>(batch.data()),
                     batchSize * sizeof(double));

        if (i > 0 && i % (10 * BATCH_SIZE) == 0) {
            cout << "生成浮点数数据进度: " << (i * 100.0 / size) << "%" << endl;
        }
    }

    outFile.close();
    cout << "浮点数数据生成完成: " << filename << " (大小: " << size << ")" << endl;
}

std::string DataGenerator::generateRandomString(size_t minLen, size_t maxLen) {
    static const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    auto& engine = getRandomEngine();
    uniform_int_distribution<size_t> lengthDist(minLen, maxLen);
    uniform_int_distribution<size_t> charDist(0, sizeof(charset) - 2);

    size_t length = lengthDist(engine);
    string str;
    str.reserve(length);

    for (size_t i = 0; i < length; i++) {
        str += charset[charDist(engine)];
    }

    return str;
}

void DataGenerator::generateStringData(const std::string& filename,
                                       int64_t size,
                                       bool allowDuplicates) {

    ofstream outFile(filename);
    if (!outFile) {
        throw runtime_error("无法打开文件: " + filename);
    }

    const int64_t BATCH_SIZE = 100000;
    const size_t MIN_LEN = 5;
    const size_t MAX_LEN = 50;

    for (int64_t i = 0; i < size; i++) {
        string randomStr = generateRandomString(MIN_LEN, MAX_LEN);
        outFile << randomStr << "\n";

        if (i > 0 && i % (10 * BATCH_SIZE) == 0) {
            cout << "生成字符串数据进度: " << (i * 100.0 / size) << "%" << endl;
        }
    }

    outFile.close();
    cout << "字符串数据生成完成: " << filename << " (大小: " << size << ")" << endl;
}

// 从二进制文件读取整数数据
std::vector<int64_t> DataGenerator::readIntegerData(const std::string& filename) {
    ifstream inFile(filename, ios::binary | ios::ate);
    if (!inFile) {
        throw runtime_error("无法打开文件: " + filename);
    }

    // 获取文件大小
    streamsize fileSize = inFile.tellg();
    inFile.seekg(0, ios::beg);

    // 计算元素数量
    size_t elementCount = fileSize / sizeof(int64_t);
    vector<int64_t> data(elementCount);

    // 读取数据
    inFile.read(reinterpret_cast<char*>(data.data()), fileSize);
    inFile.close();

    cout << "读取整数数据: " << filename << " (" << elementCount << " 个元素)" << endl;
    return data;
}

// 从二进制文件读取浮点数数据
std::vector<double> DataGenerator::readDoubleData(const std::string& filename) {
    ifstream inFile(filename, ios::binary | ios::ate);
    if (!inFile) {
        throw runtime_error("无法打开文件: " + filename);
    }

    // 获取文件大小
    streamsize fileSize = inFile.tellg();
    inFile.seekg(0, ios::beg);

    // 计算元素数量
    size_t elementCount = fileSize / sizeof(double);
    vector<double> data(elementCount);

    // 读取数据
    inFile.read(reinterpret_cast<char*>(data.data()), fileSize);
    inFile.close();

    cout << "读取浮点数数据: " << filename << " (" << elementCount << " 个元素)" << endl;
    return data;
}

// 从文本文件读取字符串数据
std::vector<std::string> DataGenerator::readStringData(const std::string& filename) {
    ifstream inFile(filename);
    if (!inFile) {
        throw runtime_error("无法打开文件: " + filename);
    }

    vector<string> data;
    string line;

    while (getline(inFile, line)) {
        data.push_back(line);
    }

    inFile.close();
    cout << "读取字符串数据: " << filename << " (" << data.size() << " 个元素)" << endl;
    return data;
}

// 验证整数文件是否已排序
bool DataGenerator::verifyIntegerSorted(const std::string& filename) {
    ifstream inFile(filename, ios::binary);
    if (!inFile) {
        throw runtime_error("无法打开文件: " + filename);
    }

    const size_t BATCH_SIZE = 1000000;
    vector<int64_t> buffer(BATCH_SIZE);
    int64_t previous = LLONG_MIN;
    size_t totalRead = 0;

    while (inFile) {
        inFile.read(reinterpret_cast<char*>(buffer.data()), BATCH_SIZE * sizeof(int64_t));
        size_t count = inFile.gcount() / sizeof(int64_t);

        for (size_t i = 0; i < count; i++) {
            if (buffer[i] < previous) {
                cout << "在位置 " << (totalRead + i) << " 发现无序: "
                     << previous << " > " << buffer[i] << endl;
                inFile.close();
                return false;
            }
            previous = buffer[i];
        }

        totalRead += count;
        if (count < BATCH_SIZE) break;
    }

    inFile.close();
    cout << "整数文件验证完成: " << filename << " (有序: 是)" << endl;
    return true;
}

// 验证浮点数文件是否已排序
bool DataGenerator::verifyDoubleSorted(const std::string& filename) {
    ifstream inFile(filename, ios::binary);
    if (!inFile) {
        throw runtime_error("无法打开文件: " + filename);
    }

    const size_t BATCH_SIZE = 1000000;
    vector<double> buffer(BATCH_SIZE);
    double previous = -DBL_MAX;
    size_t totalRead = 0;

    while (inFile) {
        inFile.read(reinterpret_cast<char*>(buffer.data()), BATCH_SIZE * sizeof(double));
        size_t count = inFile.gcount() / sizeof(double);

        for (size_t i = 0; i < count; i++) {
            if (buffer[i] < previous) {
                cout << "在位置 " << (totalRead + i) << " 发现无序: "
                     << previous << " > " << buffer[i] << endl;
                inFile.close();
                return false;
            }
            previous = buffer[i];
        }

        totalRead += count;
        if (count < BATCH_SIZE) break;
    }

    inFile.close();
    cout << "浮点数文件验证完成: " << filename << " (有序: 是)" << endl;
    return true;
}

// 验证字符串文件是否已排序
bool DataGenerator::verifyStringSorted(const std::string& filename) {
    ifstream inFile(filename);
    if (!inFile) {
        throw runtime_error("无法打开文件: " + filename);
    }

    string previous;
    string current;
    size_t position = 0;

    if (getline(inFile, previous)) {
        position++;

        while (getline(inFile, current)) {
            position++;
            if (current < previous) {
                cout << "在位置 " << position << " 发现无序: \""
                     << previous << "\" > \"" << current << "\"" << endl;
                inFile.close();
                return false;
            }
            previous = current;
        }
    }

    inFile.close();
    cout << "字符串文件验证完成: " << filename << " (有序: 是)" << endl;
    return true;
}

// 生成更真实的整数数据（模拟真实世界分布）
void DataGenerator::generateRealisticIntegerData(const std::string& filename, int64_t size) {
    ofstream outFile(filename, ios::binary);
    if (!outFile) {
        throw runtime_error("无法打开文件: " + filename);
    }

    const int64_t BATCH_SIZE = 1000000;
    auto& engine = getRandomEngine();

    // 使用多种分布组合生成更真实的数据
    // 1. 正态分布（模拟大多数数据集中在平均值附近）
    normal_distribution<double> normalDist(0.0, 10000.0);

    // 2. 均匀分布（模拟随机数据）
    uniform_int_distribution<int64_t> uniformDist(-1000000, 1000000);

    // 3. 指数分布（模拟长尾数据）
    exponential_distribution<double> expDist(0.0001);

    for (int64_t i = 0; i < size; i += BATCH_SIZE) {
        int64_t batchSize = min(BATCH_SIZE, size - i);
        vector<int64_t> batch(batchSize);

        for (int64_t j = 0; j < batchSize; j++) {
            // 随机选择一种分布生成数据
            double choice = uniform_int_distribution<>(0, 100)(engine) / 100.0;

            if (choice < 0.6) {
                // 60% 的数据使用正态分布
                batch[j] = static_cast<int64_t>(normalDist(engine));
            } else if (choice < 0.9) {
                // 30% 的数据使用均匀分布
                batch[j] = uniformDist(engine);
            } else {
                // 10% 的数据使用指数分布（长尾）
                batch[j] = static_cast<int64_t>(expDist(engine) * 10000);
            }
        }

        outFile.write(reinterpret_cast<const char*>(batch.data()),
                     batchSize * sizeof(int64_t));

        if (i > 0 && i % (10 * BATCH_SIZE) == 0) {
            cout << "生成真实整数数据进度: " << (i * 100.0 / size) << "%" << endl;
        }
    }

    outFile.close();
    cout << "真实整数数据生成完成: " << filename << endl;
}

// 生成更真实的浮点数数据
void DataGenerator::generateRealisticDoubleData(const std::string& filename, int64_t size) {
    ofstream outFile(filename, ios::binary);
    if (!outFile) {
        throw runtime_error("无法打开文件: " + filename);
    }

    const int64_t BATCH_SIZE = 1000000;
    auto& engine = getRandomEngine();

    // 使用多种分布
    normal_distribution<double> normalDist(0.0, 1.0);
    uniform_real_distribution<double> uniformDist(-1000.0, 1000.0);
    exponential_distribution<double> expDist(1.0);

    for (int64_t i = 0; i < size; i += BATCH_SIZE) {
        int64_t batchSize = min(BATCH_SIZE, size - i);
        vector<double> batch(batchSize);

        for (int64_t j = 0; j < batchSize; j++) {
            double choice = uniform_int_distribution<>(0, 100)(engine) / 100.0;

            if (choice < 0.7) {
                // 70% 正态分布
                batch[j] = normalDist(engine);
            } else if (choice < 0.95) {
                // 25% 均匀分布
                batch[j] = uniformDist(engine);
            } else {
                // 5% 指数分布（异常值）
                batch[j] = expDist(engine) * 100;
            }
        }

        outFile.write(reinterpret_cast<const char*>(batch.data()),
                     batchSize * sizeof(double));

        if (i > 0 && i % (10 * BATCH_SIZE) == 0) {
            cout << "生成真实浮点数数据进度: " << (i * 100.0 / size) << "%" << endl;
        }
    }

    outFile.close();
    cout << "真实浮点数数据生成完成: " << filename << endl;
}

// 生成更真实的字符串数据
void DataGenerator::generateRealisticStringData(const std::string& filename, int64_t size) {
    ofstream outFile(filename);
    if (!outFile) {
        throw runtime_error("无法打开文件: " + filename);
    }

    auto& engine = getRandomEngine();

    // 模拟真实世界的字符串数据
    vector<string> firstName = {"John", "Jane", "Bob", "Alice", "Charlie", "David", "Eve", "Frank"};
    vector<string> lastName = {"Smith", "Johnson", "Williams", "Brown", "Jones", "Miller", "Davis", "Wilson"};
    vector<string> cities = {"New York", "London", "Tokyo", "Paris", "Beijing", "Sydney", "Berlin", "Moscow"};
    vector<string> domains = {"gmail.com", "yahoo.com", "hotmail.com", "outlook.com"};

    uniform_int_distribution<size_t> firstDist(0, firstName.size() - 1);
    uniform_int_distribution<size_t> lastDist(0, lastName.size() - 1);
    uniform_int_distribution<size_t> cityDist(0, cities.size() - 1);
    uniform_int_distribution<size_t> domainDist(0, domains.size() - 1);
    uniform_int_distribution<int> yearDist(1980, 2020);
    uniform_int_distribution<int> numDist(1, 999);

    for (int64_t i = 0; i < size; i++) {
        stringstream ss;

        // 随机生成一种类型的字符串
        int type = uniform_int_distribution<>(0, 3)(engine);

        switch (type) {
            case 0: // 姓名
                ss << firstName[firstDist(engine)] << " " << lastName[lastDist(engine)];
                break;
            case 1: // 电子邮件
                ss << firstName[firstDist(engine)] << "."
                   << lastName[lastDist(engine)] << numDist(engine)
                   << "@" << domains[domainDist(engine)];
                break;
            case 2: // 地址
                ss << numDist(engine) << " " << lastName[lastDist(engine)]
                   << " St, " << cities[cityDist(engine)];
                break;
            case 3: // 日期
                ss << yearDist(engine) << "-"
                   << setw(2) << setfill('0') << uniform_int_distribution<>(1, 12)(engine)
                   << "-" << setw(2) << setfill('0') << uniform_int_distribution<>(1, 28)(engine);
                break;
        }

        outFile << ss.str() << "\n";

        if (i > 0 && i % 100000 == 0) {
            cout << "生成真实字符串数据进度: " << (i * 100.0 / size) << "%" << endl;
        }
    }

    outFile.close();
    cout << "真实字符串数据生成完成: " << filename << endl;
}

// 模板方法实现
template<typename T>
bool DataGenerator::isSorted(const std::vector<T>& arr,
                            std::function<bool(const T&, const T&)> comp) {
    if (arr.size() <= 1) return true;

    for (size_t i = 1; i < arr.size(); i++) {
        if (comp(arr[i], arr[i - 1])) {
            return false;
        }
    }
    return true;
}

// 显式实例化常用类型
template bool DataGenerator::isSorted<int64_t>(const std::vector<int64_t>&,
                                              std::function<bool(const int64_t&, const int64_t&)>);
template bool DataGenerator::isSorted<double>(const std::vector<double>&,
                                             std::function<bool(const double&, const double&)>);
template bool DataGenerator::isSorted<std::string>(const std::vector<std::string>&,
                                                  std::function<bool(const std::string&, const std::string&)>);
