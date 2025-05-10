#pragma once
#include <vector>
#include <windows.h>
#include <memory>

namespace SyncUtils {
    class ArrayGuard {
    public:
        explicit ArrayGuard(CRITICAL_SECTION& cs) : cs_(cs) {
            EnterCriticalSection(&cs_);
        }
        ~ArrayGuard() {
            LeaveCriticalSection(&cs_);
        }
    private:
        CRITICAL_SECTION& cs_;
    };

    class SharedArray {
    public:
        explicit SharedArray(size_t size);
        ~SharedArray();

        bool tryMark(size_t pos, int markerId);
        void clearMarkers(int markerId);
        void display() const;
        size_t size() const { return data_.size(); }
        int at(size_t pos) const { return data_[pos]; }

        ArrayGuard lock() const { return ArrayGuard(const_cast<CRITICAL_SECTION&>(cs_)); }

    private:
        std::vector<int> data_;
        CRITICAL_SECTION cs_;
    };
}