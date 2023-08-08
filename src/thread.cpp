#include "thread.h"
#include "log.h"
#include "util.h"
#include "mutex.h"
namespace sylar {

static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKNOW";

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

Thread* Thread::GetThis() {
    return t_thread;
}

const std::string& Thread::GetName() {
    return t_thread_name;
}

void Thread::SetName(const std::string& name) {
    if(t_thread) {
        t_thread->m_name = name;
    }
    t_thread_name = name;
}

//构造函数：创建线程
Thread::Thread(std::function<void()> cb, const std::string& name)
    :m_cb(cb)
    ,m_name(name) {
    if(name.empty()) {
        m_name = "UNKNOW";
    }
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if(rt) {
        SYLAR_LOG_ERROR(g_logger) << "pthread_create thread fail, rt=" << rt<< " name=" << name;
        throw std::logic_error("pthread_create error");
    }
    m_semaphore.wait();
}

//析构函数：释放线程
Thread::~Thread() {
    if(m_thread) {
        pthread_detach(m_thread);
    }
}


void Thread::join() {
    if(m_thread) {
        int rt = pthread_join(m_thread, nullptr);
        if(rt) {
            SYLAR_LOG_ERROR(g_logger) << "pthread_join thread fail, rt=" << rt
                << " name=" << m_name;
            throw std::logic_error("pthread_join error");
        }
        m_thread = 0;
    }
}

void* Thread::run(void* arg) {
    Thread* thread = (Thread*)arg;
    t_thread = thread;
    t_thread_name = thread->m_name;
    thread->m_id = sylar::GetThreadId();
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());

    std::function<void()> cb;
    cb.swap(thread->m_cb);

    thread->m_semaphore.notify();

    cb();
    return 0;
}

ThreadPool::ThreadPool(int count)
    :m_threadNums(count) {
    m_isRunning = true;
    m_sem = new OwnSemaphore(10);
    for(int i=0; i<count; ++i) {
        sylar::Thread::ptr thr(new Thread(std::bind(&ThreadPool::run, this), "test_" + std::to_string(i)));
        m_threads.push_back(thr);
    }
}

ThreadPool::~ThreadPool() {
    if(m_isRunning) {
        stop();
    }
}

void ThreadPool::add_task(task_type task) {
    m_tasks.push(task);
    m_sem->notify();
}

void ThreadPool::stop() {
    m_isRunning = false;
    for (int i = 0; i < m_threadNums; i++)
    {
        m_threads[i]->join();
    }
}

void ThreadPool::run() {
    task_type task = nullptr;
    while (m_isRunning)
    {
        while (m_tasks.empty())
        {
            if(!m_isRunning) {
                SYLAR_LOG_INFO(g_logger) << "stop";
                break;
            }
            m_sem->wait();
        }

        task_type task = m_tasks.front();
        m_tasks.pop();

        if (!task)
        {
            SYLAR_LOG_INFO(g_logger) << "thread " << pthread_self() << "will exit";
            return;
        }
        task();
    }
}

}

