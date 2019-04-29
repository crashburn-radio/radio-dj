#include "SendService.h"

#define BufferSize 1024

SendService::SendService(const char *host, int port, const char *mount, const char *username, const char *password) {

    shouter = std::make_shared<ShoutService>(
            host,
            port,
            mount,
            username,
            password,
            SHOUT_FORMAT_MP3
    );


}

void SendService::setup() {
    shouter->connect();
    encoder.setup();
    encodedQueue = encoder.getQueue();

}

void SendService::sendBlocking(int32_t *left, int32_t *right, size_t size) {

    /* mixing,encoding */
    // todo : don't use a queue here, return a vector
    encoder.encode(left, right, size);

    unsigned char sendBuffer[BufferSize];

    /* sending */
    while (!encodedQueue->empty()) {

        long sendSize = std::min(sizeof(sendBuffer), encodedQueue->size());

        for (int i = 0; i < sendSize; i++) {
            sendBuffer[i] = (unsigned char) encodedQueue->front();
            encodedQueue->pop();
        }

        shouter->send(sendBuffer, sendSize);

        // blocking here
        shouter->sync();
    }

}

