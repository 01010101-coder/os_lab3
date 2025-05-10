#pragma once
#include <windows.h>
#include "SharedResources.h"

namespace ThreadManager {
    struct ThreadContext {
        SyncUtils::SharedArray* array;
        int threadId;
        HANDLE startSignal;
        HANDLE pauseSignal;
        HANDLE resumeSignal;
        volatile bool* shouldTerminate;
        CRITICAL_SECTION* controlCS;
    };

    class ThreadController {
    public:
        static DWORD WINAPI ThreadProc(LPVOID context);
        static void InitializeThread(ThreadContext& ctx);
        static void CleanupThread(ThreadContext& ctx);
    };
}