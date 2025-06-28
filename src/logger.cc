#include "logger.h"
#include <cstdio>
#include <iostream>
#include <thread>
#include <time.h>

Logger& Logger::getInstance()
{
    // 局部静态变量的初始化，由C++11标准保证线程安全，隐式插入同步锁。首次调用时才初始化
    static Logger logger;
    return logger;
}

Logger::Logger()
{
    // 启动专门的写日志线程
    std::thread writeLogTask([&](){
        // 循环写日志
        // for(;;)的语法结构更简单，编译器通常会将其直接优化为无条件跳转指令（如jmp），不涉及条件判断。而while(true)在理论上需要每次循环都检查条件（如mov eax,1和test eax,eax），尽管现代编译器（如GCC、Clang）会优化为相同的汇编代码，但在某些旧版本或低优化级别下可能保留冗余指令
        for(;;){
            // 获取当前时间，取日志信息，写入相应的日志文件当中 a+
            time_t now = time(nullptr);
            struct tm* now_tm = localtime(&now);
            
            char file_name[128];
            sprintf(file_name, "%d-%d-%d-log.txt", now_tm->tm_year + 1900, now_tm->tm_mon + 1, now_tm->tm_mday);
            FILE* fp = fopen(file_name, "a+");
            if(fp == nullptr){
                std::cout << "open log file " << file_name << " error!" << std::endl;
                exit(EXIT_FAILURE);
            }

            std::string msg = m_lckQue.Pop();

            // 时分秒
            char time_buf[128];
            sprintf(time_buf, "%d:%d:%d => ",
                    now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);
            msg.insert(0, time_buf);
            msg.append("\n");
            fputs(msg.c_str(), fp); //fputs 首先将数据写入用户空间的缓冲区（由C标准库管理）。缓冲区满或显式调用 fflush 时，数据才会提交到内核缓冲区
            fclose(fp);
        }
    });
    // 设置分离线程
    writeLogTask.detach();
}


// 设置日志级别
void Logger::SetLogLevel(LogLevel level)
{
    m_loglevel = level;
}

// 把日志写入缓冲区中
void Logger::Log(std::string msg)
{
    m_lckQue.Push(msg);
}



