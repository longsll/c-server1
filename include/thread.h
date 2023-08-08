#ifndef __SYLAR_THREAD_H__
#define __SYLAR_THREAD_H__

#include <thread>
#include <functional>
#include <memory>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <atomic>
#include <queue> 
#include "noncopyable.h"
#include "mutex.h"


namespace sylar {


class Thread {
public:
    /// 线程智能指针类型
    typedef std::shared_ptr<Thread> ptr;
    /**
     * @brief 构造函数
     * @param[in] cb 线程执行函数
     * @param[in] name 线程名称
     */
    Thread(std::function<void()> cb, const std::string& name);
    ~Thread();
    //线程id
    pid_t getId() const { return m_id;}
    //线程名称
    const std::string& getName() const { return m_name;}
    //等待线程执行完成
    void join();
    //获取当前线程指针
    static Thread* GetThis();
    //获取当前线程名称
    static const std::string& GetName();
    //设置当前线程名称 @param[in] name 线程名称
    static void SetName(const std::string& name);
private:
    Thread(const Thread&) = delete;
    Thread(const Thread&&) = delete;
    Thread& operator=(const Thread&) = delete;
    //线程执行函数
    static void* run(void* arg);
private:
    // 线程id
    pid_t m_id = -1;
    // 线程结构
    pthread_t m_thread = 0;
    // 线程执行函数
    std::function<void()> m_cb;
    // 线程名称
    std::string m_name;
    // 信号量
    Semaphore m_semaphore;
};

class ThreadPool {
public:
    typedef std::function<void()> task_type;
    ThreadPool(int count=10);
    ~ThreadPool();
    void add_task(task_type task);

    bool IsTaskEmpty() const { return !m_tasks.empty(); }
    void run();
    void stop();

private:
    std::queue<task_type> m_tasks;
    std::vector<Thread::ptr> m_threads;
    int m_threadNums;
    bool m_isRunning;

    OwnSemaphore* m_sem;
};




}

#endif
