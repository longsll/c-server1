#include <iostream>
#include "../log.h"
#include "../filemanage.h"
#include "../thread.h"
#include "../util.h"
#include "yaml-cpp/yaml.h"

namespace sylar{
       void test_log () {
        // 创建一个日志器
        sylar::Logger::ptr logger(new sylar::Logger);
        // 输出到控制台
        logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender));
        // 输出到文件
        sylar::FileLogAppender::ptr file_appender(new sylar::FileLogAppender("./log.txt"));
        sylar::LogFormatter::ptr fmt(new sylar::LogFormatter("%d%T%p%T%m%n"));
        file_appender->setFormatter(fmt);
        file_appender->setLevel(sylar::LogLevel::ERROR);
        logger->addAppender(file_appender);

            // 输出日志
        SYLAR_LOG_INFO(logger) << "test macro";
        SYLAR_LOG_ERROR(logger) << "test macro error";
        SYLAR_LOG_FMT_ERROR(logger, "test macro fmt error %s", "aa");

        // 创建新的日志器，并输出日志
        auto l = sylar::LoggerMgr::GetInstance()->getLogger("xx");
        SYLAR_LOG_INFO(l) << "xxx";
    }
}