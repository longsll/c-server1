#include <iostream>
#include "../log.h"
#include "../filemanage.h"
#include "../thread.h"
#include "../util.h"
#include "yaml-cpp/yaml.h"

namespace sylar{
    // sylar::Logger::ptr g_logger(new sylar::Logger); 
    void test_pool() {
    
    sylar::ThreadPool threadPool(10);
    for(int i = 0; i < 20; i++)
    {
        threadPool.add_task([]{
            SYLAR_LOG_INFO(g_logger) << "xxxx" << pthread_self();
            sleep(2);
            return 0;
        });
    }

        while (1) {
    //        sleep(1);
        }
    }
}