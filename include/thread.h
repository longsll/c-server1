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
    typedef std::shared_ptr<Thread> ptr;
    Thread(std::function<void()> cb, const std::string& name);
    ~Thread();

    pid_t getId() const { return m_id;}
    const std::string& getName() const { return m_name;}

    void join();

    static Thread* GetThis();
    static const std::string& GetName();
    static void SetName(const std::string& name);
private:
    Thread(const Thread&) = delete;
    Thread(const Thread&&) = delete;
    Thread& operator=(const Thread&) = delete;

    static void* run(void* arg);
private:
    pid_t m_id = -1;
    pthread_t m_thread = 0;
    std::function<void()> m_cb;
    std::string m_name;

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
