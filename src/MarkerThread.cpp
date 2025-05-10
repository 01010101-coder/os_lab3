#include "MarkerThread.h"
#include <random>
#include <chrono>

namespace ThreadManager {

void ThreadController::InitializeThread(ThreadContext& ctx) {
    InitializeCriticalSection(ctx.controlCS);
}

void ThreadController::CleanupThread(ThreadContext& ctx) {
    DeleteCriticalSection(ctx.controlCS);
}

DWORD WINAPI ThreadController::ThreadProc(LPVOID context) {
    auto* ctx = static_cast<ThreadContext*>(context);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, ctx->array->size() - 1);

    while (!*ctx->shouldTerminate) {
        WaitForSingleObject(ctx->startSignal, INFINITE);
        
        while (!*ctx->shouldTerminate) {
            size_t pos = dist(gen);
            if (ctx->array->tryMark(pos, ctx->threadId)) {
                SetEvent(ctx->pauseSignal);
                WaitForSingleObject(ctx->resumeSignal, INFINITE);
            }
            
            if (*ctx->shouldTerminate) break;
        }
    }
    
    return 0;
}

}