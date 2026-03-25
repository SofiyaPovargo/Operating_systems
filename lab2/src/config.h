#ifndef CONFIG_H
#define CONFIG_H

struct config
{
    int* array = nullptr;
    unsigned int size = 0;
    int min_elem = 0;
    unsigned int min_index = 0;
    int max_elem = 0;
    unsigned int max_index = 0;
    double average_value = 0.0;
};

extern config conf;  // объявление внешней переменной

#endif