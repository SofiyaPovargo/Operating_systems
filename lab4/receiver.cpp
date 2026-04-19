#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>
#include "common.h"

using namespace std;

int main() {
    string file_name;
    int count_of_notes, count_senders;

    cout << "Input binary file name: "; cin >> file_name;
    cout << "Input count of notes: "; cin >> count_of_notes;

    // 1. Инициализация файла
    {
        ofstream out(file_name, ios::binary | ios::trunc);
        FileHeader header = { count_of_notes, 0, 0 };
        out.write(reinterpret_cast<char*>(&header), sizeof(header));
        Message emptyMsg = { "" };
        for (int i = 0; i < count_of_notes; i++) {
            out.write(reinterpret_cast<char*>(&emptyMsg), sizeof(emptyMsg));
        }
    }

    // 2. Создание объектов синхронизации (Явно ANSI версии 'A')
    HANDLE hMutex = CreateMutexA(NULL, FALSE, SyncConst::MUTEX_NAME);
    HANDLE hSemEmpty = CreateSemaphoreA(NULL, count_of_notes, count_of_notes, SyncConst::SEM_EMPTY_NAME);
    HANDLE hSemFull = CreateSemaphoreA(NULL, 0, count_of_notes, SyncConst::SEM_FULL_NAME);

    cout << "Input number of senders: "; cin >> count_senders;

    vector<HANDLE> readyEvents;
    vector<PROCESS_INFORMATION> sendersPI;

    for (int i = 0; i < count_senders; i++) {
        string eventName = string(SyncConst::EVENT_READY_PREFIX) + to_string(i);
        readyEvents.push_back(CreateEventA(NULL, TRUE, FALSE, eventName.c_str()));

        // Используем обычный string, БЕЗ wstring
        string cmd = "Sender.exe " + file_name + " " + to_string(i);
        
        STARTUPINFOA si = { sizeof(si) }; // STARTUPINFOA для ANSI
        PROCESS_INFORMATION pi;

        // Вызываем CreateProcessA. Второй аргумент должен быть строкой в char*
        if (CreateProcessA(NULL, &cmd[0], NULL, NULL, FALSE, 
            CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
            sendersPI.push_back(pi);
        } else {
            cout << "Failed to start sender " << i << ". Error: " << GetLastError() << endl;
        }
    }

    cout << "Waiting for senders to signal readiness..." << endl;
    WaitForMultipleObjects((DWORD)readyEvents.size(), readyEvents.data(), TRUE, INFINITE);

    // 3. Цикл чтения
    while (true) {
        cout << "\n--- Receiver Menu ---" << endl;
        cout << "1 - Read message\n2 - Exit\nChoice: ";
        int choice;
        
        if (!(cin >> choice)) { // Если ввели не число
            cin.clear();
            cin.ignore(MAXSHORT, '\n');
            continue;
        }

        if (choice == 2) break;
        if (choice != 1) continue;

        cout << "Waiting for message..." << endl;
        WaitForSingleObject(hSemFull, INFINITE); 
        WaitForSingleObject(hMutex, INFINITE);

        fstream f(file_name, ios::binary | ios::in | ios::out);
        if (f.is_open()) {
            FileHeader h; 
            f.read((char*)&h, sizeof(h));
            
            Message m;
            f.seekg(sizeof(FileHeader) + h.head * sizeof(Message));
            f.read((char*)&m, sizeof(m));

            cout << ">>> Received: " << m.text << endl;

            h.head = (h.head + 1) % h.maxMessages;
            f.seekp(0); 
            f.write((char*)&h, sizeof(h));
            f.close();
        }

        ReleaseMutex(hMutex);
        ReleaseSemaphore(hSemEmpty, 1, NULL);
    }
    // Очистка
    for (auto& pi : sendersPI) {
        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    return 0;
}