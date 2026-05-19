#include "employee.h"

bool createEmployeeFile(std::fstream& file, const std::vector<Employee>& emps) {
    
    for (Employee emp : emps) {
        file.write(reinterpret_cast<const char*>(&emp), sizeof(Employee));
    }
    
    return true;
}

bool readEmployeeById(std::fstream& file, int id, Employee& emp) {
    file.seekg(0); // курсор на чтение

    Employee temp;
    while (file.read(reinterpret_cast<char*>(&emp), sizeof(Employee))) {
        if (emp.num == id) {
            emp = temp;
            return true;
        }
    }
    return false;
}

bool writeEmployeeById(std::fstream& file, int id, const Employee& emp) {
    Employee temp;
    std::streampos pos = 0;
    
    while (file.read(reinterpret_cast<char*>(&temp), sizeof(Employee))) {
        if (temp.num == id) {
            file.seekp(pos); // курсор на запись
            file.write(reinterpret_cast<const char*>(&emp), sizeof(Employee));
            return true;
        }
        pos = file.tellg(); 
    }
    
    return false;
}

void displayAllEmployees(std::fstream& file) {
    Employee emp;

    while (file.read(reinterpret_cast<char*>(&emp), sizeof(Employee))) {
        std::cout << emp.num << "\t" << emp.name << "\t" << emp.hours << std::endl;
    }
}