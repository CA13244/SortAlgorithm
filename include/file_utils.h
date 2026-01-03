#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <vector>
#include <cstdint>

class FileUtils {
public:
    // 创建目录
    static bool createDirectory(const std::string& path);

    // 检查目录是否存在
    static bool directoryExists(const std::string& path);

    // 获取文件大小
    static int64_t getFileSize(const std::string& filename);

    // 读取二进制文件
    template<typename T>
    static std::vector<T> readBinaryFile(const std::string& filename);

    // 写入二进制文件
    template<typename T>
    static void writeBinaryFile(const std::string& filename, const std::vector<T>& data);

    // 读取文本文件
    static std::vector<std::string> readTextFile(const std::string& filename);

    // 写入文本文件
    static void writeTextFile(const std::string& filename, const std::vector<std::string>& data);

    // 清空目录
    static bool cleanDirectory(const std::string& path);

    // 删除文件
    static bool deleteFile(const std::string& filename);

    // 获取临时文件名
    static std::string getTempFilename(const std::string& prefix, int index);
};

#endif // FILE_UTILS_H
