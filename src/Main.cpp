#include "SharedResources.h"
#include "MarkerThread.h"
#include <iostream>
#include <vector>
#include <memory>

using namespace SyncUtils;
using namespace ThreadManager;

class Application {
public:
    Application(size_t arraySize, int threadCount) 
        : array_(arraySize), threadCount_(threadCount) {
        InitializeCriticalSection(&controlCS_);
        shouldTerminate_ = false;
    }

    ~Application() {
        DeleteCriticalSection(&controlCS_);
    }

    void run() {
        createThreads();
        startThreads();
        processUserInput();
        cleanup();
    }

private:
    void createThreads() {
        for (int i = 0; i < threadCount_; ++i) {
            auto ctx = std::make_unique<ThreadContext>();
            ctx->array = &array_;
            ctx->threadId = i + 1;
            ctx->startSignal = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            ctx->pauseSignal = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            ctx->resumeSignal = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            ctx->shouldTerminate = &shouldTerminate_;
            ctx->controlCS = &controlCS_;

            contexts_.push_back(std::move(ctx));
            
            HANDLE thread = CreateThread(nullptr, 0, ThreadController::ThreadProc,
                contexts_.back().get(), 0, nullptr);
            threads_.push_back(thread);
        }
    }

    void startThreads() {
        for (const auto& ctx : contexts_) {
            SetEvent(ctx->startSignal);
        }
    }

    void processUserInput() {
        while (true) {
            std::cout << "\nEnter thread number to terminate (1-" << threadCount_ 
                     << ") or 0 to exit: ";
            int choice;
            std::cin >> choice;

            if (choice == 0) {
                shouldTerminate_ = true;
                break;
            }

            if (choice > 0 && choice <= threadCount_) {
                terminateThread(choice);
            }
        }
    }

    void terminateThread(int threadId) {
        shouldTerminate_ = true;
        for (const auto& ctx : contexts_) {
            SetEvent(ctx->resumeSignal);
        }
        array_.clearMarkers(threadId);
        array_.display();
    }

    void cleanup() {
        for (const auto& ctx : contexts_) {
            CloseHandle(ctx->startSignal);
            CloseHandle(ctx->pauseSignal);
            CloseHandle(ctx->resumeSignal);
        }
        for (const auto& thread : threads_) {
            WaitForSingleObject(thread, INFINITE);
            CloseHandle(thread);
        }
    }

    SharedArray array_;
    std::vector<std::unique_ptr<ThreadContext>> contexts_;
    std::vector<HANDLE> threads_;
    int threadCount_;
    CRITICAL_SECTION controlCS_;
    volatile bool shouldTerminate_;
};

int main() {
    size_t arraySize;
    int threadCount;

    std::cout << "Enter array size: ";
    std::cin >> arraySize;
    std::cout << "Enter number of threads: ";
    std::cin >> threadCount;

    Application app(arraySize, threadCount);
    app.run();

    return 0;
}