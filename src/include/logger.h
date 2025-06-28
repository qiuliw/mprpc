#pragma once

#include "lockqueue.h"
#include <string>


enum LogLevel {
    INFO, // 普通信息
    ERROR, // 错误信息
};

// Mprpc框架提供的日志系统，单例模式
class Logger {
public:
    // 获取日志类单例
    static Logger& getInstance();

    void SetLogLevel(LogLevel level); // 设置日志级别
    void Log(std::string msg); // 写日志
private:
    int m_loglevel; // 日志级别
    LockQueue<std::string> m_lckQue; // 日志缓冲队列

    Logger();
    Logger(const Logger&) = delete;
    Logger(const Logger&&) = delete;

};

// 定义宏
#define LOG_INFO(logmsgFormat, ...) \
    do { \
        Logger& logger = Logger::getInstance(); \
        logger.SetLogLevel(INFO); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.Log(buf); \
    } while(0);

#define Log_ERROR(logmsgFormat, ...) \
    do { \
        Logger& logger = Logger::getInstance(); \
        logger.SetLogLevel(ERROR); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.Log(buf); \
    } while(0);
