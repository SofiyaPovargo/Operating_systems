cmake_minimum_required(VERSION 3.20)

project(lab1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(main main.cpp)
add_executable(Creator Creator/Creator.cpp)
add_executable(Reporter Reporter/Reporter.cpp)

target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(Creator PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(Reporter PRIVATE ${CMAKE_SOURCE_DIR})
