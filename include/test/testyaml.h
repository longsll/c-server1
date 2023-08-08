#include <iostream>
#include "yaml-cpp/yaml.h"

namespace sylar{
    void test_yaml(){
        try {
        // YAML::Node node = YAML::LoadFile("/home/longsl/test01/yamlcpp/config.yaml");
        YAML::Node node = YAML::LoadFile("/home/longsl/program1/test.yaml");
        std::cout << node << std::endl; //输出yaml数据
        } catch (...) { //文件为非yaml格式抛出异常
            std::cout << "error" << std::endl;
        }
    }
}