#include <iostream>
#include <Windows.h>
#include "config.h"
#include "min_max.h"
#include "config.h"

DWORD WINAPI min_max(LPVOID) {
    conf.min_elem = conf.array[0];
    conf.max_elem = conf.array[0];
    conf.min_index = 0;
    conf.max_index = 0;

    for (unsigned int i=0; i<conf.size; i++) {
        if (conf.array[i] < conf.min_elem) {
            conf.min_elem = conf.array[i];
            conf.min_index = i;
        }

        Sleep(7);
        if (conf.array[i] > conf.max_elem) {
            conf.max_elem = conf.array[i];
            conf.max_index = i;
        }
        
        Sleep(7);
    }
    std::cout << "minimum array element: " << conf.min_elem << std::endl;
    std::cout << "maximum array element: " << conf.max_elem << std::endl;

    return 0;
}