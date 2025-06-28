#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

// 日志队列
template <typename T>
class LockQueue
{
public:
    // 多个worker线程都会写日志queue
    void Push(const T& t){
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(t);
        m_cond.notify_one();
    }
    T Pop(){
        std::unique_lock<std::mutex> lock(m_mutex);

        // while 防止虚假唤醒。
        while (m_queue.empty()) {
            // 日志为空，wait
            m_cond.wait(lock);
        }

        T t = m_queue.front();
        m_queue.pop();
        return t;
    }
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;

};