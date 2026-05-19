// server.cpp (исправленный)
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <map>
#include <fstream>
#include <algorithm>
#include "employee.h"

struct ClientContext {
    HANDLE hPipe;
    int clientId;
};

std::string g_filename;
std::map<int, bool> g_lockedRecords; 
CRITICAL_SECTION g_cs;

bool isRecordLocked(int id) {
    auto it = g_lockedRecords.find(id);
    return (it != g_lockedRecords.end() && it->second);
}

bool lockRecord(int id) {
    if (isRecordLocked(id)) return false;
    g_lockedRecords[id] = true;
    return true;
}

void unlockRecord(int id) {
    g_lockedRecords.erase(id);
}

bool readRecordFromFile(int id, Employee& emp) {
    std::fstream file(g_filename, std::ios::in | std::ios::binary);
    if (!file) return false;
    Employee e;
    while (file.read(reinterpret_cast<char*>(&e), sizeof(Employee))) {
        if (e.num == id) {
            emp = e;
            return true;
        }
    }
    return false;
}

bool updateRecordInFile(int id, const Employee& newEmp) {
    std::fstream file(g_filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!file) return false;
    Employee e;
    std::streampos pos = 0;
    while (file.read(reinterpret_cast<char*>(&e), sizeof(Employee))) {
        if (e.num == id) {
            file.seekp(pos);
            file.write(reinterpret_cast<const char*>(&newEmp), sizeof(Employee));
            file.flush();
            return true;
        }
        pos = file.tellg();
    }
    return false;
}

void sendResponse(HANDLE hPipe, const std::string& msg) {
    DWORD bytes;
    WriteFile(hPipe, msg.c_str(), static_cast<DWORD>(msg.size() + 1), &bytes, NULL);
}

void sendEmployee(HANDLE hPipe, const Employee& emp) {
    DWORD bytes;
    WriteFile(hPipe, &emp, sizeof(Employee), &bytes, NULL);
}

DWORD WINAPI HandleClient(LPVOID lpParam) {
    ClientContext* context = static_cast<ClientContext*>(lpParam);
    HANDLE hPipe = context->hPipe;
    int clientId = context->clientId;

    sendResponse(hPipe, "Connected to employee server");

    char buffer[256];
    DWORD bytesRead;

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        if (!ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) || bytesRead == 0)
            break;

        std::string cmd(buffer);
        
        if (cmd == "READ") {
            int id;
            ReadFile(hPipe, &id, sizeof(int), &bytesRead, NULL);

            EnterCriticalSection(&g_cs);
            Employee emp;
            bool exists = readRecordFromFile(id, emp);
            bool locked = isRecordLocked(id);
            LeaveCriticalSection(&g_cs);

            if (!exists) {
                sendResponse(hPipe, "ERROR: Record not found");
                continue;
            }
            if (locked) {
                sendResponse(hPipe, "ERROR: Record locked for modification");
                continue;
            }

            sendResponse(hPipe, "OK");
            sendEmployee(hPipe, emp);

        } else if (cmd == "LOCK") {
            int id;
            ReadFile(hPipe, &id, sizeof(int), &bytesRead, NULL);

            EnterCriticalSection(&g_cs);
            Employee emp;
            bool exists = readRecordFromFile(id, emp);
            bool success = exists && lockRecord(id);
            LeaveCriticalSection(&g_cs);

            if (!exists) {
                sendResponse(hPipe, "ERROR: Record not found");
                continue;
            }
            if (!success) {
                sendResponse(hPipe, "ERROR: Already locked by another client");
                continue;
            }
            sendResponse(hPipe, "OK");
            sendEmployee(hPipe, emp);

        } else if (cmd == "UPDATE") {
            int id;
            ReadFile(hPipe, &id, sizeof(int), &bytesRead, NULL);
            Employee newEmp;
            ReadFile(hPipe, &newEmp, sizeof(Employee), &bytesRead, NULL);

            EnterCriticalSection(&g_cs);
            bool locked = isRecordLocked(id);
            if (locked) {
                updateRecordInFile(id, newEmp);
            }
            LeaveCriticalSection(&g_cs);

            if (!locked) {
                sendResponse(hPipe, "ERROR: No lock held");
            } else {
                sendResponse(hPipe, "OK");
            }

        } else if (cmd == "UNLOCK") {
            int id;
            ReadFile(hPipe, &id, sizeof(int), &bytesRead, NULL);

            EnterCriticalSection(&g_cs);
            unlockRecord(id);
            LeaveCriticalSection(&g_cs);
            sendResponse(hPipe, "OK");

        } else if (cmd == "QUIT") {
            break;
        } else {
            sendResponse(hPipe, "ERROR: Unknown command");
        }
    }

    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    delete context;
    return 0;
}

void displayFileContent() {
    std::fstream file(g_filename, std::ios::in | std::ios::binary);
    if (!file) {
        std::cout << "Unable to open file for display\n";
        return;
    }
    Employee emp;
    std::cout << "\n--- Current file content ---\n";
    while (file.read(reinterpret_cast<char*>(&emp), sizeof(Employee))) {
        std::cout << emp.num << "\t" << emp.name << "\t" << emp.hours << std::endl;
    }
    std::cout << "----------------------------\n";
    file.close();
}

int main() {
    InitializeCriticalSection(&g_cs);
    
    std::cout << "Enter filename: ";
    std::cin >> g_filename;
    
    int n;
    std::cout << "Enter number of employees: ";
    std::cin >> n;
    
    std::vector<Employee> employees;
    for (int i = 0; i < n; ++i) {
        Employee e;
        std::cout << "Enter ID, Name, Hours for employee " << i+1 << ": ";
        std::cin >> e.num >> e.name >> e.hours;
        employees.push_back(e);
    }
    
    std::fstream file(g_filename, std::ios::out | std::ios::binary);
    if (!file) {
        std::cerr << "Cannot create file!\n";
        DeleteCriticalSection(&g_cs);
        return 1;
    }
    createEmployeeFile(file, employees);
    file.close();
    
    displayFileContent();
    
    int numClients;
    std::cout << "\nEnter number of clients: ";
    std::cin >> numClients;
    
    std::vector<HANDLE> threadHandles;
    std::vector<HANDLE> clientProcesses;
    


    for (int i = 0; i < numClients; ++i) {
        HANDLE hPipe = CreateNamedPipeW(
            L"\\\\.\\pipe\\EmployeePipe",
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            4096, 4096, 0, NULL
        );
        
        if (hPipe == INVALID_HANDLE_VALUE) {
            std::cerr << "Pipe creation error for client " << i+1 << std::endl;
            continue;
        }
        

        STARTUPINFOW si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        if (CreateProcessW(
            L"Client.exe",  
            NULL,
            NULL, 
            NULL,
            FALSE,
            CREATE_NEW_CONSOLE,
            NULL,
            NULL,
            &si,
            &pi
        )) {
            clientProcesses.push_back(pi.hProcess);
            CloseHandle(pi.hThread);
            std::cout << "Client process " << i+1 << " started (PID: " << pi.dwProcessId << ")\n";
        } else {
            std::cerr << "Failed to start client process " << i+1 << "\n";
        }
        
        std::cout << "Waiting for client " << i+1 << "..." << std::endl;
        BOOL connected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

        if (connected) {
            ClientContext* ctx = new ClientContext{hPipe, i+1};
            HANDLE hThread = CreateThread(NULL, 0, HandleClient, ctx, 0, NULL);
            if (hThread) {
                threadHandles.push_back(hThread);
                std::cout << "Client " << i+1 << " connected\n";
                
                
            } else {
                delete ctx;
                CloseHandle(hPipe);
            }
        } else {
            CloseHandle(hPipe);
        }
    }
    
    std::cout << "\nAll clients connected. Server running...\n";
    std::cout << "Press Enter to shutdown server after all clients exit.\n";
    
   
    if (!threadHandles.empty()) {
        WaitForMultipleObjects(static_cast<DWORD>(threadHandles.size()), 
                               threadHandles.data(), TRUE, INFINITE);
    }
    
    for (HANDLE h : threadHandles) {
        CloseHandle(h);
    }
    
    for (HANDLE hProc : clientProcesses) {
        WaitForSingleObject(hProc, 5000); 
        CloseHandle(hProc);
    }
    
    displayFileContent();
    
    DeleteCriticalSection(&g_cs);
    std::cout << "Server finished.\n";
    return 0;
}