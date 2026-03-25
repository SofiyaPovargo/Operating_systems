cmake_minimum_required(VERSION 3.10)
project(os_lab2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем директорию src для include
include_directories(${CMAKE_SOURCE_DIR}/src)

# Основной исполняемый файл
add_executable(run 
    main.cpp 
    src/min_max.cpp 
    src/average.cpp 
    src/config.cpp
)

# Опция для сборки тестов
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    include(FetchContent)
    
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
    # Включаем тестирование
    enable_testing()
    
    # Исполняемый файл с тестами
    add_executable(tests
        test/tests.cpp
        src/min_max.cpp
        src/average.cpp
        src/config.cpp
    )
    
    # Линковка с Google Test
    target_link_libraries(tests
        PRIVATE
        gtest_main
    )
    
    # Добавляем директорию с заголовками
    target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
    
    # Добавляем тест в CTest
    add_test(NAME UnitTests COMMAND tests)
endif()