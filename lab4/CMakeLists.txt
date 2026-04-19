cmake_minimum_required(VERSION 3.10)
project(Lab4)
set(CMAKE_CXX_STANDARD 17)

# Исполняемые файлы
add_executable(Receiver receiver.cpp)
add_executable(Sender sender.cpp)

# Подключение GTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(unit_tests tests/test_logic.cpp)
target_link_libraries(unit_tests gtest_main)
include(GoogleTest)
gtest_discover_tests(unit_tests)