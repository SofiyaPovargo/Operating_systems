#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include "common.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 3) return 1;
    string fileName = argv[1];
    string id = argv[2];

    HANDLE hMutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, SyncConst::MUTEX_NAME);
    HANDLE hSemEmpty = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, SyncConst::SEM_EMPTY_NAME);
    HANDLE hSemFull = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, SyncConst::SEM_FULL_NAME);

    string eventName = string(SyncConst::EVENT_READY_PREFIX) + id;
    HANDLE hEventReady = OpenEventA(EVENT_ALL_ACCESS, FALSE, eventName.c_str());
    if (hEventReady == NULL) {
        cout << "DEBUG: Error opening event: " << GetLastError() << endl;
    } else {
        SetEvent(hEventReady);
        cout << "DEBUG: Signal sent to Receiver from Sender " << id << endl;
    }

    while (true) {
        cout << "\nSender " << id << ": 1 - Send, 2 - Exit: ";
        int ch;
        if (!(cin >> ch)) {
            cin.clear();
            cin.ignore(MAXSHORT, '\n');
            continue;
        }

        if (ch == 2) break;

        cout << "Enter message (max 19 chars): ";
        cin.ignore(MAXSHORT, '\n'); // Очищаем буфер перед getline
        
        string txt;
        getline(cin, txt); // Читаем всю строку целиком

        Message m = { "" };
        // Копируем безопасно
        strncpy_s(m.text, txt.c_str(), _TRUNCATE);

        // Синхронизация
        WaitForSingleObject(hSemEmpty, INFINITE);
        WaitForSingleObject(hMutex, INFINITE);

        fstream f(fileName, ios::binary | ios::in | ios::out);
        if (f.is_open()) {
            FileHeader h; 
            f.read((char*)&h, sizeof(h));

            f.seekp(sizeof(FileHeader) + h.tail * sizeof(Message));
            f.write((char*)&m, sizeof(m));

            h.tail = (h.tail + 1) % h.maxMessages;
            f.seekp(0); 
            f.write((char*)&h, sizeof(h));
            f.close();
        }

        ReleaseMutex(hMutex);
        ReleaseSemaphore(hSemFull, 1, NULL);
        
        cout << "Message sent!" << endl;
    }
    return 0;
}