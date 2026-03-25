#include <iostream>
#include <iomanip>
#include <Windows.h>
#include "config.h"
#include "config.h"

DWORD WINAPI average(LPVOID) {
    int sum = 0;
    for (unsigned int i=0; i<conf.size; i++) {
        sum += conf.array[i];
        Sleep(12);
    }
    conf.average_value = static_cast<double>(sum) / conf.size;
    std::cout << "average value: " << std::fixed << std::setprecision(2) << conf.average_value << std::endl;
    
    return 0;
}