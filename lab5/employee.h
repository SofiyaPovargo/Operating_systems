#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>

struct Employee {
    int num;
    char name[10];
    double hours;
};

bool createEmployeeFile(std::fstream& file, const std::vector<Employee>& emps);

bool readEmployeeById(std::fstream& file, int id, Employee& emp);

bool writeEmployeeById(std::fstream& file, int id, const Employee& emp);

void displayAllEmployees(std::fstream& file);