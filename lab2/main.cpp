#include <iostream>
#include <iomanip>
#include <Windows.h>
#include "config.h"
#include "min_max.h"
#include "average.h"

int main() {
    std::cout << "enter size of array: ";
    std::cin >> conf.size;
    conf.array = new int[conf.size];
    std::cout << "enter elements of array: ";

    for (unsigned int i=0; i<conf.size; i++) {
        std::cin >> conf.array[i];
    }
    HANDLE hMinMax;
    DWORD IDThread;
    HANDLE hAverage;
    DWORD IDAverage;

    hMinMax = CreateThread(NULL, 0, min_max, NULL, 0, &IDThread);
    hAverage = CreateThread(NULL, 0, average, NULL, 0,  &IDAverage);

    if (hMinMax == NULL || hAverage == NULL) {
        return GetLastError();
    }

    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);

    conf.array[conf.min_index] = conf.array[conf.max_index] = conf.average_value;

    for (size_t i = 0; i < conf.size; i++) {
		std::cout << conf.array[i] << " ";
	}
    CloseHandle(hMinMax);
    CloseHandle(hAverage);
    delete[] conf.array;
    system("pause");
    return 0;
}