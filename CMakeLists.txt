cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -Wall -g -c -ggdb -fPIC")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -fPIC")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/../lib")
# set(CMAKE_SHARED_LINKER_FLAGS "-shared")
add_compile_options(-std=c++17)
link_directories(/usr/local/log4cxx/lib)
link_libraries(pthread log4cxx)
include_directories(/usr/local/log4cxx/include ./ Log MessageLoop Thread Thread/Private)

aux_source_directory(Log DIR_SRCS_LOG)
aux_source_directory(MessageLoop DIR_SRCS_MESSAGELOOP)
aux_source_directory(Thread/Private DIR_SRCS_THREADPRIVATE)
aux_source_directory(Thread DIR_SRCS_THREAD)

project(Commonlib CXX)
add_library(Commonlib  SHARED  ${DIR_SRCS_LOG} ${DIR_SRCS_MESSAGELOOP} ${DIR_SRCS_THREADPRIVATE} ${DIR_SRCS_THREAD}) 
add_custom_command(TARGET Commonlib
    PRE_BUILD
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/../lib
    )
install(TARGETS Commonlib
LIBRARY DESTINATION /usr/local/Commonlib/lib)
