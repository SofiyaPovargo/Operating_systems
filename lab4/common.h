#pragma once
#include <windows.h>

struct Message {
    char text[20];
};

struct FileHeader {
    int maxMessages;
    int head;
    int tail;
};

namespace SyncConst {
    inline const char* MUTEX_NAME = "MyFileMutex";
    inline const char* SEM_EMPTY_NAME = "MySemEmpty";
    inline const char* SEM_FULL_NAME = "MySemFull";
    inline const char* EVENT_READY_PREFIX = "MySenderReady_";
}