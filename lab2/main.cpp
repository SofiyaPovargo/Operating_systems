#include <iostream>
#include <iomanip>
#include <Windows.h>

int* array = nullptr;
unsigned int size = 0;
int min_elem = 0;
unsigned int min_index = 0;
int max_elem = 0;
unsigned int max_index = 0;
double average_value = 0.0;

DWORD WINAPI min_max(LPVOID) {
    min_elem = array[0];
    max_elem = array[0];
    min_index = 0;
    max_index = 0;

    for (unsigned int i=0; i<size; i++) {
        if (array[i] < min_elem) {
            min_elem = array[i];
            min_index = i;
        }

        if (array[i] > max_elem) {
            max_elem = array[i];
            max_index = i;
        }
        
        Sleep(7);
    }
    std::cout << "minimum array element: " << min_elem << std::endl;
    std::cout << "maximum array element: " << max_elem << std::endl;

    return 0;
}

DWORD WINAPI average(LPVOID) {
    int sum = 0;
    for (unsigned int i=0; i<size; i++) {
        sum += array[i];
        Sleep(12);
    }
    average_value = static_cast<int>(sum) / size;
    std::cout << "average value: " << std::fixed << std::setprecision(2) << average_value << std::endl;
    
    return 0;
}

int main() {
    std::cout << "enter size of array: ";
    std::cin >> size;
    array = new int[size];
    std::cout << "enter elements of array: ";

    for (unsigned int i=0; i<size; i++) {
        std::cin >> array[i];
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

    array[min_index] = array[max_index] = average_value;

    for (size_t i = 0; i < size; i++) {
		std::cout << array[i] << " ";
	}
    CloseHandle(hMinMax);
    CloseHandle(hAverage);
    delete[] array;
    system("pause");
    return 0;
}