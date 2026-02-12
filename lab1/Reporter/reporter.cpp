#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <conio.h>
#include "../Employee.h"

int main(int argc, char* argv[]) {
    employee empl;
    if (argc < 3) {
        return 1;
    }
    std::string file_name = argv[1];
    std::string report_file = argv[2];
    int pay_per_hour = std::stoi(argv[3]);

    std::ifstream file_input;
    std::ofstream file_output;
    std::vector<employee> emps;
    file_input.open(file_name, std::ios::binary);

    if (file_input.is_open()) {
		while (!file_input.eof()) {
			file_input.read((char*)&empl, sizeof(employee));
			emps.push_back(empl);
		}
		emps.pop_back();
	}

	file_input.close();
	file_output.open(report_file);

	file_output.open(report_file);

    if (file_input.is_open()) {
        file_output << "Report on binary file " << file_name << std::endl;
        file_output << "ID\tName\tWorking hours\tSalary." << std::endl;
        for (size_t i=0; i < emps.size(); i++) {
            file_output << emps[i].num << "\t" << emps[i].name << "\t" << emps[i].hours << "\t" << pay_per_hour * emps[i].hours << "\n";
        }
    }

    file_output.close();

    return 0;
}