#include <iostream>
#include <Windows.h>

int arraySize = 0;
int* array = nullptr;
int amountOfThreads = 0;
HANDLE* hThreads;
HANDLE* hMarkerEvents;
HANDLE* hThreadWorkStopped;
HANDLE* hThreadsExit;
HANDLE hAllStopped;
HANDLE hGlobalStart;
CRITICAL_SECTION cs;
bool* threadActive;

DWORD WINAPI marker(LPVOID thread_index) {
    int idx = static_cast<int>(reinterpret_cast<intptr_t>(thread_index));
    int marked_numbers_counter = 0;
    int* marked_positions = new int[arraySize];
    int marked_count = 0;
    
    WaitForSingleObject(hGlobalStart, INFINITE);
    srand(idx);
    
    while (true) {
        int x = rand() % arraySize;
        
        EnterCriticalSection(&cs);
        if (array[x] == 0) {
            Sleep(5);
            array[x] = idx + 1;
            marked_positions[marked_count++] = x;
            marked_numbers_counter++;
            Sleep(5);
            LeaveCriticalSection(&cs);
            continue;
        }
        LeaveCriticalSection(&cs);
        
        std::cout << "\nMarker " << idx + 1 << "\n";
        std::cout << "Marked elements count: " << marked_numbers_counter << "\n";
        std::cout << "Unable to mark index: " << x << "\n\n";
        
        SetEvent(hAllStopped);
        SetEvent(hThreadWorkStopped[idx]);
        
        HANDLE events[2] = { hMarkerEvents[idx], hThreadsExit[idx] };
        DWORD waitResult = WaitForMultipleObjects(2, events, FALSE, INFINITE);
        
        if (waitResult == WAIT_OBJECT_0 + 1) {
            EnterCriticalSection(&cs);
            for (int i = 0; i < marked_count; i++) {
                array[marked_positions[i]] = 0;
            }
            LeaveCriticalSection(&cs);
            
            delete[] marked_positions;
            return 0;
        }
        
        ResetEvent(hMarkerEvents[idx]);
    }
}

void printArray() {
    std::cout << "\n=== Current array state ===\n";
    EnterCriticalSection(&cs);
    for (int i = 0; i < arraySize; i++) {
        std::cout << array[i] << " ";
        if ((i + 1) % 20 == 0) std::cout << std::endl;
    }
    std::cout << std::endl;
    LeaveCriticalSection(&cs);
}

int main() {
    std::cout << "Enter array size: ";
    std::cin >> arraySize;
    array = new int[arraySize]();
    
    std::cout << "Enter number of marker threads: ";
    std::cin >> amountOfThreads;
    
    InitializeCriticalSection(&cs);
    
    hThreads = new HANDLE[amountOfThreads];
    hMarkerEvents = new HANDLE[amountOfThreads];
    hThreadWorkStopped = new HANDLE[amountOfThreads];
    hThreadsExit = new HANDLE[amountOfThreads];
    threadActive = new bool[amountOfThreads];
    
    hAllStopped = CreateEvent(NULL, TRUE, FALSE, NULL);
    hGlobalStart = CreateEvent(NULL, TRUE, FALSE, NULL);
    
    for (int i = 0; i < amountOfThreads; i++) {
        hMarkerEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        hThreadWorkStopped[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        hThreadsExit[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        threadActive[i] = true;
    }
    
    for (int i = 0; i < amountOfThreads; i++) {
        hThreads[i] = CreateThread(NULL, 0, marker, 
            reinterpret_cast<LPVOID>(static_cast<intptr_t>(i)), 0, NULL);
        if (hThreads[i] == NULL) {
            std::cerr << "Error creating thread " << i + 1 
                      << ". Error code: " << GetLastError() << std::endl;
            return 1;
        }
    }
    
    std::cout << "\nStarting all marker threads...\n";
    SetEvent(hGlobalStart);
    
    int activeThreads = amountOfThreads;
    
    while (activeThreads > 0) {
        std::cout << "\nWaiting for threads to get stuck...\n";
        WaitForSingleObject(hAllStopped, INFINITE);
        ResetEvent(hAllStopped);
        
        printArray();
        
        int threadToKill;
        std::cout << "Enter thread number to terminate (1-" << amountOfThreads << "): ";
        std::cin >> threadToKill;
        threadToKill--;
        
        if (threadToKill < 0 || threadToKill >= amountOfThreads || !threadActive[threadToKill]) {
            std::cout << "Invalid thread number or thread already terminated!\n";
            continue;
        }
        
        std::cout << "Terminating thread " << threadToKill + 1 << "...\n";
        SetEvent(hThreadsExit[threadToKill]);
        
        WaitForSingleObject(hThreads[threadToKill], INFINITE);
        CloseHandle(hThreads[threadToKill]);
        threadActive[threadToKill] = false;
        activeThreads--;
        
        printArray();
        
        if (activeThreads > 0) {
            std::cout << "Resuming remaining threads...\n";
            for (int i = 0; i < amountOfThreads; i++) {
                if (threadActive[i]) {
                    ResetEvent(hThreadWorkStopped[i]);
                    SetEvent(hMarkerEvents[i]);
                }
            }
        }
    }
    
    std::cout << "\nAll threads terminated. Program finished.\n";
    
    for (int i = 0; i < amountOfThreads; i++) {
        CloseHandle(hMarkerEvents[i]);
        CloseHandle(hThreadWorkStopped[i]);
        CloseHandle(hThreadsExit[i]);
    }
    CloseHandle(hAllStopped);
    CloseHandle(hGlobalStart);
    DeleteCriticalSection(&cs);
    
    delete[] array;
    delete[] hThreads;
    delete[] hMarkerEvents;
    delete[] hThreadWorkStopped;
    delete[] hThreadsExit;
    delete[] threadActive;
    
    return 0;
}
