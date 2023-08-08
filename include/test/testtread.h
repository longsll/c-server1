#include <iostream>
#include "../log.h"
#include "../thread.h"
#include "../util.h"
#include "yaml-cpp/yaml.h"

namespace sylar{
    sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
    
    void fun2() {
            SYLAR_LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }

    void fun3() {
            SYLAR_LOG_INFO(g_logger) << "========================================";
    }

    void test_thread() {
            // sylar::Logger::ptr g_logger(new sylar::Logger);        
            SYLAR_LOG_INFO(g_logger) << "thread test begin";

            std::vector<sylar::Thread::ptr> thrs;
            for(int i = 0; i < 10; ++i) {
                sylar::Thread::ptr thr(new sylar::Thread(&fun2, "name_" + std::to_string(i * 2)));
                sylar::Thread::ptr thr2(new sylar::Thread(&fun3, "name_" + std::to_string(i * 2 + 1)));
                thrs.push_back(thr);
                thrs.push_back(thr2);
            }

            for(size_t i = 0; i < thrs.size(); ++i) {
                thrs[i]->join();
            }
            SYLAR_LOG_INFO(g_logger) << "thread test end";
    }
}