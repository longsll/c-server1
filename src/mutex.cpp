#include "mutex.h"
#include "log.h"
namespace sylar {


static thread_local std::string t_thread_name = "UNKNOW";

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");


Semaphore::Semaphore(uint32_t count) {
    if(sem_init(&m_semaphore, 0, count)) {
        throw std::logic_error("sem_init error");
    }
}

Semaphore::~Semaphore() {
    sem_destroy(&m_semaphore);
}

void Semaphore::wait() {
    if(sem_wait(&m_semaphore)) {
        throw std::logic_error("sem_wait error");
    }
}

void Semaphore::notify() {
    if(sem_post(&m_semaphore)) {
        throw std::logic_error("sem_post error");
    }
}

OwnSemaphore::OwnSemaphore(size_t count)
    :m_count(count){
}

OwnSemaphore::~OwnSemaphore() {
}

void OwnSemaphore::wait() {
    MutexType::Lock lock(m_mutex);
    while (m_count == 0) {
        m_cond.wait(m_mutex.getMutex());
    }
    --m_count;
}

void OwnSemaphore::notify() {
    m_count++;
    m_cond.signal();
}


Cond::Cond() {
    pthread_cond_init(&m_cond, NULL);
}

void Cond::wait(pthread_mutex_t mutex) {
    pthread_cond_wait(&m_cond, &mutex);
}

void Cond::signal() {
    pthread_cond_signal(&m_cond);
}

Cond::~Cond() {
    pthread_cond_destroy(&m_cond);
}

}