#include <thread>
#include <chrono>
#include "ThreadSend.h"

typedef std::chrono::duration<int, std::milli> milliseconds_type;


void ThreadSend::loop() {

    while (true) {

        while (commandQueue.isFilled()) {

            const SendCommand &command = commandQueue.pop();

            sendService->sendBlocking(
                    command.left->data(),
                    command.right->data(),
                    command.left->size()
            );

            if (commandQueue.size() < 10) {
                backpressureConditional.notify_all();
            }
        }

        // wait for queue to deliver content
        fprintf(stderr, "queue empty, should not happen, sleeping for 100ms\n");
        std::this_thread::sleep_for((milliseconds_type) 100);

    }

}

void ThreadSend::setup() {
    sendService->setup();
    backpressureConditional.notify_all();
}

void ThreadSend::send(int32_t *left, int32_t *right, size_t size) {
    std::shared_ptr<std::vector<int32_t>> leftChannel = std::make_shared<std::vector<int32_t>>();
    std::shared_ptr<std::vector<int32_t>> rightChannel = std::make_shared<std::vector<int32_t>>();
    for (int i = 0; i < size; i++) {
        leftChannel->push_back(left[i]);
        rightChannel->push_back(right[i]);
    }
    commandQueue.push(SendCommand{leftChannel, rightChannel});
}

void ThreadSend::wait() {
    std::unique_lock<std::mutex> lock(backpressureMutex);
    backpressureConditional.wait(lock);
}

ThreadSend::ThreadSend(const char *host, int port, const char *mount, const char *username, const char *password) {
    sendService = std::make_shared<SendService>(host, port, mount, username, password);
}
