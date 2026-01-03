#ifndef MEMORY_MONITOR_H
#define MEMORY_MONITOR_H

#include <string>

class MemoryMonitor {
public:
    // 开始监控内存使用
    static void start();

    // 停止监控并更新峰值
    static void stop();

    // 获取当前内存使用量（相对于start时的增量）
    static size_t getCurrentUsage();

    // 获取峰值内存使用量
    static size_t getPeakUsage();

    // 获取当前总内存使用量（绝对量）
    static size_t getTotalUsage();

    // 获取峰值总内存使用量（绝对量）
    static size_t getTotalPeakUsage();

    // 重置监控状态
    static void reset();

    // 格式化内存大小为人类可读的字符串
    static std::string format(size_t bytes);

    // 获取进程句柄（用于外部使用）
    static void* getCurrentProcessHandle();

    // 获取详细内存信息
    static void printMemoryInfo();

private:
    // 获取当前进程内存使用量的内部实现
    static size_t getCurrentMemoryUsageImpl();

    // 获取当前进程峰值内存使用量的内部实现
    static size_t getCurrentPeakMemoryUsageImpl();
};

#endif // MEMORY_MONITOR_H
