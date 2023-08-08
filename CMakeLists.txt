cmake_minimum_required (VERSION 2.8.3)

project (demo)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

aux_source_directory (src SRC_LIST)

find_package(yaml-cpp REQUIRED)
include_directories (include /usr/local/include/yaml-cpp)

# add_compile_options(-lpthread) 
link_libraries(pthread)

add_executable (main ${SRC_LIST})


target_link_libraries(main /usr/local/lib/libyaml-cpp.a)