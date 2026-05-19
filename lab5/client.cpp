#include <windows.h>
#include <iostream>
#include <string>
#include "employee.h"

HANDLE ConnectToPipe() {
    while (true) {
        HANDLE hPipe = CreateFileW(
            L"\\\\.\\pipe\\EmployeePipe",
            GENERIC_READ | GENERIC_WRITE,
            0, NULL, OPEN_EXISTING, 0, NULL
        );
        if (hPipe != INVALID_HANDLE_VALUE) return hPipe;
        if (GetLastError() == ERROR_PIPE_BUSY) {
            if (!WaitNamedPipeW(L"\\\\.\\pipe\\EmployeePipe", 5000)) {
                std::cerr << "Pipe busy timeout\n";
                return NULL;
            }
        } else {
            std::cerr << "Pipe connection error\n" << GetLastError() << "\n";
            return NULL;
        }
    }
}

std::string ReceiveResponse(HANDLE hPipe) {
    char buffer[256];
    DWORD bytes;
    if (ReadFile(hPipe, buffer, sizeof(buffer)-1, &bytes, NULL))
        return std::string(buffer);
    return "";
}

void SendCommand(HANDLE hPipe, const std::string& cmd) {
    DWORD bytes;
    WriteFile(hPipe, cmd.c_str(), cmd.size()+1, &bytes, NULL);
}

void ReadOperation(HANDLE hPipe) {
    int id;
    std::cout << "Enter employee ID to read: ";
    std::cin >> id;
    
    SendCommand(hPipe, "READ");
    WriteFile(hPipe, &id, sizeof(int), NULL, NULL);
    
    std::string resp = ReceiveResponse(hPipe);
    if (resp == "OK") {
        Employee emp;
        DWORD bytes;
        ReadFile(hPipe, &emp, sizeof(Employee), &bytes, NULL);
        std::cout << "Read: " << emp.num << " " << emp.name << " " << emp.hours << std::endl;
    } else {
        std::cout << "Read failed: " << resp << std::endl;
    }
}

void ModifyOperation(HANDLE hPipe) {
    int id;
    std::cout << "Enter employee ID to modify: ";
    std::cin >> id;
    
    SendCommand(hPipe, "LOCK");
    WriteFile(hPipe, &id, sizeof(int), NULL, NULL);
    
    std::string resp = ReceiveResponse(hPipe);
    if (resp != "OK") {
        std::cout << "Lock failed: " << resp << std::endl;
        return;
    }
    
    Employee emp;
    DWORD bytes;
    ReadFile(hPipe, &emp, sizeof(Employee), &bytes, NULL);
    std::cout << "Current: " << emp.num << " " << emp.name << " " << emp.hours << std::endl;
    
    std::cout << "Enter new name: ";
    std::cin >> emp.name;
    std::cout << "Enter new hours: ";
    std::cin >> emp.hours;
    
    SendCommand(hPipe, "UPDATE");
    WriteFile(hPipe, &id, sizeof(int), NULL, NULL);
    WriteFile(hPipe, &emp, sizeof(Employee), NULL, NULL);
    
    resp = ReceiveResponse(hPipe);
    if (resp == "OK")
        std::cout << "Record updated.\n";
    else
        std::cout << "Update failed: " << resp << std::endl;
    
    SendCommand(hPipe, "UNLOCK");
    WriteFile(hPipe, &id, sizeof(int), NULL, NULL);
    ReceiveResponse(hPipe); // consume OK
}

int main() {
    HANDLE hPipe = ConnectToPipe();
    if (!hPipe) return 1;
    
    std::cout << ReceiveResponse(hPipe) << std::endl;
    
    int choice;
    while (true) {
        std::cout << "\n1. Read record\n2. Modify record\n3. Exit\nChoice: ";
        std::cin >> choice;
        
        if (choice == 1) ReadOperation(hPipe);
        else if (choice == 2) ModifyOperation(hPipe);
        else if (choice == 3) break;
        else std::cout << "Invalid choice\n";
    }
    
    SendCommand(hPipe, "QUIT");
    CloseHandle(hPipe);
    return 0;
}