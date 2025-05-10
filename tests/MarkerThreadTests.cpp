#include <gtest/gtest.h>
#include "SharedResources.h"
#include "MarkerThread.h"
#include <thread>
#include <chrono>

using namespace SyncUtils;
using namespace ThreadManager;

class SharedArrayTest : public ::testing::Test {
protected:
    void SetUp() override {
        array = std::make_unique<SharedArray>(10);
    }

    std::unique_ptr<SharedArray> array;
};

TEST_F(SharedArrayTest, InitialState) {
    for (size_t i = 0; i < array->size(); ++i) {
        EXPECT_EQ(array->at(i), 0);
    }
}

TEST_F(SharedArrayTest, MarkElement) {
    EXPECT_TRUE(array->tryMark(5, 1));
    EXPECT_EQ(array->at(5), 1);
}

TEST_F(SharedArrayTest, CannotMarkMarkedElement) {
    EXPECT_TRUE(array->tryMark(5, 1));
    EXPECT_FALSE(array->tryMark(5, 2));
    EXPECT_EQ(array->at(5), 1);
}

TEST_F(SharedArrayTest, ClearMarkers) {
    array->tryMark(5, 1);
    array->tryMark(6, 1);
    array->clearMarkers(1);
    EXPECT_EQ(array->at(5), 0);
    EXPECT_EQ(array->at(6), 0);
}

class ThreadControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        array = std::make_unique<SharedArray>(10);
        InitializeCriticalSection(&controlCS);
        shouldTerminate = false;
    }

    void TearDown() override {
        DeleteCriticalSection(&controlCS);
    }

    std::unique_ptr<SharedArray> array;
    CRITICAL_SECTION controlCS;
    volatile bool shouldTerminate;
};

TEST_F(ThreadControllerTest, ThreadInitialization) {
    ThreadContext ctx;
    ctx.array = array.get();
    ctx.threadId = 1;
    ctx.startSignal = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    ctx.pauseSignal = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    ctx.resumeSignal = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    ctx.shouldTerminate = &shouldTerminate;
    ctx.controlCS = &controlCS;

    ThreadController::InitializeThread(ctx);
    
    HANDLE thread = CreateThread(nullptr, 0, ThreadController::ThreadProc,
        &ctx, 0, nullptr);
    
    SetEvent(ctx.startSignal);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    shouldTerminate = true;
    SetEvent(ctx.resumeSignal);
    
    WaitForSingleObject(thread, INFINITE);
    
    CloseHandle(thread);
    CloseHandle(ctx.startSignal);
    CloseHandle(ctx.pauseSignal);
    CloseHandle(ctx.resumeSignal);
}