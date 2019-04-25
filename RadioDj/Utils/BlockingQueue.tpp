//
// Created by palo on 4/25/19.
//

#ifndef RADIODJ_BLOCKINGQUEUE_TPP
#define RADIODJ_BLOCKINGQUEUE_TPP

#include <memory>
#include <queue>
#include <mutex>


/**
 * A quickly coded BlockQueue, needed for Thread - Communication
 * @tparam T
 */
template<typename T>
class BlockingQueue {

public:

    bool isFilled() {
        return !empty();
    }

    void push(T element) {
        std::lock_guard<std::mutex> lock(this->mutex);
        this->queue->push(element);
    }

    T pop() {
        std::lock_guard<std::mutex> lock(this->mutex);
        auto result = this->queue->front();
        this->queue->pop();
        return result;
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(this->mutex);
        return this->queue->empty();
    }

    size_t size() {
        std::lock_guard<std::mutex> lock(this->mutex);
        return this->queue->size();
    }


private:
    std::shared_ptr<std::queue<T>> queue = std::shared_ptr<std::queue<T>>(new std::queue<T>());
    std::mutex mutex;
};


#endif //RADIODJ_BLOCKINGQUEUE_TPP
