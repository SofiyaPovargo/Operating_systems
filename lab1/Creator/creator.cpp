#include <iostream>
#include <string>
#include <fstream>
#include <conio.h>
#include "../Employee.h"


int main(int argc, char* argv[]) {
    if (argc < 2) {
        return 1;
    }
    std::string file_name = argv[1];
    int number_of_notes = std::stoi(argv[2]);
    
    std::ofstream bf(file_name, std::ios::binary);
    if (!bf.is_open()) {
        std::cout << "Failed to open file\n";
        return 1;
    }

    if (number_of_notes == 0) {
        bf.close();
        return 0;
    }

    // employee* emp = new employee[number_of_notes];

    // for (int i=0; i<number_of_notes; i++) {
    //     std::cout << "Enter employee " << (i+1) << ":\n";
    //     std::cout << "Employee ID: ";
    //     std::cin >> emp[i].num;
    //     std::cout << "Employee name (maximum 9 characters): ";
    //     std::string name;
    //     std::cin >> emp[i].name;
    //     std::cout << "Hours worked: ";
    //     std::cin >> emp[i].hours;
    // }

    // bf.write(reinterpret_cast<char*>(&emp), sizeof(emp));
    for (int i = 0; i < number_of_notes; i++)
    {
        employee emp;

        std::cout << "\nEnter employee " << (i + 1) << ":\n";

        std::cout << "Employee ID: ";
        std::cin >> emp.num;

        std::cout << "Employee name (max 9 characters): ";
        std::cin >> emp.name;

        std::cout << "Hours worked: ";
        std::cin >> emp.hours;

        bf.write(reinterpret_cast<char*>(&emp), sizeof(employee));
    }

    bf.close();
    return 0;
}