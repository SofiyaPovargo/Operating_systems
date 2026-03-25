#include <gtest/gtest.h>
#include <windows.h>
#include <iostream>

// Подключаем заголовочные файлы проекта
#include "config.h"
#include "min_max.h"
#include "average.h"

// Вспомогательная функция для сброса глобальной конфигурации
void reset_config(int* arr, unsigned int size) {
    conf.array = arr;
    conf.size = size;
    conf.min_elem = arr[0];
    conf.min_index = 0;
    conf.max_elem = arr[0];
    conf.max_index = 0;
    conf.average_value = 0.0;
}

// Тесты для min_max
TEST(MinMaxTest, FindMinAndMax) {
    int arr[] = {5, 2, 8, 1, 9, 3};
    unsigned int size = 6;
    
    reset_config(arr, size);
    
    // Создаем поток для min_max
    HANDLE hThread = CreateThread(NULL, 0, min_max, NULL, 0, NULL);
    ASSERT_TRUE(hThread != NULL) << "Failed to create thread";
    
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    
    EXPECT_EQ(conf.min_elem, 1);
    EXPECT_EQ(conf.max_elem, 9);
    EXPECT_EQ(conf.min_index, 3);  // индекс элемента 1
    EXPECT_EQ(conf.max_index, 4);  // индекс элемента 9
}

TEST(MinMaxTest, SingleElement) {
    int arr[] = {42};
    unsigned int size = 1;
    
    reset_config(arr, size);
    
    HANDLE hThread = CreateThread(NULL, 0, min_max, NULL, 0, NULL);
    ASSERT_TRUE(hThread != NULL);
    
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    
    EXPECT_EQ(conf.min_elem, 42);
    EXPECT_EQ(conf.max_elem, 42);
    EXPECT_EQ(conf.min_index, 0);
    EXPECT_EQ(conf.max_index, 0);
}

TEST(MinMaxTest, NegativeNumbers) {
    int arr[] = {-5, -2, -8, -1, -9, -3};
    unsigned int size = 6;
    
    reset_config(arr, size);
    
    HANDLE hThread = CreateThread(NULL, 0, min_max, NULL, 0, NULL);
    ASSERT_TRUE(hThread != NULL);
    
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    
    EXPECT_EQ(conf.min_elem, -9);
    EXPECT_EQ(conf.max_elem, -1);
    EXPECT_EQ(conf.min_index, 4);  // индекс элемента -9
    EXPECT_EQ(conf.max_index, 3);  // индекс элемента -1
}

TEST(MinMaxTest, AllSameElements) {
    int arr[] = {7, 7, 7, 7, 7};
    unsigned int size = 5;
    
    reset_config(arr, size);
    
    HANDLE hThread = CreateThread(NULL, 0, min_max, NULL, 0, NULL);
    ASSERT_TRUE(hThread != NULL);
    
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    
    EXPECT_EQ(conf.min_elem, 7);
    EXPECT_EQ(conf.max_elem, 7);
    EXPECT_EQ(conf.min_index, 0);  // первый элемент
    EXPECT_EQ(conf.max_index, 0);  // первый элемент
}

TEST(MinMaxTest, DescendingOrder) {
    int arr[] = {9, 8, 7, 6, 5, 4, 3, 2, 1};
    unsigned int size = 9;
    
    reset_config(arr, size);
    
    HANDLE hThread = CreateThread(NULL, 0, min_max, NULL, 0, NULL);
    ASSERT_TRUE(hThread != NULL);
    
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    
    EXPECT_EQ(conf.min_elem, 1);
    EXPECT_EQ(conf.max_elem, 9);
    EXPECT_EQ(conf.min_index, 8);  // последний элемент
    EXPECT_EQ(conf.max_index, 0);  // первый элемент
}

TEST(MinMaxTest, AscendingOrder) {
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    unsigned int size = 9;
    
    reset_config(arr, size);
    
    HANDLE hThread = CreateThread(NULL, 0, min_max, NULL, 0, NULL);
    ASSERT_TRUE(hThread != NULL);
    
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    
    EXPECT_EQ(conf.min_elem, 1);
    EXPECT_EQ(conf.max_elem, 9);
    EXPECT_EQ(conf.min_index, 0);  // первый элемент
    EXPECT_EQ(conf.max_index, 8);  // последний элемент
}

// Тесты для average
TEST(AverageTest, CalculateAverage) {
    int arr[] = {1, 2, 3, 4, 5};
    unsigned int size = 5;
    
    reset_config(arr, size);
    
    HANDLE hThread = CreateThread(NULL, 0, average, NULL, 0, NULL);
    ASSERT_TRUE(hThread != NULL);
    
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    
    EXPECT_DOUBLE_EQ(conf.average_value, 3.0);
}

TEST(AverageTest, SingleElementAverage) {
    int arr[] = {42};
    unsigned int size = 1;
    
    reset_config(arr, size);
    
    HANDLE hThread = CreateThread(NULL, 0, average, NULL, 0, NULL);
    ASSERT_TRUE(hThread != NULL);
    
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    
    EXPECT_DOUBLE_EQ(conf.average_value, 42.0);
}

TEST(AverageTest, NegativeNumbersAverage) {
    int arr[] = {-5, -2, -8, -1, -9, -3};
    unsigned int size = 6;
    
    reset_config(arr, size);
    
    HANDLE hThread = CreateThread(NULL, 0, average, NULL, 0, NULL);
    ASSERT_TRUE(hThread != NULL);
    
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    
    double expected = (-5 - 2 - 8 - 1 - 9 - 3) / 6.0;
    EXPECT_DOUBLE_EQ(conf.average_value, expected);
}

TEST(AverageTest, ZerosAverage) {
    int arr[] = {0, 0, 0, 0, 0};
    unsigned int size = 5;
    
    reset_config(arr, size);
    
    HANDLE hThread = CreateThread(NULL, 0, average, NULL, 0, NULL);
    ASSERT_TRUE(hThread != NULL);
    
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    
    EXPECT_DOUBLE_EQ(conf.average_value, 0.0);
}

TEST(AverageTest, LargeNumbers) {
    int arr[] = {1000, 2000, 3000, 4000, 5000};
    unsigned int size = 5;
    
    reset_config(arr, size);
    
    HANDLE hThread = CreateThread(NULL, 0, average, NULL, 0, NULL);
    ASSERT_TRUE(hThread != NULL);
    
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    
    EXPECT_DOUBLE_EQ(conf.average_value, 3000.0);
}

TEST(AverageTest, MixedNumbers) {
    int arr[] = {-10, 0, 10, 20, -5};
    unsigned int size = 5;
    
    reset_config(arr, size);
    
    HANDLE hThread = CreateThread(NULL, 0, average, NULL, 0, NULL);
    ASSERT_TRUE(hThread != NULL);
    
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    
    double expected = (-10 + 0 + 10 + 20 - 5) / 5.0;
    EXPECT_DOUBLE_EQ(conf.average_value, expected);
}

// Интеграционные тесты
TEST(IntegrationTest, BothThreadsWork) {
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    unsigned int size = 10;
    
    reset_config(arr, size);
    
    HANDLE hMinMax = CreateThread(NULL, 0, min_max, NULL, 0, NULL);
    HANDLE hAverage = CreateThread(NULL, 0, average, NULL, 0, NULL);
    
    ASSERT_TRUE(hMinMax != NULL);
    ASSERT_TRUE(hAverage != NULL);
    
    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);
    
    CloseHandle(hMinMax);
    CloseHandle(hAverage);
    
    EXPECT_EQ(conf.min_elem, 1);
    EXPECT_EQ(conf.max_elem, 10);
    EXPECT_DOUBLE_EQ(conf.average_value, 5.5);
}

TEST(IntegrationTest, ArrayModification) {
    int arr[] = {10, 20, 30, 40, 50};
    unsigned int size = 5;
    
    reset_config(arr, size);
    
    HANDLE hMinMax = CreateThread(NULL, 0, min_max, NULL, 0, NULL);
    HANDLE hAverage = CreateThread(NULL, 0, average, NULL, 0, NULL);
    
    ASSERT_TRUE(hMinMax != NULL);
    ASSERT_TRUE(hAverage != NULL);
    
    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);
    
    // Имитация замены min и max на среднее (как в main)
    int average_int = static_cast<int>(conf.average_value);
    arr[conf.min_index] = average_int;
    arr[conf.max_index] = average_int;
    
    EXPECT_EQ(arr[0], 30);  // min был 10 (индекс 0), заменен на 30
    EXPECT_EQ(arr[4], 30);  // max был 50 (индекс 4), заменен на 30
    EXPECT_EQ(arr[1], 20);  // остальные не изменились
    EXPECT_EQ(arr[2], 30);
    EXPECT_EQ(arr[3], 40);
    
    CloseHandle(hMinMax);
    CloseHandle(hAverage);
}

TEST(IntegrationTest, MinMaxBeforeAverage) {
    int arr[] = {5, 3, 8, 1, 9, 2};
    unsigned int size = 6;
    
    reset_config(arr, size);
    
    HANDLE hMinMax = CreateThread(NULL, 0, min_max, NULL, 0, NULL);
    WaitForSingleObject(hMinMax, INFINITE);
    CloseHandle(hMinMax);
    
    EXPECT_EQ(conf.min_elem, 1);
    EXPECT_EQ(conf.max_elem, 9);
    
    HANDLE hAverage = CreateThread(NULL, 0, average, NULL, 0, NULL);
    WaitForSingleObject(hAverage, INFINITE);
    CloseHandle(hAverage);
    
    double expected = (5 + 3 + 8 + 1 + 9 + 2) / 6.0;
    EXPECT_DOUBLE_EQ(conf.average_value, expected);
}

TEST(IntegrationTest, AverageBeforeMinMax) {
    int arr[] = {5, 3, 8, 1, 9, 2};
    unsigned int size = 6;
    
    reset_config(arr, size);
    
    HANDLE hAverage = CreateThread(NULL, 0, average, NULL, 0, NULL);
    WaitForSingleObject(hAverage, INFINITE);
    CloseHandle(hAverage);
    
    double expected = (5 + 3 + 8 + 1 + 9 + 2) / 6.0;
    EXPECT_DOUBLE_EQ(conf.average_value, expected);
    
    HANDLE hMinMax = CreateThread(NULL, 0, min_max, NULL, 0, NULL);
    WaitForSingleObject(hMinMax, INFINITE);
    CloseHandle(hMinMax);
    
    EXPECT_EQ(conf.min_elem, 1);
    EXPECT_EQ(conf.max_elem, 9);
}

// Главная функция
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}