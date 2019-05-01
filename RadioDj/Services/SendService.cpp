#include "SendService.h"

#define BufferSize 1024

void SendService::setup() {
    shoutService->connect();
    encoderService->setup();
}

void SendService::sendBlocking(int32_t *left, int32_t *right, size_t size) {

    std::shared_ptr<std::vector<unsigned char>> encodedVector = encoderService->encode(left, right, size);
    shoutService->send(encodedVector->data(), encodedVector->size());
    // blocking here
    shoutService->sync();

}


