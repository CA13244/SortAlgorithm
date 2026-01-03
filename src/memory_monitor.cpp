#include "memory_monitor.h"

// Windows特定头文件和库
#ifdef _WIN32
    #include <windows.h>
    #include <psapi.h>
#endif

#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>

// 只在MSVC编译器中使用pragma指令
#ifdef _MSC_VER
    #pragma comment(lib, "psapi.lib")
#endif

using namespace std;

namespace {
    size_t startMemory = 0;
    size_t startPeakMemory = 0;
    size_t peakMemory = 0;
    bool isMonitoring = false;

#ifdef _WIN32
    HANDLE hProcess = NULL;
#endif
}

// 获取当前进程内存使用量的内部实现
size_t MemoryMonitor::getCurrentMemoryUsageImpl() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize; // 工作集大小（实际使用的物理内存）
    }
#endif
    return 0;
}

// 获取当前进程峰值内存使用量的内部实现
size_t MemoryMonitor::getCurrentPeakMemoryUsageImpl() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.PeakWorkingSetSize; // 峰值工作集大小
    }
#endif
    return 0;
}

void MemoryMonitor::start() {
    if (isMonitoring) {
        reset();
    }

#ifdef _WIN32
    // 获取当前进程句柄
    hProcess = GetCurrentProcess();
#endif

    // 获取起始内存使用量
    startMemory = getCurrentMemoryUsageImpl();
    startPeakMemory = getCurrentPeakMemoryUsageImpl();
    peakMemory = startMemory;
    isMonitoring = true;

    cout << "内存监控已启动" << endl;
    cout << "初始内存使用: " << format(startMemory) << endl;
}

void MemoryMonitor::stop() {
    if (!isMonitoring) return;

    size_t current = getCurrentMemoryUsageImpl();
    if (current > peakMemory) {
        peakMemory = current;
    }

    cout << "内存监控已停止" << endl;
    cout << "峰值内存增加量: " << format(getPeakUsage()) << endl;
}

size_t MemoryMonitor::getCurrentUsage() {
    if (!isMonitoring) return 0;

    size_t current = getCurrentMemoryUsageImpl();
    if (current < startMemory) {
        return 0; // 不应该发生，但以防万一
    }

    return current - startMemory;
}

size_t MemoryMonitor::getPeakUsage() {
    if (!isMonitoring) return 0;

    size_t currentPeak = peakMemory;
    if (currentPeak < startMemory) {
        return 0;
    }

    return currentPeak - startMemory;
}

size_t MemoryMonitor::getTotalUsage() {
    return getCurrentMemoryUsageImpl();
}

size_t MemoryMonitor::getTotalPeakUsage() {
    return getCurrentPeakMemoryUsageImpl();
}

void MemoryMonitor::reset() {
    startMemory = 0;
    startPeakMemory = 0;
    peakMemory = 0;
    isMonitoring = false;

#ifdef _WIN32
    hProcess = NULL;
#endif
}

string MemoryMonitor::format(size_t bytes) {
    if (bytes == 0) return "0 B";

    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    double size = static_cast<double>(bytes);

    while (size >= 1024.0 && unitIndex < 4) {
        size /= 1024.0;
        unitIndex++;
    }

    stringstream ss;
    ss << fixed << setprecision(2) << size << " " << units[unitIndex];
    return ss.str();
}

void* MemoryMonitor::getCurrentProcessHandle() {
#ifdef _WIN32
    return GetCurrentProcess();
#else
    return nullptr;
#endif
}

void MemoryMonitor::printMemoryInfo() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        cout << "\n=== 进程内存信息 ===" << endl;
        cout << "工作集大小: " << format(pmc.WorkingSetSize) << endl;
        cout << "峰值工作集大小: " << format(pmc.PeakWorkingSetSize) << endl;
        cout << "页面文件使用: " << format(pmc.PagefileUsage) << endl;
        cout << "峰值页面文件使用: " << format(pmc.PeakPagefileUsage) << endl;

        // 如果是扩展版本，可以获取更多信息
        PROCESS_MEMORY_COUNTERS_EX pmcEx;
        if (GetProcessMemoryInfo(GetCurrentProcess(),
                                (PROCESS_MEMORY_COUNTERS*)&pmcEx, sizeof(pmcEx))) {
            cout << "私有工作集: " << format(pmcEx.PrivateUsage) << endl;
        }

        // 获取系统内存信息
        MEMORYSTATUSEX memStatus;
        memStatus.dwLength = sizeof(memStatus);
        if (GlobalMemoryStatusEx(&memStatus)) {
            cout << "\n=== 系统内存信息 ===" << endl;
            cout << "总物理内存: " << format(memStatus.ullTotalPhys) << endl;
            cout << "可用物理内存: " << format(memStatus.ullAvailPhys) << endl;
            cout << "已用物理内存: " << format(memStatus.ullTotalPhys - memStatus.ullAvailPhys) << endl;
            cout << "使用率: " << setprecision(1)
                 << (100.0 * (memStatus.ullTotalPhys - memStatus.ullAvailPhys) / memStatus.ullTotalPhys)
                 << "%" << endl;
        }
    } else {
        cout << "无法获取内存信息" << endl;
    }
#else
    cout << "内存信息仅支持Windows平台" << endl;
#endif
}

