#include <iostream>
#include <windows.h>
#include <conio.h>
#include <string>
#include <fstream>
#include "Employee.h"
#include <stdexcept>


void start_process(std::string command_line
					// LPTSTR lpszCreatorProcessCommandLine
					) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if (!CreateProcess(NULL, command_line.data(), NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) 
	{
		throw std::runtime_error("Creator process is not created. Check a name of the process.");
	}
	WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}


void print_bin_file(const std::string& bin_file_name) {
	std::fstream bf(bin_file_name, std::ios::in | std::ios::binary);
	if (!bf) {
		throw std::runtime_error("Error! Unable to open and print binary file");
	}
	employee emp;
    while (bf.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
        std::cout << "ID: " << emp.num 
				  << " Name: " << emp.name 
				  << " Working hours: " << emp.hours 
				  << std::endl;
    }
    bf.close();
}


void print_report(const std::string& report_file_name) {
	std::fstream rep(report_file_name, std::ios::in);
	if (!rep) {
		throw std::runtime_error("Error! Unable to open and print binary file");
	}
	while(!rep.eof()){
		std::string str;
		std::getline(rep, str);
		std::cout << str << std::endl;
	}
	rep.close();
}


int main() {
	SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

	std::string bin_file_name;
	unsigned int records;

	std::cout << "Enter binary file name\n";
	std::cin >> bin_file_name;
	std::cout << "Enter number of records\n";
	std::cin >> records;
	
	std::string creator_cmd = "Creator.exe " + bin_file_name + " " + std::to_string(records);
	// std::wstring converting_creator_to_lpwstr = std::wstring(creator_cmd.begin(), creator_cmd.end());
	// LPWSTR lpszCreatorProcessCommandLine = &converting_creator_to_lpwstr[0];

	try {
		start_process(creator_cmd);
	}
	catch (const std::exception& e) {
	    std::cerr << e.what();
		_getch();
        return 1;	
	}

	try {
		print_bin_file(bin_file_name);
	} 
	catch (const std::exception& e) {
	    std::cerr << e.what();
		_getch();
        return 1;	
	}

	std::string repr_file_name;
	unsigned int price_per_hour;

	std::cout << "Enter report file name\n";
	std::cin >> repr_file_name;
	std::cout << "Enter salary per hour of work\n";
	std::cin >> price_per_hour;
	
	std::string rep_cmd = "Reporter.exe " + bin_file_name + " " +
						repr_file_name + " " +
					    std::to_string(price_per_hour);
	// std::wstring converting_reporter_to_lpwstr = std::wstring(rep_cmd.begin(), rep_cmd.end());
	// LPWSTR lpszReporterProcessCommandLine = &converting_reporter_to_lpwstr[0];

	try {
		start_process(rep_cmd);
	}
	catch (const std::exception& e) {
	    std::cerr << e.what();
		_getch();
        return 1;	
	}

	try {
		print_report(repr_file_name);
	} 
	catch (const std::exception& e) {
	    std::cerr << e.what();
		_getch();
        return 1;	
	}
	std::cout << "\nProgram completed successfully!";
	_getch();
	return 0;
}