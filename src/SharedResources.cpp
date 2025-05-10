#include "SharedResources.h"
#include <iostream>
#include <algorithm>

namespace SyncUtils {

SharedArray::SharedArray(size_t size) : data_(size, 0) {
    InitializeCriticalSection(&cs_);
}

SharedArray::~SharedArray() {
    DeleteCriticalSection(&cs_);
}

bool SharedArray::tryMark(size_t pos, int markerId) {
    if (pos >= data_.size()) return false;
    
    auto guard = lock();
    if (data_[pos] == 0) {
        data_[pos] = markerId;
        return true;
    }
    return false;
}

void SharedArray::clearMarkers(int markerId) {
    auto guard = lock();
    std::replace_if(data_.begin(), data_.end(),
        [markerId](int val) { return val == markerId; },
        0);
}

void SharedArray::display() const {
    auto guard = lock();
    std::cout << "Array state: [";
    for (size_t i = 0; i < data_.size(); ++i) {
        std::cout << data_[i];
        if (i < data_.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}

}