#include "file_utils.h"
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

using namespace std;

bool FileUtils::createDirectory(const string& path) {
#ifdef _WIN32
    return _mkdir(path.c_str()) == 0;
#else
    return mkdir(path.c_str(), 0777) == 0;
#endif
}

bool FileUtils::directoryExists(const string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return false;
    }
    return (info.st_mode & S_IFDIR);
}

int64_t FileUtils::getFileSize(const string& filename) {
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

template<typename T>
vector<T> FileUtils::readBinaryFile(const string& filename) {
    ifstream inFile(filename, ios::binary | ios::ate);
    if (!inFile) {
        throw runtime_error("无法打开文件: " + filename);
    }

    streamsize fileSize = inFile.tellg();
    inFile.seekg(0, ios::beg);

    size_t elementCount = fileSize / sizeof(T);
    vector<T> data(elementCount);

    inFile.read(reinterpret_cast<char*>(data.data()), fileSize);
    inFile.close();

    return data;
}

template<typename T>
void FileUtils::writeBinaryFile(const string& filename, const vector<T>& data) {
    ofstream outFile(filename, ios::binary);
    if (!outFile) {
        throw runtime_error("无法打开文件: " + filename);
    }

    outFile.write(reinterpret_cast<const char*>(data.data()),
                 data.size() * sizeof(T));
    outFile.close();
}

vector<string> FileUtils::readTextFile(const string& filename) {
    ifstream inFile(filename);
    if (!inFile) {
        throw runtime_error("无法打开文件: " + filename);
    }

    vector<string> lines;
    string line;

    while (getline(inFile, line)) {
        lines.push_back(line);
    }

    inFile.close();
    return lines;
}

void FileUtils::writeTextFile(const string& filename, const vector<string>& data) {
    ofstream outFile(filename);
    if (!outFile) {
        throw runtime_error("无法打开文件: " + filename);
    }

    for (const auto& line : data) {
        outFile << line << "\n";
    }

    outFile.close();
}

bool FileUtils::cleanDirectory(const string& path) {
    if (!directoryExists(path)) {
        return true; // 目录不存在，视为清理成功
    }

#ifdef _WIN32
    string command = "del /Q \"" + path + "\\*\"";
    system(command.c_str());
    return true;
#else
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        return false;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        string name = entry->d_name;
        if (name == "." || name == "..") {
            continue;
        }

        string fullPath = path + "/" + name;
        remove(fullPath.c_str());
    }

    closedir(dir);
    return true;
#endif
}

bool FileUtils::deleteFile(const string& filename) {
    return remove(filename.c_str()) == 0;
}

string FileUtils::getTempFilename(const string& prefix, int index) {
    return "temp/" + prefix + "_" + to_string(index) + ".dat";
}

// 显式实例化模板
template vector<int64_t> FileUtils::readBinaryFile<int64_t>(const string&);
template vector<double> FileUtils::readBinaryFile<double>(const string&);
template void FileUtils::writeBinaryFile<int64_t>(const string&, const vector<int64_t>&);
template void FileUtils::writeBinaryFile<double>(const string&, const vector<double>&);
